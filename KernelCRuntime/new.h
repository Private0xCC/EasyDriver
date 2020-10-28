#pragma once

#ifdef __cplusplus
#include "KernelCRuntime.h"
#include <wdm.h>

/// <summary>
/// C++��׼ ������ new ���ں�ʵ�֣����ڴ���з��� NonPagedPool ���ڴ档
/// ����: int * pInt = new int;
/// </summary>
/// <param name="Size">����ռ��С</param>
/// <returns>���ڴ���з�����ڴ��</returns>
void* __CRTDECL operator new(
	size_t Size
	);

/// <summary>
/// �ò�����Ϊ�ں���չ�ķǱ�׼����������Ҫ������ͷ�ļ��ſ��������á�
/// ���ں��з��� PoolTypeָ�����͵� ���ڴ档
/// ����: int * pInt = new(POOL_TYPE::NonPagedPool) int;
/// </summary>
/// <param name="Size">����ռ��С</param>
/// <param name="PoolType">���ڴ�����</param>
/// <returns>���ڴ���з�����ڴ��</returns>
void* __CRTDECL operator new(
	size_t Size,
	POOL_TYPE PoolType
	);

/// <summary>
/// ����: int * Arr = new int[2];
/// </summary>
/// <param name="Size">����ռ��С</param>
/// <returns>���ڴ���з�����ڴ��</returns>
void* __CRTDECL operator new[](
	size_t Size
	);

/// <summary>
/// �ò�����Ϊ�ں���չ�ķǱ�׼����������Ҫ������ͷ�ļ��ſ��������á�
/// ����: int * Arr = new(POOL_TYPE::NonPagedPool) int[2];
/// </summary>
/// <param name="Size">����ռ��С</param>
/// <param name="PoolType">���ڴ�����</param>
/// <returns>���ڴ���з�����ڴ��</returns>
void* __CRTDECL operator new[](
	size_t Size,
	POOL_TYPE PoolType
	);

#endif


