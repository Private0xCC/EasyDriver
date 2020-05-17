#pragma once

#include <fltKernel.h>

class Minifilter
{
private:
	static const FLT_OPERATION_REGISTRATION __Callbacks[40];

	static const FLT_REGISTRATION __FilterRegistration;

	//保存必要的全局对象
	static PFLT_FILTER __Filter;

	static PFLT_PORT __ServerPort;

	static PFLT_PORT __ClientPort;

	//保存自定义例程
	static PFLT_PRE_OPERATION_CALLBACK __PreOperationCallback1;

	static PFLT_POST_OPERATION_CALLBACK __PostOperationCallback1;

	static FLT_REGISTRATION __FilterRegistration1;

	static PFLT_CONNECT_NOTIFY __ConnectNotifyCallback1;

	static PFLT_DISCONNECT_NOTIFY __DisconnectNotifyCallback1;

	static PFLT_MESSAGE_NOTIFY __MessageNotifyCallback1;

private:
#pragma region Callbacks

	static FLT_PREOP_CALLBACK_STATUS FLTAPI __PreOperationCallback(
		_Inout_ PFLT_CALLBACK_DATA Data,
		_In_ PCFLT_RELATED_OBJECTS FltObjects,
		_Outptr_result_maybenull_ PVOID* CompletionContext
	);
	static FLT_POSTOP_CALLBACK_STATUS FLTAPI __PostOperationCallback(
		_Inout_ PFLT_CALLBACK_DATA Data,
		_In_ PCFLT_RELATED_OBJECTS FltObjects,
		_In_opt_ PVOID CompletionContext,
		_In_ FLT_POST_OPERATION_FLAGS Flags
		);

	static NTSTATUS FLTAPI __FilterUnloadCallback(
		FLT_FILTER_UNLOAD_FLAGS Flags
	);

	static NTSTATUS FLTAPI __InstanceSetupCallback(
		_In_ PCFLT_RELATED_OBJECTS FltObjects,
		_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
		_In_ DEVICE_TYPE VolumeDeviceType,
		_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
	);

	static NTSTATUS FLTAPI __InstanceQueryTeardownCallback(
		_In_ PCFLT_RELATED_OBJECTS FltObjects,
		_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
	);

	static VOID FLTAPI __InstanceTeardownStartCallback(
		_In_ PCFLT_RELATED_OBJECTS FltObjects,
		_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
	);

	static VOID FLTAPI __InstanceTeardownCompleteCallback(
		_In_ PCFLT_RELATED_OBJECTS FltObjects,
		_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
	);

	static NTSTATUS FLTAPI __ConnectNotifyCallback(
		_In_ PFLT_PORT ClientPort,
		_In_opt_ PVOID ServerPortCookie,
		_In_reads_bytes_opt_(SizeOfContext) PVOID ConnectionContext,
		_In_ ULONG SizeOfContext,
		_Outptr_result_maybenull_ PVOID* ConnectionPortCookie
	);

	static VOID FLTAPI __DisconnectNotifyCallback(
		_In_opt_ PVOID ConnectionCookie
	);

	static NTSTATUS FLTAPI __MessageNotifyCallback(
		_In_opt_ PVOID PortCookie,
		_In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
		_In_ ULONG InputBufferLength,
		_Out_writes_bytes_to_opt_(OutputBufferLength, *ReturnOutputBufferLength) PVOID OutputBuffer,
		_In_ ULONG OutputBufferLength,
		_Out_ PULONG ReturnOutputBufferLength
	);

#pragma endregion

public:
	
	static PFLT_FILTER GetFilter();

	static NTSTATUS FLTAPI Register(
		_In_ PDRIVER_OBJECT Driver
	);

	static void FLTAPI Unregister();

	static NTSTATUS FLTAPI StartFiltering(
		_In_opt_ PFLT_PRE_OPERATION_CALLBACK PreOperationCallback = nullptr,
		_In_opt_ PFLT_POST_OPERATION_CALLBACK PostOperationCallback = nullptr);

	static NTSTATUS FLTAPI CreateCommunicationPort(
			_In_ PUNICODE_STRING PortName,
			_In_opt_ PVOID ServerPortCookie = nullptr,
			_In_opt_ PFLT_CONNECT_NOTIFY ConnectNotifyCallback = nullptr,
			_In_opt_ PFLT_DISCONNECT_NOTIFY DisconnectNotifyCallback = nullptr,
			_In_opt_ PFLT_MESSAGE_NOTIFY MessageNotifyCallback = nullptr
		);

	static VOID FLTAPI CloseCommunicationPort();

	static VOID FLTAPI CloseClientPort();

};

