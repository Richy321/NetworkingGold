#pragma once
#include "INetworkService.h"
#include "NetworkServices.h"

#if PLATFORM == PLATFORM_UNIX
	#include <sys/socket.h> 
	#include <netinet/in.h> 
	#include <fcntl.h>
#endif
namespace networking
{
	class NetworkServicesUnix : public INetworkService
	{
	public:

		NetworkServicesUnix()
		{
		}

		~NetworkServicesUnix()
		{
		}

		ISocket* CreateSocket(SocketType sockType)
		{
			//return new WinSocket();
		}
	};
}

