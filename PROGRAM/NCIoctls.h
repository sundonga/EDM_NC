// IOCTLS.H -- IOCTL code definitions for fileio driver
// Copyright (C) 1999 by Walter Oney
// All rights reserved

#ifndef NCIOCTLS_H
#define NCIOCTLS_H

#ifndef CTL_CODE
	#pragma message("CTL_CODE undefined. Include winioctl.h or wdm.h")
#endif

//读取机床绝对位置信息
#define NC_POSITION_READ CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x806, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//写取机床绝对位置信息
#define NC_POSITION_WRITE CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x807, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//写译码共享内存
#define NC_CODE_WRITE CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x808, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)



//读取机床速度信息
#define NC_VELOCITY_READ CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x809, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

#define NC_FPGA_READ CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x80A, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

#define NC_FPGA_WRITE CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x80B, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

#define NC_TRANSMIT_EVENT CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x80C, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

#define NC_HPIBOOTLOAD CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x80D, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

#define NC_HPI_DECODE_LOAD CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x80E, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

#define DSP_PARAMETER_WRITE CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x80F, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)
#define DSP_PARAMETER_READ CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x810, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)
#define DSP_TAPEPARAMETER_WRITE CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x811, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)
#define DSP_MEMORY_READ CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x812, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)
/*

//用以建立用户层和驱动间共享内存的指令，采用直接映射方法 METHOD_IN_DIRECT,打开设备注意使其具有读写权限，共享内存区的长度在头文件中以宏的形式声明
#define NC_SHM_MAP CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x802, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//MDI运行方式的实现命令，以直接映射方式实现，METHOD_IN_DIRECT，把内存中含有MDI运行方式代码的内存区以输入参数的形式传入驱动程序，由于代码量可能比较大，故采用了直接映射的方式
#define NC_MDI_RUN CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x803, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//MEMORY运行方式的实现命令，采用在内核模式下直接读取文件的方法进行译码输入，有效地避免了大量数据传输的问题，但需要保持文件的同步，由用户层软件在开始发出命令之前确保所调用的文件处于关闭状态
#define NC_MEMORY_RUN CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x804, \
			METHOD_IN_DIRECT, \
			FILE_ANY_ACCESS)

//机床各轴手动运动指令，输入参数为要发生运动的轴和运动方向，与IO板驱动中手持操作面板读取的指令IO_PORT_READ指令配合使用，由用户层软件负责总体逻辑控制
#define NC_MANUAL_MOTION CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x805, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//机床归零的实现指令，通过同步IRP方式实现，由用户层界面完成等待与查询的逻辑控制
#define NC_MACHINE_HOME CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x806, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//DDA和MANUAL模式转换命令，输入值将被直接写入FPGA
#define NC_DDA_MANUAL CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x807, \
			METHOD_IN_DIRECT, \
			FILE_ANY_ACCESS)

//NC板FPGA运动是能指令，输入值将直接写入FPGA
#define NC_START_STOP CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x808, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//NC板调试信息读取指令，输出值为常用调试信息，如插补信息，中断信息等
#define NC_DEBUG_INFO_READ CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x809, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

#define NC_CODE_WRITE CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x80A, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)
*/
#endif
