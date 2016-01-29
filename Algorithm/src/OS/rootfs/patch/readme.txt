目录说明：

App：应用程序存放目录(默认为空目录)，制作调试版文件系统需要将下列程序放到此目录下：

loader：守护进程(网络启动模式下仅用于一体机加载fpga，正式版本才起守护进程作用)

相机端：
CamApp：ARM端应用程序
CamDsp.out：DSP端程序

识别端(单板)：
LprApp：ARM端应用程序(此文件由不同分型应用程序改名而成)
HvDsp.out：DSP端程序

fpga.xsvf：fpga文件

制作正式版文件系统只需将loader添加到App目录下。

###############################################################################

Script：脚本及驱动存放目录(无需改动)

脚本：
dm.sh:磁盘管理脚本
run.sh：运行程序脚本文件
reloadko.sh：正式版本重新加载驱动脚本(网络启动未使用)
loadko_pci_master.sh：加载相机端PCI驱动脚本
loadko_pci_slave_200w.sh：加载识别端PCI驱动脚本(200万像素一体机)
loadko_pci_slave_500w.sh：加载识别端PCI驱动脚本(500万像素一体机)

驱动及配置文件：
slw-dev_200w.ko：200万一体机SW-DEV驱动文件
slw-dev_500w.ko：500万一体机SW-DEV驱动文件
slw-dev_single.ko：单板SW-DEV驱动文件
slw-pci.master.ko：一体机主PCI驱动文件(不区分200万、500万)
slw-pci.slave.ko：一体机从PCI驱动文件(不区分200万、500万)
rc.local：单板设置网口脚本
S99hdc：系统启动脚本

(注：由于相机端和识别端加载的PCI驱动不同，这里分不同的脚本文件，但在根文件系统中，统一修改文件名为loadko.sh，此操作由制作文件系统脚本自动完成，sw-dev驱动文件亦类似。)