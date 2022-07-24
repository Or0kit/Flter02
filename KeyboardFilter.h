#pragma once
#include<ntddk.h>
#include<ntddkbd.h>


// ��ȡ����ȫ�ֱ�������xp ��Դ���о���������ȡ�ģ����ϵĲ���
extern POBJECT_TYPE* IoDriverObjectType;

// kbdClass ����������
#define KBD_DRIVER_NAME L"\\Driver\\Kbdclass"

// ʱ�䶨�� 1000*1000*1000���� = 1000*1000΢�� = 1000���� = 1��
#define  DELAY_ONE_MICROSECOND  (-10)	// �ں���CPU�Ļ�׼��λʱ����100���� ����10����1΢��
#define  DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND*1000)
#define  DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND*1000)

// ����δ��������
NTSTATUS
ObReferenceObjectByName(
	IN PUNICODE_STRING ObjectName,
	IN ULONG Attributes,
	IN PACCESS_STATE AccessState OPTIONAL,
	IN ACCESS_MASK DesiredAccess OPTIONAL,
	IN POBJECT_TYPE ObjectType,
	IN KPROCESSOR_MODE AccessMode,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID* Object
);


// λ��

typedef struct _KB_STATUS {
	// ��ʱֻ��ʶ Shift �� CapsLock ����
	UCHAR F_SHIFT : 1;
	UCHAR F_CAPS : 1;
}KB_STATUS, * PKB_STATUS;


// �豸��չ
typedef struct _DEV_EXT
{
	// �ṹ���С
	ULONG ulSize;
	// �����豸����
	PDEVICE_OBJECT pFilterDeviceObject;
	// �󶨵�Ŀ���豸����
	PDEVICE_OBJECT pTargetDeviceObject;
	// ���ض���
	PDEVICE_OBJECT pLowerDeviceObject;
	// ������
	KSPIN_LOCK IoRequestsSpinLock;
	// �¼�
	KEVENT IoInProgressEvent;
}DEV_EXT, * PDEV_EXT;


// ����󶨲�ɾ���豸
VOID c2pDetach(IN PDEVICE_OBJECT pDeviceObject);

// ж�غ���
VOID myDriverUnload(PDRIVER_OBJECT pDriver);

// �ַ�����

// ��ͨ�ַ�����
NTSTATUS myDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
);

// ��Դ�������
NTSTATUS myPowerDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
);

// �����������
NTSTATUS myReadComplete(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp,
	IN PVOID Context
);

// ������
NTSTATUS myReadDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
);

// PNP
NTSTATUS myPNPDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
);


// ��ʼ���豸��չ
void DevExtInit(
	IN PDEV_EXT pdevext,
	IN PDEVICE_OBJECT pFilterDeviceObject,
	IN PDEVICE_OBJECT pTargetDeviceObjecct,
	IN PDEVICE_OBJECT pLowerDeviceObjecct
);


// ���豸
// ��ȡ KbdClassd ����������Ȼ����������豸����
NTSTATUS kbdAttachDevices(
	IN PDRIVER_OBJECT pDriver,
	IN PUNICODE_STRING pRegPath
);


