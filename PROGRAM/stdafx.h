// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <Commdlg.h>
#include <commctrl.h>
#include <direct.h>
#include <time.h>
#include <io.h>
#include <winuser.h>
#include <winioctl.h>
#include <process.h>
#include <initguid.h>
#include <setupapi.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
//2010-5-7
#include <gl\gl.h>//openGL绘图需要用到的头文件
#include <gl\glu.h>//openGL绘图需要用到的头文件
#include <gl\glaux.h>//openGL绘图需要用到的头文件
#include <gl\glut.h>//openGL绘图需要用到的头文件
//openGL在编译过程中不仅需要添加上诉头文件，同时编译时的dll链接文件也需要进行修改，
//具体操作时属性/连接器/输入/附加依赖项  内加入 opengl32.lib glu32.lib glut32.lib  glaux.lib
//201-5-7

//#include <wdm.h>
//#include <ntddk.h>




// TODO: 在此处引用程序要求的附加头文件
