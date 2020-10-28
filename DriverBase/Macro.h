#pragma once
//该头文件主要用于定义一些公共宏

#ifdef __DriverBase_Export__
#define DriverBaseAPI __declspec(dllexport)
#else
#define DriverBaseAPI __declspec(dllimport)
#endif

