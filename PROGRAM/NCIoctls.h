// IOCTLS.H -- IOCTL code definitions for fileio driver
// Copyright (C) 1999 by Walter Oney
// All rights reserved

#ifndef NCIOCTLS_H
#define NCIOCTLS_H

#ifndef CTL_CODE
	#pragma message("CTL_CODE undefined. Include winioctl.h or wdm.h")
#endif

//��ȡ��������λ����Ϣ
#define NC_POSITION_READ CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x806, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//дȡ��������λ����Ϣ
#define NC_POSITION_WRITE CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x807, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//д���빲���ڴ�
#define NC_CODE_WRITE CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x808, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)



//��ȡ�����ٶ���Ϣ
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

//���Խ����û���������乲���ڴ��ָ�����ֱ��ӳ�䷽�� METHOD_IN_DIRECT,���豸ע��ʹ����ж�дȨ�ޣ������ڴ����ĳ�����ͷ�ļ����Ժ����ʽ����
#define NC_SHM_MAP CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x802, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//MDI���з�ʽ��ʵ�������ֱ��ӳ�䷽ʽʵ�֣�METHOD_IN_DIRECT�����ڴ��к���MDI���з�ʽ������ڴ����������������ʽ���������������ڴ��������ܱȽϴ󣬹ʲ�����ֱ��ӳ��ķ�ʽ
#define NC_MDI_RUN CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x803, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//MEMORY���з�ʽ��ʵ������������ں�ģʽ��ֱ�Ӷ�ȡ�ļ��ķ��������������룬��Ч�ر����˴������ݴ�������⣬����Ҫ�����ļ���ͬ�������û�������ڿ�ʼ��������֮ǰȷ�������õ��ļ����ڹر�״̬
#define NC_MEMORY_RUN CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x804, \
			METHOD_IN_DIRECT, \
			FILE_ANY_ACCESS)

//���������ֶ��˶�ָ��������ΪҪ�����˶�������˶�������IO���������ֲֳ�������ȡ��ָ��IO_PORT_READָ�����ʹ�ã����û���������������߼�����
#define NC_MANUAL_MOTION CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x805, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//���������ʵ��ָ�ͨ��ͬ��IRP��ʽʵ�֣����û��������ɵȴ����ѯ���߼�����
#define NC_MACHINE_HOME CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x806, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//DDA��MANUALģʽת���������ֵ����ֱ��д��FPGA
#define NC_DDA_MANUAL CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x807, \
			METHOD_IN_DIRECT, \
			FILE_ANY_ACCESS)

//NC��FPGA�˶�����ָ�����ֵ��ֱ��д��FPGA
#define NC_START_STOP CTL_CODE(\
			FILE_DEVICE_UNKNOWN, \
			0x808, \
			METHOD_BUFFERED, \
			FILE_ANY_ACCESS)

//NC�������Ϣ��ȡָ����ֵΪ���õ�����Ϣ����岹��Ϣ���ж���Ϣ��
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
