#pragma once
#include "INetworkServices.h"
#include <WinSock2.h>
#include <Windows.h>

class NetworkServicesWindows : INetworkServices
{
public:


	virtual int sendMessage() = 0;
	virtual int receiveMessage() = 0;


	void Initialise()
	{
		WSADATA wsaData;;
	
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		{
			//printf(stderr, "WSAStartup failed.\n");
		}
	}

	NetworkServicesWindows()
	{
	}

	~NetworkServicesWindows()
	{
	}
};

