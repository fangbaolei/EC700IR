/**
* @version	v1.0
* @brief	一体机升级包制作程序。备注：同时支持Windows和Linux平台
* @author	Signalway
* @date		2011-10-26
*/

#include <stdio.h>
#include "HvUpdatePackCreater.h"

#ifdef WIN32
char M_FileTable[5][256] = {".\\data\\CamDsp.out", ".\\data\\CamApp", ".\\data\\uImage.master", ".\\data\\initrd.img.gz.master", ".\\data\\fpga.xsvf"};
char S_FileTable[4][256] = {".\\data\\HvDsp.out", ".\\data\\LprApp", ".\\data\\uImage.slave", ".\\data\\initrd.img.gz.slave"};
char UbootFileTable[2][256] = {".\\data\\uboot.master", ".\\data\\uboot.slave"};
char DM6467_Single_FileTable[4][256] = {".\\data\\HvDsp.out", ".\\data\\LprApp", ".\\data\\uImage.6467s", ".\\data\\initrd.img.gz.6467s"};
char IPTFileTable[1][256] = {".\\data\\IPT.bin"};
#else
char M_FileTable[5][256] = {"./data/CamDsp.out", "./data/CamApp", "./data/uImage.master", "./data/initrd.img.gz.master", "./data/fpga.xsvf"};
char S_FileTable[4][256] = {"./data/HvDsp.out", "./data/LprApp", "./data/uImage.slave", "./data/initrd.img.gz.slave"};
char UbootFileTable[2][256] = {"./data/uboot.master", "./data/uboot.slave"};
char DM6467_Single_FileTable[4][256] = {"./data/HvDsp.out", "./data/LprApp", "./data/uImage.6467s", "./data/initrd.img.gz.6467s"};
char IPTFileTable[1][256] = {"./data/IPT.bin"};
#endif

unsigned char keyMAC[3] = {0};

static void usage(char* szExeName);

static int MasterFileExist();
static int SlaveFileExist();
static int UbootFileExist();
static int FpgaFileExist();
static int Dm6467SingleFileExist();
static int IPTFileExist();

static void CreateMasterUpdatePack(char* szExeName, int fWithFpga, unsigned char (*keyMAC)[3]);
static void CreateSlaveUpdatePack(char* szExeName, unsigned char (*keyMAC)[3]);
static void CreateAllUpdatePack(char* szExeName, int fWithFpga, unsigned char (*keyMAC)[3]);
static void CreateUbootUpdatePack(char* szExeName, unsigned char (*keyMAC)[3]);
static void CreateDm6467SingleUpdatePack(char* szExeName, unsigned char (*keyMAC)[3]);
static void CreateIPTUpdatePack(char* szExeName, unsigned char (*keyMAC)[3]);

int main(int argc, char** argv)
{
	if ( argc != 2 && argc != 3 )
	{
		usage(argv[0]);
		return 0;
	}

	if ( 3 == argc )
	{
		int iTmp1=0,iTmp2=0,iTmp3=0;
		sscanf(argv[2], "%x-%x-%x", &iTmp1,&iTmp2,&iTmp3);
		keyMAC[0] = iTmp1;
		keyMAC[1] = iTmp2;
		keyMAC[2] = iTmp3;
	}

	switch ( atoi(argv[1]) )
	{
	case 0:
		if ( 0 != MasterFileExist() || 0 != SlaveFileExist() )
		{
			printf("File incomplete!\n");
			return -1;
		}
		CreateAllUpdatePack(argv[0], 0, &keyMAC);
		break;

	case 1:
		if ( 0 != MasterFileExist() )
		{
			printf("File incomplete!\n");
			return -1;
		}
		CreateMasterUpdatePack(argv[0], 0, &keyMAC);
		break;

	case 2:
		if ( 0 != SlaveFileExist() )
		{
			printf("File incomplete!\n");
			return -1;
		}
		CreateSlaveUpdatePack(argv[0], &keyMAC);
		break;

	case 3:
		if ( 0 != UbootFileExist() )
		{
			printf("File incomplete!\n");
			return -1;
		}
		CreateUbootUpdatePack(argv[0], &keyMAC);
		break;

	case 4:
		if ( 0 != MasterFileExist() 
			|| 0 != SlaveFileExist() 
			|| 0 != FpgaFileExist() )
		{
			printf("File incomplete!\n");
			return -1;
		}
		CreateAllUpdatePack(argv[0], 1, &keyMAC);
		break;

	case 5:
		if ( 0 != MasterFileExist() || 0 != FpgaFileExist() )
		{
			printf("File incomplete!\n");
			return -1;
		}
		CreateMasterUpdatePack(argv[0], 1, &keyMAC);
		break;

	case 6:
		if ( 0 != Dm6467SingleFileExist() )
		{
			printf("File incomplete!\n");
			return -1;
		}
		CreateDm6467SingleUpdatePack(argv[0], &keyMAC);
		break;

	case 7:
		if ( 0 != IPTFileExist() )
		{
			printf("File incomplete!\n");
			return -1;
		}
		CreateIPTUpdatePack(argv[0], &keyMAC);
		break;

	default:
		usage(argv[0]);
		break;
	}

	return 0;
}

static void usage(char* szExeName)
{
	printf("\n用法: %s 0|1|2|3|4|5|6|7 [MAC地址高24位]\n\n0: 一体机完整升级包\n1: 一体机主CPU升级包\n2: 一体机从CPU升级包\n3: Uboot完整升级包\n4: 一体机完整升级包（含FPGA）\n5: 一体机主CPU升级包（含FPGA）\n6: 单板DM6467高清识别器升级包\n7: IPT升级包\n\n示例：%s 4 6c-ca-16\n", szExeName, szExeName);
}

// -------------------------------------------------------------------------------------

static int MasterFileExist()
{
	if ( -1 == ExistFile(M_FileTable[0])
		|| -1 == ExistFile(M_FileTable[1])
		|| -1 == ExistFile(M_FileTable[2])
		|| -1 == ExistFile(M_FileTable[3]) )
	{
		return -1;
	}
	return 0;
}

static int SlaveFileExist()
{
	if ( -1 == ExistFile(S_FileTable[0])
		|| -1 == ExistFile(S_FileTable[1])
		|| -1 == ExistFile(S_FileTable[2])
		|| -1 == ExistFile(S_FileTable[3]) )
	{
		return -1;
	}
	return 0;
}

static int UbootFileExist()
{
	if ( -1 == ExistFile(UbootFileTable[0])
		|| -1 == ExistFile(UbootFileTable[1]) )
	{
		return -1;
	}
	return 0;
}

static int FpgaFileExist()
{
	if ( -1 == ExistFile(M_FileTable[4]) )
	{
		return -1;
	}
	return 0;
}

static int Dm6467SingleFileExist()
{
	if ( -1 == ExistFile(DM6467_Single_FileTable[0])
		|| -1 == ExistFile(DM6467_Single_FileTable[1])
		|| -1 == ExistFile(DM6467_Single_FileTable[2])
		|| -1 == ExistFile(DM6467_Single_FileTable[3]) )
	{
		return -1;
	}
	return 0;
}

static int IPTFileExist()
{
	if ( -1 == ExistFile(IPTFileTable[0]) )
	{
		return -1;
	}
	return 0;
}

// -------------------------------------------------------------------------------------

static void CreateMasterUpdatePack(char* szExeName, int fWithFpga, unsigned char (*keyMAC)[3])
{
	char szArgument[256];

	HvEncryptFile_DSP(M_FileTable[0], "firmware.bin");
	strcpy(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'firmware' -d firmware.bin firmware.img");
	HvMakeImage(szExeName, szArgument);

	HvEncryptFile(M_FileTable[1], "tools.bin");
	strcpy(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'tools' -d tools.bin tools.img");
	HvMakeImage(szExeName, szArgument);

	HvCryptFile(M_FileTable[2], "uImage.bin");
	strcpy(szArgument, "-A arm -O linux -T kernel -C none -a 0x0 -e 0x0 -n 'kernel' -d uImage.bin kernel.img");
	HvMakeImage(szExeName, szArgument);

	HvCryptFile(M_FileTable[3], "rootfs.bin");
	strcpy(szArgument, "-A arm -O linux -T ramdisk -C none -a 0x0 -e 0x0 -n 'rootfs' -d rootfs.bin rootfs.img");
	HvMakeImage(szExeName, szArgument);

	if ( fWithFpga != 0 )
	{
		HvEncryptFile(M_FileTable[4], "fpga.bin");
		strcpy(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'fpga' -d fpga.bin fpga.img");
		HvMakeImage(szExeName, szArgument);

		strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'master' -d kernel.img:rootfs.img:tools.img:firmware.img:fpga.img master.img");
		HvMakeImage(szExeName, szArgument);
	}
	else
	{
		strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'master' -d kernel.img:rootfs.img:tools.img:firmware.img master.img");
		HvMakeImage(szExeName, szArgument);
	}

#ifdef WIN32
	DeleteFile("firmware.bin");
	DeleteFile("firmware.img");
	DeleteFile("tools.bin");
	DeleteFile("tools.img");
	DeleteFile("uImage.bin");
	DeleteFile("kernel.img");
	DeleteFile("rootfs.bin");
	DeleteFile("rootfs.img");
#else
	remove("firmware.bin");
	remove("firmware.img");
	remove("tools.bin");
	remove("tools.img");
	remove("uImage.bin");
	remove("kernel.img");
	remove("rootfs.bin");
	remove("rootfs.img");
#endif

	if ( fWithFpga != 0 )
	{
#ifdef WIN32
		DeleteFile("fpga.bin");
		DeleteFile("fpga.img");
#else
		remove("fpga.bin");
		remove("fpga.img");
#endif
	}

	if ( 0!=(*keyMAC)[0] && 0!=(*keyMAC)[1] && 0!=(*keyMAC)[2] )
	{
		CryptoByMAC_File("master.img", "HvUpdatePacket_master.bin", (unsigned char*)keyMAC);
		strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'upgrade' -d HvUpdatePacket_master.bin HvUpdatePacket_master.img");
		HvMakeImage(szExeName, szArgument);

#ifdef WIN32
		DeleteFile("HvUpdatePacket_master.bin");
#else
		remove("HvUpdatePacket_master.bin");
#endif
	}
}

static void CreateSlaveUpdatePack(char* szExeName, unsigned char (*keyMAC)[3])
{
	char szArgument[256];

	HvEncryptFile_DSP(S_FileTable[0], "s_firmware.bin");
	strcpy(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'firmware' -d s_firmware.bin s_firmware.img");
	HvMakeImage(szExeName, szArgument);

	HvEncryptFile(S_FileTable[1], "s_tools.bin");
	strcpy(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'tools' -d s_tools.bin s_tools.img");
	HvMakeImage(szExeName, szArgument);

	HvCryptFile(S_FileTable[2], "s_uImage.bin");
	strcpy(szArgument, "-A arm -O linux -T kernel -C none -a 0x0 -e 0x0 -n 'kernel' -d s_uImage.bin s_kernel.img");
	HvMakeImage(szExeName, szArgument);

	HvCryptFile(S_FileTable[3], "s_rootfs.bin");
	strcpy(szArgument, "-A arm -O linux -T ramdisk -C none -a 0x0 -e 0x0 -n 'rootfs' -d s_rootfs.bin s_rootfs.img");
	HvMakeImage(szExeName, szArgument);

	strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'slave' -d s_kernel.img:s_rootfs.img:s_tools.img:s_firmware.img slave.img");
	HvMakeImage(szExeName, szArgument);

#ifdef WIN32
	DeleteFile("s_firmware.bin");
	DeleteFile("s_firmware.img");
	DeleteFile("s_tools.bin");
	DeleteFile("s_tools.img");
	DeleteFile("s_uImage.bin");
	DeleteFile("s_kernel.img");
	DeleteFile("s_rootfs.bin");
	DeleteFile("s_rootfs.img");
#else
	remove("s_firmware.bin");
	remove("s_firmware.img");
	remove("s_tools.bin");
	remove("s_tools.img");
	remove("s_uImage.bin");
	remove("s_kernel.img");
	remove("s_rootfs.bin");
	remove("s_rootfs.img");
#endif

	if ( 0!=(*keyMAC)[0] && 0!=(*keyMAC)[1] && 0!=(*keyMAC)[2] )
	{
		CryptoByMAC_File("slave.img", "HvUpdatePacket_slave.bin", (unsigned char*)keyMAC);
		strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'upgrade' -d HvUpdatePacket_slave.bin HvUpdatePacket_slave.img");
		HvMakeImage(szExeName, szArgument);

#ifdef WIN32
		DeleteFile("HvUpdatePacket_slave.bin");
#else
		remove("HvUpdatePacket_slave.bin");
#endif
	}
}

static void CreateAllUpdatePack(char* szExeName, int fWithFpga, unsigned char (*keyMAC)[3])
{
	char szArgument[256];

	CreateMasterUpdatePack(szExeName, fWithFpga, keyMAC);
	CreateSlaveUpdatePack(szExeName, keyMAC);
	strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'all' -d master.img:slave.img upgrade.img");
	HvMakeImage(szExeName, szArgument);

#ifdef WIN32
	DeleteFile("master.img");
	DeleteFile("slave.img");
#else
	remove("master.img");
	remove("slave.img");
#endif

	if ( 0!=(*keyMAC)[0] && 0!=(*keyMAC)[1] && 0!=(*keyMAC)[2] )
	{
		CryptoByMAC_File("upgrade.img", "HvUpdatePacket.bin", (unsigned char*)keyMAC);
		strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'upgrade' -d HvUpdatePacket.bin HvUpdatePacket.img");
		HvMakeImage(szExeName, szArgument);

#ifdef WIN32
		DeleteFile("upgrade.img");
		DeleteFile("HvUpdatePacket.bin");
#else
		remove("upgrade.img");
		remove("HvUpdatePacket.bin");
#endif
	}
}

static void CreateUbootUpdatePack(char* szExeName, unsigned char (*keyMAC)[3])
{
	char szArgument[256];

	sprintf(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'uboot-m' -d %s uboot.img", UbootFileTable[0]);
	HvMakeImage(szExeName, szArgument);

	sprintf(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'uboot-s' -d %s s_uboot.img", UbootFileTable[1]);
	HvMakeImage(szExeName, szArgument);

	strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'uboot-all' -d uboot.img:s_uboot.img uboot_all.img");
	HvMakeImage(szExeName, szArgument);

#ifdef WIN32
	DeleteFile("uboot.img");
	DeleteFile("s_uboot.img");
#else
	remove("uboot.img");
	remove("s_uboot.img");
#endif

	if ( 0!=(*keyMAC)[0] && 0!=(*keyMAC)[1] && 0!=(*keyMAC)[2] )
	{
		CryptoByMAC_File("uboot_all.img", "HvUpdatePacket_uboot_all.bin", (unsigned char*)keyMAC);
		strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'upgrade' -d HvUpdatePacket_uboot_all.bin HvUpdatePacket_uboot_all.img");
		HvMakeImage(szExeName, szArgument);

#ifdef WIN32
		DeleteFile("HvUpdatePacket_uboot_all.bin");
#else
		remove("HvUpdatePacket_uboot_all.bin");
#endif
	}
}

static void CreateDm6467SingleUpdatePack(char* szExeName, unsigned char (*keyMAC)[3])
{
	char szArgument[256];

	HvEncryptFile_DSP(DM6467_Single_FileTable[0], "firmware.bin");
	strcpy(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'firmware' -d firmware.bin firmware.img");
	HvMakeImage(szExeName, szArgument);

	HvEncryptFile(DM6467_Single_FileTable[1], "tools.bin");
	strcpy(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'tools' -d tools.bin tools.img");
	HvMakeImage(szExeName, szArgument);

	HvCryptFile(DM6467_Single_FileTable[2], "uImage.bin");
	strcpy(szArgument, "-A arm -O linux -T kernel -C none -a 0x0 -e 0x0 -n 'kernel' -d uImage.bin kernel.img");
	HvMakeImage(szExeName, szArgument);

	HvCryptFile(DM6467_Single_FileTable[3], "rootfs.bin");
	strcpy(szArgument, "-A arm -O linux -T ramdisk -C none -a 0x0 -e 0x0 -n 'rootfs' -d rootfs.bin rootfs.img");
	HvMakeImage(szExeName, szArgument);

	strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'master' -d kernel.img:rootfs.img:tools.img:firmware.img upgrade_6467s.img");
	HvMakeImage(szExeName, szArgument);

#ifdef WIN32
	DeleteFile("firmware.bin");
	DeleteFile("firmware.img");
	DeleteFile("tools.bin");
	DeleteFile("tools.img");
	DeleteFile("uImage.bin");
	DeleteFile("kernel.img");
	DeleteFile("rootfs.bin");
	DeleteFile("rootfs.img");
#else
	remove("firmware.bin");
	remove("firmware.img");
	remove("tools.bin");
	remove("tools.img");
	remove("uImage.bin");
	remove("kernel.img");
	remove("rootfs.bin");
	remove("rootfs.img");
#endif

	if ( 0!=(*keyMAC)[0] && 0!=(*keyMAC)[1] && 0!=(*keyMAC)[2] )
	{
		CryptoByMAC_File("upgrade_6467s.img", "HvUpdatePacket_master.bin", (unsigned char*)keyMAC);
		strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'upgrade' -d HvUpdatePacket_master.bin HvUpdatePacket_6467s.img");
		HvMakeImage(szExeName, szArgument);

#ifdef WIN32
		DeleteFile("HvUpdatePacket_master.bin");
#else
		remove("HvUpdatePacket_master.bin");
#endif
	}
}

static void CreateIPTUpdatePack(char* szExeName, unsigned char (*keyMAC)[3])
{
	char szArgument[256];

	sprintf(szArgument, "-A arm -O linux -T firmware -C none -a 0x0 -e 0x0 -n 'IPT' -d %s IPT.img", IPTFileTable[0]);
	HvMakeImage(szExeName, szArgument);

	if ( 0!=(*keyMAC)[0] && 0!=(*keyMAC)[1] && 0!=(*keyMAC)[2] )
	{
		CryptoByMAC_File("IPT.img", "HvUpdatePacket_IPT.bin", (unsigned char*)keyMAC);
		strcpy(szArgument, "-A arm -O linux -T multi -C none -a 0x0 -e 0x0 -n 'upgrade' -d HvUpdatePacket_IPT.bin HvUpdatePacket_IPT.img");
		HvMakeImage(szExeName, szArgument);

#ifdef WIN32
		DeleteFile("HvUpdatePacket_IPT.bin");
#else
		remove("HvUpdatePacket_IPT.bin");
#endif
	}
}
