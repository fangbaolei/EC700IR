#include "swpa.h"
#include "swpa_version.h"

int swpa_version(void)
{
#ifdef VERSION
	return VERSION;
#else
	return -1;
#endif
}

