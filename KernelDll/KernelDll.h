#pragma once
#include <wdm.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __KernelDll_Self__//__KernelDll_Self__ ��ʾ�� KernelDll �Լ���ʹ�ñ�ͷ�ļ�����ô�������������Ҫ

	/*
	�ں�dll����Ҫ�ṩ������������DllInitialize DllUnload DriverEntry(Ĭ���������Ҫ���ɸ�����ڵ�)
	����ע���еĴ����ѡ�� WRK 1.2
	*/
	/*
	DllInitialize �� DllUnload ֻ���ڵ�ǰģ����Ϊ����ģ��(����ģ��)��ʱ��Żᱻ����
	MmLoadSystemImage,ģ�鱻��Ϊ��ģ��
	MiLoadImportDll,ģ�鱻��Ϊ����ģ��
	*/

	/*
	NTSTATUS
		MmCallDllInitialize(
			IN PKLDR_DATA_TABLE_ENTRY DataTableEntry,
			IN PLIST_ENTRY ModuleListHead
		)
	{
		NTSTATUS st;
		PMM_DLL_INITIALIZE Func;
		Func = (PMM_DLL_INITIALIZE)(ULONG_PTR)MiLocateExportName(DataTableEntry->DllBase, "DllInitialize");
		if (!Func) {
			return STATUS_SUCCESS;
		}
		st = Func(&RegistryPath);
		if ((NT_SUCCESS(st)) &&
			(MiFirstDriverLoadEver == 0) &&
			(MiVerifierThunksAdded != ThunksAdded)) {

			MiReApplyVerifierToLoadedModules(ModuleListHead);
		}
		return st;
	}

	NTSTATUS
		MiResolveImageReferences(
			PVOID ImageBase,
			IN PUNICODE_STRING ImageFileDirectory,
			IN PUNICODE_STRING NamePrefix OPTIONAL,
			OUT PCHAR* MissingProcedureName,
			OUT PWSTR* MissingDriverName,
			OUT PLOAD_IMPORTS* LoadedImports
		)
	{
		NTSTATUS st;
		st = MmCallDllInitialize(TableEntry, &PsLoadedModuleList);//���û�е��� DllInitialize ����ֱ�ӷ��� STATUS_SUCCESS��������򷵻�DllInitialize�ķ���ֵ
		//
		// If the module could not be properly initialized,
		// unload it.
		//
		if (!NT_SUCCESS(st)) {
			//���DllInitialize����ȴû�з��سɹ�������ڴ���ж��
			MmUnloadSystemImage((PVOID)TableEntry);
			Loaded = FALSE;
		}
	}

	�ܽ�:����е��� DllInitialize ���̵���û�з��� STATUS_SUCCESS��������ȷ���ص��ڴ���
	*/
	NTSTATUS DllInitialize(IN PUNICODE_STRING RegistryPath);

	/*
	LOGICAL
	MiCallDllUnloadAndUnloadDll(
		IN PKLDR_DATA_TABLE_ENTRY DataTableEntry
		)
	{
		LOGICAL Unloaded = FALSE;
		PMM_DLL_UNLOAD Func = (PMM_DLL_UNLOAD)(ULONG_PTR)MiLocateExportName(DataTableEntry->DllBase, "DllUnload");
		if (Func)
		{
			Status = Func();//���� dll �� DllUnload
			if (NT_SUCCESS(Status)) {
				MmUnloadSystemImage((PVOID)DataTableEntry);//���ڴ���ж��
				Unloaded = TRUE;
			}
		}
		return Unloaded;
	}
	ֻ�е��������� TRUE ��ʱ��dll�ű���ȷж�أ������פ�����ڴ���
	�ܽ�:���û�е��� DllUnload ���̻���û�з��� STATUS_SUCCESS���� dll �޷���ж��


	*/
	NTSTATUS DllUnload(VOID);

#define __DEFAULT_LIB__ "KernelDll.lib"


//���������__INIT_UNLOAD__��
//����Ϊ�������Զ�����DllInitialize��DllUnload����
#ifdef __INIT_UNLOAD__
#define __DllInitialize__
#define __DllUnload__
#endif

#define ADDLIB(LibPath) comment(lib,LibPath)
//����������ѡ���
#define LINKER(x) comment(linker,x)
#define EXPORT(ExportDefine) LINKER("/EXPORT:"##ExportDefine)

//���������� /EXPORT ѡ��Ĳ�����
//x86��x64,extern "C" �淶�Ļ�������һ����x64 ��ֻ��һ�ֵ���Լ�� __fastcall,���� extern "C" �淶����Ҫ���л���
#ifdef _WIN64
#define EXPORT_DllInitialize "DllInitialize=DllInitialize,PRIVATE"
#define EXPORT_DllUnload "DllUnload=DllUnload,PRIVATE"
#define EXPORT_DefaultDllInitialize "DllInitialize=DefaultDllInitialize,PRIVATE"
#define EXPORT_DefaultDllUnload "DllUnload=DefaultDllUnload,PRIVATE"
#else
#define EXPORT_DllInitialize "DllInitialize=_DllInitialize@4,PRIVATE"
#define EXPORT_DllUnload "DllUnload=_DllUnload@0,PRIVATE"
#define EXPORT_DefaultDllInitialize "DllInitialize=_DefaultDllInitialize@4,PRIVATE"
#define EXPORT_DefaultDllUnload "DllUnload=_DefaultDllUnload@0,PRIVATE"
#endif


#ifdef __DllInitialize__
//����ʹ���Զ��������
//#pragma comment(linker, "/EXPORT:DllInitialize=_DllInitialize@4,PRIVATE")
#pragma EXPORT(EXPORT_DllInitialize)
#pragma message("���û���Զ���� [DllInitialize] ����,��Ҫ���� [__DllInitialize__] ��")
#else
//���û���Զ�������̣���ʹ��Ĭ�ϵģ�ͬʱ��Ҫ��������� lib
#define __REQUIRE_DEFAULT_LIB__
//#pragma comment(linker, "/EXPORT:DllInitialize=_DefaultDllInitialize@4,PRIVATE")
#pragma EXPORT(EXPORT_DefaultDllInitialize)
#pragma message("������Զ���� [DllInitialize] ����,���� [__DllInitialize__] ��")
#endif

#ifdef __DllUnload__
//����ʹ���Զ��������
//#pragma comment(linker, "/EXPORT:DllUnload=_DllUnload@0,PRIVATE")
#pragma EXPORT(EXPORT_DllUnload)
#pragma message("���û���Զ���� [DllUnload] ����,��Ҫ���� [__DllUnload__] ��")
#else
//���û���Զ�������̣���ʹ��Ĭ�ϵģ�ͬʱ��Ҫ��������� lib
#define __REQUIRE_DEFAULT_LIB__

//#pragma comment(linker, "/EXPORT:DllUnload=_DefaultDllUnload@0,PRIVATE")
#pragma EXPORT(EXPORT_DefaultDllUnload)
#pragma message("������Զ���� [DllUnload] ����,���� [__DllUnload__] ��")
#endif

/*
Ĭ����� _GsDriverEntry@8 �������� DriverEntry.
�����ǰģ����Ϊ������ģ�������ģ�飬����ں�����Զ������á�
����Ϊ�˱�֤����ͨ�������Ա���Ҫ���ϡ�
�����dll��Ŀ�Ҳ����Զ��� DriverEntry ������̣���ֱ��ʹ�� DefaultDriverEntry �� ����Ĭ�� DriverEntry ����
����������Զ��� DriverEntry �Ҳ�Ҫʹ�� DefaultDriverEntry ��
���磺ĳЩsys Ҳϣ������������Ϊ������ģ�������ģ�飬��������һ�� sys �����Լ��� DriverEntry,���ԾͲ�����ʹ�����������Ĭ�ϵ�DriverEntry
*/

#ifdef __DriverEntry__
#pragma message("���û���Զ���� [DriverEntry] ���̣���Ҫ���� [__DriverEntry__] ��")
#else
	NTSTATUS NTAPI DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
	{
		KdBreakPoint();
		UNREFERENCED_PARAMETER(driver);
		UNREFERENCED_PARAMETER(reg_path);
		KdPrint(("%s\n", __FUNCTION__));
		KdPrint(("driver : %p\nreg_path : %p\n", driver, reg_path));
		return STATUS_UNSUCCESSFUL;
	}
#pragma message("������Զ���� [DriverEntry] ���̣����� [__DriverEntry__] ��")
#endif

#ifdef __REQUIRE_DEFAULT_LIB__
	/*
	�������������꣬����Ҫ����Ĭ�ϵ�lib
	������ѡ�� /nodefaultlib �����������ע������
	��ʱ��Ҫȥ�� /nodefaultlib �����ֶ���� Ĭ�� lib
	*/

#pragma message(\
">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>KernelDll Warning<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"\
"��Ҫ����Ĭ�ϵ� [ KernelDll.lib ]."\
"����libʹ����ע����:[ #pragma comment(lib,__DEFAULT_LIB__) ]\n"\
"������ѡ�� [ /nodefaultlib ] �����������ע������\n"\
"�����ʾ�Ҳ������ţ�����Ҫȥ�� [ /nodefaultlib ] �����ֶ���� [ KernelDll.lib ].\n"\
)

	//#pragma comment(lib,__DEFAULT_LIB__)
#pragma ADDLIB(__DEFAULT_LIB__)

#endif// end of __REQUIRE_DEFAULT_LIB__

#endif//end of __KernelDll_Self__

#ifdef __cplusplus
}
#endif
