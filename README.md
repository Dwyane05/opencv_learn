# opencv_learn
this is opencv examples

1_image_read_write 读图片，保存图片
2_video_image_test 读取视频或摄像头  并保存三个图片
3_mat 实现矩阵的操作，图片像素的读取
4_xml_store 实现XML/YAML持久层存储数据，test.yml
5_simple_gui 简单的界面， 移动位置， 创建多个窗口;此程序在arm上不能运行
6_trackBar_mouse 在交互界面上添加滑动条，调整模糊度；创建使用鼠标事件在图像上画绿圈；此程序在arm上不能运行
7_histogram 直方图均衡化，实现了计算并绘制直方图、直方图均衡化、LOMO效果、cartoon效果；main.cpp是qt下的按钮回调，histogram.cpp是直接显示所有窗口
8_object_detect 目标检测：先对图像进行中值滤波（去除椒盐噪音）--然后去除灯光、背景--二值化--使用两种连通区域算法:connectComponents(),connectComponentWithStats() 和 findControus()函数； 最后显示处理后的图像
9_svm_recognition 实现训练SVM模型，并识别对应商品；



