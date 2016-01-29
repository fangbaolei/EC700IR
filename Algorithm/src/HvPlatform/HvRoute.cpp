#include <net/route.h>
#include <net/if.h>

// ---------------------------------------------------------------------

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>

#define ENABLE_FEATURE_CLEAN_UP 1
#define ENABLE_FEATURE_ETC_NETWORKS 0

#define ALIGN1
#define NOINLINE
#define FAST_FUNC

#define bb_show_usage(...)
#define bb_error_msg_and_die(...)

#define xsocket socket
#define xioctl ioctl
#define xstrdup strdup
#define xmalloc malloc

#define getopt32(...) 0 // todo:

int xatoul(const char* strNum)
{
    return atoi(strNum);
}

int xatoul_range(const char* strNum, int iMin, int iMax)
{
    int iRet = 0;

    iRet = xatoul(strNum);

    if ( iRet < iMin ) iRet = iMin;
    if ( iRet > iMax ) iRet = iMax;

    return iRet;
}

int FAST_FUNC INET_resolve(const char *name, struct sockaddr_in *s_in, int hostfirst)
{
	struct hostent *hp;
#if ENABLE_FEATURE_ETC_NETWORKS
	struct netent *np;
#endif

	/* Grmpf. -FvK */
	s_in->sin_family = AF_INET;
	s_in->sin_port = 0;

	/* Default is special, meaning 0.0.0.0. */
	if (strcmp(name, "default") == 0) {
		s_in->sin_addr.s_addr = INADDR_ANY;
		return 1;
	}
	/* Look to see if it's a dotted quad. */
	if (inet_aton(name, &s_in->sin_addr)) {
		return 0;
	}
	/* If we expect this to be a hostname, try hostname database first */
#ifdef DEBUG
	if (hostfirst) {
		bb_error_msg("gethostbyname(%s)", name);
	}
#endif
	if (hostfirst) {
		hp = gethostbyname(name);
		if (hp != NULL) {
			memcpy(&s_in->sin_addr, hp->h_addr_list[0],
				sizeof(struct in_addr));
			return 0;
		}
	}
#if ENABLE_FEATURE_ETC_NETWORKS
	/* Try the NETWORKS database to see if this is a known network. */
#ifdef DEBUG
	bb_error_msg("getnetbyname(%s)", name);
#endif
	np = getnetbyname(name);
	if (np != NULL) {
		s_in->sin_addr.s_addr = htonl(np->n_net);
		return 1;
	}
#endif
	if (hostfirst) {
		/* Don't try again */
		return -1;
	}
#ifdef DEBUG
	res_init();
	_res.options |= RES_DEBUG;
	bb_error_msg("gethostbyname(%s)", name);
#endif
	hp = gethostbyname(name);
	if (hp == NULL) {
		return -1;
	}
	memcpy(&s_in->sin_addr, hp->h_addr_list[0], sizeof(struct in_addr));
	return 0;
}

/* numeric: & 0x8000: default instead of *,
 *          & 0x4000: host instead of net,
 *          & 0x0fff: don't resolve
 */
char* FAST_FUNC INET_rresolve(struct sockaddr_in *s_in, int numeric, uint32_t netmask)
{
	/* addr-to-name cache */
	struct addr {
		struct addr *next;
		struct sockaddr_in addr;
		int host;
		char name[1];
	};
	static struct addr *cache = NULL;

	struct addr *pn;
	char *name;
	uint32_t ad, host_ad;
	int host = 0;

	if (s_in->sin_family != AF_INET) {
#ifdef DEBUG
		bb_error_msg("rresolve: unsupported address family %d!",
				  s_in->sin_family);
#endif
		errno = EAFNOSUPPORT;
		return NULL;
	}
	ad = s_in->sin_addr.s_addr;
#ifdef DEBUG
	bb_error_msg("rresolve: %08x, mask %08x, num %08x", (unsigned)ad, netmask, numeric);
#endif
	if (ad == INADDR_ANY) {
		if ((numeric & 0x0FFF) == 0) {
			if (numeric & 0x8000)
				return xstrdup("default");
			return xstrdup("*");
		}
	}
	if (numeric & 0x0FFF)
		return xstrdup(inet_ntoa(s_in->sin_addr));

	if ((ad & (~netmask)) != 0 || (numeric & 0x4000))
		host = 1;
	pn = cache;
	while (pn) {
		if (pn->addr.sin_addr.s_addr == ad && pn->host == host) {
#ifdef DEBUG
			bb_error_msg("rresolve: found %s %08x in cache",
					  (host ? "host" : "net"), (unsigned)ad);
#endif
			return xstrdup(pn->name);
		}
		pn = pn->next;
	}

	host_ad = ntohl(ad);
	name = NULL;
	if (host) {
		struct hostent *ent;
#ifdef DEBUG
		bb_error_msg("gethostbyaddr (%08x)", (unsigned)ad);
#endif
		ent = gethostbyaddr((char *) &ad, 4, AF_INET);
		if (ent)
			name = xstrdup(ent->h_name);
	} else if (ENABLE_FEATURE_ETC_NETWORKS) {
		struct netent *np;
#ifdef DEBUG
		bb_error_msg("getnetbyaddr (%08x)", (unsigned)host_ad);
#endif
		np = getnetbyaddr(host_ad, AF_INET);
		if (np)
			name = xstrdup(np->n_name);
	}
	if (!name)
		name = xstrdup(inet_ntoa(s_in->sin_addr));
	pn = (struct addr *)xmalloc(sizeof(*pn) + strlen(name)); /* no '+ 1', it's already accounted for */
	pn->next = cache;
	pn->addr = *s_in;
	pn->host = host;
	strcpy(pn->name, name);
	cache = pn;
	return name;
}

// ---------------------------------------------------------------------

#ifndef RTF_UP
/* Keep this in sync with /usr/src/linux/include/linux/route.h */
#define RTF_UP          0x0001	/* route usable                 */
#define RTF_GATEWAY     0x0002	/* destination is a gateway     */
#define RTF_HOST        0x0004	/* host entry (net otherwise)   */
#define RTF_REINSTATE   0x0008	/* reinstate route after tmout  */
#define RTF_DYNAMIC     0x0010	/* created dyn. (by redirect)   */
#define RTF_MODIFIED    0x0020	/* modified dyn. (by redirect)  */
#define RTF_MTU         0x0040	/* specific MTU for this route  */
#ifndef RTF_MSS
#define RTF_MSS         RTF_MTU	/* Compatibility :-(            */
#endif
#define RTF_WINDOW      0x0080	/* per route window clamping    */
#define RTF_IRTT        0x0100	/* Initial round trip time      */
#define RTF_REJECT      0x0200	/* Reject route                 */
#endif

#if defined(SIOCADDRTOLD) || defined(RTF_IRTT)	/* route */
#define HAVE_NEW_ADDRT 1
#endif

#if HAVE_NEW_ADDRT
#define mask_in_addr(x) (((struct sockaddr_in *)&((x).rt_genmask))->sin_addr.s_addr)
#define full_mask(x) (x)
#else
#define mask_in_addr(x) ((x).rt_genmask)
#define full_mask(x) (((struct sockaddr_in *)&(x))->sin_addr.s_addr)
#endif

/* The RTACTION entries must agree with tbl_verb[] below! */
#define RTACTION_ADD 1
#define RTACTION_DEL 2

/* For the various tbl_*[] arrays, the 1st byte is the offset to
 * the next entry and the 2nd byte is return value. */

#define NET_FLAG  1
#define HOST_FLAG 2

/* We remap '-' to '#' to avoid problems with getopt. */
static const char tbl_hash_net_host[] ALIGN1 =
	"\007\001#net\0"
/*	"\010\002#host\0" */
	"\007\002#host"				/* Since last, we can save a byte. */
;

#define KW_TAKES_ARG            020
#define KW_SETS_FLAG            040

#define KW_IPVx_METRIC          020
#define KW_IPVx_NETMASK         021
#define KW_IPVx_GATEWAY         022
#define KW_IPVx_MSS             023
#define KW_IPVx_WINDOW          024
#define KW_IPVx_IRTT            025
#define KW_IPVx_DEVICE          026

#define KW_IPVx_FLAG_ONLY       040
#define KW_IPVx_REJECT          040
#define KW_IPVx_MOD             041
#define KW_IPVx_DYN             042
#define KW_IPVx_REINSTATE       043

static const char tbl_ipvx[] ALIGN1 =
	/* 020 is the "takes an arg" bit */
#if HAVE_NEW_ADDRT
	"\011\020metric\0"
#endif
	"\012\021netmask\0"
	"\005\022gw\0"
	"\012\022gateway\0"
	"\006\023mss\0"
	"\011\024window\0"
#ifdef RTF_IRTT
	"\007\025irtt\0"
#endif
	"\006\026dev\0"
	"\011\026device\0"
	/* 040 is the "sets a flag" bit - MUST match flags_ipvx[] values below. */
#ifdef RTF_REJECT
	"\011\040reject\0"
#endif
	"\006\041mod\0"
	"\006\042dyn\0"
/*	"\014\043reinstate\0" */
	"\013\043reinstate"			/* Since last, we can save a byte. */
;

static const int flags_ipvx[] = { /* MUST match tbl_ipvx[] values above. */
#ifdef RTF_REJECT
	RTF_REJECT,
#endif
	RTF_MODIFIED,
	RTF_DYNAMIC,
	RTF_REINSTATE
};

static int kw_lookup(const char *kwtbl, char ***pargs)
{
	if (**pargs) {
		do {
			if (strcmp(kwtbl+2, **pargs) == 0) { /* Found a match. */
				*pargs += 1;
				if (kwtbl[1] & KW_TAKES_ARG) {
					if (!**pargs) {	/* No more args! */
						bb_show_usage();
					}
					*pargs += 1; /* Calling routine will use args[-1]. */
				}
				return kwtbl[1];
			}
			kwtbl += *kwtbl;
		} while (*kwtbl);
	}
	return 0;
}

/* Add or delete a route, depending on action. */

static NOINLINE int INET_setroute(int action, char **args)
{
	/* char buffer instead of bona-fide struct avoids aliasing warning */
	char rt_buf[sizeof(struct rtentry)];
	struct rtentry *const rt = (struct rtentry *)rt_buf;

	const char *netmask = NULL;
	int skfd, isnet, xflag;

	/* Grab the -net or -host options.  Remember they were transformed. */
	xflag = kw_lookup(tbl_hash_net_host, &args);

	/* If we did grab -net or -host, make sure we still have an arg left. */
	if (*args == NULL) {
		bb_show_usage();
	}

	/* Clean out the RTREQ structure. */
	memset(rt, 0, sizeof(*rt));

	{
		const char *target = *args++;
		char *prefix;

		/* recognize x.x.x.x/mask format. */
		prefix = strchr((char*)target, '/');
		if (prefix) {
			int prefix_len;

			prefix_len = xatoul_range(prefix+1, 0, 32);
			mask_in_addr(*rt) = htonl( ~(0xffffffffUL >> prefix_len));
			*prefix = '\0';
#if HAVE_NEW_ADDRT
			rt->rt_genmask.sa_family = AF_INET;
#endif
		} else {
			/* Default netmask. */
			netmask = "default";
		}
		/* Prefer hostname lookup is -host flag (xflag==1) was given. */
		isnet = INET_resolve(target, (struct sockaddr_in *) &rt->rt_dst,
							 (xflag & HOST_FLAG));
		if (isnet < 0) {
			bb_error_msg_and_die("resolving %s", target);
		}
		if (prefix) {
			/* do not destroy prefix for process args */
			*prefix = '/';
		}
	}

	if (xflag) {		/* Reinit isnet if -net or -host was specified. */
		isnet = (xflag & NET_FLAG);
	}

	/* Fill in the other fields. */
	rt->rt_flags = ((isnet) ? RTF_UP : (RTF_UP | RTF_HOST));

	while (*args) {
		int k = kw_lookup(tbl_ipvx, &args);
		const char *args_m1 = args[-1];

		if (k & KW_IPVx_FLAG_ONLY) {
			rt->rt_flags |= flags_ipvx[k & 3];
			continue;
		}

#if HAVE_NEW_ADDRT
		if (k == KW_IPVx_METRIC) {
			rt->rt_metric = xatoul(args_m1) + 1;
			continue;
		}
#endif

		if (k == KW_IPVx_NETMASK) {
			struct sockaddr mask;

			if (mask_in_addr(*rt)) {
				bb_show_usage();
			}

			netmask = args_m1;
			isnet = INET_resolve(netmask, (struct sockaddr_in *) &mask, 0);
			if (isnet < 0) {
				bb_error_msg_and_die("resolving %s", netmask);
			}
			rt->rt_genmask = full_mask(mask);
			continue;
		}

		if (k == KW_IPVx_GATEWAY) {
			if (rt->rt_flags & RTF_GATEWAY) {
				bb_show_usage();
			}

			isnet = INET_resolve(args_m1,
						(struct sockaddr_in *) &rt->rt_gateway, 1);
			rt->rt_flags |= RTF_GATEWAY;

			if (isnet) {
				if (isnet < 0) {
					bb_error_msg_and_die("resolving %s", args_m1);
				}
				bb_error_msg_and_die("gateway %s is a NETWORK", args_m1);
			}
			continue;
		}

		if (k == KW_IPVx_MSS) {	/* Check valid MSS bounds. */
			rt->rt_flags |= RTF_MSS;
			rt->rt_mss = xatoul_range(args_m1, 64, 32768);
			continue;
		}

		if (k == KW_IPVx_WINDOW) {	/* Check valid window bounds. */
			rt->rt_flags |= RTF_WINDOW;
			rt->rt_window = xatoul_range(args_m1, 128, INT_MAX);
			continue;
		}

#ifdef RTF_IRTT
		if (k == KW_IPVx_IRTT) {
			rt->rt_flags |= RTF_IRTT;
			rt->rt_irtt = xatoul(args_m1);
			rt->rt_irtt *= (sysconf(_SC_CLK_TCK) / 100);	/* FIXME */
#if 0					/* FIXME: do we need to check anything of this? */
			if (rt->rt_irtt < 1 || rt->rt_irtt > (120 * HZ)) {
				bb_error_msg_and_die("bad irtt");
			}
#endif
			continue;
		}
#endif

		/* Device is special in that it can be the last arg specified
		 * and doesn't requre the dev/device keyword in that case. */
		if (!rt->rt_dev && ((k == KW_IPVx_DEVICE) || (!k && !*++args))) {
			/* Don't use args_m1 here since args may have changed! */
			rt->rt_dev = args[-1];
			continue;
		}

		/* Nothing matched. */
		bb_show_usage();
	}

#ifdef RTF_REJECT
	if ((rt->rt_flags & RTF_REJECT) && !rt->rt_dev) {
		rt->rt_dev = (char*)"lo";
	}
#endif

	/* sanity checks.. */
	if (mask_in_addr(*rt)) {
		uint32_t mask = mask_in_addr(*rt);

		mask = ~ntohl(mask);
		if ((rt->rt_flags & RTF_HOST) && mask != 0xffffffff) {
			bb_error_msg_and_die("netmask %.8x and host route conflict",
								 (unsigned int) mask);
		}
		if (mask & (mask + 1)) {
			bb_error_msg_and_die("bogus netmask %s", netmask);
		}
		mask = ((struct sockaddr_in *) &rt->rt_dst)->sin_addr.s_addr;
		if (mask & ~(uint32_t)mask_in_addr(*rt)) {
			bb_error_msg_and_die("netmask and route address conflict");
		}
	}

	/* Fill out netmask if still unset */
	if ((action == RTACTION_ADD) && (rt->rt_flags & RTF_HOST)) {
		mask_in_addr(*rt) = 0xffffffff;
	}

	{
        int iRet = 0;

        /* Create a socket to the INET kernel. */
        skfd = xsocket(AF_INET, SOCK_DGRAM, 0);

        if (action == RTACTION_ADD)
            iRet = xioctl(skfd, SIOCADDRT, rt);
        else
            iRet = xioctl(skfd, SIOCDELRT, rt);

        if (ENABLE_FEATURE_CLEAN_UP) close(skfd);

        return (iRet < 0) ? (-1) : (0);
	}
}

static const unsigned flagvals[] = { /* Must agree with flagchars[]. */
	RTF_GATEWAY,
	RTF_HOST,
	RTF_REINSTATE,
	RTF_DYNAMIC,
	RTF_MODIFIED
};

#define IPV4_MASK (RTF_GATEWAY|RTF_HOST|RTF_REINSTATE|RTF_DYNAMIC|RTF_MODIFIED)
#define IPV6_MASK (RTF_GATEWAY|RTF_HOST|RTF_DEFAULT|RTF_ADDRCONF|RTF_CACHE)

/* Must agree with flagvals[]. */
static const char flagchars[] ALIGN1 =
	"GHRDM"
;

static void set_flags(char *flagstr, int flags)
{
	int i;

	*flagstr++ = 'U';

	for (i = 0; (*flagstr = flagchars[i]) != 0; i++) {
		if (flags & flagvals[i]) {
			++flagstr;
		}
	}
}

/* also used in netstat */
void FAST_FUNC bb_displayroutes(int noresolve, int netstatfmt)
{
	char devname[64], flags[16], *sdest, *sgw;
	unsigned long d, g, m;
	int flgs, ref, use, metric, mtu, win, ir;
	struct sockaddr_in s_addr;
	struct in_addr mask;

	FILE *fp = fopen("/proc/net/route", "r");

	printf("Kernel IP routing table\n"
	       "Destination     Gateway         Genmask         Flags %s Iface\n",
			netstatfmt ? "  MSS Window  irtt" : "Metric Ref    Use");

	if (fscanf(fp, "%*[^\n]\n") < 0) { /* Skip the first line. */
		goto ERROR;		   /* Empty or missing line, or read error. */
	}
	while (1) {
		int r;
		r = fscanf(fp, "%63s%lx%lx%X%d%d%d%lx%d%d%d\n",
				   devname, &d, &g, &flgs, &ref, &use, &metric, &m,
				   &mtu, &win, &ir);
		if (r != 11) {
			if ((r < 0) && feof(fp)) { /* EOF with no (nonspace) chars read. */
				break;
			}
 ERROR:
			bb_error_msg_and_die("fscanf");
		}

		if (!(flgs & RTF_UP)) { /* Skip interfaces that are down. */
			continue;
		}

		set_flags(flags, (flgs & IPV4_MASK));
#ifdef RTF_REJECT
		if (flgs & RTF_REJECT) {
			flags[0] = '!';
		}
#endif

		memset(&s_addr, 0, sizeof(struct sockaddr_in));
		s_addr.sin_family = AF_INET;
		s_addr.sin_addr.s_addr = d;
		sdest = INET_rresolve(&s_addr, (noresolve | 0x8000), m); /* 'default' instead of '*' */
		s_addr.sin_addr.s_addr = g;
		sgw = INET_rresolve(&s_addr, (noresolve | 0x4000), m); /* Host instead of net */
		mask.s_addr = m;
		/* "%15.15s" truncates hostnames, do we really want that? */
		printf("%-15.15s %-15.15s %-16s%-6s", sdest, sgw, inet_ntoa(mask), flags);
		free(sdest);
		free(sgw);
		if (netstatfmt) {
			printf("%5d %-5d %6d %s\n", mtu, win, ir, devname);
		} else {
			printf("%-6d %-2d %7d %s\n", metric, ref, use, devname);
		}
	}

	fclose(fp);
}

#define ROUTE_OPT_A     0x01
#define ROUTE_OPT_n     0x02
#define ROUTE_OPT_e     0x04
#define ROUTE_OPT_INET6 0x08 /* Not an actual option. See below. */

/* 1st byte is offset to next entry offset.  2nd byte is return value. */
/* 2nd byte matches RTACTION_* code */
static const char tbl_verb[] ALIGN1 =
	"\006\001add\0"
	"\006\002del\0"
/*	"\011\002delete\0" */
	"\010\002delete"  /* Since it's last, we can save a byte. */
;

int route_main(int argc, char **argv)
{
	unsigned opt;
	int what;
	char *family;
	char **p;

	/* First, remap '-net' and '-host' to avoid getopt problems. */
	p = argv;
	while (*++p) {
		if (strcmp(*p, "-net") == 0 || strcmp(*p, "-host") == 0) {
			p[0][0] = '#';
		}
	}

	opt = getopt32(argv, "A:ne", &family);

	if ((opt & ROUTE_OPT_A) && strcmp(family, "inet") != 0) {
		bb_show_usage();
	}

	argv += optind;

	/* No more args means display the routing table. */
	if (!*argv) {
		int noresolve = (opt & ROUTE_OPT_n) ? 0x0fff : 0;
        bb_displayroutes(noresolve, opt & ROUTE_OPT_e);
        fflush(stdout);
        return EXIT_SUCCESS;
	}

	/* Check verb.  At the moment, must be add, del, or delete. */
	what = kw_lookup(tbl_verb, &argv);
	if (!what || !*argv) {		/* Unknown verb or no more args. */
		bb_show_usage();
	}

    if ( 0 == INET_setroute(what, argv) )
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

// ------------------------------------------------------------------

// Comment by Shaorg: 这部分代码主要是从开源工程busybox-1.19.3中提取和整理的。

#define ROUTE_PARAM_COUNT_MAX 8
#define ROUTE_PARAM_CONTEXT_LEN_MAX 32
#define ROUTE_PARAM_BUFFER_LEN_MAX 256

// 用法简述：
// 与使用Linux的route命令基本一致，
// 其中的szArg就是在命令行下使用route命令添加在后面的参数串。
// 例如：
// 在命令行下的route add default gw 172.18.9.28就等价于
// 函数调用HvRoute("add default gw 172.18.9.28");
int HvRoute(char* szArg)
{
    int n = 0;
    char szArgument[ROUTE_PARAM_BUFFER_LEN_MAX] = {0};
    char* argv[ROUTE_PARAM_COUNT_MAX+1] = {NULL};
    char table[ROUTE_PARAM_COUNT_MAX][ROUTE_PARAM_CONTEXT_LEN_MAX];
    int iArgCount = 0;
    char* token = NULL;

    strcpy(szArgument, szArg);
    token = strtok(szArgument, " ");
    while ( token != NULL )
    {
        strcpy(table[iArgCount++], token);
        token = strtok(NULL, " ");
    }

    if ( iArgCount > ROUTE_PARAM_COUNT_MAX )
    {
        return -1;
    }

    argv[0] = "HvRoute";
    for ( n=0; n<iArgCount; ++n )
    {
        argv[n+1] = table[n];
    }

    return route_main(iArgCount+1, (char**)&argv);
}
