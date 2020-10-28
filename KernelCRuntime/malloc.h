#pragma once

#include "KernelCRuntime.h"

#include <ntdef.h>
#include <wdm.h>

#define POOL_TAG 'prvt'

EXTERN_C_START

/// <summary>
/// C��׼���е� malloc ���ں�ʵ�֣����ڴ���з����ڴ�
/// </summary>
/// <param name="_Size">�����С</param>
/// <returns></returns>
void* __CRTDECL malloc(
	size_t _Size
);

/// <summary>
/// C��׼���е� free ���ں�ʵ�֣��ͷ��ڴ���е��ڴ�
/// </summary>
/// <param name="_Block">���ͷŵ��ڴ��</param>
void __CRTDECL free(
	void* _Block
);


EXTERN_C_END

/// <summary>
/// �ں�ģʽ��չʵ�� malloc,���ڴ���з���ָ�������ڴ�
/// </summary>
/// <param name="_Size">�����С</param>
/// <param name="PoolType">���ڴ�����</param>
/// <returns></returns>
void* __CRTDECL malloc(
	size_t _Size,
	POOL_TYPE PoolType
);
