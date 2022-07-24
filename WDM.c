#include"KeyboardFilter.h"

// ��ں���
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	NTSTATUS status;
	ULONG i;

	DbgPrint("��������,��������\t\n");

	// ���÷ַ�����

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriver->MajorFunction[i] = myDispatch;
	}
	// ������дһ�� IRP_MJ_POWER ������ ��Ϊ��Ҫ�Ĺ��˾��Ƕ�ȡ���İ�����Ϣ�������Ķ�����Ҫ
	pDriver->MajorFunction[IRP_MJ_READ]  = myReadDispatch;

	// ����дһ�� IRP_MJ_POWER ����
	pDriver->MajorFunction[IRP_MJ_POWER] = myPowerDispatch;
	
	// PNP(���弴��) ����Ҫ֪��ʲôʱ��һ���󶨹����豸��ж�أ�����ӻ����ϰε���
	pDriver->MajorFunction[IRP_MJ_PNP] = myPNPDispatch;
	
	// ж�غ���
	pDriver->DriverUnload = myDriverUnload;
	
	// �����еļ����豸
	status = kbdAttachDevices(pDriver, pRegPath);
	return status;
}


