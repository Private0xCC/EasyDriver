#pragma once

#ifdef __cplusplus
#include "KernelCRuntime.h"
#include <wdm.h>

/// <summary>
/// C++标准 操作符 new 的内核实现，从内存池中分配 NonPagedPool 池内存。
/// 例如: int * pInt = new int;
/// </summary>
/// <param name="Size">分配空间大小</param>
/// <returns>从内存池中分配的内存块</returns>
void* __CRTDECL operator new(
	size_t Size
	);

/// <summary>
/// 该操作符为内核拓展的非标准操作符，需要包含该头文件才可正常调用。
/// 在内核中分配 PoolType指定类型的 池内存。
/// 例如: int * pInt = new(POOL_TYPE::NonPagedPool) int;
/// </summary>
/// <param name="Size">分配空间大小</param>
/// <param name="PoolType">池内存类型</param>
/// <returns>从内存池中分配的内存块</returns>
void* __CRTDECL operator new(
	size_t Size,
	POOL_TYPE PoolType
	);

/// <summary>
/// 例如: int * Arr = new int[2];
/// </summary>
/// <param name="Size">分配空间大小</param>
/// <returns>从内存池中分配的内存块</returns>
void* __CRTDECL operator new[](
	size_t Size
	);

/// <summary>
/// 该操作符为内核拓展的非标准操作符，需要包含该头文件才可正常调用。
/// 例如: int * Arr = new(POOL_TYPE::NonPagedPool) int[2];
/// </summary>
/// <param name="Size">分配空间大小</param>
/// <param name="PoolType">池内存类型</param>
/// <returns>从内存池中分配的内存块</returns>
void* __CRTDECL operator new[](
	size_t Size,
	POOL_TYPE PoolType
	);

#endif


