#pragma once
#include "ISocket.h"
#include "IConnection.h"

class Connection;

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
		virtual IConnection* CreateConnection(const int protocolId, const float timeout) = 0;
	};
}

