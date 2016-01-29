金星编译、做包步骤(以下操作都在源代码根目录SW_LPR_Main中进行)：

1.编译
make all
注：直接运行make可以查看帮助

2.做包
./copybin.sh
注：做包是区分方案的，通过copybin.sh中的选项可做不同的方案的升级包


生成的升级包位于SW_LPR_Main/rootfs/filesys/venus/XXX.img和代码根目录XXX.img，可直接使用升级工具HvUpgrade升级.

注：XXX.img 是当前选择的升级包名字，且名字符合公司的命名规范，例如：App_PCC200一体机_3.1.0.2479_视频流收费站.img