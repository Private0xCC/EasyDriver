#pragma once
#include <wdm.h>
//操作符无法导出

void* __cdecl operator new(size_t size);

void* __cdecl operator new(size_t size, POOL_TYPE type);

void __cdecl operator delete(void* addr, size_t size);

void* __cdecl operator new[](size_t size);

void* __cdecl operator new[](size_t size, POOL_TYPE type);

void __cdecl operator delete[](void* addr);

void __cdecl operator delete[](void* addr, size_t size);
