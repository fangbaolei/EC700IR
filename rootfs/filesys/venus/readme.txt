
========================================
注意事项：
1、请使用root权限执行脚本。
2、如.sh脚本不能执行，请使用chmod +x *.sh添加可执行属性。

========================================
脚本说明：
mkrootfs_venus.sh：制作正式根文件系统脚本
mkrootfs_venus_back.sh：制作备份根文件系统脚本
mkubi_app.sh：制作app分区镜像脚本
mkubi_net.sh：制作.net分区脚本

用法：
1、制作正式根文件系统命令：
1.1、执行脚本：
mkrootfs_venus.sh
->venus_rootfs.img

2、制作备份根文件系统命令：
2.1、将需要放到根文件系统的程序放到backup_sys_app目录(比如：守护进程，xml文件、ipc客户端程序等)。
2.2、执行脚本：
mkrootfs_venus_back.sh
->venus_rootfs_back.img

3、制作备份app分区镜像：
3.1、接2.1步
3.2、执行脚本：
mkubi_app_back.sh
->ubifs_app_back.img
(工厂烧写文件：ubifs_app_bak.factoryuse.img)

4、制作app分区镜像：
4.1、将需要放到app分区的程序放到App目录。
4.2、执行脚本：
mkubi_app.sh App
->ubifs_app.img

5、制作.net分区镜像：
mkubi_net.sh net_patch




========================================
2013-12-18：
因为当前不确定有哪些程序，故所有制作脚本没有拷贝应用程序(在脚本中注释掉)。


========================================
关于做升级包（类似于之前的windows 下的CreatorGUI.exe）

金星做包工具目前只支持在linux环境下运行，做不同类型的升级包运行不同的做包脚本即可。

mk_venus_any.sh 	做任意包，所谓任意包即包含的镜像类型任意，镜像个数任意，该脚本需要带镜像所在路径参数
mk_venus_app.sh 	做APP包，只包含APP镜像，脚本不需要带参数，但是要求是先运行mkubi_app.sh生成app的ubi镜像
mk_venus_backsys.sh 	做备份包，包含备份kernel和备份rootfs,要求先运行mkrootfs_venus_back.sh生成备份文件系统ubi镜像
mk_venus_firmware.sh 	做固件包，包含正式kernel和正式rootfs,要求先运行mkrootfs_venus.sh生成正式文件系统ubi镜像
mk_venus_netfw.sh 	做.Net包，只包含.net镜像，要求先运行mkubi_net.sh生成.net的ubi镜像
mk_venus_uboot.sh 	做uboot包，包含uboot和uboot-min


============================
2014-2-27
============================
更新rootfs包，增加root用户的密码。
root用户密码：Swvenus2014signalway


============================
2014-07-09
将挂载data分区工具smart_mount更新为1.3版本，对未格式化分区强制进行格式化，以保证UBI挂载正常。(注：生产部门测试时发现有此问题，故改正)
去掉mount命令-r参数，以适应可读写/只读两种(注：只要APP包是只读的，挂载就是只读的，反之，就是可读写的)。
将根文件系统所有文件权限改为root，防止调试时因权限问题不生成coredump。
(注：2014-07-18提交svn)
============================

2015-06-23
修正rootfs中关于时间戳文件判断的问题。具体表现为上位机设置时间小于2012年11月13日则会强制从该日期开始计时。
添加iperf vnstat tcpdump工具
