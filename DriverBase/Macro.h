#pragma once
//��ͷ�ļ���Ҫ���ڶ���һЩ������

#define __TAG__ 'chen'

#ifdef __cplusplus
#define CStyle extern "C"
#else
#define CStyle
#endif

#ifdef __DriverBase_Export__
#define DriverBaseAPI __declspec(dllexport)
#else
#define DriverBaseAPI __declspec(dllimport)
#endif

