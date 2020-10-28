#pragma once

#ifndef TCPIPFILTER_NO_PROMPT

#pragma message(\
"[TCPIPFilter.lib] 依赖于:\n"\
"[uuid.lib] [fwpkclnt.lib]"\
"\n请去掉 链接器 /NODEFAULTLIB 选项，或者手动添加上述依赖项.\n"\
"隐藏该提示请在包含 Minifilter.h 前定义 [TCPIPFILTER_NO_PROMPT]\n"\
)

#endif // MINIFILTERQUICKSTART_NO_PROMPT





#define NDIS683
#include <ntddk.h>
#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union
#include <fwpsk.h>
#pragma warning(pop)
#include <fwpmk.h>

#include <wdm.h>
#include <ws2def.h>


namespace TCPIPFilter
{
	enum class Operation
	{
		UnknowOperation = 0,

		/// <summary>
		/// 绑定本地端口
		/// </summary>
		Bind,

		/// <summary>
		/// 监听端口
		/// </summary>
		Listen,

		/// <summary>
		/// 接受连接
		/// </summary>
		Accept,

		/// <summary>
		/// 发起连接
		/// </summary>
		Connect,

		/// <summary>
		/// 已建立连接
		/// </summary>
		Connected,

		/// <summary>
		/// 传输层 发送数据
		/// </summary>
		Send,

		/// <summary>
		/// 传输层 接收数据
		/// </summary>
		Recv,

		/// <summary>
		/// 断开连接
		/// </summary>
		Disconnect,

		/// <summary>
		/// 解绑本地端口
		/// </summary>
		Unbind,

		OperationMax
	};

	union IPAddress
	{
		UINT32 V4;
		UINT8 V6[16];
	};



	typedef class TCPIP_CONTEXT
	{
	public:
		ULONGLONG ProcessId;
		Operation Operation;
		FWP_DIRECTION Direction;
		IPPROTO Protocol;
		ADDRESS_FAMILY AddressFamily;
		IPAddress LocalAddress;
		UINT16 LocalPort;
		IPAddress RemoteAddress;
		UINT16 RemotePort;

	}*PTCPIP_CONTEXT,&RTCPIP_CONTEXT;
	typedef const TCPIP_CONTEXT* PCTCPIP_CONTEXT;
	typedef const RTCPIP_CONTEXT RCTCPIP_CONTEXT;

	typedef bool (TCPIP_FILTING)(RCTCPIP_CONTEXT);
	typedef TCPIP_FILTING* PTCPIP_FILTING;

	NTSTATUS StartFilting(
		_In_ PDEVICE_OBJECT DeviceObject,
		_In_ TCPIP_FILTING TcpipFilting
	);

	void StopFilting();
}


