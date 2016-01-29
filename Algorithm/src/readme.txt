
编译前必须先运行syncbuild.sh脚本，生成DspBuildNo.h文件。

////////////////////////////////////////////////////////
工程文件说明：

CamApp_DM6467.workspace：相机端(一体机主)

下面为一体机从端工程或单板工程

CapFaceApp_DM6467.workspace：卡口人脸

CoverCtrlApp_DM6467.workspace：断面控制

EPoliceApp_DM6467.workspace：电子警察

HvcApp_DM6467.workspace：分体式收费站(抓拍识别)

TollGateApp_DM6467.workspace：收费站

TrafficGateApp_DM6467.workspace：卡口

DSP端工程文件：

CamDsp_DM6467.pjt：一体机主DSP端工程

HvDsp_DM6467.pjt：一体机从或单板DSP端工程

其它工程：
Loader_DM6467.workspace：loader守护进程工程

HvUpdatePackCreater_PC.workspace：制作升级包程序工程(Linux平台)

/////////////////////////////////////////////////////
工程主函数目录说明：

CamApp：相机端(一体机主)

CamDsp：一体机主DSP端工程

CapFaceApp：卡口人脸

CoverCtrlApp：断面控制

EPoliceApp：电子警察

HvcApp：分体式收费站(抓拍识别)

TollGateApp：收费站

TrafficGateApp：卡口

Loader：loader守护进程

//////////////////////////////////////////////////////////
其它与一体机有关的目录说明：

1、HvTelnet
Telnet模块

2、HvPlatform
与平台相关的模块，工程中分多个虚拟目录。

3、SignalMatch
信号匹配模块

4、HvDspLink
DspLink相关

5、HvPciLink
PCI协议相关

6、ImgGather
获取图像模块，如VPIF采集，创宇协议，等。

7、DevCtrl
设备控制，如上位机传来的命令的执行函数。

8、ResultProcess
结果处理，网络命令连接、图像连接、视频连接、识别结果连接，网络硬盘等等。

9、ImgProcess
图像处理模块

//////////////////////////////////////////////////////////
其它目录说明：

FrontPannelTest：控制板测试上位机程序

HvDeviceDLL：SDK程序

HvDeviceNewTest：SDK测试上位机程序

HvUpdatePackCreater：制作升级包程序，内含VS工程文件及Codeblock工程文件

Inc：头文件目录

OS：u-boot、内核、FPGA、文件系统目录
