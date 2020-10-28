#define MINIFILTERQUICKSTART_NO_PROMPT
#include "MinifilterQuickStart.h"

#include <ntstrsafe.h>

#pragma comment(lib,"fltMgr.lib")

namespace MinifilterQuickStart
{

#pragma region Callbacks 声明

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

#pragma region 全局静态变量定义

#pragma region CONST

static const FLT_OPERATION_REGISTRATION __Callbacks[] = {
	{ IRP_MJ_CREATE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_CREATE_NAMED_PIPE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_CLOSE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_READ,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_WRITE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_QUERY_INFORMATION,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_SET_INFORMATION,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_QUERY_EA,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_SET_EA,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_FLUSH_BUFFERS,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_QUERY_VOLUME_INFORMATION,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_SET_VOLUME_INFORMATION,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_DIRECTORY_CONTROL,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_FILE_SYSTEM_CONTROL,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_DEVICE_CONTROL,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_INTERNAL_DEVICE_CONTROL,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_SHUTDOWN,
	  0,
	  __PreOperationCallback,
	  nullptr },                               //post operations not supported

	{ IRP_MJ_LOCK_CONTROL,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_CLEANUP,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_CREATE_MAILSLOT,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_QUERY_SECURITY,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_SET_SECURITY,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_QUERY_QUOTA,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_SET_QUOTA,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_PNP,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_RELEASE_FOR_MOD_WRITE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_RELEASE_FOR_CC_FLUSH,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_NETWORK_QUERY_OPEN,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_MDL_READ,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_MDL_READ_COMPLETE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_PREPARE_MDL_WRITE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_MDL_WRITE_COMPLETE,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_VOLUME_MOUNT,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_VOLUME_DISMOUNT,
	  0,
	  __PreOperationCallback,
	  __PostOperationCallback },

	{ IRP_MJ_OPERATION_END }
};

static const FLT_REGISTRATION __FilterRegistration = {

	sizeof(FLT_REGISTRATION),                           //  Size
	FLT_REGISTRATION_VERSION,                           //  Version
	0,                                                  //  Flags

	nullptr,                                            //  Context
	__Callbacks,                            //  Operation callbacks

	__FilterUnloadCallback,                 //  MiniFilterUnload

	__InstanceSetupCallback,                //  InstanceSetup
	__InstanceQueryTeardownCallback,        //  InstanceQueryTeardown
	__InstanceTeardownStartCallback,        //  InstanceTeardownStart
	__InstanceTeardownCompleteCallback,     //  InstanceTeardownComplete

	nullptr,                                            //  GenerateFileName
	nullptr,                                            //  GenerateDestinationFileName
	nullptr                                             //  NormalizeNameComponent
};

#pragma endregion

//保存必要的全局对象
static PDRIVER_OBJECT __Driver;

static PFLT_FILTER __Filter;

static PFLT_PORT __ServerPort;

static PFLT_PORT __ClientPort;

//全局标记
static bool __IsStartFiltering;

//保存自定义例程
static FLT_REGISTRATION __FilterRegistration1;

static PFLT_PRE_OPERATION_CALLBACK __PreOperationCallback1;

static PFLT_POST_OPERATION_CALLBACK __PostOperationCallback1;

static PFLT_CONNECT_NOTIFY __ConnectNotifyCallback1;

static PFLT_DISCONNECT_NOTIFY __DisconnectNotifyCallback1;

static PFLT_MESSAGE_NOTIFY __MessageNotifyCallback1;

#pragma endregion

#pragma region Callbacks

FLT_PREOP_CALLBACK_STATUS FLTAPI __PreOperationCallback(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	FLT_PREOP_CALLBACK_STATUS status = FLT_PREOP_CALLBACK_STATUS::FLT_PREOP_SUCCESS_WITH_CALLBACK;

	__try
	{
		if (__PreOperationCallback1)
		{
			status = __PreOperationCallback1(Data, FltObjects, CompletionContext);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = FLT_PREOP_CALLBACK_STATUS::FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}

	return status;
}

FLT_POSTOP_CALLBACK_STATUS FLTAPI __PostOperationCallback(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);

	FLT_POSTOP_CALLBACK_STATUS status = FLT_POSTOP_CALLBACK_STATUS::FLT_POSTOP_FINISHED_PROCESSING;

	__try
	{
		if (__PostOperationCallback1)
		{
			status = __PostOperationCallback1(Data, FltObjects, CompletionContext, Flags);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = FLT_POSTOP_CALLBACK_STATUS::FLT_POSTOP_FINISHED_PROCESSING;
	}

	return status;
}

NTSTATUS FLTAPI __FilterUnloadCallback(FLT_FILTER_UNLOAD_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Flags);

	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	__try
	{
		if (__FilterRegistration1.FilterUnloadCallback)
		{
			status = __FilterRegistration1.FilterUnloadCallback(Flags);
		}
		else
		{
			CloseCommunicationPort();
			Unregister();

			status = STATUS_SUCCESS;
		}


	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	

	return status;
}

NTSTATUS FLTAPI __InstanceSetupCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_SETUP_FLAGS Flags, DEVICE_TYPE VolumeDeviceType, FLT_FILESYSTEM_TYPE VolumeFilesystemType)
{
	KdPrint(("%s\n", __FUNCTION__));

	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(VolumeDeviceType);
	UNREFERENCED_PARAMETER(VolumeFilesystemType);

	NTSTATUS status = STATUS_SUCCESS;

	__try
	{

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_SUCCESS;
	}

	return status;
}

NTSTATUS FLTAPI __InstanceQueryTeardownCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	NTSTATUS status = STATUS_SUCCESS;

	__try
	{

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_SUCCESS;
	}

	return status;
}

VOID FLTAPI __InstanceTeardownStartCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	__try
	{

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

VOID FLTAPI __InstanceTeardownCompleteCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);

	__try
	{

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

NTSTATUS FLTAPI __ConnectNotifyCallback(PFLT_PORT ClientPort, PVOID ServerPortCookie, PVOID ConnectionContext, ULONG SizeOfContext, PVOID* ConnectionPortCookie)
{
	UNREFERENCED_PARAMETER(ClientPort);
	UNREFERENCED_PARAMETER(ServerPortCookie);
	UNREFERENCED_PARAMETER(ConnectionContext);
	UNREFERENCED_PARAMETER(SizeOfContext);
	UNREFERENCED_PARAMETER(ConnectionPortCookie);

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	__ClientPort = ClientPort;

	__try
	{
		if (__ConnectNotifyCallback1)
		{
			status = __ConnectNotifyCallback1(ClientPort, ServerPortCookie, ConnectionContext, SizeOfContext, ConnectionPortCookie);
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	if (!NT_SUCCESS(status))
	{
		__ClientPort = nullptr;
	}

	return status;
}

VOID FLTAPI __DisconnectNotifyCallback(PVOID ConnectionCookie)
{
	UNREFERENCED_PARAMETER(ConnectionCookie);

	__try
	{
		if (__DisconnectNotifyCallback1)
		{
			__DisconnectNotifyCallback1(ConnectionCookie);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	CloseClientPort();
}

NTSTATUS FLTAPI __MessageNotifyCallback(PVOID PortCookie, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength, PULONG ReturnOutputBufferLength)
{
	UNREFERENCED_PARAMETER(PortCookie);
	UNREFERENCED_PARAMETER(InputBuffer);
	UNREFERENCED_PARAMETER(InputBufferLength);
	UNREFERENCED_PARAMETER(OutputBuffer);
	UNREFERENCED_PARAMETER(OutputBufferLength);
	UNREFERENCED_PARAMETER(ReturnOutputBufferLength);

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	__try
	{
		if (__MessageNotifyCallback1)
		{
			status = __MessageNotifyCallback1(PortCookie, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, ReturnOutputBufferLength);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	return status;
}

#pragma endregion

#pragma region 接口实现

PDRIVER_OBJECT GetDriver()
{
	return __Driver;
}

PFLT_FILTER GetFilter()
{
	return __Filter;
}

NTSTATUS FLTAPI RegisterInstance(
	PDRIVER_OBJECT Driver,
	PCWCH InstanceName,
	PCWCH Altitude,
	unsigned int Flags
)
{
	NTSTATUS status = STATUS_SUCCESS;

	WCHAR Buffer[1024] = { 0 };
	UNICODE_STRING KeyName = { 0,sizeof(Buffer),Buffer };

	ULONG Disposition = 0;//REG_CREATED_NEW_KEY  REG_OPENED_EXISTING_KEY
	OBJECT_ATTRIBUTES attr = { 0 };

	HANDLE KeyInstances = NULL;
	HANDLE KeyInstanceName = NULL;


	do
	{
#define FAILED_BREAK if (!NT_SUCCESS(status)){break;}

		RtlUnicodeStringPrintf(&KeyName, L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\%wZ\\Instances\\",
			&Driver->DriverExtension->ServiceKeyName);
		InitializeObjectAttributes(&attr, &KeyName, OBJ_KERNEL_HANDLE, NULL, NULL);

		status = ZwCreateKey(&KeyInstances, KEY_ALL_ACCESS, &attr, 0, nullptr, REG_OPTION_NON_VOLATILE, &Disposition);
		FAILED_BREAK;

		RtlUnicodeStringCatString(&KeyName, InstanceName);
		InitializeObjectAttributes(&attr, &KeyName, OBJ_KERNEL_HANDLE, NULL, NULL);

		status = ZwCreateKey(&KeyInstanceName, KEY_ALL_ACCESS, &attr, 0, nullptr, REG_OPTION_NON_VOLATILE, &Disposition);
		FAILED_BREAK;

		UNICODE_STRING ValueName_DefaultInstance;
		RtlInitUnicodeString(&ValueName_DefaultInstance, L"DefaultInstance");
		status = ZwSetValueKey(KeyInstances, &ValueName_DefaultInstance, 0, REG_SZ, PVOID(InstanceName), ULONG(wcslen(InstanceName) + 1) * sizeof(WCHAR));
		FAILED_BREAK;

		UNICODE_STRING ValueName_Altitude;
		RtlInitUnicodeString(&ValueName_Altitude, L"Altitude");
		status = ZwSetValueKey(KeyInstanceName, &ValueName_Altitude, 0, REG_SZ, PVOID(Altitude), ULONG(wcslen(Altitude) + 1) * sizeof(WCHAR));
		FAILED_BREAK;

		UNICODE_STRING ValueName_Flags;
		RtlInitUnicodeString(&ValueName_Flags, L"Flags");
		status = ZwSetValueKey(KeyInstanceName, &ValueName_Flags, 0, REG_DWORD, &Flags, ULONG(sizeof(Flags)));
		FAILED_BREAK;

#undef FAILED_BREAK
	} while (false);


	if (KeyInstances != NULL)
	{
		ZwClose(KeyInstances);
	}
	if (KeyInstanceName != NULL)
	{
		ZwClose(KeyInstanceName);
	}
	return status;
}

NTSTATUS FLTAPI Register(PDRIVER_OBJECT Driver)
{
	NTSTATUS status = STATUS_SUCCESS;

	status = FltRegisterFilter(Driver, &__FilterRegistration, &__Filter);

	__Driver = Driver;

	return status;
}

void FLTAPI Unregister()
{
	if (__Filter)
	{
		FltUnregisterFilter(__Filter);
		__Filter = nullptr;
		__Driver = nullptr;
	}

}

NTSTATUS FLTAPI StartFiltering(PFLT_PRE_OPERATION_CALLBACK PreOperationCallback, PFLT_POST_OPERATION_CALLBACK PostOperationCallback)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if (__Filter && !__IsStartFiltering)
	{
		__try
		{
			status = FltStartFiltering(__Filter);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			status = STATUS_UNSUCCESSFUL;
		}
	}
	else
	{
		status = STATUS_INVALID_PARAMETER;
	}

	if (NT_SUCCESS(status))
	{
		__PreOperationCallback1 = PreOperationCallback;
		__PostOperationCallback1 = PostOperationCallback;

		__IsStartFiltering = true;
	}
	else
	{
		//Minifilter::Unregister();
	}

	return status;
}

bool IsStartFiltering()
{
	return __IsStartFiltering;
}

NTSTATUS FLTAPI CreateCommunicationPort(PUNICODE_STRING PortName, PVOID ServerPortCookie, PFLT_CONNECT_NOTIFY ConnectNotifyCallback, PFLT_DISCONNECT_NOTIFY DisconnectNotifyCallback, PFLT_MESSAGE_NOTIFY MessageNotifyCallback)
{
	NTSTATUS status = STATUS_SUCCESS;

	PSECURITY_DESCRIPTOR sd;

	status = FltBuildDefaultSecurityDescriptor(&sd, FLT_PORT_ALL_ACCESS);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	OBJECT_ATTRIBUTES attr;
	InitializeObjectAttributes(&attr, PortName, OBJ_KERNEL_HANDLE, NULL, sd);

	__try
	{
		status = FltCreateCommunicationPort(__Filter, &__ServerPort, &attr, ServerPortCookie, __ConnectNotifyCallback, __DisconnectNotifyCallback, __MessageNotifyCallback, 1);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	if (NT_SUCCESS(status))
	{
		__ConnectNotifyCallback1 = ConnectNotifyCallback;
		__DisconnectNotifyCallback1 = DisconnectNotifyCallback;
		__MessageNotifyCallback1 = MessageNotifyCallback;
	}




	return status;
}

VOID FLTAPI CloseCommunicationPort()
{
	if (__ServerPort)
	{
		FltCloseCommunicationPort(__ServerPort);
		__ServerPort = nullptr;
	}

}

VOID FLTAPI CloseClientPort()
{
	if (__Filter && __ClientPort)
	{
		FltCloseClientPort(__Filter, &__ClientPort);
		__ClientPort = nullptr;
	}

}

PFLT_FILTER_UNLOAD_CALLBACK SetFilterUnloadCallback(PFLT_FILTER_UNLOAD_CALLBACK NewFilterUnloadCallback)
{
	PFLT_FILTER_UNLOAD_CALLBACK Old = __FilterRegistration1.FilterUnloadCallback;
	__FilterRegistration1.FilterUnloadCallback = NewFilterUnloadCallback;
	return Old;
}

PFLT_PRE_OPERATION_CALLBACK SetPreOperationCallback(PFLT_PRE_OPERATION_CALLBACK NewPreOperationCallback)
{
	PFLT_PRE_OPERATION_CALLBACK Old = __PreOperationCallback1;
	__PreOperationCallback1 = NewPreOperationCallback;
	return Old;
}

PFLT_POST_OPERATION_CALLBACK SetPostOperationCallback(PFLT_POST_OPERATION_CALLBACK NewPostOperationCallback)
{
	PFLT_POST_OPERATION_CALLBACK Old = __PostOperationCallback1;
	__PostOperationCallback1 = NewPostOperationCallback;
	return Old;
}

#pragma endregion

};

