#include"KeyboardFilter.h"

// 入口函数
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	NTSTATUS status;
	ULONG i;

	DbgPrint("我是驱动,我运行了\t\n");

	// 设置分发函数

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriver->MajorFunction[i] = myDispatch;
	}
	// 单独填写一个 IRP_MJ_POWER 函数， 因为重要的过滤就是读取来的按键信息，其他的都不重要
	pDriver->MajorFunction[IRP_MJ_READ]  = myReadDispatch;

	// 单独写一个 IRP_MJ_POWER 函数
	pDriver->MajorFunction[IRP_MJ_POWER] = myPowerDispatch;
	
	// PNP(即插即用) 我们要知道什么时候一个绑定过的设备被卸载（比如从机器上拔掉）
	pDriver->MajorFunction[IRP_MJ_PNP] = myPNPDispatch;
	
	// 卸载函数
	pDriver->DriverUnload = myDriverUnload;
	
	// 绑定所有的键盘设备
	status = kbdAttachDevices(pDriver, pRegPath);
	return status;
}


