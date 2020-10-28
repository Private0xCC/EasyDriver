#include "malloc.h"

EXTERN_C_START

void* __CRTDECL malloc(
	size_t _Size
)
{
	return malloc(_Size, POOL_TYPE::NonPagedPool);
}

void __CRTDECL free(
	void* _Block
)
{
	ExFreePoolWithTag(_Block, POOL_TAG);
}

EXTERN_C_END


void* __CRTDECL malloc(
	size_t _Size,
	POOL_TYPE PoolType
)
{
	void* p = ExAllocatePoolWithTag(PoolType, _Size, POOL_TAG);
	if (p == nullptr)
	{
		KdBreakPoint();
	}
	ASSERT(p != nullptr);
	return p;
}
