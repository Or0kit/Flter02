#include "KeyboardFilter.h"

ULONG gC2pKeyCount = 0;



// 这是一个标记，用来保存当前键盘的状态。低2比特位，分别表示 Caps Lock 键和Shift 键是否按下了
// 这里有一个问题如果
static KB_STATUS kb_status = { 0 };

unsigned char asciiTbl[] = {
	0x00, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x08, 0x09, //normal  
	0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69, 0x6F, 0x70, 0x5B, 0x5D, 0x0D, 0x00, 0x61, 0x73,
	0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B, 0x27, 0x60, 0x00, 0x5C, 0x7A, 0x78, 0x63, 0x76,
	0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F, 0x00, 0x2A, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x38, 0x39, 0x2D, 0x34, 0x35, 0x36, 0x2B, 0x31,
	0x32, 0x33, 0x30, 0x2E,
	0x00, 0x1B, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x08, 0x09, //caps  
	0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49, 0x4F, 0x50, 0x5B, 0x5D, 0x0D, 0x00, 0x41, 0x53,
	0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3B, 0x27, 0x60, 0x00, 0x5C, 0x5A, 0x58, 0x43, 0x56,
	0x42, 0x4E, 0x4D, 0x2C, 0x2E, 0x2F, 0x00, 0x2A, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x38, 0x39, 0x2D, 0x34, 0x35, 0x36, 0x2B, 0x31,
	0x32, 0x33, 0x30, 0x2E,
	0x00, 0x1B, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26, 0x2A, 0x28, 0x29, 0x5F, 0x2B, 0x08, 0x09, //shift  
	0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49, 0x4F, 0x50, 0x7B, 0x7D, 0x0D, 0x00, 0x41, 0x53,
	0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3A, 0x22, 0x7E, 0x00, 0x7C, 0x5A, 0x58, 0x43, 0x56,
	0x42, 0x4E, 0x4D, 0x3C, 0x3E, 0x3F, 0x00, 0x2A, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x38, 0x39, 0x2D, 0x34, 0x35, 0x36, 0x2B, 0x31,
	0x32, 0x33, 0x30, 0x2E,
	0x00, 0x1B, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26, 0x2A, 0x28, 0x29, 0x5F, 0x2B, 0x08, 0x09, //caps + shift  
	0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69, 0x6F, 0x70, 0x7B, 0x7D, 0x0D, 0x00, 0x61, 0x73,
	0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3A, 0x22, 0x7E, 0x00, 0x7C, 0x7A, 0x78, 0x63, 0x76,
	0x62, 0x6E, 0x6D, 0x3C, 0x3E, 0x3F, 0x00, 0x2A, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x38, 0x39, 0x2D, 0x34, 0x35, 0x36, 0x2B, 0x31,
	0x32, 0x33, 0x30, 0x2E
};


void __stdcall print_keystroke(UCHAR sch)
{
	UCHAR ch = 0;
	int off = 0;


	if ((sch & 0x80) == 0)  //如果是按下
	{

		switch (sch)
		{
			//Caps Lock 键是“按下两次”等于没按过一样的“反复键”
			//所以这里用异或来设置标志。也就是说，按一次起作用，再按一次就不起作用了
		case 0x3A:
			kb_status.F_CAPS ^= 1;
			break;
			// 0x2a是左边的Shift，0x36是右边的Shift。
		case 0x2a:
		case 0x36:
			// 松开SHIFT键后就失效了			
			kb_status.F_SHIFT = 1;			
			break;
		default:
			break;
		}

		//如果按下了字母或者数字等可见字符
		if ((sch < 0x47) || ((sch >= 0x47 && sch < 0x54)))
		{
			//最终得到哪一个字符由Caps Lock 和Shift这几个键的状态来决定

			if ((kb_status.F_CAPS ) && (!(kb_status.F_SHIFT )))
				off = 0x54;
			if ((!(kb_status.F_CAPS)) && (kb_status.F_SHIFT))
				off = 0xA8;
			if ((kb_status.F_CAPS) && ((kb_status.F_SHIFT)))
				off = 0xFC;
			// DbgPrint("off:[%X]\t\n", off);
			ch = asciiTbl[off + sch];
		}
		
	}

	// 不知道这段代码有什么用,书上也没说。这段代码也没有被触发
	/*else {
		if (sch == 0xAA || sch == 0xB6)
			// DbgPrint("sch:%x\t\n", sch);
			kb_status.F_SHIFT &= ~S_SHIFT;
	}*/

	if (ch >= 0x20 && ch < 0x7F)
	{
		DbgPrint(">>按键为: [%c]\t\n", ch);
		// 打印之后 设置 Shift 键为松开状态
		kb_status.F_SHIFT = 0;
	}
}


// 解除绑定并删除设备
VOID c2pDetach(IN PDEVICE_OBJECT pDeviceObject)
{
	PDEV_EXT devExt;
	BOOLEAN NoRequestsOutstanding = FALSE;
	devExt = (PDEV_EXT)pDeviceObject->DeviceExtension;
	__try
	{
		__try
		{
			IoDetachDevice(devExt->pTargetDeviceObject);
			devExt->pTargetDeviceObject = NULL;
			IoDeleteDevice(pDeviceObject);
			devExt->pFilterDeviceObject = NULL;
			DbgPrint(("Detach Finished\n"));
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}
	}
	__finally {}
	return;
}

// 卸载函数
VOID myDriverUnload(PDRIVER_OBJECT pDriver) {
	PDEVICE_OBJECT pDeviceObject;
	PDEVICE_OBJECT OldDeviceObject;
	PDEV_EXT pdevext;

	LARGE_INTEGER lDelay;
	PRKTHREAD CurrentThread;

	lDelay = RtlConvertLongToLargeInteger(100 * DELAY_ONE_MILLISECOND);
	CurrentThread = KeGetCurrentThread();

	// 把当前线程设置为低实时模式，一遍让它的运行尽量少影响其他程序
	KeSetPriorityThread(CurrentThread, LOW_REALTIME_PRIORITY);
	
	// UNREFERENCED_PARAMETER(pDriver);

	DbgPrint("DriverEntry unload..\t\n.");

	// 遍历所有设备并一律解除绑定
	pDeviceObject = pDriver->DeviceObject;
	while (pDeviceObject)
	{
		// 解除绑定并删除设备
		c2pDetach(pDeviceObject);
		pDeviceObject = pDeviceObject->NextDevice;
	}

	ASSERT(NULL == DriverObject->DeviceObject);

	while (gC2pKeyCount)
	{
		// 睡眠 1毫秒
		KeDelayExecutionThread(KernelMode, FALSE, &lDelay);
	}
	DbgPrint("我是驱动，我卸载了\t\n");
	return;
}

// 普通分发函数
NTSTATUS myDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
)
{
	//	其他的分发函数，直接skip，然后用IoCallDriver 把IRP发送到真实设备的设备对象上
	DbgPrint("Other Disapatch!\t\n");
	IoSkipCurrentIrpStackLocation(irp);
	return IoCallDriver(((PDEV_EXT)pDevice->DeviceExtension)->pLowerDeviceObject, irp);

};


// 电源相关请求
NTSTATUS myPowerDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
)
{
	PDEV_EXT pdevext;
	pdevext = (PDEV_EXT)(pDevice->DeviceExtension);

	PoStartNextPowerIrp(irp);
	IoSkipCurrentIrpStackLocation(irp);
	return PoCallDriver(pdevext->pLowerDeviceObject, irp);

}

NTSTATUS myReadComplete(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp,
	IN PVOID Context
)
{
	NTSTATUS status;
	PIO_STACK_LOCATION pIrpStack;
	ULONG  keys;
	ULONG buf_len = 0;
	PKEYBOARD_INPUT_DATA pKeyBoardData;
	
	// 获取IRP 堆栈
	pIrpStack = IoGetCurrentIrpStackLocation(irp);
	// 判断这个IRP是否成功
	if (NT_SUCCESS(irp->IoStatus.Status))
	{
		// 获取读请求完成后的输出缓冲区
		pKeyBoardData = irp->AssociatedIrp.SystemBuffer;
		// irp->IoStatus.Information其实就是完成信息的大小。得到了按下的键的数量
		keys = (irp->IoStatus.Information) / sizeof(KEYBOARD_INPUT_DATA);
		for (size_t i = 0; i < keys; i++)
		{
			// 按键码不等于字符
			DbgPrint("->按键数为:[%u]\tMakeCode: [%X]\tFlags: [%X]\t\n", keys, pKeyBoardData[i].MakeCode, pKeyBoardData[i].Flags);
			
			//if (pKeyBoardData->MakeCode == 0x1f)
			//{
			//	// 过滤按键，修改为0x20 
			//	// 这里也可以做其他的许多事情：键盘记录等等
			//	pKeyBoardData->MakeCode = 0x20;
			//}

			// 打印按键信息
			if (pKeyBoardData[i].Flags == KEY_MAKE)
				print_keystroke((UCHAR)pKeyBoardData[i].MakeCode);
			
			
		}
	}
	gC2pKeyCount--;

	if (irp->PendingReturned)
	{
		IoMarkIrpPending(irp);
	}

	return irp->IoStatus.Status;
}

// 读请求
NTSTATUS myReadDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
) 
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEV_EXT pdevext;
	PIO_STACK_LOCATION pIrpStack;
	KEVENT waitevent;

	
	KeInitializeEvent(&waitevent, NotificationEvent, FALSE);

	// 判断是否到达了irp栈的最低端
	if (irp->CurrentLocation == 1)
	{
		ULONG ReturnedInformation = 0;
		DbgPrint("Dispatach encountered bogus current location\t\n");
		status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Status = status;
		irp->IoStatus.Information = ReturnedInformation;
		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return status;
	}

	// 全局变量键计数器加1
	gC2pKeyCount++;

	// 得到设备扩展，目的是为了获得下一个设备的指针
	pdevext = (PDEV_EXT)pDevice->DeviceExtension;

	// 设置回调函数并把IRP传递下去。之后读的处理也就结束了。剩下的任务是等待读请求完成
	pIrpStack = IoGetCurrentIrpStackLocation(irp);
	// 复制当前IRP栈空间
	IoCopyCurrentIrpStackLocationToNext(irp);
	// 设置完成回调函数
	IoSetCompletionRoutine(
		irp,
		myReadComplete,
		pDevice, TRUE, TRUE, TRUE);

	return IoCallDriver(pdevext->pLowerDeviceObject, irp);
}

// PNP 处理
NTSTATUS myPNPDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
)
{
	PDEV_EXT pdevext;
	PIO_STACK_LOCATION pIrpStack;
	NTSTATUS status = STATUS_SUCCESS;
	KIRQL oldIrql;
	KEVENT event;

	// 获取这真实设备
	pdevext = (PDEV_EXT)(pDevice->DeviceExtension);
	pIrpStack = IoGetCurrentIrpStackLocation(irp);

	// 获取次功能号
	switch (pIrpStack->MinorFunction)
	{
	case IRP_MN_REMOVE_DEVICE:
		DbgPrint("IRP_MN_REMOVE_DEVICE\t\n");
		// 首先把请求发下去
		IoSkipCurrentIrpStackLocation(irp);
		IoCallDriver(pdevext->pLowerDeviceObject, irp);
		// 然后接触绑定
		IoDetachDevice(pdevext->pLowerDeviceObject);
		// 删除我们生成的虚拟设备
		IoDeleteDevice(pDevice);
		status = STATUS_SUCCESS;
		break;


	default:
		// 对于其他类型的IRP，全部直接下发即可
		IoSkipCurrentIrpStackLocation(irp);
		status = IoCallDriver(pdevext->pLowerDeviceObject, irp);
		break;
	}

	return status;
}

// 初始化设备扩展
void DevExtInit(
	IN PDEV_EXT pdevext,
	IN PDEVICE_OBJECT pFilterDeviceObject,
	IN PDEVICE_OBJECT pTargetDeviceObject,
	IN PDEVICE_OBJECT pLowerDeviceObject
)
{
	RtlZeroMemory(pdevext, sizeof(DEV_EXT));
	pdevext->ulSize = sizeof(DEV_EXT);
	pdevext->pFilterDeviceObject = pFilterDeviceObject;
	pdevext->pTargetDeviceObject = pTargetDeviceObject;
	pdevext->pLowerDeviceObject = pLowerDeviceObject;
	KeInitializeSpinLock(&(pdevext->IoRequestsSpinLock));
	KeInitializeEvent(&(pdevext->IoInProgressEvent), NotificationEvent, FALSE);
}


// 绑定设备
// 获取 KbdClassd 的驱动对象，然后绑定其所有设备对象
NTSTATUS kbdAttachDevices(
	IN PDRIVER_OBJECT pDriver,
	IN PUNICODE_STRING pRegPath
)
{
	NTSTATUS status = 0;
	UNICODE_STRING uniNtNameString;
	PDEV_EXT pdevext;
	PDEVICE_OBJECT pFilterDeviceObject = NULL;
	PDEVICE_OBJECT pTargetDeviceObject = NULL;
	PDEVICE_OBJECT pLowerDeviceObject = NULL;

	PDRIVER_OBJECT KbdDriverObject = NULL;
	DbgPrint("MyAttach!\t\n");

	// 获取 KbdClass 驱动对象

	RtlInitUnicodeString(&uniNtNameString, KBD_DRIVER_NAME);
	status = ObReferenceObjectByName(
		&uniNtNameString,
		OBJ_CASE_INSENSITIVE, NULL, 0,
		*IoDriverObjectType,		// 在xp源码中看到就是这样的
		KernelMode,
		NULL, &KbdDriverObject
	);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("MyAttach:Couldn't get the [KbdClass DriverObject]\t\n");
		return status;
	}
	// 解引用
	ObDereferenceObject(KbdDriverObject);

	// 获取 KbdClass 驱动对象下的所有设备对象并绑定

	// 设备链中第一个设备
	pTargetDeviceObject = KbdDriverObject->DeviceObject;

	// 遍历设备链
	while (pTargetDeviceObject)
	{
		// 首先生成一个过滤设备
		status = IoCreateDevice(
			IN pDriver,
			IN sizeof(DEV_EXT),
			IN NULL,
			IN pTargetDeviceObject->DeviceType,
			IN pTargetDeviceObject->Characteristics,
			IN FALSE,
			OUT & pFilterDeviceObject
		);

		if (!NT_SUCCESS(status))
		{
			DbgPrint("MyAttach:Couldn't Create Filter Device Object\t\n");
			pFilterDeviceObject = NULL;
			pTargetDeviceObject = NULL;
			return status;
		}

		// 绑定
		// pLowerDeviceObject 是目标设备 设备栈栈顶的那个设备！
		status = IoAttachDeviceToDeviceStackSafe(
			pFilterDeviceObject,
			pTargetDeviceObject,
			&pLowerDeviceObject
		);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("MyAtach:Couldn't attach KbdClass Device Object\t\n");
			IoDeleteDevice(pFilterDeviceObject);
			pFilterDeviceObject = NULL;
			return status;
		}

		// 设置 设备扩展
		pdevext = (PDEV_EXT)(pFilterDeviceObject->DeviceExtension);
		// 初始化设备扩展
		DevExtInit(pdevext, pFilterDeviceObject, pTargetDeviceObject, pLowerDeviceObject);

		// 设置相关属性
		pFilterDeviceObject->DeviceType = pLowerDeviceObject->DeviceType;
		pFilterDeviceObject->Characteristics = pLowerDeviceObject->Characteristics;
		pFilterDeviceObject->StackSize = pLowerDeviceObject->StackSize+1;
		pFilterDeviceObject->Flags |= pLowerDeviceObject->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);

		// 获取下一个设备对象
		pTargetDeviceObject = pTargetDeviceObject->NextDevice;
	}
	return status;
}