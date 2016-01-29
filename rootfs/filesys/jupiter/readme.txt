
========================================
注意事项：
1、请使用root权限执行脚本。
2、如执行.sh脚本提示无执行权限，请使用chmod +x *.sh添加可执行属性。
3、如果.sh脚本提示不能执行，请查看一下sh脚本文件行末是不是以“\r\n”结尾，linux脚本只能是“\n”结尾。如果不是“\n”请转换。

========================================
脚本说明：
大部分脚本同金星平台，请自行参考。
有部分文件与金星平台共用，在本目录脚本中进行。
目前相同的有：根文件系统原始镜像、内核、.net分区中的.net补丁。

========================================
脚本说明：
mkrootfs_jupiter.sh：制作正式根文件系统脚本
mkrootfs_jupiter_back.sh：制作备份根文件系统脚本
mkubi_app.sh：制作app分区镜像脚本
mkubi_app_back.sh: 制作备份app分区镜像脚本
mkubi_net.sh：制作.net分区脚本

========================================
一、制作中间镜像文件(如生产烧写文件、做升级包的输入镜像文件)
用法：
1、制作正式根文件系统命令：
1.1、执行脚本：
./mkrootfs_jupiter.sh
->jupiter_rootfs.img

2、制作备份根文件系统命令：
2.1、将需要放到根文件系统的程序放到backup_sys_app目录(比如：守护进程，xml文件、ipc客户端程序等)。
2.2、执行脚本：
./mkrootfs_jupiter_back.sh
->jupiter_rootfs_back.img

3、制作备份app分区镜像：
3.1、接2.1步
3.2、执行脚本：
./mkubi_app_back.sh
->ubifs_app_back.img
(工厂烧写文件：ubifs_app_bak.factoryuse.img)

4、制作app分区镜像：
4.1、将需要放到app分区的程序放到App目录。
4.2、执行脚本：
./mkubi_app.sh App
->ubifs_app.img

5、制作.net分区镜像：
./mkubi_net.sh

========================================


二、制作升级包文件(只能使用升级工具升级的文件)

关于做升级包（类似于之前的windows下的CreatorGUI.exe）

金星做包工具目前只支持在linux环境下运行，做不同类型的升级包运行不同的做包脚本即可。

mk_jupiter_any.sh       做任意包，所谓任意包即包含的镜像类型任意，镜像个数任意，该脚本需要带镜像所在路径参数
mk_jupiter_app.sh       做APP包，只包含APP镜像，脚本不需要带参数，但是要求是先运行mkubi_app.sh生成app的ubi镜像
mk_jupiter_backsys.sh   做备份包，包含备份kernel和备份rootfs,要求先运行mkrootfs_jupiter_back.sh生成备份文件系统ubi镜像
mk_jupiter_firmware.sh  做固件包，包含正式kernel和正式rootfs,要求先运行mkrootfs_jupiter.sh生成正式文件系统ubi镜像
mk_jupiter_netfw.sh     做.Net包，只包含.net镜像，要求先运行mkubi_net.sh生成.net的ubi镜像
mk_jupiter_uboot.sh     做uboot包，包含uboot和uboot-min


========================================

2014-2-27
更新rootfs包，增加root用户的密码。
root用户密码：Swvenus2014signalway

2014-07-09
将挂载data分区工具smart_mount更新为1.3版本，对未格式化分区强制进行格式化，以保证UBI挂载正常。
去年mount命令-r参数，以适应可读写/只读两种(注：只要APP包是只读的，挂载就是只读的，反之，就是可读写的)。
将根文件系统所有文件权限改为root，防止调试时因权限问题不生成coredump。


2014-07-18
新加木星平台制作脚本，除mk_jupiter_any.sh未修改外未验证外，其它均已修改并自测。
启动脚本已经添加对串口的链接，并去掉FPGA加载。
========================================
