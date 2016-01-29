#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <time.h>
#include <ctype.h>

#include <drv_device.h>

#include "swpa.h"

#ifdef SWPA_DEVICE_SETTING
#define SWPA_DEVICE_SETTING_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_DEVICE_SETTING_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_DEVICE_SETTING_PRINT(fmt, ...)
#define SWPA_DEVICE_SETTING_CHECK(arg)
#endif

#define    VALID_TIME_RECORD    99    // 有效记录数
#define    MAX_TIME_RECORD      (VALID_TIME_RECORD+1)

#define    MAX_LOG_LENGTH        64
#define    VALID_LOG_RECORD      24    // 有效记录数
#define    MAX_LOG_RECORD        (VALID_LOG_RECORD+1)

#define    MASTER    1
#define    SLAVER    0

#pragma pack(push,1)

///* 数据结构说明:用来定义实时时间. */
typedef struct {
    uint16_t wYear;    //年数.
    uint16_t wMonth;   //月数.
    uint16_t wDay;     //号数.
    uint16_t wWeekNum; //当前日期的星期数.
    uint16_t wHour;    //小数数,24小时制.
    uint16_t wMinute;  //小时数.
    uint16_t wSecond;  //秒数.
    uint16_t wMSecond; //毫秒数.
} REAL_TIME_STRUCT;

typedef struct boot_time_table_head_s {
    uint32_t head_crc;                // 表头crc
    uint32_t current_times;            // 当前启动次数
    uint32_t head_index;                // 时间记录表头索引
    uint32_t current_index;            // 当前记录索引
} boot_time_table_head_t;

typedef struct boot_time_s {
    uint32_t times;                         // 当前启动次数
    REAL_TIME_STRUCT real_time;            // 启动时间
} boot_time_t;

typedef struct boot_time_table_s {
    boot_time_table_head_t head;    // 启动时间表头
    boot_time_t    boot_time[MAX_TIME_RECORD];        // 启动时间
} boot_time_table_t;

typedef struct boot_log_table_head_s {
    uint32_t head_crc;                // 表头crc
    uint32_t version;                // 版本号
    uint32_t head_index;                // 日志记录表头索引
    uint32_t current_index;            // 当前记录索引
} boot_log_table_head_t;

typedef struct boot_log_s {
    uint32_t boot_times;                // 当前启动次数
    uint8_t  boot_message[MAX_LOG_LENGTH];        // 日志信息
} boot_log_t;

typedef struct boot_log_table_s {
    boot_log_table_head_t head;        // 日志表头
    boot_log_t    boot_log[MAX_LOG_RECORD];        // 日志
} boot_log_table_t;

typedef struct boot_arg_s {
    uint32_t     table_crc;          // 表crc值
    uint32_t     factory_setting;    // 出厂标记
    uint32_t     version;            // 表版本号
    uint32_t     rtm_flag;           // rtm值
    uint32_t     boot_count;         // 启动计数
    uint32_t     bootdelay;          // 锁串口设置
    uint8_t      bootarg[256];       // bootarg设置
} boot_arg_t;

typedef struct boot_arg_s2 {
    uint32_t    table_crc;          // 表crc值
    uint32_t    factory_setting;    // 出厂标记
    uint32_t    version;            // 表版本号
    uint32_t    rtm_flag;           // rtm值
    uint32_t    boot_count;         // 启动计数
    uint32_t    bootdelay;          // 锁串口设置
    uint8_t     bootarg[256];       // bootarg设置
    uint32_t    boot_count_en;      // 使能启动计数(新加)
} boot_arg_t2;

typedef union boot_configure {
    boot_arg_t boot_table;
    boot_arg_t2 boot_table2;
    uint8_t fill_empty[1024];
} boot_cfg_u;

// flash环境变量
//#define CFG_DEF_EEPROM_ADDR F_DM6467_ADDR_UBOOT
#define CFG_SPI_FLASH_SECTOR_LEN (1024*64)
#define CFG_ENV_OFFSET (CFG_SPI_FLASH_SECTOR_LEN * 7) // uboot = 512k, 8个64k
#define CFG_ENV_SIZE   (CFG_SPI_FLASH_SECTOR_LEN)     // env = 64k

typedef    struct environment_s {
    unsigned long    crc;        /* CRC32 over data bytes    */
#ifdef CFG_REDUNDAND_ENVIRONMENT
    unsigned char    flags;        /* active/obsolete flags    */
#endif
    unsigned char    data[CFG_ENV_SIZE]; /* Environment data        */
} env_t;


#pragma pack(pop)

static boot_cfg_u g_boot_cfg;
static boot_time_table_t g_boot_time_table;
static boot_log_table_t g_boot_log_table;

static void convert_ms2time(uint32_t time_low, uint32_t time_high, REAL_TIME_STRUCT* ts)
{
    uint64_t millsecond = 0;
    time_t second = 0;
    struct tm* timeinfo = NULL;

    if (ts == NULL)
        return;

    millsecond = time_high;
    millsecond <<= 32;
    millsecond |= time_low;

    ts->wMSecond = (uint32_t)(millsecond % 1000);
    second = (time_t)((millsecond - ts->wMSecond) / 1000);

    timeinfo = localtime(&second);
    ts->wYear = timeinfo->tm_year + 1900;
    ts->wMonth  = timeinfo->tm_mon + 1;
    ts->wWeekNum = timeinfo->tm_wday;
    ts->wDay  = timeinfo->tm_mday;
    ts->wHour = timeinfo->tm_hour;
    ts->wMinute  = timeinfo->tm_min;
    ts->wSecond  = timeinfo->tm_sec;
}

static void convert_time2ms(REAL_TIME_STRUCT* ts, uint32_t* time_low, uint32_t* time_high)
{
    struct tm tm_time;
    time_t time = 0;
    uint64_t millsecond = 0;

    if ((NULL == ts) || (NULL ==time_low) || (NULL == time_high))
        return;

    tm_time.tm_year = ts->wYear - 1900;
    tm_time.tm_mon = ts->wMonth - 1;
    tm_time.tm_mday = ts->wDay;
    tm_time.tm_hour = ts->wHour;
    tm_time.tm_min = ts->wMinute;
    tm_time.tm_sec = ts->wSecond;

    time = mktime(&tm_time);    /* 得到秒数 */
    /*
    不能合并为下式
        millsecond = time*1000 + ts->wMSecond;
    */
    millsecond = time;
    millsecond *= 1000;
    millsecond += ts->wMSecond;

    *time_low = (uint32_t)(millsecond);
    *time_high = (uint32_t)(millsecond >> 32);
}

static int get_timezone(int* timezone)
{
    char* date_cmd = "date +%z";
    FILE *fp = NULL;
    char timezone_info[32] = {0};
    int tmp_time_zone = 0;

    fp = popen(date_cmd, "r");
    if (fp == NULL)
    {
        return -1;
    }
    fread(timezone_info, sizeof(timezone_info), 1, fp);
    pclose(fp);

    timezone_info[strlen(timezone_info) - 1] = '\0';

    if (isdigit(timezone_info[1]) && isdigit(timezone_info[2]))
    {
        tmp_time_zone = (timezone_info[1] - '0') * 10 + (timezone_info[2] - '0');
        if (timezone_info[0] == '-')
        {
            tmp_time_zone = - tmp_time_zone;
        }
        *timezone = tmp_time_zone;

        return 0;
    }
    else
    {
        *timezone = 0;
        return -1;
    }

    return 0;
}

// 调整时区，old_time和new_time可以是同一结构体指针
static void adjust_time(REAL_TIME_STRUCT* old_time, REAL_TIME_STRUCT* new_time, int timezone)
{
    uint32_t time_high = 0;
    uint32_t time_low = 0;

    convert_time2ms(old_time, &time_low, &time_high);

    time_low += timezone * 3600 * 1000;

    convert_ms2time(time_low, time_high, new_time);
}

//////////////////////////////////////////////////////////

int read_boot_configure(int is_master)
{
    int retry_times;
    int crc_init = 0;
    int ret;
    uint32_t tmp_crc;
    int eeprom_adr;
    unsigned int len = E_LEN_PARAM_BACKUP;

    eeprom_adr = is_master ? E_ADDR_CRC : E_ADDR_PARAM_BACKUP;

    retry_times = 5;
    while ( retry_times-- )
    {

        ret = drv_eeprom_read(DEVID_IIC_EEPROM, eeprom_adr, (unsigned char*)&g_boot_cfg, &len);
        if (ret != SWPAR_OK)
        {
            SWPA_DEVICE_SETTING_PRINT("Read boot configure table %s error\n",
                    is_master ? "master" : "slaver");
        }
        else
        {
            break;
        }
    }

    if (retry_times == 0)
    {
        SWPA_DEVICE_SETTING_PRINT("Read boot configure table exit \n");
        return SWPAR_FAIL;
    }

    tmp_crc = swpa_utils_calc_crc32(crc_init, (unsigned char*)&g_boot_cfg.boot_table.factory_setting,
            sizeof(boot_cfg_u)-sizeof(g_boot_cfg.boot_table.table_crc));

    if (tmp_crc != g_boot_cfg.boot_table.table_crc)
    {
        SWPA_DEVICE_SETTING_PRINT("Check boot configure table crc fail tmp: %x save: %x \n", tmp_crc,g_boot_cfg.boot_table.table_crc);
        return SWPAR_FAIL;
    }

    return SWPAR_OK;
}

int write_boot_configure(int is_master)
{
    int retry_times;
    int crc_init = 0;
    int ret;
    int eeprom_adr;
    unsigned int len = E_LEN_PARAM_BACKUP;

    g_boot_cfg.boot_table.table_crc = swpa_utils_calc_crc32(crc_init, (unsigned char*)&g_boot_cfg.boot_table.factory_setting,
                sizeof(boot_cfg_u)-sizeof(g_boot_cfg.boot_table.table_crc));

    eeprom_adr = is_master ? E_ADDR_CRC : E_ADDR_PARAM_BACKUP;

    retry_times = 5;
    while (retry_times--)
    {
        ret = drv_eeprom_write(DEVID_IIC_EEPROM, eeprom_adr, (unsigned char*)&g_boot_cfg, &len);
        if (ret != SWPAR_OK)
        {
            SWPA_DEVICE_SETTING_PRINT("Write boot configure table %s error\n",
                    is_master ? "master" : "slaver");
        }
        else
        {
            break;
        }
    }

    if (retry_times == 0)
    {
        SWPA_DEVICE_SETTING_PRINT("Write boot configure table exit \n");
        return SWPAR_FAIL;
    }

    return SWPAR_OK;
}

static int load_boot_config(void)
{
    int ret;

    memset(&g_boot_cfg, 0, sizeof(boot_cfg_u));
    ret = read_boot_configure(MASTER);

    // 读主不成功，再从备份
    if (SWPAR_OK != ret)
    {
        SWPA_DEVICE_SETTING_PRINT("read_boot_configure(master) error, then read slave configure\n");
        ret = read_boot_configure(SLAVER);
        if (SWPAR_OK != ret)
        {
            SWPA_DEVICE_SETTING_PRINT("read_boot_configure(slaver) error\n");
            return SWPAR_FAIL;
        }
    }

    return SWPAR_OK;
}

////////////////////////////////////////////////////////////////////
int swpa_device_set_bootcount(int count)
{
    int ret = 0;

    ret = load_boot_config();
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("Read boot configure failed!!\n");
        return SWPAR_FAIL;
    }

    g_boot_cfg.boot_table.boot_count = count;

    ret = write_boot_configure(MASTER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write master fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    ret = write_boot_configure(SLAVER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write slaver fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    return SWPAR_OK;
}

int swpa_device_set_bootdelay(int delaytime)
{
    int ret = 0;
    if (delaytime > 31 && delaytime < 0)
    {
        SWPA_DEVICE_SETTING_PRINT("wrong delay value!\n");
        return -2;
    }

    ret = load_boot_config();
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("Read boot configure failed!!\n");
        return SWPAR_FAIL;
    }

    g_boot_cfg.boot_table.bootdelay = delaytime;

    ret = write_boot_configure(MASTER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write master fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    ret = write_boot_configure(SLAVER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write slaver fail return %08x\n", ret);
        return SWPAR_FAIL;
    }
    return SWPAR_OK;
}

int swpa_device_lock_uboot(void)
{
    int ret = 0;

    ret = load_boot_config();
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("Read boot configure failed!!\n");
        return SWPAR_FAIL;
    }

    g_boot_cfg.boot_table.bootdelay = 0;

    ret = write_boot_configure(MASTER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write master fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    ret = write_boot_configure(SLAVER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write slaver fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    return SWPAR_OK;
}

int swpa_device_unlock_uboot(void)
{
    int ret = 0;

    ret = load_boot_config();
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("Read boot configure failed!!\n");
        return SWPAR_FAIL;
    }

    g_boot_cfg.boot_table.bootdelay = 2;

    ret = write_boot_configure(MASTER);
    if (ret != SWPAR_OK) {
        SWPA_DEVICE_SETTING_PRINT("write master fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    ret = write_boot_configure(SLAVER);
    if (ret != SWPAR_OK) {
        SWPA_DEVICE_SETTING_PRINT("write slaver fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    return SWPAR_OK;
}

int swpa_device_lock_linuxinfo(void)
{
    char arguments[256] =
    "mem=96M console=null,115200n8 root=/dev/ram rw initrd=0x83000000,32M  ramdisk_size=32768";

    return swpa_device_write_bootargs(arguments);
}

int swpa_device_unlock_linuxinfo(void)
{
    char arguments[256] =
    "mem=96M console=ttyS0,115200n8 root=/dev/ram rw initrd=0x83000000,32M  ramdisk_size=32768";

    return swpa_device_write_bootargs(arguments);
}

int swpa_device_restore_defaultinfo()
{
    int ret = 0;
    unsigned int len = E_LEN_PARAM_BACKUP;


    /////////////////////
    len = sizeof(boot_time_table_t);
    memset(&g_boot_time_table, 0, sizeof(boot_time_table_t));

    ret = drv_eeprom_read(DEVID_IIC_EEPROM, E_ADDR_START_TABLE, (unsigned char*)&g_boot_time_table, &len);
    if (SWPAR_OK != ret)
    {
        SWPA_DEVICE_SETTING_PRINT("ERROR >> Read boot time table error return %08x\n", ret);
        return SWPAR_FAIL;
    }

    g_boot_time_table.head.head_crc = 0;

    ret = drv_eeprom_write(DEVID_IIC_EEPROM, E_ADDR_START_TABLE, (unsigned char*)&g_boot_time_table, &len);
    if (SWPAR_OK != ret)
    {
        SWPA_DEVICE_SETTING_PRINT("ERROR >> Write boot time table error return %08x\n", ret);
        return SWPAR_FAIL;
    }

    /////////////////////
    len = sizeof(boot_log_table_t);

    memset(&g_boot_log_table, 0, sizeof(boot_log_table_t));

    ret = drv_eeprom_read(DEVID_IIC_EEPROM, E_ADDR_UBOOT_LOG, (unsigned char*)&g_boot_log_table, &len);
    if (SWPAR_OK != ret)
    {
        SWPA_DEVICE_SETTING_PRINT("ERROR >> Read boot time table error return %08x\n", ret);
        return SWPAR_FAIL;
    }

    g_boot_log_table.head.head_crc = 0;

    ret = drv_eeprom_write(DEVID_IIC_EEPROM, E_ADDR_UBOOT_LOG, (unsigned char*)&g_boot_log_table, &len);
    if (SWPAR_OK != ret)
    {
        SWPA_DEVICE_SETTING_PRINT("ERROR >> Write boot time table error return %08x\n", ret);
        return SWPAR_FAIL;
    }

    return SWPAR_OK;
}

int swpa_device_write_bootargs(char* bootargs)
{
    int ret = 0;
    char arguments[256] ={0};

    SWPA_DEVICE_SETTING_CHECK(NULL != bootargs);
    if (strlen(bootargs) > 256)
    {
        return -2;
    }

    memcpy(arguments, bootargs, strlen(bootargs));

    ret = load_boot_config();
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("Read boot configure failed!!\n");
        return SWPAR_FAIL;
    }

    memcpy(g_boot_cfg.boot_table.bootarg, arguments, strlen(arguments)+1);

    ret = write_boot_configure(MASTER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write master fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    ret = write_boot_configure(SLAVER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write slaver fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    return SWPAR_OK;
}

int swpa_device_read_bootargs(char* bootargs, unsigned int len)
{
    int ret = 0;
    char arguments[256] ={0};

    SWPA_DEVICE_SETTING_CHECK(NULL != bootargs);

    memset(arguments, '\0', sizeof(arguments));

    ret = load_boot_config();
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("Read boot configure failed!!\n");
        return SWPAR_FAIL;
    }

    memcpy(arguments, g_boot_cfg.boot_table.bootarg, sizeof(arguments));

    memcpy(bootargs, arguments, len);

    return SWPAR_OK;
}

int swpa_device_read_boottime(char* boottime, unsigned int len)
{
    unsigned int tmp_len = 0;
    tmp_len = sizeof(boot_time_table_t);
    int index = 0;
    int index_end = 0;
    int ret = 0;
    int timezone = 0;
    char boot_buffer[4096] = {0};
    char tmp_buffer[32] = {0};
    char* ptr = NULL;

    SWPA_DEVICE_SETTING_CHECK(NULL != boottime);
    if (len > 4096)
    {
        return -2;
    }

    memset(&g_boot_time_table, 0, sizeof(boot_time_table_t));

    get_timezone(&timezone);

    ret = drv_eeprom_read(DEVID_IIC_EEPROM, E_ADDR_START_TABLE, (unsigned char*)&g_boot_time_table, &tmp_len);
    if (SWPAR_OK != ret)
    {
        SWPA_DEVICE_SETTING_PRINT("ERROR >> Read boot time table error return %08x\n", ret);
        return SWPAR_FAIL;
    }

    /////////////

    // todo: 先校验crc 再显示
//    printf("boot_time_table head\n");
//    printf("crc        times        head_index     next_index\n");
//    printf("%08x    %08x    %08x    %08x\n",
//            g_boot_time_table.head.head_crc,
//            g_boot_time_table.head.current_times,
//            g_boot_time_table.head.head_index,
//            g_boot_time_table.head.current_index
//            );
//
//    printf("times    real_time\n");

    index = g_boot_time_table.head.head_index;
    index_end = g_boot_time_table.head.current_index;

    ptr = boot_buffer;
    while (index != index_end)
    {
        // adjust time zone...
        adjust_time(&g_boot_time_table.boot_time[index].real_time, &g_boot_time_table.boot_time[index].real_time, timezone);

        sprintf(tmp_buffer, "[%d]: %04d-%02d-%02d %02d:%02d:%02d\n",
                g_boot_time_table.boot_time[index].times,
                g_boot_time_table.boot_time[index].real_time.wYear,
                g_boot_time_table.boot_time[index].real_time.wMonth,
                g_boot_time_table.boot_time[index].real_time.wDay,
                g_boot_time_table.boot_time[index].real_time.wHour,
                g_boot_time_table.boot_time[index].real_time.wMinute,
                g_boot_time_table.boot_time[index].real_time.wSecond);

        strcpy(ptr, tmp_buffer);
        ptr += strlen(tmp_buffer);

        index++;
        if (index == MAX_TIME_RECORD)
        {
            index = 0;
        }
    }

    memcpy(boottime, boot_buffer, len);

    return SWPAR_OK;
}

int swpa_device_read_bootlog(char* bootlog, unsigned int len)
{
    unsigned int tmp_len = 0;
    tmp_len = sizeof(boot_log_table_t);
    int index = 0;
    int index_end = 0;
    int ret = 0;
    int timezone = 0;
    char boot_buffer[4096] = {0};
    char tmp_buffer[128] = {0};
    char tmp_info[64] = {0};
    char* ptr = NULL;
    REAL_TIME_STRUCT real_time;
    int year, month, day, hour, minute, second;

    SWPA_DEVICE_SETTING_CHECK(NULL != bootlog);
    if (len > 4096)
    {
        return -2;
    }

    get_timezone(&timezone);

    memset(&g_boot_log_table, 0, sizeof(boot_log_table_t));
    ret = drv_eeprom_read(DEVID_IIC_EEPROM, E_ADDR_UBOOT_LOG, (unsigned char*)&g_boot_log_table, &tmp_len);
    if (SWPAR_OK != ret)
    {
        SWPA_DEVICE_SETTING_PRINT("ERROR >> Read log table error return %08x\n", ret);
        return SWPAR_FAIL;
    }

    // todo: 先校验crc 再显示
//    printf("boot_log_table head\n");
//    printf("crc        version        head_index     next_index\n");
//    printf("%08x    %08x    %08x    %08x\n",
//            g_boot_log_table.head.head_crc,
//            g_boot_log_table.head.version,
//            g_boot_log_table.head.head_index,
//            g_boot_log_table.head.current_index
//    );

    if (g_boot_log_table.head.head_index == g_boot_log_table.head.current_index)
    {
        SWPA_DEVICE_SETTING_PRINT("No more boot log\n");
        return SWPAR_OK;
    }

    index = g_boot_log_table.head.head_index;;
    index_end = g_boot_log_table.head.current_index;

    ptr = boot_buffer;

    while (index != index_end)
    {
        //1. read time info
        sscanf((char*)g_boot_log_table.boot_log[index].boot_message,
                "%04d-%02d-%02d %02d:%02d:%02d%[ -~]",
                &year, &month, &day, &hour, &minute, &second,
                tmp_info);

        real_time.wYear = year;
        real_time.wMonth = month;
        real_time.wDay = day;
        real_time.wHour = hour;
        real_time.wMinute = minute;
        real_time.wSecond = second;

        // 2. adjust time zone
        adjust_time(&real_time, &real_time, timezone);

        // 3. make new info
        sprintf(tmp_buffer, "[%d]: %04d-%02d-%02d %02d:%02d:%02d%s\n",
                g_boot_log_table.boot_log[index].boot_times,
                real_time.wYear, real_time.wMonth, real_time.wDay,
                real_time.wHour, real_time.wMinute, real_time.wSecond,
                tmp_info);

        strcpy(ptr, tmp_buffer);
        ptr += strlen(tmp_buffer);

        index++;

        if (index == MAX_LOG_RECORD)
        {
            index = 0;
        }
    }

    memcpy(bootlog, boot_buffer, len);

    return SWPAR_OK;
}

////////////////////////////////////////////////////////////////////

int swpa_device_write_sn(char* sn)
{
    // 防止写序列号出错，每次最多写255字节
    unsigned int len = 255 < strlen(sn)+1 ? 255 : strlen(sn)+1;
    unsigned char buffer[256] = {0};
    int ret = 0;

    SWPA_DEVICE_SETTING_CHECK(NULL != sn);

    strncpy((char*)buffer, sn, len);
    ret = drv_eeprom_write(DEVID_IIC_EEPROM, E_ADDR_MACHINE_SN, buffer, &len);

    return ret;
}

int swpa_device_read_sn(char* sn,  unsigned int* len)
{
    SWPA_DEVICE_SETTING_CHECK(NULL != sn);
    SWPA_DEVICE_SETTING_CHECK(NULL != len);
	int ret = 0;

    ret = drv_eeprom_read(DEVID_IIC_EEPROM, E_ADDR_MACHINE_SN, (unsigned char*)sn, len);
	if (0 == ret)
	{
		*len = 255 < strlen(sn) ? 255 : strlen(sn);
	}

	return ret;
}

// 注：主机字节序
static int MyGetIpString(unsigned int ip, char* ip_str, int len)
{
    int ret = 0;
    if( ip_str == NULL ) return -1;

    unsigned char* byte=(unsigned char*)&ip;
    ret = snprintf((char *)ip_str, len, "%d.%d.%d.%d", byte[3],byte[2],byte[1],byte[0]);
    if( ret < 0)
    {
        return -1;
    }
    return 0;
}

// 注：主机字节序
static int MyGetIpDWord(char* ip_str, unsigned int* ip)
{
    int ret = 0;
    if( ip_str == NULL ) return -1;

    unsigned int tmp[4]={0};

    ret = sscanf((char*)ip_str,"%d.%d.%d.%d",tmp+3,tmp+2,tmp+1,tmp);
    if(ret != 4)
    {
        return -1;
    }

    *ip = (tmp[3]<<24)|(tmp[2]<<16)|(tmp[1]<<8)|tmp[0];
    return 0;
}

int swpa_device_write_ipinfo(char* ip_str, char* netmark_str, char* gateway_str)
{
    unsigned char buffer[12] = {0};
    unsigned int ip = 0;
    unsigned int netmask = 0;
    unsigned int gateway = 0;
    unsigned int len = 4 * 3;
    int ret = 0;

    SWPA_DEVICE_SETTING_CHECK(NULL != ip_str);
    SWPA_DEVICE_SETTING_CHECK(NULL != netmark_str);
    SWPA_DEVICE_SETTING_CHECK(NULL != gateway_str);

    MyGetIpDWord(ip_str, &ip);
    MyGetIpDWord(netmark_str, &netmask);
    MyGetIpDWord(gateway_str, &gateway);

    ip = htonl(ip);
    netmask = htonl(netmask);
    gateway = htonl(gateway);

    memcpy(buffer, (void*)&ip, 4);
    memcpy(buffer + 4, (void*)&gateway, 4);
    memcpy(buffer + 4 + 4, (void*)&netmask, 4);

    ret = drv_eeprom_write(DEVID_IIC_EEPROM, E_ADDR_IP_ADDR, buffer, &len);

    if (ret < 0)
    {
        SWPA_DEVICE_SETTING_PRINT("write ip failed!\n");
        return SWPAR_FAIL;
    }
    return SWPAR_OK;
}

int swpa_device_read_ipinfo(char* ip_str, int ip_len, char* netmark_str, int netmark_len, char* gateway_str, int gateway_len)
{
    unsigned char buffer[12] = {0};
    unsigned int ip = 0;
    unsigned int gateway = 0;
    unsigned int netmask = 0;
    unsigned int len = 4 * 3;
    int ret = 0;

    SWPA_DEVICE_SETTING_CHECK(NULL != ip_str);
    SWPA_DEVICE_SETTING_CHECK(NULL != netmark_str);
    SWPA_DEVICE_SETTING_CHECK(NULL != gateway_str);

    ret = drv_eeprom_read(DEVID_IIC_EEPROM, E_ADDR_IP_ADDR, buffer, &len);
    if (ret < 0)
    {
        SWPA_DEVICE_SETTING_PRINT("read ip failed!\n");
        return SWPAR_FAIL;
    }

    memcpy((void *)&ip, buffer, 4);
    memcpy((void *)&gateway, buffer + 4, 4);
    memcpy((void *)&netmask, buffer + 4 + 4, 4);

    ip = htonl(ip);
    netmask = htonl(netmask);
    gateway = htonl(gateway);

    ret = MyGetIpString(ip, ip_str, ip_len);
    if (ret < 0)
    {
        return SWPAR_FAIL;
    }
    ret = MyGetIpString(netmask, netmark_str, netmark_len);
    if (ret < 0)
    {
        return SWPAR_FAIL;
    }
    ret = MyGetIpString(gateway, gateway_str, gateway_len);
    if (ret < 0)
    {
        return SWPAR_FAIL;
    }
    return SWPAR_OK;
}

int swpa_device_set_bootcount_flag(int is_enable)
{
    int ret = 0;

    SWPA_DEVICE_SETTING_CHECK((0 == is_enable || 1 == is_enable));

    ret = load_boot_config();
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("Read boot configure failed!!\n");
        return SWPAR_FAIL;
    }

    if (g_boot_cfg.boot_table.version == 1)
    {
        SWPA_DEVICE_SETTING_PRINT("Old uboot version, No boot_count_en to write!!\n");
        return SWPAR_OK;
    }

    g_boot_cfg.boot_table2.boot_count_en = is_enable;

    ret = write_boot_configure(MASTER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write master fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    ret = write_boot_configure(SLAVER);
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("write slaver fail return %08x\n", ret);
        return SWPAR_FAIL;
    }

    return SWPAR_OK;
}

int swpa_device_get_bootcount_flag(int* flag)
{
    int ret = 0;
    uint32_t tmp = 0;

    SWPA_DEVICE_SETTING_CHECK(NULL != flag);

    ret = load_boot_config();
    if (ret != SWPAR_OK)
    {
        SWPA_DEVICE_SETTING_PRINT("Read boot configure failed!!\n");
        return SWPAR_FAIL;
    }

    if (g_boot_cfg.boot_table.version == 1)
    {
        SWPA_DEVICE_SETTING_PRINT("Old uboot version, No boot_count_en to read!!\n");
        *flag = -1;
        return SWPAR_OK;
    }
    tmp = g_boot_cfg.boot_table2.boot_count_en;

    *flag = (int)tmp;

    return SWPAR_OK;
}


// ---------------------- 设置硬盘标志 START-----------------------------
 /**
 * 设置硬盘标志
 *
 * @param has_hdd [in]   : 1: 设备上存在硬盘 0: 设备无硬盘
 *
 * @return 成功：0  失败：-1
 *
 * @note
 */
int swpa_device_set_hdd_flag(int has_hdd)
{
	return drv_set_hdd_flag(has_hdd);
}

 /**
 * 设置硬盘标志
 *
 * @param has_hdd [out]   : 1: 设备上存在硬盘 0: 设备无硬盘 -1: 数据读取无效
 *
 * @return 成功：0  失败：-1
 *
 * @note
 */
int swpa_device_get_hdd_flag(int* has_hdd)
{
	return drv_get_hdd_flag(has_hdd);
}




/**
 * 通过CPLD设置H264的分辨率
 *
 * @param resolution_sel [in] : 分辨率选择，0: 720P 1：1080P
 * @return 成功：SWPAR_OK  失败：SWPAR_FAIL 参数非法：-2
 *
 * @note
 寄存器0x0c
 01000       720P
 10000       1080P

 */
int swpa_device_set_resolution(int resolution_sel)
{
	unsigned int addr;
	unsigned char data;
	int ret = 0;

    SWPA_DEVICE_SETTING_CHECK((0 == resolution_sel || 1 == resolution_sel));

	addr = 0x0C;

	ret = drv_cpld_read(DEVID_SPI_CPLD, addr, &data);

    // 720P
    if (resolution_sel == 0)
    {
        data &= 0x0f;   // 第4位清零
        data |= (1 << 3);   // 第3位置1
    }
    // 1080P
    else if (resolution_sel == 1)
    {
        data &= 0x17;   // 第3位清零
        data |= (1 << 4); // 第4位置1
    }
    else
    {
        return -2;
    }

	ret = drv_cpld_write(DEVID_SPI_CPLD, addr, data);
	if (ret< 0)
	{
	   SWPA_DEVICE_SETTING_PRINT("Set resolution failed(cpld io)!!\n");
	   return -1;
	}
	return 0;
}



