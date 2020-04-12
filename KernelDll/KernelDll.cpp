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
΢��ٷ��������ע�Ľ��ͣ�Places a comment record into an object file or executable file.
���ǰ���ע������д�뵽���ɵ� .obj ��

��������������� .Lib �ļ�:
https://docs.microsoft.com/zh-cn/previous-versions/ba1z7822%28v%3dvs.120%29

LINK ���� COFF ��׼��� COFF ����⣬������ͨ����������չ�� .lib��
��׼��������󣬲��� LIB ���ߴ�����
���������й����������е�������Ϣ��
�� LINK ���ɰ��������ĳ���ʱ������� LINK ���������� LIB ���ߴ�����
*/
/*
����Ŀ��Ϊ [��̬��] ��Ŀ,�����õ��� .lib �ļ����ڱ�׼�⣬û�е�����
������������������ʵ���ϲ�û����Ч��
���� .lib �ļ��ᱣ������������ȵ�������ģ����������dllʱ���������������Ч�ˡ�
����ͨ��dumpbin /all xxx.lib �鿴 lib ����Ϣ������ linker ���ֱ��������������
����������:
   Linker Directives
   -----------------
   /EXPORT:DllInitialize=_DllInitialize@4,PRIVATE
   /EXPORT:DllUnload=_DllUnload@0,PRIVATE

����ͨ�� .def�ļ� �ķ�ʽ�������������ţ����ܴﵽԤ�ڵ�Ч����
���.def�ļ�ʵ��������������/DEFѡ��,���Ǿ�̬�ⲻ��ʹ��������(����Ϊ��̬����Ŀ�󣬡��������� �������û��)

 #pragma comment(linker,"/EXPORT: xxx") ���ַ�ʽʵ���Ͼ����ڱ���ʱ����������Ҫ��ӵ�����д�뵽��Ӧ�� .obj �ļ���,���ձ��浽 .lib �ļ���
����ʱ�����������ӵ�ѡ�����������ʱ��

һ��extern "C" __stdcall ��������������ĺ�����
#define C_STDCALL_DECORATED(func,paramete_bytes) "_"###func##"@"###paramete_bytes

//���ڸ�Ϊ�ɺ���ƾ��嵼����Щ������Ϊ���̣�����.lib �̶�����������Ϊ���̣����������ɿ����߿�����ʹ���Զ�������̻���ʹ��Ĭ�ϵ�����
//#pragma comment(linker, "/EXPORT:DllInitialize=_DllInitialize@4,PRIVATE")
//#pragma comment(linker, "/EXPORT:DllUnload=_DllUnload@0,PRIVATE")


in .def file

sample:

EXPORTS
	;DllInitialize �� DllUnload ����ʹ�� PRIVATE,�� �÷��Ų������ڵ�����У�lib�ļ���,����ģ���޷�(��ʽ)���ӵ�����������(����dll�еĵ���������Ȼ�У�������ʽ����)
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
