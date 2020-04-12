#pragma once
#include "Macro.h"
#include <wdm.h>


#define __FastIoDispatch__(macro) \
	macro(FAST_IO_CHECK_IF_POSSIBLE, FastIoCheckIfPossible);\
	macro(FAST_IO_READ, FastIoRead);\
	macro(FAST_IO_WRITE, FastIoWrite);\
	macro(FAST_IO_QUERY_BASIC_INFO, FastIoQueryBasicInfo);\
	macro(FAST_IO_QUERY_STANDARD_INFO, FastIoQueryStandardInfo);\
	macro(FAST_IO_LOCK, FastIoLock);\
	macro(FAST_IO_UNLOCK_SINGLE, FastIoUnlockSingle);\
	macro(FAST_IO_UNLOCK_ALL, FastIoUnlockAll);\
	macro(FAST_IO_UNLOCK_ALL_BY_KEY, FastIoUnlockAllByKey);\
	macro(FAST_IO_DEVICE_CONTROL, FastIoDeviceControl);\
	macro(FAST_IO_ACQUIRE_FILE, AcquireFileForNtCreateSection);\
	macro(FAST_IO_RELEASE_FILE, ReleaseFileForNtCreateSection);\
	macro(FAST_IO_DETACH_DEVICE, FastIoDetachDevice);\
	macro(FAST_IO_QUERY_NETWORK_OPEN_INFO, FastIoQueryNetworkOpenInfo);\
	macro(FAST_IO_ACQUIRE_FOR_MOD_WRITE, AcquireForModWrite);\
	macro(FAST_IO_MDL_READ, MdlRead);\
	macro(FAST_IO_MDL_READ_COMPLETE, MdlReadComplete);\
	macro(FAST_IO_PREPARE_MDL_WRITE, PrepareMdlWrite);\
	macro(FAST_IO_MDL_WRITE_COMPLETE, MdlWriteComplete);\
	macro(FAST_IO_READ_COMPRESSED, FastIoReadCompressed);\
	macro(FAST_IO_WRITE_COMPRESSED, FastIoWriteCompressed);\
	macro(FAST_IO_MDL_READ_COMPLETE_COMPRESSED, MdlReadCompleteCompressed);\
	macro(FAST_IO_MDL_WRITE_COMPLETE_COMPRESSED, MdlWriteCompleteCompressed);\
	macro(FAST_IO_QUERY_OPEN, FastIoQueryOpen);\
	macro(FAST_IO_RELEASE_FOR_MOD_WRITE, ReleaseForModWrite);\
	macro(FAST_IO_ACQUIRE_FOR_CCFLUSH, AcquireForCcFlush);\
	macro(FAST_IO_RELEASE_FOR_CCFLUSH, ReleaseForCcFlush);

typedef class Driver* PDriver;

/*
需要有数据成员保存调用者自定义的例程(例如 DriverUnload ,各种派遣函数，FastIo )
为了方便，直接继承 DRIVER_OBJECT，因为 DRIVER_OBJECT 本身就包含这些数据成员
(虽然可能会造成空间浪费,以后有时间再优化)
我可真是个小机灵鬼
*/
typedef class DriverBaseAPI Driver
{
public:
	enum class DriverType
	{
		NT, WDM
	};
#pragma region 静态数据成员
protected:
	//该类所有对象都保存到这个链表中
	static PDriver _FirstDirver;
#pragma endregion

#pragma region 数据成员
protected:


	//系统驱动对象
	PDRIVER_OBJECT _DriverObject;
	PUNICODE_STRING _RegistryPath;

	//驱动类型
	DriverType _DriverType;

	//和其他Driver对象形成一个双向链表
	//由于在内核中使用STL有诸多限制，所以不采用STL
	//使用模板，不方便将整个类导出
	PDriver _Prev;
	PDriver _Next;

	//保存默认的FAST_IO_DISPATCH
	PFAST_IO_DISPATCH _DefaultFastIoDispatch;

	//用户自定义数据
	PVOID _UserData;

#pragma region 开发者自定义例程
protected:
	//开发者自定义例程
	PDRIVER_ADD_DEVICE _AddDevice;
	FAST_IO_DISPATCH _FastIoDispatch;
	PDRIVER_INITIALIZE _DriverInit;
	PDRIVER_STARTIO _DriverStartIo;
	PDRIVER_UNLOAD _DriverUnload;
	PDRIVER_DISPATCH _MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
#pragma endregion

#pragma endregion

#pragma region 静态函数

#pragma region DEVICE_OBJECT中各例程入口
protected:
	/*
	当 DriverObject->DriverExtension->AddDevice 不为 NULL 时，系统认为该驱动支持即插即用(PNP)，该驱动才会收到PNP类型的IRP.
	如果没有正确处理 PNP 类型的 IRP,则驱动无法正确加载运行
	*/
	static NTSTATUS _Routine_AddDevice(PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT PhysicalDeviceObject);

	static VOID _Routine_DriverUnload(PDRIVER_OBJECT driverObject);

	//派遣函数入口
	static NTSTATUS _Routine_MajorFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp);

#pragma region 默认 FastIo 例程
	//不是定义静态数据成员，是定义静态函数
	/*
	typedef void(Function)();
	//函数声明
	Function FuncName;
	*/
	//默认 FastIo 例程函数名
#define __Routine_FastIoDispatch_Name__(name) _Routine_FastIoDispatch_##name
	//声明 FastIo 例程
#define __DECL_Routine_FastIoDispatch__(type,name) static type __Routine_FastIoDispatch_Name__(name)
	__FastIoDispatch__(__DECL_Routine_FastIoDispatch__)
#pragma endregion

#pragma endregion

protected:
	static bool _RemoveDriver(PDRIVER_OBJECT driverObject);
public:
	//外部通过静态函数获取类对象，由该类统一管理类对象
	static PDriver CreateDriver(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath);

	static PDriver GetDriver(PDRIVER_OBJECT driverObject);
#pragma endregion

protected:
	//构造函数不对外开放
	Driver(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath);
	~Driver();
public:
	void _InitDefaultFastIoDispatch();
public:
	NTSTATUS Init(DriverType type = DriverType::NT);

	PVOID GetUserData();

	bool InitUserData(ULONG size);

	bool ReleaseUserData();

#pragma region 例程设置函数
#pragma region FastIO 设置函数
public:
	//SetFastIoDispatch_xxx 函数声明
#define __DECL_SetFastIoDispatch__(type,name) void SetFastIoDispatch_##name(type name)
	__FastIoDispatch__(__DECL_SetFastIoDispatch__)
#pragma endregion

	void SetAddDevice(PDRIVER_ADD_DEVICE Add_Device);

	void SetDriverInit(PDRIVER_INITIALIZE Init);

	void SetDriverUnload(PDRIVER_UNLOAD Unload);

	void SetMajorFunction(UCHAR mj, PDRIVER_DISPATCH dispatch);

#pragma endregion



}*PDriver;

