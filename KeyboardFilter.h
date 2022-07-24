#pragma once
#include<ntddk.h>
#include<ntddkbd.h>


// 获取导出全局变量，在xp 的源码中就是这样获取的，书上的不对
extern POBJECT_TYPE* IoDriverObjectType;

// kbdClass 驱动的名字
#define KBD_DRIVER_NAME L"\\Driver\\Kbdclass"

// 时间定义 1000*1000*1000纳秒 = 1000*1000微秒 = 1000毫秒 = 1秒
#define  DELAY_ONE_MICROSECOND  (-10)	// 内核中CPU的基准单位时间是100纳秒 乘于10等于1微秒
#define  DELAY_ONE_MILLISECOND (DELAY_ONE_MICROSECOND*1000)
#define  DELAY_ONE_SECOND (DELAY_ONE_MILLISECOND*1000)

// 声明未公开函数
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


// 位域

typedef struct _KB_STATUS {
	// 暂时只标识 Shift 和 CapsLock 按键
	UCHAR F_SHIFT : 1;
	UCHAR F_CAPS : 1;
}KB_STATUS, * PKB_STATUS;


// 设备扩展
typedef struct _DEV_EXT
{
	// 结构体大小
	ULONG ulSize;
	// 过滤设备对象
	PDEVICE_OBJECT pFilterDeviceObject;
	// 绑定的目标设备对象
	PDEVICE_OBJECT pTargetDeviceObject;
	// 返回对象
	PDEVICE_OBJECT pLowerDeviceObject;
	// 自旋锁
	KSPIN_LOCK IoRequestsSpinLock;
	// 事件
	KEVENT IoInProgressEvent;
}DEV_EXT, * PDEV_EXT;


// 解除绑定并删除设备
VOID c2pDetach(IN PDEVICE_OBJECT pDeviceObject);

// 卸载函数
VOID myDriverUnload(PDRIVER_OBJECT pDriver);

// 分发函数

// 普通分发函数
NTSTATUS myDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
);

// 电源相关请求
NTSTATUS myPowerDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
);

// 读完成请求处理
NTSTATUS myReadComplete(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp,
	IN PVOID Context
);

// 读请求
NTSTATUS myReadDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
);

// PNP
NTSTATUS myPNPDispatch(
	IN PDEVICE_OBJECT pDevice,
	IN PIRP irp
);


// 初始化设备扩展
void DevExtInit(
	IN PDEV_EXT pdevext,
	IN PDEVICE_OBJECT pFilterDeviceObject,
	IN PDEVICE_OBJECT pTargetDeviceObjecct,
	IN PDEVICE_OBJECT pLowerDeviceObjecct
);


// 绑定设备
// 获取 KbdClassd 的驱动对象，然后绑定其所有设备对象
NTSTATUS kbdAttachDevices(
	IN PDRIVER_OBJECT pDriver,
	IN PUNICODE_STRING pRegPath
);


