//表明自己自定义了两个例程
#define __INIT_UNLOAD__ //__DllInitialize__ __DllUnload__ __INIT_UNLOAD__
//没有DriverEntry，使用默认DriverEntry
//#define __DriverEntry__
#include "../KernelDll/KernelDll.h"
#include "Macro.h"
#include <wdm.h>

/*
该文件主要用来构建一个正确的内核dll
KernelDll.h 中导出了内核需要的两个例程
引用 KernelDll 工程，可以快速构建一个正确的内核dll
由于 KernelDll 提供的默认例程不能满足调试需求，所以自定义例程
*/


/*
该文件提供作为内核dll必须要提供的三个函数。
这三个函数不需要对外导出，所以也不提供对应的头文件。
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

