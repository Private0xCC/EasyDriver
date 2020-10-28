#define TCPIPFILTER_NO_PROMPT
#include "TCPIPFilter.h"

#define INITGUID
#include <guiddef.h>

#pragma comment(lib,"uuid.lib")
#pragma comment(lib,"fwpkclnt.lib")

#define FAILED_BREAK() if (!NT_SUCCESS(status)){break;}


namespace TCPIPFilter
{
#define _FILTER(layerKey) {{},{L""},FWPM_FILTER_FLAG_NONE,nullptr,{},(layerKey),{},\
{ FWP_EMPTY },0,nullptr,{ FWP_ACTION_CALLOUT_TERMINATING,0 },0,\
nullptr,0,{}}

	/// <summary>
	/// 当前会话所打开的过滤引擎句柄
	/// </summary>
	static HANDLE Engine;

	/// <summary>
	/// 当前会话
	/// </summary>
	static FWPM_SESSION Session = {};

	static const GUID* LayerKeys[] = {
		&FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4,//bind()
		&FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V6,
		&FWPM_LAYER_ALE_AUTH_LISTEN_V4,//TCP listen()
		&FWPM_LAYER_ALE_AUTH_LISTEN_V6,
		&FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4,// TCP accept() / non-TCP first recv()
		&FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6,
		&FWPM_LAYER_ALE_AUTH_CONNECT_V4,// TCP connect() / non-TCP first send()
		&FWPM_LAYER_ALE_AUTH_CONNECT_V6,
		&FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4,
		&FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6,
		&FWPM_LAYER_INBOUND_TRANSPORT_V4,
		&FWPM_LAYER_INBOUND_TRANSPORT_V6,
		&FWPM_LAYER_OUTBOUND_TRANSPORT_V4,
		&FWPM_LAYER_OUTBOUND_TRANSPORT_V6,
		&FWPM_LAYER_ALE_RESOURCE_RELEASE_V4,
		&FWPM_LAYER_ALE_RESOURCE_RELEASE_V6,
		&FWPM_LAYER_ALE_ENDPOINT_CLOSURE_V4,
		&FWPM_LAYER_ALE_ENDPOINT_CLOSURE_V6
	};

	/// <summary>
	/// 需要添加到过滤引擎中的过滤器
	/// </summary>
	static FWPM_FILTER Filters[sizeof(LayerKeys) / sizeof(LayerKeys[0])] = {
	};



	DEFINE_GUID(EMPTY_GUID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	/// <summary>
	/// 所有过滤器都使用这一个子层
	/// </summary>
	static GUID SubLayerKey;

	static PTCPIP_FILTING Filting;

	/// <summary>
	/// 根据当前系统时间生成一个GUID
	/// </summary>
	/// <param name="guid">接收新生成的GUID</param>
	static void GenerateGUID(
		_Out_ GUID& guid
	);

	static NTSTATUS SubLayerAdd(
		_Inout_opt_ GUID* subLayerKey,
		_Out_opt_ UINT32* SubLayerId = nullptr
	);

	/// <summary>
	/// 注册一个 Callout
	/// </summary>
	/// <param name="DeviceObject"></param>
	/// <param name="CalloutKey">
	/// 如果不为空，且不等于 EMPTY_GUID，则使用 CalloutKey 指定的 GUID 进行注册。
	/// 如果等于 EMPTY_GUID，则使用生成的 GUID 进行注册,并通过该参数进行返回生成的 GUID。
	/// 如果为空，则使用生成的 GUID 进行注册。
	/// </param>
	/// <param name="CalloutId">返回运行时Id</param>
	/// <returns></returns>
	static NTSTATUS CalloutRegister(
		_In_ PDEVICE_OBJECT DeviceObject,
		_Inout_opt_ GUID* CalloutKey,
		_Out_opt_ UINT32* CalloutId = nullptr
	);

	static NTSTATUS CalloutAdd(
		_In_ GUID& CalloutKey,
		_In_ GUID& ApplicableLayer,
		_Out_opt_ UINT32* Id = nullptr
	);


	static NTSTATUS FilterAdd(
		_Inout_ FWPM_FILTER& Filter,
		_In_ PDEVICE_OBJECT DeviceObject
	);

	static void FilterDelete(
		_Inout_ FWPM_FILTER& Filter
	);

	static const FWPM_FILTER& GetFilterByFilterId(
		_In_ UINT64 FilterId
	);

	NTSTATUS Init(
		_In_ PDEVICE_OBJECT DeviceObject
	);

	void Deinit();

#pragma region Routine
	void NTAPI CalloutClassify3(
		const FWPS_INCOMING_VALUES0* inFixedValues,
		const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
		void* layerData,
		const void* classifyContext,
		const FWPS_FILTER3* filter,
		UINT64 flowContext,
		FWPS_CLASSIFY_OUT0* classifyOut
	)
	{
		inFixedValues; inMetaValues; layerData; classifyContext; filter; flowContext; classifyOut;

		if (!(classifyOut->rights & FWPS_RIGHT_ACTION_WRITE))
		{
			return;
		}

		classifyOut->actionType = FWP_ACTION_PERMIT;//放行

		if (Filting)
		{
			

			do
			{
				const FWPM_FILTER& Filter = GetFilterByFilterId(filter->filterId);

				TCPIP_CONTEXT context = {};

				if (inMetaValues->currentMetadataValues & FWPS_METADATA_FIELD_PROCESS_ID)
				{
					context.ProcessId = inMetaValues->processId;
				}
				else
				{
					context.ProcessId = ULONGLONG(PsGetCurrentProcessId());
				}
				

				if (Filter.layerKey == FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4 || Filter.layerKey == FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V6)
				{
					context.Operation = Operation::Bind;

					if (Filter.layerKey == FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4)
					{
						context.AddressFamily = AF_INET;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_PROTOCOL].value.uint8);
						context.LocalAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_ADDRESS].value.uint32;
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_PORT].value.uint16;
					}
					else
					{
						context.AddressFamily = AF_INET6;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_PROTOCOL].value.uint8);

						if (inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_ADDRESS].value.byteArray16)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_ADDRESS].value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}

						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V6_IP_LOCAL_PORT].value.uint16;
					}
				}
				else if (Filter.layerKey == FWPM_LAYER_ALE_AUTH_LISTEN_V4 || Filter.layerKey == FWPM_LAYER_ALE_AUTH_LISTEN_V6)
				{
					context.Operation = Operation::Listen;
					//FWPM_LAYER_ALE_AUTH_LISTEN_V4 只针对 TCP listen
					context.Protocol = IPPROTO::IPPROTO_TCP;

					if (Filter.layerKey == FWPM_LAYER_ALE_AUTH_LISTEN_V4)
					{
						context.AddressFamily = AF_INET;
						context.LocalAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_LISTEN_V4_IP_LOCAL_ADDRESS].value.uint32;
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_LISTEN_V4_IP_LOCAL_PORT].value.uint16;
					}
					else
					{
						context.AddressFamily = AF_INET6;
						if (inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_LISTEN_V6_IP_LOCAL_ADDRESS].value.byteArray16)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_LISTEN_V6_IP_LOCAL_ADDRESS].value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_LISTEN_V6_IP_LOCAL_PORT].value.uint16;
					}
				}
				else if (Filter.layerKey == FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4 || Filter.layerKey == FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6)
				{
					context.Operation = Operation::Accept;

					if (Filter.layerKey == FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4)
					{
						context.AddressFamily = AF_INET;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_PROTOCOL].value.uint8);
						context.LocalAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_LOCAL_ADDRESS].value.uint32;
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_LOCAL_PORT].value.uint16;
						context.RemoteAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_REMOTE_ADDRESS].value.uint32;
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_REMOTE_PORT].value.uint16;
					}
					else
					{
						context.AddressFamily = AF_INET6;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_PROTOCOL].value.uint8);
						if (inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_LOCAL_ADDRESS].value.byteArray16)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_LOCAL_ADDRESS].value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_LOCAL_PORT].value.uint16;
						if (inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_REMOTE_ADDRESS].value.byteArray16)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_REMOTE_ADDRESS].value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_REMOTE_PORT].value.uint16;
					}

					if (context.Protocol != IPPROTO::IPPROTO_TCP)
					{
						context.Operation = Operation::Recv;
					}
				}
				else if (Filter.layerKey == FWPM_LAYER_ALE_AUTH_CONNECT_V4 || Filter.layerKey == FWPM_LAYER_ALE_AUTH_CONNECT_V6)
				{
					context.Operation = Operation::Connect;

					if (Filter.layerKey == FWPM_LAYER_ALE_AUTH_CONNECT_V4)
					{
						context.AddressFamily = AF_INET;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL].value.uint8);
						context.LocalAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS].value.uint32;
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_PORT].value.uint16;
						context.RemoteAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS].value.uint32;
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_PORT].value.uint16;
					}
					else
					{
						context.AddressFamily = AF_INET6;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_PROTOCOL].value.uint8);
						if (inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_ADDRESS].value.byteArray16)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_ADDRESS].value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_PORT].value.uint16;
						if (inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_ADDRESS].value.byteArray16)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_ADDRESS].value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_PORT].value.uint16;
					}

					if (context.Protocol != IPPROTO::IPPROTO_TCP)
					{
						context.Operation = Operation::Send;
					}
				}
				else if (Filter.layerKey == FWPM_LAYER_ALE_RESOURCE_RELEASE_V4 || Filter.layerKey == FWPM_LAYER_ALE_RESOURCE_RELEASE_V6)
				{
					context.Operation = Operation::Unbind;

					if (Filter.layerKey == FWPM_LAYER_ALE_RESOURCE_RELEASE_V4)
					{
						context.AddressFamily = AF_INET6;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_RELEASE_V4_IP_PROTOCOL].value.uint8);
						context.LocalAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_RELEASE_V4_IP_LOCAL_ADDRESS].value.uint32;
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_RELEASE_V4_IP_LOCAL_PORT].value.uint16;
					}
					else
					{
						context.AddressFamily = AF_INET6;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_RELEASE_V6_IP_PROTOCOL].value.uint8);

						FWPS_INCOMING_VALUE0& LocalAddress = inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_RELEASE_V6_IP_LOCAL_ADDRESS];
						if (LocalAddress.value.type == FWP_DATA_TYPE::FWP_BYTE_ARRAY16_TYPE)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								LocalAddress.value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						else
						{
							//KdBreakPoint();
							break;
						}

						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_RESOURCE_RELEASE_V6_IP_LOCAL_PORT].value.uint16;
					}

				}
				else if (Filter.layerKey == FWPM_LAYER_ALE_ENDPOINT_CLOSURE_V4 || Filter.layerKey == FWPM_LAYER_ALE_ENDPOINT_CLOSURE_V6)
				{
					context.Operation = Operation::Disconnect;

					if (Filter.layerKey == FWPM_LAYER_ALE_ENDPOINT_CLOSURE_V4)
					{
						context.AddressFamily = AF_INET;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V4_IP_PROTOCOL].value.uint8);

						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V4_IP_LOCAL_PORT].value.uint16;

						context.RemotePort = inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V4_IP_REMOTE_PORT].value.uint16;

						context.LocalAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V4_IP_LOCAL_ADDRESS].value.uint32;
						context.RemoteAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V4_IP_REMOTE_ADDRESS].value.uint32;
					}
					else
					{
						context.AddressFamily = AF_INET6;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V6_IP_PROTOCOL].value.uint8);
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V6_IP_LOCAL_PORT].value.uint16;
						context.RemotePort = inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V4_IP_REMOTE_PORT].value.uint16;

						FWPS_INCOMING_VALUE0& LocalAddress = inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V6_IP_LOCAL_ADDRESS];
						FWPS_INCOMING_VALUE0& RemoteAddress = inFixedValues->incomingValue[FWPS_FIELD_ALE_ENDPOINT_CLOSURE_V6_IP_REMOTE_ADDRESS];
						if (LocalAddress.value.type == FWP_DATA_TYPE::FWP_BYTE_ARRAY16_TYPE && RemoteAddress.value.type == FWP_DATA_TYPE::FWP_BYTE_ARRAY16_TYPE)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								LocalAddress.value.byteArray16,
								sizeof(context.LocalAddress.V6));
							RtlCopyMemory(context.LocalAddress.V6,
								RemoteAddress.value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						else
						{
							//KdBreakPoint();
							break;
						}
					}

				}
				else if (Filter.layerKey == FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4 || Filter.layerKey == FWPM_LAYER_ALE_FLOW_ESTABLISHED_V6)
				{
					context.Operation = Operation::Connected;

					if (Filter.layerKey == FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4)
					{
						context.AddressFamily = AF_INET;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_PROTOCOL].value.uint8);

						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_PORT].value.uint16;

						context.RemotePort = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_PORT].value.uint16;

						context.LocalAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS].value.uint32;
						context.RemoteAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_ADDRESS].value.uint32;
					}
					else
					{
						context.AddressFamily = AF_INET6;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_PROTOCOL].value.uint8);
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_PORT].value.uint16;
						context.RemotePort = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_PORT].value.uint16;

						FWPS_INCOMING_VALUE0& LocalAddress = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_LOCAL_ADDRESS];
						FWPS_INCOMING_VALUE0& RemoteAddress = inFixedValues->incomingValue[FWPS_FIELD_ALE_FLOW_ESTABLISHED_V6_IP_REMOTE_ADDRESS];
						if (LocalAddress.value.type == FWP_DATA_TYPE::FWP_BYTE_ARRAY16_TYPE && RemoteAddress.value.type == FWP_DATA_TYPE::FWP_BYTE_ARRAY16_TYPE)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								LocalAddress.value.byteArray16,
								sizeof(context.LocalAddress.V6));
							RtlCopyMemory(context.LocalAddress.V6,
								RemoteAddress.value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						else
						{
							//KdBreakPoint();
							break;
						}
					}
				}
				else if (Filter.layerKey == FWPM_LAYER_INBOUND_TRANSPORT_V4 || Filter.layerKey == FWPM_LAYER_INBOUND_TRANSPORT_V6)
				{
					context.Operation = Operation::Recv;

					if (Filter.layerKey == FWPM_LAYER_INBOUND_TRANSPORT_V4)
					{
						context.AddressFamily = AF_INET;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_PROTOCOL].value.uint8);

						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_LOCAL_PORT].value.uint16;

						context.RemotePort = inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_REMOTE_PORT].value.uint16;

						context.LocalAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_LOCAL_ADDRESS].value.uint32;
						context.RemoteAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_REMOTE_ADDRESS].value.uint32;
					}
					else
					{
						context.AddressFamily = AF_INET6;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_PROTOCOL].value.uint8);
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_LOCAL_PORT].value.uint16;
						context.RemotePort = inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_REMOTE_PORT].value.uint16;

						FWPS_INCOMING_VALUE0& LocalAddress = inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_LOCAL_ADDRESS];
						FWPS_INCOMING_VALUE0& RemoteAddress = inFixedValues->incomingValue[FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_REMOTE_ADDRESS];
						if (LocalAddress.value.type == FWP_DATA_TYPE::FWP_BYTE_ARRAY16_TYPE && RemoteAddress.value.type == FWP_DATA_TYPE::FWP_BYTE_ARRAY16_TYPE)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								LocalAddress.value.byteArray16,
								sizeof(context.LocalAddress.V6));
							RtlCopyMemory(context.LocalAddress.V6,
								RemoteAddress.value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						else
						{
							//KdBreakPoint();
							break;
						}
					}
				}
				else if (Filter.layerKey == FWPM_LAYER_OUTBOUND_TRANSPORT_V4 || Filter.layerKey == FWPM_LAYER_OUTBOUND_TRANSPORT_V6)
				{
					context.Operation = Operation::Send;

					if (Filter.layerKey == FWPM_LAYER_OUTBOUND_TRANSPORT_V4)
					{
						context.AddressFamily = AF_INET;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_PROTOCOL].value.uint8);

						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_LOCAL_PORT].value.uint16;

						context.RemotePort = inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_REMOTE_PORT].value.uint16;

						context.LocalAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_LOCAL_ADDRESS].value.uint32;
						context.RemoteAddress.V4 = inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_REMOTE_ADDRESS].value.uint32;
					}
					else
					{
						context.AddressFamily = AF_INET6;
						context.Protocol = IPPROTO(inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_PROTOCOL].value.uint8);
						context.LocalPort = inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_LOCAL_PORT].value.uint16;
						context.RemotePort = inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_REMOTE_PORT].value.uint16;

						FWPS_INCOMING_VALUE0& LocalAddress = inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_LOCAL_ADDRESS];
						FWPS_INCOMING_VALUE0& RemoteAddress = inFixedValues->incomingValue[FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_REMOTE_ADDRESS];
						if (LocalAddress.value.type == FWP_DATA_TYPE::FWP_BYTE_ARRAY16_TYPE && RemoteAddress.value.type == FWP_DATA_TYPE::FWP_BYTE_ARRAY16_TYPE)
						{
							RtlCopyMemory(context.LocalAddress.V6,
								LocalAddress.value.byteArray16,
								sizeof(context.LocalAddress.V6));
							RtlCopyMemory(context.LocalAddress.V6,
								RemoteAddress.value.byteArray16,
								sizeof(context.LocalAddress.V6));
						}
						else
						{
							//KdBreakPoint();
							break;
						}
					}
				}
				else
				{
					KdBreakPoint();
					ASSERT(false);
				}
				__try
				{
					if (!Filting(context))
					{
						classifyOut->actionType = FWP_ACTION_BLOCK;
					}
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					classifyOut->actionType = FWP_ACTION_PERMIT;//放行
				}
			} while (false);

			
			
		}

		//清除FWPS_RIGHT_ACTION_WRITE标记
		if (filter->flags & FWPS_FILTER_FLAG_CLEAR_ACTION_RIGHT)
		{
			classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
		}
	}

	NTSTATUS NTAPI CalloutNotify3(FWPS_CALLOUT_NOTIFY_TYPE notifyType, const GUID* filterKey, FWPS_FILTER3* filter)
	{
		notifyType; filterKey; filter;
		return STATUS_SUCCESS;
	}

	void NTAPI CalloutFlowDeleteNotify0(UINT16 layerId, UINT32 calloutId, UINT64 flowContext)
	{
		layerId; calloutId; flowContext;
	}
#pragma endregion


	void GenerateGUID(
		GUID& guid
	)
	{
		static LARGE_INTEGER LAST = {};
		//根据系统时间生成GUID,俩次调用得到的结果有可能一样

		LARGE_INTEGER CurrentTime;
		KeQuerySystemTime(&CurrentTime);
		RtlCopyMemory(&guid, &CurrentTime, sizeof(CurrentTime));
		LARGE_INTEGER CurrentCount;

		do
		{
			KeQueryTickCount(&CurrentCount);
		} while (CurrentCount.QuadPart == LAST.QuadPart);

		LAST = CurrentCount;
		RtlCopyMemory(&guid.Data4, &CurrentCount, sizeof(CurrentCount));

	}

	NTSTATUS SubLayerAdd(
		GUID* subLayerKey,
		UINT32* SubLayerId
	)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		if (SubLayerKey == EMPTY_GUID && Engine != NULL)
		{
			FWPM_SUBLAYER mSubLayer = {};

			mSubLayer.flags = 0;
			mSubLayer.weight = UINT16(-1);

			mSubLayer.displayData.name = L"TCPIPFilterSubLayer";
			//mSubLayer.displayData.description = mSubLayer.displayData.name;


			if (subLayerKey)
			{
				mSubLayer.subLayerKey = *subLayerKey;
			}

			//动态生成的GUID可能会重复，所以重试几次
			unsigned int TryTimes = 3;
			if (mSubLayer.subLayerKey == EMPTY_GUID)
			{
				GenerateGUID(mSubLayer.subLayerKey);
			}
			else
			{
				TryTimes = 1;//调用者传递了有效的GUID,则不重试
			}


			while (TryTimes-- > 0)
			{
				status = FwpmSubLayerAdd(Engine, &mSubLayer, SubLayerId);
				if (status == STATUS_FWP_ALREADY_EXISTS)
				{
					GenerateGUID(mSubLayer.subLayerKey);
				}
				else
				{
					break;//其他错误码不重试
				}
			}

			if (NT_SUCCESS(status) && subLayerKey != nullptr)
			{
				*subLayerKey = mSubLayer.subLayerKey;
			}
		}

		return status;
	}

	DEFINE_GUID(CalloutKeyV4, 0xd969fc67, 0x6fb2, 0x4504, 0x91, 0xce, 0xa9, 0x7c, 0x3c, 0x32, 0xad, 0x36);

	NTSTATUS CalloutRegister(
		PDEVICE_OBJECT DeviceObject,
		GUID* CalloutKey,
		UINT32* CalloutId
	)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		if (Engine != NULL)
		{
			FWPS_CALLOUT sCallout = { };

			sCallout.flags = 0;
			sCallout.classifyFn = CalloutClassify3;
			sCallout.notifyFn = CalloutNotify3;
			sCallout.flowDeleteFn = CalloutFlowDeleteNotify0;

			if (CalloutKey)
			{
				sCallout.calloutKey = *CalloutKey;
			}
			unsigned int TryTimes = 3;
			if (sCallout.calloutKey == EMPTY_GUID)
			{
				GenerateGUID(sCallout.calloutKey);
			}
			else
			{
				TryTimes = 1;
			}

			while (TryTimes-- > 0)
			{
				status = FwpsCalloutRegister(DeviceObject, &sCallout, CalloutId);
				if (status == STATUS_FWP_ALREADY_EXISTS)
				{
					GenerateGUID(sCallout.calloutKey);
				}
				else
				{
					break;//其他错误码不重试
				}
			}

			if (NT_SUCCESS(status) && CalloutKey != nullptr)
			{
				//成功后返回GUID
				*CalloutKey = sCallout.calloutKey;
			}
		}

		return status;
	}

	NTSTATUS CalloutAdd(
		GUID& CalloutKey,
		GUID& ApplicableLayer,
		UINT32* Id
	)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		if (Engine != NULL)
		{
			FWPM_CALLOUT mCallout = {};

			mCallout.flags = 0;

			mCallout.displayData.name = L"TCPIPFilterCallout";

			mCallout.calloutKey = CalloutKey;
			mCallout.applicableLayer = ApplicableLayer;

			status = FwpmCalloutAdd(Engine, &mCallout, nullptr, Id);

			if (status == STATUS_FWP_ALREADY_EXISTS)
			{
				status = STATUS_SUCCESS;
			}
		}

		return status;
	}



	NTSTATUS FilterAdd(
		FWPM_FILTER& Filter,
		PDEVICE_OBJECT DeviceObject
	)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		if (Engine != NULL)
		{
			do
			{
				status = CalloutRegister(DeviceObject, &Filter.action.calloutKey);
				if (!NT_SUCCESS(status))
				{
					KdBreakPoint();
				}
				FAILED_BREAK();
				status = CalloutAdd(Filter.action.calloutKey, Filter.layerKey);
				if (!NT_SUCCESS(status))
				{
					KdBreakPoint();
				}
				FAILED_BREAK();

				status = FwpmFilterAdd(Engine, &Filter, nullptr, &Filter.filterId);
				if (!NT_SUCCESS(status))
				{
					KdBreakPoint();
				}
				FAILED_BREAK();
			} while (false);
		}

		return status;
	}

	void FilterDelete(
		FWPM_FILTER& Filter
	)
	{
		if (Filter.filterId)
		{
			FwpmFilterDeleteById(Engine, Filter.filterId);
			Filter.filterId = 0;
		}

		if (Filter.subLayerKey != EMPTY_GUID)
		{
			FwpmSubLayerDeleteByKey(Engine, &Filter.subLayerKey);
			Filter.subLayerKey = EMPTY_GUID;
		}

		if (Filter.action.calloutKey != EMPTY_GUID)
		{
			FwpmCalloutDeleteByKey(Engine, &Filter.action.calloutKey);
			FwpsCalloutUnregisterByKey(&Filter.action.calloutKey);
			Filter.action.calloutKey = EMPTY_GUID;
		}
	}

	const FWPM_FILTER& GetFilterByFilterId(UINT64 FilterId)
	{
		if (Engine != NULL)
		{
			for (size_t i = 0; i < sizeof(Filters) / sizeof(Filters[0]); i++)
			{
				FWPM_FILTER& Filter = Filters[i];
				if (Filter.filterId == FilterId)
				{
					return Filter;
				}
			}
		}
		return *(FWPM_FILTER*)nullptr;
	}


#pragma region 接口实现

	NTSTATUS Init(
		PDEVICE_OBJECT DeviceObject
	)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		if (Engine == NULL)
		{
			bool InTransaction = false;
			do
			{
				status = FwpmEngineOpen(nullptr, RPC_C_AUTHN_WINNT, nullptr, &Session, &Engine);
				FAILED_BREAK();

				status = FwpmTransactionBegin(Engine, 0);
				FAILED_BREAK();
				InTransaction = true;

				status = SubLayerAdd(&SubLayerKey);
				FAILED_BREAK();

				
				for (size_t i = 0; i < _ARRAYSIZE(Filters); i++)
				{
					FWPM_FILTER& Filter = Filters[i];
					Filter.layerKey = *LayerKeys[i];
					Filter.displayData.name = L"TCPIPFilter";
					Filter.flags = FWPM_FILTER_FLAG_NONE;
					Filter.weight.type = FWP_EMPTY;
					Filter.action.type = FWP_ACTION_CALLOUT_TERMINATING;
					Filter.numFilterConditions = 0;
					Filter.filterCondition = nullptr;

					Filter.subLayerKey = SubLayerKey;
					status = FilterAdd(Filter, DeviceObject);
					FAILED_BREAK();
				}
				FAILED_BREAK();

				status = FwpmTransactionCommit(Engine);
				FAILED_BREAK();

				InTransaction = false;
			} while (false);

			if (!NT_SUCCESS(status))
			{
				KdBreakPoint();
				if (InTransaction)
				{
					FwpmTransactionAbort(Engine);
				}
			}

		}

		return status;
	}

	void Deinit()
	{

		if (Engine != NULL)
		{
			if (SubLayerKey != EMPTY_GUID)
			{
				FwpmSubLayerDeleteByKey(Engine, &SubLayerKey);

				SubLayerKey = EMPTY_GUID;
			}

			for (size_t i = 0; i < _ARRAYSIZE(Filters); i++)
			{
				FWPM_FILTER& Filter = Filters[i];
				Filter.subLayerKey = EMPTY_GUID;
				FilterDelete(Filters[i]);
			}

			FwpmEngineClose(Engine);
			Engine = NULL;
		}

	}

	NTSTATUS StartFilting(PDEVICE_OBJECT DeviceObject, TCPIP_FILTING TcpipFilting)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;

		if (Engine == NULL)
		{
			Filting = TcpipFilting;
			status = Init(DeviceObject);
			if (!NT_SUCCESS(status))
			{
				Filting = nullptr;
			}
		}

		return status;
	}

	void StopFilting()
	{
		Deinit();
		Filting = nullptr;
	}

#pragma endregion


}