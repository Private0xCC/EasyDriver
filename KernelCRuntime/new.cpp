#ifdef __cplusplus

#include "new.h"
#include "malloc.h"

void* __CRTDECL operator new(
	size_t Size
	)
{
	return malloc(Size);
}

void* __CRTDECL operator new(
	size_t Size,
	POOL_TYPE PoolType
	)
{
	return malloc(Size, PoolType);
}

void* __CRTDECL operator new[](
	size_t Size
	)
{
	return operator new(Size);
}

void* __CRTDECL operator new[](
	size_t Size,
	POOL_TYPE PoolType
	)
{
	return operator new(Size, PoolType);
}

#endif
