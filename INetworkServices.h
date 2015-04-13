#pragma once
class INetworkServices
{
public:
	virtual ~INetworkServices() {}

	virtual int sendMessage() = 0;
	virtual int receiveMessage() = 0;
};

