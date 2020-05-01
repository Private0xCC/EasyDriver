#define __DriverBase_Export__
#include "Driver.h"
#include "FunctionCode.h"
#include "Operator.h"

PDriver Driver::_FirstDirver = nullptr;

#pragma region DEVICE_OBJECT中各例程入口

NTSTATUS Driver::_Routine_AddDevice(PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT PhysicalDeviceObject)
{
	KdPrint(("%s Enter\n", __FUNCTION__));
	KdPrint(("DriverObject : %p  PhysicalDeviceObject : %p\n", DriverObject, PhysicalDeviceObject));
	NTSTATUS status = STATUS_SUCCESS;
	PDriver driver = GetDriver(DriverObject);
	if (driver && driver->_AddDevice)
	{
		status = driver->_AddDevice(DriverObject, PhysicalDeviceObject);
	}
	KdPrint(("%s Leave  status : 0x%08X\n", __FUNCTION__, status));
	return status;
}

VOID Driver::_Routine_DriverUnload(PDRIVER_OBJECT driverObject)
{
	KdPrint(("%s Enter\n", __FUNCTION__));


	KdPrint(("DRIVER_OBJECT : %p\n", driverObject));

	PDriver driver = GetDriver(driverObject);
	if (driver && driver->_DriverUnload)
	{
		driver->_DriverUnload(driverObject);
		Driver::_RemoveDriver(driverObject);
	}

	KdPrint(("%s Leave\n", __FUNCTION__));
}

NTSTATUS Driver::_Routine_MajorFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	KdPrint(("%s Enter\n", __FUNCTION__));

	KdPrint(("DeviceObject : %p  Irp : %p\n", DeviceObject, Irp));
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
	const UCHAR mj = irpsp->MajorFunction;
	const UCHAR mn = irpsp->MinorFunction;
	PCCHAR mjName, mnName;
	::GetFunctionName(&mjName, &mnName, mj, mn);

	KdPrint(("MajorFunction(0x%08X) : %s\n", mj, mjName ? mjName : ""));
	KdPrint(("MinorFunction(0x%08X) : %s\n", mn, mnName ? mnName : ""));

	NTSTATUS status = STATUS_SUCCESS;

	PDRIVER_OBJECT driverObject = DeviceObject->DriverObject;
	PDriver driver = GetDriver(driverObject);

	if (driver)
	{
		if (driver->_MajorFunction[mj])
		{
			status = driver->_MajorFunction[mj](DeviceObject, Irp);
		}
		else
		{
			//默认处理
			KdPrint(("默认处理\n"));
			status = STATUS_SUCCESS;
			Irp->IoStatus.Information = 0;
			Irp->IoStatus.Status = status;
			IoCompleteRequest(Irp, IO_NO_INCREMENT);
		}

	}

	KdPrint(("%s Leave  status : 0x%08X\n", __FUNCTION__, status));
	return status;
}

#pragma region 默认 FastIo 例程
//默认FastIo例程函数签名宏
#define __Routine_FastIoDispatch_Signature__(name) BOOLEAN Driver::__Routine_FastIoDispatch_Name__(name)
#define __Routine_FastIoDispatch_DefaultHandle__(name,...) \
KdPrint(("%s Enter\n",__FUNCTION__));\
BOOLEAN ret = false;\
PDriver driver = GetDriver(DeviceObject->DriverObject);\
if (driver && driver->_FastIoDispatch.##name)\
{\
	ret = driver->_FastIoDispatch.##name(__VA_ARGS__);\
}\
KdPrint(("%s Leave    Return : 0x%08X\n", __FUNCTION__, ret));\
return ret;\

__Routine_FastIoDispatch_Signature__(FastIoCheckIfPossible)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ ULONG Length,
	_In_ BOOLEAN Wait,
	_In_ ULONG LockKey,
	_In_ BOOLEAN CheckForReadOperation,
	_Pre_notnull_
	_When_(return != FALSE, _Post_equal_to_(_Old_(IoStatus)))
	_When_(return == FALSE, _Post_valid_)
	PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoCheckIfPossible, FileObject, FileOffset, Length, Wait, LockKey, CheckForReadOperation, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoRead)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ ULONG Length,
	_In_ BOOLEAN Wait,
	_In_ ULONG LockKey,
	_Out_ PVOID Buffer,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoRead, FileObject, FileOffset, Length, Wait, LockKey, Buffer, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoWrite)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ ULONG Length,
	_In_ BOOLEAN Wait,
	_In_ ULONG LockKey,
	_Out_ PVOID Buffer,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoWrite, FileObject, FileOffset, Length, Wait, LockKey, Buffer, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoQueryBasicInfo)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ BOOLEAN Wait,
	_Out_ PFILE_BASIC_INFORMATION Buffer,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoQueryBasicInfo, FileObject, Wait, Buffer, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoQueryStandardInfo)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ BOOLEAN Wait,
	_Out_ PFILE_STANDARD_INFORMATION Buffer,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoQueryStandardInfo, FileObject, Wait, Buffer, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoLock)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ PLARGE_INTEGER Length,
	_In_ PEPROCESS ProcessId,
	_In_ ULONG Key,
	_In_ BOOLEAN FailImmediately,
	_In_ BOOLEAN ExclusiveLock,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoLock, FileObject, FileOffset, Length, ProcessId, Key, FailImmediately, ExclusiveLock, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoUnlockSingle)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ PLARGE_INTEGER Length,
	_In_ PEPROCESS ProcessId,
	_In_ ULONG Key,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoUnlockSingle, FileObject, FileOffset, Length, ProcessId, Key, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoUnlockAll)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PEPROCESS ProcessId,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoUnlockAll, FileObject, ProcessId, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoUnlockAllByKey)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PVOID ProcessId,
	_In_ ULONG Key,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoUnlockAllByKey, FileObject, ProcessId, Key, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoDeviceControl)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ BOOLEAN Wait,
	_In_opt_ PVOID InputBuffer,
	_In_ ULONG InputBufferLength,
	_Out_opt_ PVOID OutputBuffer,
	_In_ ULONG OutputBufferLength,
	_In_ ULONG IoControlCode,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoDeviceControl, FileObject, Wait, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, IoControlCode, IoStatus, DeviceObject);
}

VOID Driver::__Routine_FastIoDispatch_Name__(AcquireFileForNtCreateSection)(
	_In_ struct _FILE_OBJECT* FileObject
	)
{
	KdPrint(("%s Enter\n", __FUNCTION__));
	PDEVICE_OBJECT DeviceObject = IoGetRelatedDeviceObject(FileObject);
	PDriver driver = GetDriver(DeviceObject->DriverObject);
	if (driver && driver->_FastIoDispatch.AcquireFileForNtCreateSection)
	{
		driver->_FastIoDispatch.AcquireFileForNtCreateSection(FileObject);
	}
	KdPrint(("%s Leave\n", __FUNCTION__));
}

VOID Driver::__Routine_FastIoDispatch_Name__(ReleaseFileForNtCreateSection)(
	_In_ struct _FILE_OBJECT* FileObject
	)
{
	KdPrint(("%s Enter\n", __FUNCTION__));
	PDEVICE_OBJECT DeviceObject = IoGetRelatedDeviceObject(FileObject);
	PDriver driver = GetDriver(DeviceObject->DriverObject);
	if (driver && driver->_FastIoDispatch.ReleaseFileForNtCreateSection)
	{
		driver->_FastIoDispatch.ReleaseFileForNtCreateSection(FileObject);
	}
	KdPrint(("%s Leave\n", __FUNCTION__));
}

VOID Driver::__Routine_FastIoDispatch_Name__(FastIoDetachDevice)(
	_In_ struct _DEVICE_OBJECT* SourceDevice,// our device object
	_In_ struct _DEVICE_OBJECT* TargetDevice
	)
{
	KdPrint(("%s Enter\n", __FUNCTION__));
	PDriver driver = GetDriver(SourceDevice->DriverObject);
	if (driver && driver->_FastIoDispatch.FastIoDetachDevice)
	{
		driver->_FastIoDispatch.FastIoDetachDevice(SourceDevice, TargetDevice);
	}
	KdPrint(("%s Leave\n", __FUNCTION__));
}

__Routine_FastIoDispatch_Signature__(FastIoQueryNetworkOpenInfo)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ BOOLEAN Wait,
	_Out_ struct _FILE_NETWORK_OPEN_INFORMATION* Buffer,
	_Out_ struct _IO_STATUS_BLOCK* IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoQueryNetworkOpenInfo, FileObject, Wait, Buffer, IoStatus, DeviceObject);
}

NTSTATUS Driver::__Routine_FastIoDispatch_Name__(AcquireForModWrite)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER EndingOffset,
	_Out_ struct _ERESOURCE** ResourceToRelease,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	KdPrint(("%s Enter\n", __FUNCTION__));
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PDriver driver = GetDriver(DeviceObject->DriverObject);
	if (driver && driver->_FastIoDispatch.AcquireForModWrite)
	{
		status = driver->_FastIoDispatch.AcquireForModWrite(FileObject, EndingOffset,ResourceToRelease,DeviceObject);
	}
	KdPrint(("%s Leave Retuen : 0x%08X\n", __FUNCTION__,status));
	return status;
}

__Routine_FastIoDispatch_Signature__(MdlRead)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ ULONG Length,
	_In_ ULONG LockKey,
	_Out_ PMDL* MdlChain,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(MdlRead, FileObject, FileOffset, Length, LockKey, MdlChain, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(MdlReadComplete)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PMDL MdlChain,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(MdlReadComplete, FileObject, MdlChain, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(PrepareMdlWrite)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ ULONG Length,
	_In_ ULONG LockKey,
	_Out_ PMDL* MdlChain,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(PrepareMdlWrite, FileObject, FileOffset, Length, LockKey, MdlChain, IoStatus, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(MdlWriteComplete)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ PMDL MdlChain,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(MdlWriteComplete, FileObject, FileOffset, MdlChain, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoReadCompressed)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ ULONG Length,
	_In_ ULONG LockKey,
	_Out_ PVOID Buffer,
	_Out_ PMDL* MdlChain,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_Out_writes_bytes_(CompressedDataInfoLength) struct _COMPRESSED_DATA_INFO* CompressedDataInfo,
	_In_ ULONG CompressedDataInfoLength,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoReadCompressed, FileObject, FileOffset, Length, LockKey, Buffer, MdlChain, IoStatus, CompressedDataInfo, CompressedDataInfoLength,DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoWriteCompressed)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ ULONG Length,
	_In_ ULONG LockKey,
	_In_ PVOID Buffer,
	_Out_ PMDL* MdlChain,
	_Out_ PIO_STATUS_BLOCK IoStatus,
	_In_reads_bytes_(CompressedDataInfoLength) struct _COMPRESSED_DATA_INFO* CompressedDataInfo,
	_In_ ULONG CompressedDataInfoLength,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoWriteCompressed, FileObject, FileOffset, Length, LockKey, Buffer, MdlChain, IoStatus, CompressedDataInfo, CompressedDataInfoLength, DeviceObject);
}


__Routine_FastIoDispatch_Signature__(MdlReadCompleteCompressed)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PMDL MdlChain,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(MdlReadCompleteCompressed, FileObject, MdlChain, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(MdlWriteCompleteCompressed)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ PLARGE_INTEGER FileOffset,
	_In_ PMDL MdlChain,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(MdlWriteCompleteCompressed, FileObject, FileOffset, MdlChain, DeviceObject);
}

__Routine_FastIoDispatch_Signature__(FastIoQueryOpen)(
	_Inout_ struct _IRP* Irp,
	_Out_ PFILE_NETWORK_OPEN_INFORMATION NetworkInformation,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	__Routine_FastIoDispatch_DefaultHandle__(FastIoQueryOpen, Irp, NetworkInformation, DeviceObject);
}

NTSTATUS Driver::__Routine_FastIoDispatch_Name__(ReleaseForModWrite)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ struct _ERESOURCE* ResourceToRelease,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	KdPrint(("%s Enter\n", __FUNCTION__));
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PDriver driver = GetDriver(DeviceObject->DriverObject);
	if (driver && driver->_FastIoDispatch.ReleaseForModWrite)
	{
		status = driver->_FastIoDispatch.ReleaseForModWrite(FileObject, ResourceToRelease, DeviceObject);
	}
	KdPrint(("%s Leave Retuen : 0x%08X\n", __FUNCTION__, status));
	return status;
}

NTSTATUS Driver::__Routine_FastIoDispatch_Name__(AcquireForCcFlush)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	KdPrint(("%s Enter\n", __FUNCTION__));
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PDriver driver = GetDriver(DeviceObject->DriverObject);
	if (driver && driver->_FastIoDispatch.AcquireForCcFlush)
	{
		status = driver->_FastIoDispatch.AcquireForCcFlush(FileObject, DeviceObject);
	}
	KdPrint(("%s Leave Retuen : 0x%08X\n", __FUNCTION__, status));
	return status;
}

NTSTATUS Driver::__Routine_FastIoDispatch_Name__(ReleaseForCcFlush)(
	_In_ struct _FILE_OBJECT* FileObject,
	_In_ struct _DEVICE_OBJECT* DeviceObject
	)
{
	KdPrint(("%s Enter\n", __FUNCTION__));
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PDriver driver = GetDriver(DeviceObject->DriverObject);
	if (driver && driver->_FastIoDispatch.ReleaseForCcFlush)
	{
		status = driver->_FastIoDispatch.ReleaseForCcFlush(FileObject, DeviceObject);
	}
	KdPrint(("%s Leave Retuen : 0x%08X\n", __FUNCTION__, status));
	return status;
}

#pragma endregion

#pragma endregion

bool Driver::_RemoveDriver(PDRIVER_OBJECT driverObject)
{
	//将对应的 Driver 对象从链表中移除
	PDriver driver = Driver::GetDriver(driverObject);
	if (driver)
	{
		PDriver prev = driver->_Prev;
		PDriver next = driver->_Next;


		prev->_Next = next;
		next->_Prev = prev;

		delete driver;

		if (prev == driver)//只有一个
		{
			Driver::_FirstDirver = nullptr;
		}
		else if (Driver::_FirstDirver == driver)//要移除的是第一个
		{
			Driver::_FirstDirver = next;
		}

		return true;
	}
	return false;
}

PDriver Driver::CreateDriver(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath)
{
	UNREFERENCED_PARAMETER(registryPath);
	PDriver driver = Driver::GetDriver(driverObject);
	if (driver)
	{
		return driver;
	}

	//创建新的Driver对象
	driver = new Driver(driverObject, registryPath);

	//插入到最后
	if (Driver::_FirstDirver)
	{
		PDriver tail = _FirstDirver->_Prev;
		tail->_Next = driver;
		driver->_Prev = tail;
		driver->_Next = Driver::_FirstDirver;
		Driver::_FirstDirver->_Prev = driver;
	}
	else
	{
		Driver::_FirstDirver = driver;
		driver->_Prev = driver;
		driver->_Next = driver;
	}

	return driver;
}

PDriver Driver::GetDriver(PDRIVER_OBJECT driverObject)
{
	if (Driver::_FirstDirver)
	{
		//遍历查找对应的 Driver
		PDriver driver = Driver::_FirstDirver;
		do
		{
			if (driver->_DriverObject == driverObject)
			{
				return driver;
			}

			driver = driver->_Next;
		} while (driver != Driver::_FirstDirver);

		return nullptr;
	}
	else
	{
		return nullptr;
	}

}

Driver::Driver(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath)
	:_DriverObject(driverObject), _RegistryPath(registryPath), _DriverType(DriverType::NT)
{
	KdPrint(("%s Enter\n", __FUNCTION__));

	KdPrint(("DriverObject : %p\nRegistryPath : %p\n", driverObject, registryPath));

	KdPrint(("%s Leave\n", __FUNCTION__));
}

Driver::~Driver()
{
	if (this->_DefaultFastIoDispatch)
	{
		delete this->_DefaultFastIoDispatch;
	}	
}

void Driver::_InitDefaultFastIoDispatch()
{
	if (!this->_DefaultFastIoDispatch)
	{
		this->_DefaultFastIoDispatch = PFAST_IO_DISPATCH(ExAllocatePool(NonPagedPool, sizeof(FAST_IO_DISPATCH)));
		RtlZeroMemory(this->_DefaultFastIoDispatch, sizeof(FAST_IO_DISPATCH));
		this->_DefaultFastIoDispatch->SizeOfFastIoDispatch = sizeof(FAST_IO_DISPATCH);

#define __SetDefaultFastIoDispatch__(unuse,name) this->_DefaultFastIoDispatch->##name = Driver::__Routine_FastIoDispatch_Name__(name)
		//设置所有FastIo例程
		__FastIoDispatch__(__SetDefaultFastIoDispatch__);

		PDRIVER_OBJECT driverObject = this->_DriverObject;

		driverObject->FastIoDispatch = this->_DefaultFastIoDispatch;


		RtlZeroMemory(&this->_FastIoDispatch, sizeof(FAST_IO_DISPATCH));
	}
}

NTSTATUS Driver::Init(DriverType type)
{
	KdPrint(("%s Enter\n", __FUNCTION__));
	PDRIVER_OBJECT driverObject = this->_DriverObject;

	this->EnableDriverUnload();

	for (UCHAR i = 0; i <= IRP_MJ_PNP; i++)
	{
		this->_MajorFunction[i] = driverObject->MajorFunction[i];
		driverObject->MajorFunction[i] = Driver::_Routine_MajorFunction;
	}

	if (type == DriverType::NT)
	{
		driverObject->DriverExtension->AddDevice = nullptr;
	}
	else
	{
		//设置AddDevice例程，系统才认为是支持PNP的新型驱动(WDM)，驱动才能收到PNP类型IRP
		driverObject->DriverExtension->AddDevice = Driver::_Routine_AddDevice;
	}
	KdPrint(("%s Leave\n", __FUNCTION__));
	return STATUS_SUCCESS;
}

PVOID Driver::GetUserData()
{
	return this->_UserData;
}

bool Driver::InitUserData(ULONG size)
{
	size;
	if (this->_UserData)
	{
		return false;//已经初始化了
	}
	this->_UserData = new CHAR[size];
	return true;
}

bool Driver::ReleaseUserData()
{
	if (this->_UserData)
	{
		delete[]this->_UserData;
		return true;
	}
	return false;
}

#pragma region 例程设置函数

#pragma region FastIO 设置函数

//SetFastIoDispatch_xxx 函数定义
#define __DEFINE_SetFastIoDispatch__(type,name) \
void Driver::SetFastIoDispatch_##name(type name)\
{\
	if (!this->_DefaultFastIoDispatch)\
	{\
		this->_InitDefaultFastIoDispatch();\
	}\
	this->_FastIoDispatch.name = name;\
}

__FastIoDispatch__(__DEFINE_SetFastIoDispatch__)

#pragma endregion

void Driver::SetAddDevice(PDRIVER_ADD_DEVICE Add_Device)
{
	this->_AddDevice = Add_Device;
}

void Driver::SetDriverInit(PDRIVER_INITIALIZE Init) { this->_DriverInit = Init; }

void Driver::SetDriverUnload(PDRIVER_UNLOAD Unload) { this->_DriverUnload = Unload; }

void Driver::SetMajorFunction(UCHAR mj, PDRIVER_DISPATCH dispatch)
{
	if (mj <= IRP_MJ_MAXIMUM_FUNCTION)
	{
		this->_MajorFunction[mj] = dispatch;
	}
}

void Driver::DisableDriverUnload()
{
	if (this->_DriverObject)
	{
		this->_DriverObject = nullptr;
	}
}

void Driver::EnableDriverUnload()
{
	if (this->_DriverObject)
	{
		this->_DriverObject->DriverUnload = Driver::_Routine_DriverUnload;
	}
}

#pragma endregion
