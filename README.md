# Nano Light 星野赤道仪

Nano Light是一个起源于神奇的bilibili的开源DIY赤道仪项目，戳 [这里](https://www.bilibili.com/video/BV1gi4y1R7HK?spm_id_from=333.337.search-card.all.click) 了解更多。 

欢迎加入QQ交流群： 681038889。

本仓库为NanoLight二代所需的代码，包括烧录到控制板的代码以及用于手机控制的网页。

## 硬件部分

硬件部分分为用于3D打印的模型以及主板PCB。

### 3D打印模型

进群获取。 （TBD @www）

### 控制板PCB

https://oshwhub.com/Flanker-E/Nano-Light-xing-ye-chi-dao-yi#P1

（TBD @flanker-e）

## 软件部分

### 说明

本项目分为两个部分，分别为烧录进esp32的server以及用于显示的网页，分别在`nano_core`和`nano_view`文件夹中。 

以下部分会按照顺序说明安装顺序烧录方法。

### nano view

用于手机控制的网页，使用react框架进行开发； 依赖node以及npm。

以下安装方式二选一，如无修改必要，推荐第二种方案。

* 手动构建
    ``` bash
    # 安装依赖
    # 进入项目目录
    cd nano_view
    # build
    npm run build
    ```
    build的过程会自动将生成的文件拷贝到`/nano_core/data`中。
* 使用生成好的包 【推荐】
    下载打包好的页面，手动解压到`/nano_core/data`中。

### nano core

本部分包含对赤道仪电机的控制以及控制网页的server； 以下步骤需要在完成`nano view`部分之后进行。

1. 环境安装
    1. 下载安装 vs code 参考： https://code.visualstudio.com/
    2. 安装PlatformIO， 参考： https://platformio.org/install/ide?install=vscode
2. 打开工程
3. build
4. upload
5. build file system
6. upload file system


## 开源协议

所有文件遵循CC BY-SA协议公开。

