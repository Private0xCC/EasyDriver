#pragma once


#ifdef __cplusplus
#include "KernelCRuntime.h"

/// <summary>
/// C++��׼�� ������ delete ���ں�ʵ�֣��ͷ��ڴ���е��ڴ�
/// </summary>
/// <param name="_Blokc">���ͷŵ��ڴ��</param>
/// <param name="Size">���ͷŵ��ڴ���С</param>
void __CRTDECL operator delete(
	void* _Block,
	size_t Size
	);

void __CRTDECL operator delete[](
	void* _Block
	);

void __CRTDECL operator delete[](
	void* _Block,
	size_t Size
	);
#endif
