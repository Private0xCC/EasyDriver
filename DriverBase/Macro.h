#pragma once
//该头文件主要用于定义一些公共宏

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

