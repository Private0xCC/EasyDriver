#pragma once
//��ͷ�ļ���Ҫ���ڶ���һЩ������

#ifdef __DriverBase_Export__
#define DriverBaseAPI __declspec(dllexport)
#else
#define DriverBaseAPI __declspec(dllimport)
#endif

