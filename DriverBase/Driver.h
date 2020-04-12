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
��Ҫ�����ݳ�Ա����������Զ��������(���� DriverUnload ,������ǲ������FastIo )
Ϊ�˷��㣬ֱ�Ӽ̳� DRIVER_OBJECT����Ϊ DRIVER_OBJECT ����Ͱ�����Щ���ݳ�Ա
(��Ȼ���ܻ���ɿռ��˷�,�Ժ���ʱ�����Ż�)
�ҿ����Ǹ�С�����
*/
typedef class DriverBaseAPI Driver
{
public:
	enum class DriverType
	{
		NT, WDM
	};
#pragma region ��̬���ݳ�Ա
protected:
	//�������ж��󶼱��浽���������
	static PDriver _FirstDirver;
#pragma endregion

#pragma region ���ݳ�Ա
protected:


	//ϵͳ��������
	PDRIVER_OBJECT _DriverObject;
	PUNICODE_STRING _RegistryPath;

	//��������
	DriverType _DriverType;

	//������Driver�����γ�һ��˫������
	//�������ں���ʹ��STL��������ƣ����Բ�����STL
	//ʹ��ģ�壬�����㽫�����ർ��
	PDriver _Prev;
	PDriver _Next;

	//����Ĭ�ϵ�FAST_IO_DISPATCH
	PFAST_IO_DISPATCH _DefaultFastIoDispatch;

	//�û��Զ�������
	PVOID _UserData;

#pragma region �������Զ�������
protected:
	//�������Զ�������
	PDRIVER_ADD_DEVICE _AddDevice;
	FAST_IO_DISPATCH _FastIoDispatch;
	PDRIVER_INITIALIZE _DriverInit;
	PDRIVER_STARTIO _DriverStartIo;
	PDRIVER_UNLOAD _DriverUnload;
	PDRIVER_DISPATCH _MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
#pragma endregion

#pragma endregion

#pragma region ��̬����

#pragma region DEVICE_OBJECT�и��������
protected:
	/*
	�� DriverObject->DriverExtension->AddDevice ��Ϊ NULL ʱ��ϵͳ��Ϊ������֧�ּ��弴��(PNP)���������Ż��յ�PNP���͵�IRP.
	���û����ȷ���� PNP ���͵� IRP,�������޷���ȷ��������
	*/
	static NTSTATUS _Routine_AddDevice(PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT PhysicalDeviceObject);

	static VOID _Routine_DriverUnload(PDRIVER_OBJECT driverObject);

	//��ǲ�������
	static NTSTATUS _Routine_MajorFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp);

#pragma region Ĭ�� FastIo ����
	//���Ƕ��徲̬���ݳ�Ա���Ƕ��徲̬����
	/*
	typedef void(Function)();
	//��������
	Function FuncName;
	*/
	//Ĭ�� FastIo ���̺�����
#define __Routine_FastIoDispatch_Name__(name) _Routine_FastIoDispatch_##name
	//���� FastIo ����
#define __DECL_Routine_FastIoDispatch__(type,name) static type __Routine_FastIoDispatch_Name__(name)
	__FastIoDispatch__(__DECL_Routine_FastIoDispatch__)
#pragma endregion

#pragma endregion

protected:
	static bool _RemoveDriver(PDRIVER_OBJECT driverObject);
public:
	//�ⲿͨ����̬������ȡ������ɸ���ͳһ���������
	static PDriver CreateDriver(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath);

	static PDriver GetDriver(PDRIVER_OBJECT driverObject);
#pragma endregion

protected:
	//���캯�������⿪��
	Driver(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath);
	~Driver();
public:
	void _InitDefaultFastIoDispatch();
public:
	NTSTATUS Init(DriverType type = DriverType::NT);

	PVOID GetUserData();

	bool InitUserData(ULONG size);

	bool ReleaseUserData();

#pragma region �������ú���
#pragma region FastIO ���ú���
public:
	//SetFastIoDispatch_xxx ��������
#define __DECL_SetFastIoDispatch__(type,name) void SetFastIoDispatch_##name(type name)
	__FastIoDispatch__(__DECL_SetFastIoDispatch__)
#pragma endregion

	void SetAddDevice(PDRIVER_ADD_DEVICE Add_Device);

	void SetDriverInit(PDRIVER_INITIALIZE Init);

	void SetDriverUnload(PDRIVER_UNLOAD Unload);

	void SetMajorFunction(UCHAR mj, PDRIVER_DISPATCH dispatch);

#pragma endregion



}*PDriver;

