#pragma once
#include "ISocket.h"
namespace networking
{
	class INetworkService
	{
	public:
		enum SocketType
		{
			UDP,
			TCP
		};
		virtual ISocket* CreateSocket(SocketType sockType = INetworkService::SocketType::UDP) = 0;
	};
}

