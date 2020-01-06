#pragma once
#include <wdm.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __KernelDll_Self__//__KernelDll_Self__ 表示是 KernelDll 自己在使用本头文件，那么下面的内容则不需要

	/*
	内核dll必须要提供的三个函数。DllInitialize DllUnload DriverEntry(默认情况下需要，可更改入口点)
	下面注释中的代码节选自 WRK 1.2
	*/
	/*
	DllInitialize 和 DllUnload 只有在当前模块作为引用模块(非主模块)的时候才会被调用
	MmLoadSystemImage,模块被作为主模块
	MiLoadImportDll,模块被作为引用模块
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
		st = MmCallDllInitialize(TableEntry, &PsLoadedModuleList);//如果没有导出 DllInitialize ，则直接返回 STATUS_SUCCESS，如果有则返回DllInitialize的返回值
		//
		// If the module could not be properly initialized,
		// unload it.
		//
		if (!NT_SUCCESS(st)) {
			//如果DllInitialize存在却没有返回成功，则从内存中卸载
			MmUnloadSystemImage((PVOID)TableEntry);
			Loaded = FALSE;
		}
	}

	总结:如果有导出 DllInitialize 例程但是没有返回 STATUS_SUCCESS，则不能正确加载到内存中
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
			Status = Func();//调用 dll 的 DllUnload
			if (NT_SUCCESS(Status)) {
				MmUnloadSystemImage((PVOID)DataTableEntry);//从内存中卸载
				Unloaded = TRUE;
			}
		}
		return Unloaded;
	}
	只有当函数返回 TRUE 的时候，dll才被正确卸载，否则会驻留在内存中
	总结:如果没有导出 DllUnload 例程或者没有返回 STATUS_SUCCESS，则 dll 无法被卸载


	*/
	NTSTATUS DllUnload(VOID);

#define __DEFAULT_LIB__ "KernelDll.lib"


//如果定义了__INIT_UNLOAD__宏
//则认为开发者自定义了DllInitialize和DllUnload例程
#ifdef __INIT_UNLOAD__
#define __DllInitialize__
#define __DllUnload__
#endif

#define ADDLIB(LibPath) comment(lib,LibPath)
//定义链接器选项宏
#define LINKER(x) comment(linker,x)
#define EXPORT(ExportDefine) LINKER("/EXPORT:"##ExportDefine)

//定义链接器 /EXPORT 选项的参数宏
//x86和x64,extern "C" 规范的换名规则不一样，x64 下只有一种调用约定 __fastcall,所以 extern "C" 规范不需要进行换名
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
//优先使用自定义的例程
//#pragma comment(linker, "/EXPORT:DllInitialize=_DllInitialize@4,PRIVATE")
#pragma EXPORT(EXPORT_DllInitialize)
#pragma message("如果没有自定义的 [DllInitialize] 例程,则不要定义 [__DllInitialize__] 宏")
#else
//如果没有自定义的例程，则使用默认的，同时需要链接所需的 lib
#define __REQUIRE_DEFAULT_LIB__
//#pragma comment(linker, "/EXPORT:DllInitialize=_DefaultDllInitialize@4,PRIVATE")
#pragma EXPORT(EXPORT_DefaultDllInitialize)
#pragma message("如果有自定义的 [DllInitialize] 例程,则定义 [__DllInitialize__] 宏")
#endif

#ifdef __DllUnload__
//优先使用自定义的例程
//#pragma comment(linker, "/EXPORT:DllUnload=_DllUnload@0,PRIVATE")
#pragma EXPORT(EXPORT_DllUnload)
#pragma message("如果没有自定义的 [DllUnload] 例程,则不要定义 [__DllUnload__] 宏")
#else
//如果没有自定义的例程，则使用默认的，同时需要链接所需的 lib
#define __REQUIRE_DEFAULT_LIB__

//#pragma comment(linker, "/EXPORT:DllUnload=_DefaultDllUnload@0,PRIVATE")
#pragma EXPORT(EXPORT_DefaultDllUnload)
#pragma message("如果有自定义的 [DllUnload] 例程,则定义 [__DllUnload__] 宏")
#endif

/*
默认入口 _GsDriverEntry@8 中引用了 DriverEntry.
如果当前模块作为其他主模块的引用模块，则入口函数永远不会调用。
但是为了保证编译通过，所以必须要加上。
如果是dll项目且不想自定义 DriverEntry 入口例程，则直接使用 DefaultDriverEntry 宏 生成默认 DriverEntry 函数
其他情况请自定义 DriverEntry 且不要使用 DefaultDriverEntry 宏
例如：某些sys 也希望导出符号作为其他主模块的引用模块，但是由于一般 sys 会有自己的 DriverEntry,所以就不能再使用这个宏生成默认的DriverEntry
*/

#ifdef __DriverEntry__
#pragma message("如果没有自定义的 [DriverEntry] 例程，则不要定义 [__DriverEntry__] 宏")
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
#pragma message("如果有自定义的 [DriverEntry] 例程，则定义 [__DriverEntry__] 宏")
#endif

#ifdef __REQUIRE_DEFAULT_LIB__
	/*
	如果定义了这个宏，则需要引入默认的lib
	链接器选项 /nodefaultlib 将导致这个杂注被忽略
	此时需要去掉 /nodefaultlib 或者手动添加 默认 lib
	*/

#pragma message(\
">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>KernelDll Warning<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"\
"需要引入默认的 [ KernelDll.lib ]."\
"引入lib使用杂注命令:[ #pragma comment(lib,__DEFAULT_LIB__) ]\n"\
"链接器选项 [ /nodefaultlib ] 将导致这个杂注被忽略\n"\
"如果提示找不到符号，则需要去掉 [ /nodefaultlib ] 或者手动添加 [ KernelDll.lib ].\n"\
)

	//#pragma comment(lib,__DEFAULT_LIB__)
#pragma ADDLIB(__DEFAULT_LIB__)

#endif// end of __REQUIRE_DEFAULT_LIB__

#endif//end of __KernelDll_Self__

#ifdef __cplusplus
}
#endif
