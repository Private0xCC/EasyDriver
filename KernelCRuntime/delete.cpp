#ifdef __cplusplus

#include "delete.h"
#include "malloc.h"


void __CRTDECL operator delete(
	void* _Block,
	size_t Size
	)
{
	UNREFERENCED_PARAMETER(Size);
	free(_Block);
}


void __CRTDECL operator delete[](
	void* _Block
	)
{
	free(_Block);
}

void __CRTDECL operator delete[](
	void* _Block,
	size_t Size
	)
{
	operator delete(_Block, Size);
}

#endif
