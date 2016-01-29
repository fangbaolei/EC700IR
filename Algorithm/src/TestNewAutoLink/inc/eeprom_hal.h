#ifndef _EEPROM_HAL_H
#define _EEPROM_HAL_H

#ifdef __cplusplus
	extern "C" {
#endif	//#ifdef __cplusplus

//在使用EEPROM之前必须先看此文档,如果考虑到与以前所有非正式的程序兼容.
//老版本使用的EEPROM系统配置只使了前面2K的地址空间.		0x0000 ~ 0x07FF
//现加入了CRC校验之后地址空间为紧跟其后的2K地址空间.	0x0800 ~ 0x0FFF
//生产使用的记录EEPROM使用地址空间为紧跟其后的4K地间.	0x1000 ~ 0x1FFF
//使用新版本的EEPROM方式地址空间.(INI文件).				0x2000 ~ 0xBFFF
//复位记录使用EEPROM地址空间.							0xC000 ~ 0xFFFF

#define SYS_MAX_VD_NUM			6

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
#define BOOT_CONFIG_START_ADDR	0x0000

#define BOOT_ACTIVE_BLEN		2
#define BOOT_VERSION_BLEN		2
#define BOOT_BOOT_BLEN			2
#define BOOT_RESERVE_BLEN		2
#define BOOT_CRC_CHECK_BLEN		4
#define BOOT_BLENS_CRC			( BOOT_ACTIVE_BLEN + BOOT_VERSION_BLEN + BOOT_BOOT_BLEN \
									+ BOOT_RESERVE_BLEN + BOOT_CRC_CHECK_BLEN )
#define BOOT_BLENS				( BOOT_ACTIVE_BLEN + BOOT_VERSION_BLEN + BOOT_BOOT_BLEN \
									+ BOOT_RESERVE_BLEN )

#define BOOT_ACTIVE_OFFSET		0
#define BOOT_VERSION_OFFSET		( BOOT_ACTIVE_OFFSET + BOOT_ACTIVE_BLEN )
#define BOOT_BOOT_OFFSET		( BOOT_VERSION_OFFSET + BOOT_VERSION_BLEN )
#define BOOT_CRC_CHECK_OFFSET	( BOOT_BOOT_OFFSET + BOOT_BOOT_BLEN )

#define BOOT_CONFIG_ALL_BLEN	128
/*------------------------------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
#define SYS_CONFIG_START_ADDR	( BOOT_CONFIG_START_ADDR + BOOT_CONFIG_ALL_BLEN )

#define SYS_ACTIVE_BLEN			2
#define SYS_ID_BLEN				8
#define SYS_NAME_LEN_BLEN		1
#define SYS_SYSMODE_BLEN		1
#define SYS_RECOGFONT_BLEN		1
#define SYS_RESETCNT_BLEN		4
#define SYS_CONFIG_CRC_BLEN		4
#define SYS_USED_BLEN			( SYS_ID_BLEN + SYS_NAME_LEN_BLEN + SYS_SYSMODE_BLEN \
									+ SYS_RECOGFONT_BLEN + SYS_RESETCNT_BLEN )
#define SYS_USED_BLEN_CRC		( SYS_USED_BLEN + SYS_CONFIG_CRC_BLEN )

#define SYS_GLOBAL_ALL_BLEN		32
#define SYS_NAME_BLEN			256
#define SYS_CONFIG_ALL_BLEN		( SYS_GLOBAL_ALL_BLEN + SYS_NAME_BLEN )

#define SYS_ACTIVE_OFFSET		0
#define SYS_ID_ADDR_OFFSET		( SYS_ACTIVE_OFFSET + 0 )
#define SYS_NAME_LEN_OFFSET		( SYS_ID_ADDR_OFFSET + SYS_ID_BLEN )
#define SYS_SYSMODE_OFFSET		( SYS_NAME_LEN_OFFSET + SYS_NAME_LEN_BLEN )
#define SYS_RECOGFONT_OFFSET	( SYS_SYSMODE_OFFSET + SYS_SYSMODE_BLEN )
#define SYS_RESETCNT_OFFSET		( SYS_RECOGFONT_OFFSET + SYS_RECOGFONT_BLEN )
#define SYS_CONFIG_CRC_OFFSET	( SYS_VD_BRIGHT_OFFSET + SYS_VD_BRIGHT_BLEN )
#define SYS_USED_OFFSET			( SYS_ACTIVE_OFFSET + SYS_ACTIVE_BLEN )
#define SYS_NAME_OFFSET			SYS_GLOBAL_ALL_BLEN

#define SYS_ACTIVE_ADDR			( SYS_CONFIG_START_ADDR + SYS_ACTIVE_OFFSET )
#define SYS_ID_ADDR				( SYS_CONFIG_START_ADDR + SYS_ID_ADDR_OFFSET )
#define SYS_NAME_LEN_ADDR		( SYS_CONFIG_START_ADDR + SYS_NAME_LEN_OFFSET )
#define SYS_SYSMODE_ADDR		( SYS_CONFIG_START_ADDR + SYS_SYSMODE_OFFSET )
#define SYS_RECOGFONT_ADDR		( SYS_CONFIG_START_ADDR + SYS_RECOGFONT_OFFSET )
#define SYS_RESETCNT_ADDR		( SYS_CONFIG_START_ADDR + SYS_RESETCNT_OFFSET )
#define SYS_USED_ADDR			( SYS_CONFIG_START_ADDR + SYS_USED_OFFSET )
#define SYS_NAME_ADDR			( SYS_CONFIG_START_ADDR + SYS_NAME_OFFSET )
#define SYS_CONFIG_CRC_ADDR		( SYS_CONFIG_START_ADDR + SYS_CONFIG_CRC_OFFSET )

//系统工作场合模式设置机制说明:在设置工作场合时,只有SYS_WORK_MODE_FLAG相关位为1时才表示为设置工作场合,而其余情况
//均视为只为复位系统.
#define SYS_WORK_MODE_FLAG		0x80
/*------------------------------------------------------------------------------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------------------------------------------------------------------------*/
/*
VD有关配置EEPROM内存分配方式如下:
1.每个ID占用16字节的存储空间,目前使用前7个字节,别留有8个字节用于扩展域.
2.VD流名称放置在VD流配置基本信息之后的位置.
3.VD分为三类数据保存:
	A.全局VD配置信息(32字节),系统当前所使用的VD流的总个数;哪些ID流是系统当前所使用的(由一个字节的相关位Bit5~Bit0来表示,位值为1表示使用,为0表示不使用);
	B.每个VD流基本配置信息(16字节 X 6 ),以VD流的ID号为索引存放;
	C.每个VD流的名称(256字节 X 6 ).
备注:
1.VD ID号的命名,使用Bit1,Bit0来表示VD映射到CPU的VP接口状态,Bit2~Bit1为表示当前ID接入的CPU的VP端口号分别表示为0~2(3为保留且当前无意义);Bit0表示当前
视频流使用CPU的缓冲区编号,为0表示使用A缓冲器,为1表示使用B缓冲器.
*/
#define VD_CONFIG_START_ADDR	( SYS_CONFIG_START_ADDR + SYS_CONFIG_ALL_BLEN )

#define VD_GLOBAL_ACTIVE_BLEN	2	//VD全局配置信息中当前有效配置的字节数.
#define VD_TAB_ACTIVE_BLEN		7	//VD流基本配表中当前有效配置的字节数.

#define VD_GLOBAL_ALL_BLEN		32
#define VD_TAB_ALL_BLEN			16
#define VD_VDNAME_ALL_BLEN		256
#define VD_CONFIG_ALL_BLEN		( VD_GLOBAL_ALL_BLEN + ( VD_TAB_ALL_BLEN + VD_VDNAME_ALL_BLEN ) * SYS_MAX_VD_NUM )

#define VD_GLOBAL_OFFSET		0
#define VD_TAB_OFFSET			( VD_GLOBAL_OFFSET + VD_GLOBAL_ALL_BLEN )
#define VD_VDNAME_OFFSET		( VD_TAB_OFFSET + ( SYS_MAX_VD_NUM * VD_TAB_ALL_BLEN ) )
#define VD_TAB_NAMELEN_OFFSET	6

#define VD_GLOBAL_ADDR			( VD_CONFIG_START_ADDR + VD_GLOBAL_OFFSET )
#define VD_TAB_ADDR				( VD_CONFIG_START_ADDR + VD_TAB_OFFSET )
#define VD_VDNAME_ADDR			( VD_CONFIG_START_ADDR + VD_VDNAME_OFFSET )
/*------------------------------------------------------------------------------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------------------------------------------------------------------------*/
#define COMM_CONFIG_START_ADDR	( VD_CONFIG_START_ADDR + VD_CONFIG_ALL_BLEN )

#define COMM_TCPIP_BLEN			12	//VD全局配置信息中当前有效配置的字节数.
#define COMM_IPPORT_BLEN		9
#define COMM_USED_BLEN			( COMM_TCPIP_BLEN + COMM_IPPORT_BLEN )
#define COMM_ALL_BLEN			32

#define	COMM_TCPIP_OFFSET		0
#define COMM_IPPORT_OFFSET		( COMM_TCPIP_OFFSET + COMM_TCPIP_BLEN )

#define COMM_TCPIP_ADDR			( COMM_CONFIG_START_ADDR + COMM_TCPIP_OFFSET )
#define COMM_IPPORT_ADDR		( COMM_CONFIG_START_ADDR + COMM_IPPORT_OFFSET )
/*------------------------------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
#define PWD_CONFIG_START_ADDR 	( COMM_CONFIG_START_ADDR + COMM_ALL_BLEN )

#define PWD_ACTIVE_BLEN	2
#define PWD_MAX_BLEN 	8   //密码长度
#define PWD_USED_BLEN 	24
#define PWD_CRC_BLEN	4 
#define PWD_ALL_BLEN	64

#define PWD_ACTIVE_OFFSET 		0
#define PWD_SETTING_OFFSET 	( PWD_ACTIVE_OFFSET + PWD_ACTIVE_BLEN)
#define PWD_DEBUG_OFFSET 		( PWD_SETTING_OFFSET + PWD_MAX_BLEN)
#define PWD_MAINTAIN_OFFSET 	( PWD_DEBUG_OFFSET + PWD_MAX_BLEN)
#define PWD_CRC_OFFSET 		( PWD_MAINTAIN_OFFSET + PWD_MAX_BLEN)

#define PWD_ACTIVE_ADDR 		( PWD_CONFIG_START_ADDR + PWD_ACTIVE_OFFSET)
#define PWD_SETTING_ADDR 		( PWD_CONFIG_START_ADDR + PWD_SETTING_OFFSET)
#define PWD_DEBUG_ADDR 		( PWD_CONFIG_START_ADDR + PWD_DEBUG_OFFSET)
#define PWD_MAINTAIN_ADDR 		( PWD_CONFIG_START_ADDR + PWD_MAINTAIN_OFFSET)
#define PWD_CRC_ADDR			( PWD_CONFIG_START_ADDR + PWD_CRC_OFFSET)
/*------------------------------------------------------------------------------------------------------------------------------------------------*/

#define BAK_CONFIG_START_ADDR		PWD_CONFIG_START_ADDR
#define BOOT_CONFIG_BAK_START_ADDR	( BAK_CONFIG_START_ADDR + PWD_ALL_BLEN )
#define SYS_CONFIG_BAK_START_ADDR	( BOOT_CONFIG_BAK_START_ADDR + BOOT_CONFIG_ALL_BLEN )
#define SYS_NAME_BAK_ADDR			( SYS_CONFIG_BAK_START_ADDR + SYS_GLOBAL_ALL_BLEN )
#define COMM_CONFIG_BAK_START_ADDR	( SYS_NAME_BAK_ADDR + SYS_NAME_BLEN )
#define PWD_CONFIG_BAK_START_ADDR	( COMM_CONFIG_BAK_START_ADDR + COMM_ALL_BLEN )

/*------------------------------------------------------------------------------------------------------------------------------------------------*/
#define SYS_PRODUCT_START_ADDR		0x1000

#define PNAME_CONFIG_START_ADDR		SYS_PRODUCT_START_ADDR

#define PNAME_MAX_BLEN			128
#define PNAME_ACTIVE_BLEN		2
#define PNAME_LEN_BLEN			1
#define PNAME_NAME_BLEN			29

#define PNAME_ACTIVE_OFFSET		0
#define PNAME_LEN_OFFSET		( PNAME_ACTIVE_OFFSET + PNAME_ACTIVE_BLEN )
#define PNAME_NAME_OFFSET		( PNAME_LEN_OFFSET + PNAME_LEN_BLEN )

#define PNAME_ACTIVE_ADDR		( PNAME_CONFIG_START_ADDR + PNAME_ACTIVE_OFFSET )
#define PNAME_LEN_ADDR			( PNAME_CONFIG_START_ADDR + PNAME_LEN_OFFSET )
#define PNAME_NAME_ADDR			( PNAME_CONFIG_START_ADDR + PNAME_NAME_OFFSET )

#define TEST_REPORT_START_ADDR	( PNAME_CONFIG_START_ADDR + PNAME_MAX_BLEN )

#define TEST_ERR_MAX_NUM		11

#define TEST_MAX_BLEN			128
#define TEST_ACTIVE_BLEN		2
#define TEST_LEN_BLEN			2
#define TEST_RCOUNT_BLEN		4
#define TEST_ALLCOUNT_BLEN		4
#define TEST_ALLERR_BLEN		4
#define TEST_ERRCOUNT_BLEN		( TEST_ERR_MAX_NUM * 4 )
#define TEST_ITEMCOUNT_BLEN		( TEST_ERR_MAX_NUM * 4 )
#define TEST_CRC_CHECK			4
#define TEST_COUNT_BLEN			( TEST_LEN_BLEN + TEST_ALLCOUNT_BLEN + 			\
									TEST_ALLERR_BLEN + TEST_ERRCOUNT_BLEN + 	\
									TEST_ITEMCOUNT_BLEN + TEST_RCOUNT_BLEN + 	\
									TEST_CRC_CHECK )

#define TEST_ACTIVE_OFFSET		0
#define TEST_ALLCOUNT_OFFSET	( TEST_ACTIVE_OFFSET + TEST_ACTIVE_BLEN )
#define TEST_ALLERR_OFFSET		( TEST_ALLCOUNT_OFFSET + TEST_ALLCOUNT_BLEN )
#define TEST_ERRCOUNT_OFFSET	( TEST_ALLERR_OFFSET + TEST_ALLERR_BLEN )


#define TEST_ACTIVE_ADDR		( TEST_REPORT_START_ADDR + TEST_ACTIVE_OFFSET )
#define TEST_ALLCOUNT_ADDR		( TEST_REPORT_START_ADDR + TEST_ALLCOUNT_OFFSET )
#define TEST_ALLERR_ADDR		( TEST_REPORT_START_ADDR + TEST_ALLERR_OFFSET )
#define TEST_ERRCOUNT_ADDR		( TEST_REPORT_START_ADDR + TEST_ERRCOUNT_OFFSET )

#define BACKUP_START_ADDR		( TEST_REPORT_START_ADDR + TEST_MAX_BLEN )
#define BACKUP_MAX_NUM			2
#define BACKUP_STEP_SIZE		TEST_MAX_BLEN

/*------------------------------------------------------------------------------------------------------------------------------------------------*/

#define PARAM_FILE_START_ADDR 0x2000
#define PARAM_FILE_DAT_LEN 32768
#define PARAM_FILE_ALL_LEN ( PARAM_FILE_DAT_LEN + 1024 )

#define EEPROM_FILE_START_ADDR		(PARAM_FILE_START_ADDR + PARAM_FILE_ALL_LEN)
#define BOOT_FILE_MAX_LEN			128
#define COUNT_FILE_MAX_LEN			128
#define REPORT_FILE_MAX_LEN			256

#define EEPROM_RESET_COUNT_START_ADDR 		0xC000
#define EEPROM_RESET_RECORD_START_ADDR		EEPROM_RESET_COUNT_START_ADDR + 32			
#define EEPROM_RESET_RECORD_BLOCK_LEN		32
#define EEPROM_RESET_RECORD_MAX_NUM			511

#ifdef __cplusplus
	}
#endif	//#ifdef __cplusplus

#endif	//#ifndef _EEPROM_HAL_H


