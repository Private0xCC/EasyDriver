#include "Minifilter.h"

#pragma region 数据成员定义

#pragma region CONST
const FLT_OPERATION_REGISTRATION Minifilter::__Callbacks[] = {
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

const FLT_REGISTRATION Minifilter::__FilterRegistration = {

	sizeof(FLT_REGISTRATION),                           //  Size
	FLT_REGISTRATION_VERSION,                           //  Version
	0,                                                  //  Flags

	nullptr,                                            //  Context
	Minifilter::__Callbacks,                            //  Operation callbacks

	Minifilter::__FilterUnloadCallback,                 //  MiniFilterUnload

	Minifilter::__InstanceSetupCallback,                //  InstanceSetup
	Minifilter::__InstanceQueryTeardownCallback,        //  InstanceQueryTeardown
	Minifilter::__InstanceTeardownStartCallback,        //  InstanceTeardownStart
	Minifilter::__InstanceTeardownCompleteCallback,     //  InstanceTeardownComplete

	nullptr,                                            //  GenerateFileName
	nullptr,                                            //  GenerateDestinationFileName
	nullptr                                             //  NormalizeNameComponent
};

#pragma endregion

//保存必要的全局对象
PFLT_FILTER Minifilter::__Filter;

PFLT_PORT Minifilter::__ServerPort;

PFLT_PORT Minifilter::__ClientPort;

//保存自定义例程
PFLT_PRE_OPERATION_CALLBACK Minifilter::__PreOperationCallback1;

PFLT_POST_OPERATION_CALLBACK Minifilter::__PostOperationCallback1;

FLT_REGISTRATION Minifilter::__FilterRegistration1;

PFLT_CONNECT_NOTIFY Minifilter::__ConnectNotifyCallback1;

PFLT_DISCONNECT_NOTIFY Minifilter::__DisconnectNotifyCallback1;

PFLT_MESSAGE_NOTIFY Minifilter::__MessageNotifyCallback1;

#pragma endregion

#pragma region Callbacks

FLT_PREOP_CALLBACK_STATUS FLTAPI Minifilter::__PreOperationCallback(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);

	FLT_PREOP_CALLBACK_STATUS status = FLT_PREOP_CALLBACK_STATUS::FLT_PREOP_SUCCESS_WITH_CALLBACK;

	__try
	{
		if (Minifilter::__PreOperationCallback1)
		{
			status = Minifilter::__PreOperationCallback1(Data, FltObjects, CompletionContext);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = FLT_PREOP_CALLBACK_STATUS::FLT_PREOP_SUCCESS_WITH_CALLBACK;
	}

	return status;
}

FLT_POSTOP_CALLBACK_STATUS FLTAPI Minifilter::__PostOperationCallback(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);

	FLT_POSTOP_CALLBACK_STATUS status = FLT_POSTOP_CALLBACK_STATUS::FLT_POSTOP_FINISHED_PROCESSING;

	__try
	{
		if (Minifilter::__PostOperationCallback1)
		{
			status = Minifilter::__PostOperationCallback1(Data, FltObjects, CompletionContext, Flags);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = FLT_POSTOP_CALLBACK_STATUS::FLT_POSTOP_FINISHED_PROCESSING;
	}

	return status;
}

NTSTATUS FLTAPI Minifilter::__FilterUnloadCallback(FLT_FILTER_UNLOAD_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Flags);

	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	__try
	{
		if (Minifilter::__FilterRegistration1.FilterUnloadCallback)
		{
			status = Minifilter::__FilterRegistration1.FilterUnloadCallback(Flags);
		}
		else
		{
			Minifilter::CloseCommunicationPort();
			Minifilter::Unregister();

			status = STATUS_SUCCESS;
		}


	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	

	return status;
}

NTSTATUS FLTAPI Minifilter::__InstanceSetupCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_SETUP_FLAGS Flags, DEVICE_TYPE VolumeDeviceType, FLT_FILESYSTEM_TYPE VolumeFilesystemType)
{
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

NTSTATUS FLTAPI Minifilter::__InstanceQueryTeardownCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
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

VOID FLTAPI Minifilter::__InstanceTeardownStartCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Flags)
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

VOID FLTAPI Minifilter::__InstanceTeardownCompleteCallback(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Flags)
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

NTSTATUS FLTAPI Minifilter::__ConnectNotifyCallback(PFLT_PORT ClientPort, PVOID ServerPortCookie, PVOID ConnectionContext, ULONG SizeOfContext, PVOID* ConnectionPortCookie)
{
	UNREFERENCED_PARAMETER(ClientPort);
	UNREFERENCED_PARAMETER(ServerPortCookie);
	UNREFERENCED_PARAMETER(ConnectionContext);
	UNREFERENCED_PARAMETER(SizeOfContext);
	UNREFERENCED_PARAMETER(ConnectionPortCookie);

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	__try
	{
		if (Minifilter::__ConnectNotifyCallback1)
		{
			status = Minifilter::__ConnectNotifyCallback1(ClientPort, ServerPortCookie, ConnectionContext, SizeOfContext, ConnectionPortCookie);
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	return status;
}

VOID FLTAPI Minifilter::__DisconnectNotifyCallback(PVOID ConnectionCookie)
{
	UNREFERENCED_PARAMETER(ConnectionCookie);

	__try
	{
		if (Minifilter::__DisconnectNotifyCallback1)
		{
			Minifilter::__DisconnectNotifyCallback1(ConnectionCookie);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

NTSTATUS FLTAPI Minifilter::__MessageNotifyCallback(PVOID PortCookie, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength, PULONG ReturnOutputBufferLength)
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
		if (Minifilter::__MessageNotifyCallback1)
		{
			status = Minifilter::__MessageNotifyCallback1(PortCookie, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, ReturnOutputBufferLength);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	return status;
}

#pragma endregion

PFLT_FILTER Minifilter::GetFilter()
{
	return Minifilter::__Filter;
}

NTSTATUS Minifilter::Register(PDRIVER_OBJECT Driver)
{
	NTSTATUS status = STATUS_SUCCESS;

	status = FltRegisterFilter(Driver, &Minifilter::__FilterRegistration, &Minifilter::__Filter);

	return status;
}

void Minifilter::Unregister()
{
	if (Minifilter::__Filter)
	{
		FltUnregisterFilter(Minifilter::__Filter);
		Minifilter::__Filter = nullptr;
	}
	
}

NTSTATUS FLTAPI Minifilter::StartFiltering(PFLT_PRE_OPERATION_CALLBACK PreOperationCallback, PFLT_POST_OPERATION_CALLBACK PostOperationCallback)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if (Minifilter::__Filter)
	{
		__try
		{
			status = FltStartFiltering(Minifilter::__Filter);
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
		Minifilter::__PreOperationCallback1 = PreOperationCallback;
		Minifilter::__PostOperationCallback1 = PostOperationCallback;
	}
	else
	{
		//Minifilter::Unregister();
	}

	return status;
}

NTSTATUS FLTAPI Minifilter::CreateCommunicationPort(PUNICODE_STRING PortName, PVOID ServerPortCookie, PFLT_CONNECT_NOTIFY ConnectNotifyCallback, PFLT_DISCONNECT_NOTIFY DisconnectNotifyCallback, PFLT_MESSAGE_NOTIFY MessageNotifyCallback)
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
		status = FltCreateCommunicationPort(Minifilter::__Filter, &Minifilter::__ServerPort, &attr, ServerPortCookie, Minifilter::__ConnectNotifyCallback, Minifilter::__DisconnectNotifyCallback, Minifilter::__MessageNotifyCallback, 1);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = STATUS_UNSUCCESSFUL;
	}

	if (NT_SUCCESS(status))
	{
		Minifilter::__ConnectNotifyCallback1 = ConnectNotifyCallback;
		Minifilter::__DisconnectNotifyCallback1 = DisconnectNotifyCallback;
		Minifilter::__MessageNotifyCallback1 = MessageNotifyCallback;
	}
	

	
	
	return status;
}

VOID FLTAPI Minifilter::CloseCommunicationPort()
{
	if (Minifilter::__ServerPort)
	{
		FltCloseCommunicationPort(Minifilter::__ServerPort);
		Minifilter::__ServerPort = nullptr;
	}
	
}

VOID FLTAPI Minifilter::CloseClientPort()
{
	if (Minifilter::__Filter && Minifilter::__ClientPort)
	{
		FltCloseClientPort(Minifilter::__Filter, &Minifilter::__ClientPort);
		Minifilter::__ClientPort = nullptr;
	}
	
}


