#pragma once
#include <wdm.h>

typedef class MinorFunction* PMinorFunction;

typedef class MajorFunction
{
	
public:
	static MajorFunction _MajorFunctions[IRP_MJ_MAXIMUM_FUNCTION + 1];
	UCHAR MajorFunctionCode;
	PCCHAR MajorFunctionName;
	UCHAR MaxMinorFunctionCode;
	PMinorFunction MinorFunctions;
}*PMajorFunction;

typedef class MinorFunction
{
public:
	static MinorFunction _PNP[IRP_MN_DEVICE_ENUMERATED + 1];
	PMajorFunction BelongMajorFunction;
	UCHAR MinorFunctionCode;
	PCCHAR MinorFunctionName;
}* PMinorFunction;

void GetFunctionName(PCCHAR*mjName, PCCHAR* mnName,UCHAR mj, UCHAR mn);

