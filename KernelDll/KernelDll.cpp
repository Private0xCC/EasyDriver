#define __KernelDll_Self__
#include "KernelDll.h"
#include <wdm.h>

#ifdef DBG
#define DebugInfo(x) DbgPrint x
#else
#define DebugInfo(x)
#endif


/*
comment pragma
#pragma comment

https://docs.microsoft.com/en-us/cpp/preprocessor/comment-c-cpp?redirectedfrom=MSDN&view=vs-2019
微软官方这这个杂注的解释：Places a comment record into an object file or executable file.
就是把杂注的内容写入到生成的 .obj 中

用作链接器输入的 .Lib 文件:
https://docs.microsoft.com/zh-cn/previous-versions/ba1z7822%28v%3dvs.120%29

LINK 接受 COFF 标准库和 COFF 导入库，这两者通常都具有扩展名 .lib。
标准库包含对象，并用 LIB 工具创建。
导入库包含有关其他程序中导出的信息。
当 LINK 生成包含导出的程序时导入库由 LINK 创建，或由 LIB 工具创建。
*/
/*
本项目作为 [静态库] 项目,编译后得到的 .lib 文件属于标准库，没有导出表
即下面两个导出命令实际上并没有生效。
但是 .lib 文件会保留这两个命令，等到和其他模块链接生成dll时，这两个命令就生效了。
可以通过dumpbin /all xxx.lib 查看 lib 的信息，其中 linker 部分保留了这两个命令。
像下面这样:
   Linker Directives
   -----------------
   /EXPORT:DllInitialize=_DllInitialize@4,PRIVATE
   /EXPORT:DllUnload=_DllUnload@0,PRIVATE

但是通过 .def文件 的方式导出这两个符号，则不能达到预期的效果。
添加.def文件实际上是链接器的/DEF选项,但是静态库不会使用链接器(设置为静态库项目后，【链接器】 设置项就没了)

 #pragma comment(linker,"/EXPORT: xxx") 这种方式实际上就是在编译时，把链接需要添加的命令写入到对应的 .obj 文件里,最终保存到 .lib 文件中
链接时给链接器增加的选项，发生在链接时。

一个extern "C" __stdcall 函数经过换名后的函数名
#define C_STDCALL_DECORATED(func,paramete_bytes) "_"###func##"@"###paramete_bytes

//现在改为由宏控制具体导出哪些函数作为例程，不在.lib 固定导出函数作为例程，更加灵活，可由开发者控制是使用自定义的例程还是使用默认的例程
//#pragma comment(linker, "/EXPORT:DllInitialize=_DllInitialize@4,PRIVATE")
//#pragma comment(linker, "/EXPORT:DllUnload=_DllUnload@0,PRIVATE")


in .def file

sample:

EXPORTS
	;DllInitialize 和 DllUnload 必须使用 PRIVATE,即 该符号不出现在导入库中（lib文件）,其他模块无法(隐式)链接到这两个符号(但是dll中的导出表中仍然有，可以显式链接)
	DllInitialize = DllInitialize PRIVATE
	DllUnload = DllUnload PRIVATE

*/
#ifdef __cplusplus
extern "C"
{
	
#endif

NTSTATUS NTAPI DefaultDllInitialize(IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DebugInfo(("%s\nRegistryPath : %wZ\n", __FUNCTION__, RegistryPath));
	
	return STATUS_SUCCESS;
}

NTSTATUS NTAPI DefaultDllUnload(VOID)
{
	DebugInfo(("%s\n", __FUNCTION__));
	
	return STATUS_SUCCESS;
}

#ifdef __cplusplus
}
#endif
