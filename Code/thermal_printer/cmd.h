#ifndef CMD_H
#define CMD_H

/*
 
ESC 2
设置行间距为 32 点 

ESC @
ESC@命令初始化打印机

ESC j n
打印缓冲区数据并走纸 n 点行 

ESC d n
打印缓冲区数据并走纸 n 行

ESC ! n
设置打印字体，n为 0-4

ESC 3 n
设置行间距为 n 点行

ESC v n 
向主机传送打印机状态

ESC a n
设置对齐方式，左对齐0，右对齐2，居中对齐1

ESC - n 
设置下划线的点高度,n为0-2

ESC SP n
设置字符间距n个点

DC2 # n
n 为打印浓度代码:0-F
GS L nL nH

GS v 0 p wL wH hL hH d1…dk 
位图打印 
p: 打印位图格式为0
W=wL+wH*256 表示水平宽度字节数 
H=hL+hH*256 表示垂直高度点数 
位图使用 MSB 格式，最高位在打印位置的左边，先送的数据在打印位置 的左边。

LF
打印行缓冲器里的内容并向前走纸一行

DC2 T
打印测试页

GS V \0 or GS V \1
切纸,打印一行分割示意条 ，=-=-=-=-=-=-=-=-=-=-=-=-=

*/


#endif
