//�����Լ��Զ�������������
#define __INIT_UNLOAD__ //__DllInitialize__ __DllUnload__ __INIT_UNLOAD__
//û��DriverEntry��ʹ��Ĭ��DriverEntry
//#define __DriverEntry__
#include "../KernelDll/KernelDll.h"
#include "Macro.h"
#include <wdm.h>

/*
���ļ���Ҫ��������һ����ȷ���ں�dll
KernelDll.h �е������ں���Ҫ����������
���� KernelDll ���̣����Կ��ٹ���һ����ȷ���ں�dll
���� KernelDll �ṩ��Ĭ�����̲�������������������Զ�������
*/


/*
���ļ��ṩ��Ϊ�ں�dll����Ҫ�ṩ������������
��������������Ҫ���⵼��������Ҳ���ṩ��Ӧ��ͷ�ļ���
*/

CStyle NTSTATUS NTAPI DllInitialize(IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	KdPrint(("DriverBase %s : %p \nRegistryPath : %wZ\n", __FUNCTION__, DllInitialize, RegistryPath));
	return STATUS_SUCCESS;
}

CStyle NTSTATUS NTAPI DllUnload(VOID)
{
	KdBreakPoint();
	KdPrint(("DriverBase %s\n", __FUNCTION__));
	return STATUS_SUCCESS;
}

