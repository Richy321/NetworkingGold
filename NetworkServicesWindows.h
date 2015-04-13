#pragma once
#include "INetworkService.h"
#include "NetworkServices.h"
#include "WinSocket.h"

#if PLATFORM == PLATFORM_WINDOWS 
	#include <WinSock2.h>
	#include <Windows.h>
	#pragma comment( lib, "wsock32.lib" ) 
#endif

namespace networking
{
	class NetworkServicesWindows : public INetworkService
	{
	public:
		#pragma region Construction/Destruction
		NetworkServicesWindows()
		{
			assert(Initialise());
		}

		~NetworkServicesWindows()
		{
			CleanUp();
		}

		#pragma endregion

		bool Initialise()
		{
			WSADATA wsaData;
			return WSAStartup(MAKEWORD(2, 0), &wsaData) == NO_ERROR;
		}

		void CleanUp()
		{
			WSACleanup();
		}

		ISocket* CreateSocket(SocketType sockType)
		{
			return new WinSocket();
		}

	};
}

