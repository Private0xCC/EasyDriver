#pragma once

#include "KernelCRuntime.h"

#include <ntdef.h>
#include <wdm.h>

#define POOL_TAG 'prvt'

EXTERN_C_START

/// <summary>
/// C标准库中的 malloc 的内核实现，从内存池中分配内存
/// </summary>
/// <param name="_Size">分配大小</param>
/// <returns></returns>
void* __CRTDECL malloc(
	size_t _Size
);

/// <summary>
/// C标准库中的 free 的内核实现，释放内存池中的内存
/// </summary>
/// <param name="_Block">待释放的内存块</param>
void __CRTDECL free(
	void* _Block
);


EXTERN_C_END

/// <summary>
/// 内核模式拓展实现 malloc,从内存池中分配指定类型内存
/// </summary>
/// <param name="_Size">分配大小</param>
/// <param name="PoolType">池内存类型</param>
/// <returns></returns>
void* __CRTDECL malloc(
	size_t _Size,
	POOL_TYPE PoolType
);
