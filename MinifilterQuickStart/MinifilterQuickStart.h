#pragma once

#ifndef MINIFILTERQUICKSTART_NO_PROMPT

#pragma message(\
"[MinifilterQuickStart.lib] 依赖于:\n"\
"[fltMgr.lib]"\
"\n请去掉 链接器 /NODEFAULTLIB 选项，或者手动添加上述依赖项.\n"\
"隐藏该提示请在包含 MinifilterQuickStart.h 前定义 [MINIFILTERQUICKSTART_NO_PROMPT]\n"\
)

#endif // MINIFILTERQUICKSTART_NO_PROMPT

#include <fltKernel.h>

namespace MinifilterQuickStart
{

#pragma region 接口

	PDRIVER_OBJECT GetDriver();

	PFLT_FILTER GetFilter();

	NTSTATUS FLTAPI RegisterInstance(
		_In_ PDRIVER_OBJECT Driver,
		_In_ PCWCH InstanceName,
		_In_ PCWCH Altitude,
		_In_ unsigned int Flags
	);

	NTSTATUS FLTAPI Register(
		_In_ PDRIVER_OBJECT Driver
	);

	void FLTAPI Unregister();

	NTSTATUS FLTAPI StartFiltering(
		_In_opt_ PFLT_PRE_OPERATION_CALLBACK PreOperationCallback = nullptr,
		_In_opt_ PFLT_POST_OPERATION_CALLBACK PostOperationCallback = nullptr);

	bool IsStartFiltering();

	NTSTATUS FLTAPI CreateCommunicationPort(
		_In_ PUNICODE_STRING PortName,
		_In_opt_ PVOID ServerPortCookie = nullptr,
		_In_opt_ PFLT_CONNECT_NOTIFY ConnectNotifyCallback = nullptr,
		_In_opt_ PFLT_DISCONNECT_NOTIFY DisconnectNotifyCallback = nullptr,
		_In_opt_ PFLT_MESSAGE_NOTIFY MessageNotifyCallback = nullptr
	);

	VOID FLTAPI CloseCommunicationPort();

	VOID FLTAPI CloseClientPort();

	PFLT_FILTER_UNLOAD_CALLBACK SetFilterUnloadCallback(PFLT_FILTER_UNLOAD_CALLBACK NewFilterUnloadCallback);

	PFLT_PRE_OPERATION_CALLBACK SetPreOperationCallback(
		_In_ PFLT_PRE_OPERATION_CALLBACK NewPreOperationCallback
	);

	PFLT_POST_OPERATION_CALLBACK SetPostOperationCallback(
		_In_ PFLT_POST_OPERATION_CALLBACK NewPostOperationCallback
	);

#pragma endregion
}
