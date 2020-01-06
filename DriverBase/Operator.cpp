#include "Operator.h"
#include "Macro.h"
#include <wdm.h>

/*
内核中没有 new delete 操作符
自己重写，方便申请释放内存
*/
void* __cdecl operator new(size_t size)
{
	return operator new(size, PagedPool);
}


void* __cdecl operator new(size_t size, POOL_TYPE type/* = PagedPool*/)
{
	void* p = nullptr;
#ifdef DBG
	p = ExAllocatePoolWithTag(type, size, __TAG__);
#else
	p = ExAllocatePool(PagedPool, size);
#endif
	RtlZeroMemory(p, size);
	return p;
}

void __cdecl operator delete(void* addr, size_t size)
{
	UNREFERENCED_PARAMETER(size);
#ifdef DBG
	ExFreePoolWithTag(addr, __TAG__);
#else
	ExFreePool(addr);//ExFreePoolWithTag(addr, 0);
#endif
}

void* __cdecl operator new[](size_t size)
{
	return operator new(size, PagedPool);
}

void* __cdecl operator new[](size_t size, POOL_TYPE type)
{
	return operator new(size, type);
}

void __cdecl operator delete[](void* addr)
{
	operator delete(addr,0);
}

void __cdecl operator delete[](void* addr, size_t size)
{
	operator delete(addr, size);
}


