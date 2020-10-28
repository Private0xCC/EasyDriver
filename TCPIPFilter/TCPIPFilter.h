#pragma once

#ifndef TCPIPFILTER_NO_PROMPT

#pragma message(\
"[TCPIPFilter.lib] ������:\n"\
"[uuid.lib] [fwpkclnt.lib]"\
"\n��ȥ�� ������ /NODEFAULTLIB ѡ������ֶ��������������.\n"\
"���ظ���ʾ���ڰ��� Minifilter.h ǰ���� [TCPIPFILTER_NO_PROMPT]\n"\
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
		/// �󶨱��ض˿�
		/// </summary>
		Bind,

		/// <summary>
		/// �����˿�
		/// </summary>
		Listen,

		/// <summary>
		/// ��������
		/// </summary>
		Accept,

		/// <summary>
		/// ��������
		/// </summary>
		Connect,

		/// <summary>
		/// �ѽ�������
		/// </summary>
		Connected,

		/// <summary>
		/// ����� ��������
		/// </summary>
		Send,

		/// <summary>
		/// ����� ��������
		/// </summary>
		Recv,

		/// <summary>
		/// �Ͽ�����
		/// </summary>
		Disconnect,

		/// <summary>
		/// ��󱾵ض˿�
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


