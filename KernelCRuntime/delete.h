#pragma once


#ifdef __cplusplus
#include "KernelCRuntime.h"

/// <summary>
/// C++标准中 操作符 delete 的内核实现，释放内存池中的内存
/// </summary>
/// <param name="_Blokc">待释放的内存块</param>
/// <param name="Size">待释放的内存块大小</param>
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
