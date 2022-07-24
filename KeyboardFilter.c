#include "KeyboardFilter.h"

ULONG gC2pKeyCount = 0;



// ����һ����ǣ��������浱ǰ���̵�״̬����2����λ���ֱ��ʾ Caps Lock ����Shift ���Ƿ�����
// ������һ���������
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


	if ((sch & 0x80) == 0)  //����ǰ���
	{

		switch (sch)
		{
			//Caps Lock ���ǡ��������Ρ�����û����һ���ġ���������
			//������������������ñ�־��Ҳ����˵����һ�������ã��ٰ�һ�ξͲ���������
		case 0x3A:
			kb_status.F_CAPS ^= 1;
			break;
			// 0x2a����ߵ�Shift��0x36���ұߵ�Shift��
		case 0x2a:
		case 0x36:
			// �ɿ�SHIFT�����ʧЧ��			
			kb_status.F_SHIFT = 1;			
			break;
		default:
			break;
		}

		//�����������ĸ�������ֵȿɼ��ַ�
		if ((sch < 0x47) || ((sch >= 0x47 && sch < 0x54)))
		{
			//���յõ���һ���ַ���Caps Lock ��Shift�⼸������״̬������

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

	// ��֪����δ�����ʲô��,����Ҳû˵����δ���Ҳû�б�����
	/*else {
		if (sch == 0xAA || sch == 0xB6)
			// DbgPrint("sch:%x\t\n", sch);
			kb_status.F_SHIFT &= ~S_SHIFT;
	}*/

	if (ch >= 0x20 && ch < 0x7F)
	{
		DbgPrint(">>����Ϊ: [%c]\t\n", ch);
		// ��ӡ֮�� ���� Shift ��Ϊ�ɿ�״̬
		kb_status.F_SHIFT = 0;
	}
}


// ����󶨲�ɾ���豸
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

// ж�غ���
VOID myDriverUnload(PDRIVER_OBJECT pDriver) {
	PDEVICE_OBJECT pDeviceObject;
	PDEVICE_OBJECT OldDeviceObject;
	PDEV_EXT pdevext;

	LARGE_INTEGER lDelay;
	PRKTHREAD CurrentThread;

	lDelay = RtlConvertLongToLargeInteger(100 * DELAY_ONE_MILLISECOND);
	CurrentThread = KeGetCurrentThread();

	// �ѵ�ǰ�߳�����Ϊ��ʵʱģʽ��һ�����������о�����Ӱ����������
	KeSetPriorityThread(CurrentThread, LOW_REALTIME_PRIORITY);
	
	// UNREFERENCED_PARAMETER(pDriver);

	DbgPrint("DriverEntry unload..\t\n.");

	// ���������豸��һ�ɽ����
	pDeviceObject = pDriver->DeviceObject;
	while (pDeviceObject)
	{
		// ����󶨲�ɾ���豸
		c2pDetach(pDeviceObject);
		pDeviceObject = pDeviceObject->NextDevice;
	}

	ASSERT(NULL == DriverObject->DeviceObject);

	while (gC2pKeyCount)
	{
		// ˯�� 1����
		KeDelayExecutionThread(KernelMode, FALSE, &lDelay);
	}
	DbgPrint("������������ж����\t\n");
	return;
}

// ��ͨ�ַ�����
NTSTATUS myDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
)
{
	//	�����ķַ�������ֱ��skip��Ȼ����IoCallDriver ��IRP���͵���ʵ�豸���豸������
	DbgPrint("Other Disapatch!\t\n");
	IoSkipCurrentIrpStackLocation(irp);
	return IoCallDriver(((PDEV_EXT)pDevice->DeviceExtension)->pLowerDeviceObject, irp);

};


// ��Դ�������
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
	
	// ��ȡIRP ��ջ
	pIrpStack = IoGetCurrentIrpStackLocation(irp);
	// �ж����IRP�Ƿ�ɹ�
	if (NT_SUCCESS(irp->IoStatus.Status))
	{
		// ��ȡ��������ɺ�����������
		pKeyBoardData = irp->AssociatedIrp.SystemBuffer;
		// irp->IoStatus.Information��ʵ���������Ϣ�Ĵ�С���õ��˰��µļ�������
		keys = (irp->IoStatus.Information) / sizeof(KEYBOARD_INPUT_DATA);
		for (size_t i = 0; i < keys; i++)
		{
			// �����벻�����ַ�
			DbgPrint("->������Ϊ:[%u]\tMakeCode: [%X]\tFlags: [%X]\t\n", keys, pKeyBoardData[i].MakeCode, pKeyBoardData[i].Flags);
			
			//if (pKeyBoardData->MakeCode == 0x1f)
			//{
			//	// ���˰������޸�Ϊ0x20 
			//	// ����Ҳ������������������飺���̼�¼�ȵ�
			//	pKeyBoardData->MakeCode = 0x20;
			//}

			// ��ӡ������Ϣ
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

// ������
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

	// �ж��Ƿ񵽴���irpջ����Ͷ�
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

	// ȫ�ֱ�������������1
	gC2pKeyCount++;

	// �õ��豸��չ��Ŀ����Ϊ�˻����һ���豸��ָ��
	pdevext = (PDEV_EXT)pDevice->DeviceExtension;

	// ���ûص���������IRP������ȥ��֮����Ĵ���Ҳ�ͽ����ˡ�ʣ�µ������ǵȴ����������
	pIrpStack = IoGetCurrentIrpStackLocation(irp);
	// ���Ƶ�ǰIRPջ�ռ�
	IoCopyCurrentIrpStackLocationToNext(irp);
	// ������ɻص�����
	IoSetCompletionRoutine(
		irp,
		myReadComplete,
		pDevice, TRUE, TRUE, TRUE);

	return IoCallDriver(pdevext->pLowerDeviceObject, irp);
}

// PNP ����
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

	// ��ȡ����ʵ�豸
	pdevext = (PDEV_EXT)(pDevice->DeviceExtension);
	pIrpStack = IoGetCurrentIrpStackLocation(irp);

	// ��ȡ�ι��ܺ�
	switch (pIrpStack->MinorFunction)
	{
	case IRP_MN_REMOVE_DEVICE:
		DbgPrint("IRP_MN_REMOVE_DEVICE\t\n");
		// ���Ȱ�������ȥ
		IoSkipCurrentIrpStackLocation(irp);
		IoCallDriver(pdevext->pLowerDeviceObject, irp);
		// Ȼ��Ӵ���
		IoDetachDevice(pdevext->pLowerDeviceObject);
		// ɾ���������ɵ������豸
		IoDeleteDevice(pDevice);
		status = STATUS_SUCCESS;
		break;


	default:
		// �����������͵�IRP��ȫ��ֱ���·�����
		IoSkipCurrentIrpStackLocation(irp);
		status = IoCallDriver(pdevext->pLowerDeviceObject, irp);
		break;
	}

	return status;
}

// ��ʼ���豸��չ
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


// ���豸
// ��ȡ KbdClassd ����������Ȼ����������豸����
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

	// ��ȡ KbdClass ��������

	RtlInitUnicodeString(&uniNtNameString, KBD_DRIVER_NAME);
	status = ObReferenceObjectByName(
		&uniNtNameString,
		OBJ_CASE_INSENSITIVE, NULL, 0,
		*IoDriverObjectType,		// ��xpԴ���п�������������
		KernelMode,
		NULL, &KbdDriverObject
	);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("MyAttach:Couldn't get the [KbdClass DriverObject]\t\n");
		return status;
	}
	// ������
	ObDereferenceObject(KbdDriverObject);

	// ��ȡ KbdClass ���������µ������豸���󲢰�

	// �豸���е�һ���豸
	pTargetDeviceObject = KbdDriverObject->DeviceObject;

	// �����豸��
	while (pTargetDeviceObject)
	{
		// ��������һ�������豸
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

		// ��
		// pLowerDeviceObject ��Ŀ���豸 �豸ջջ�����Ǹ��豸��
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

		// ���� �豸��չ
		pdevext = (PDEV_EXT)(pFilterDeviceObject->DeviceExtension);
		// ��ʼ���豸��չ
		DevExtInit(pdevext, pFilterDeviceObject, pTargetDeviceObject, pLowerDeviceObject);

		// �����������
		pFilterDeviceObject->DeviceType = pLowerDeviceObject->DeviceType;
		pFilterDeviceObject->Characteristics = pLowerDeviceObject->Characteristics;
		pFilterDeviceObject->StackSize = pLowerDeviceObject->StackSize+1;
		pFilterDeviceObject->Flags |= pLowerDeviceObject->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO | DO_POWER_PAGABLE);

		// ��ȡ��һ���豸����
		pTargetDeviceObject = pTargetDeviceObject->NextDevice;
	}
	return status;
}