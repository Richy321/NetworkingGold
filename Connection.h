#pragma once
#include "IConnection.h"
#include "INetworkService.h"
#include "SharedNetworkConfiguration.h"

using namespace networking;
/// A virtual connection for UDP
class Connection : public IConnection
{
public:

	static const int HeaderSize = 4;
	INetworkService& networkService;

	Connection(unsigned int protocolId, float timeout, INetworkService& networkServ) : IConnection(protocolId, timeout), networkService(networkServ)
	{
		this->protocolId = protocolId;
		this->timeout = timeout;
		mode = None;
		socket = networkService.CreateSocket();
		running = false;
		ClearData();
	}

	~Connection() 
	{
		if (running)
			Stop();
	}

	bool Start(int port)override
	{
		assert(!running);
		printf("start connection on port &d\n", port);
		if (!socket->Open(port))
			return false;
		running = true;
		return true;
	}

	void Listen() override
	{
		printf("server listening for connection\n");
		ClearData();
		mode = Server;
		state = Listening;
	}

	void Stop() override
	{
		assert(running);
		printf("stop connection\n");
		ClearData();
		socket->Close();
		running = false;
	}

	void Connect(const Address address) override
	{
		printf("client connecting to %d.%d.%d.%d:%d\n",
			address.GetA(), address.GetB(), address.GetC(), address.GetD(), address.GetPort());
		ClearData();
		mode = Client;
		state = Connecting;
		this->address = address;
	}

	//Accessors
	bool IsConnecting() const override { return state == Connecting; }
	bool ConnectFailed() const override { return state == ConnectFail; }
	bool IsConnected() const override { return state == Connected; }
	bool IsListening() const override { return state == Listening; }
	Mode GetMode() const override { return mode; }

	void Update(float deltaTime) override
	{
		assert(running);
		timeoutAccumulator += deltaTime;
		if (timeoutAccumulator > timeout)
		{
			if (state == Connecting)
			{
				printf("connect timed out\n");
				ClearData();
				state = ConnectFail;
			}
			else if (state == Connected)
			{
				printf("connection timed out\n");
				ClearData();
				if (state == Connecting)
					state = ConnectFail;
			}
		}
	}

	bool SendPacket(const unsigned char data[], const int size) override
	{
		assert(running);
		if (address.GetAddress() == 0)
			return false;
		unsigned char *packet;
		packet = new unsigned char[size + HeaderSize];

		packet[0] = (unsigned char)(protocolId >> 24);
		packet[1] = (unsigned char)((protocolId >> 16) & 0xFF);
		packet[2] = (unsigned char)((protocolId >> 8) & 0xFF);
		packet[3] = (unsigned char)((protocolId)& 0xFF);
		memcpy(&packet[4], data, size);
		return socket->Send(address, packet, size + HeaderSize);
	}

	int ReceivePacket(unsigned char data[], int size) override
	{
		assert(running);
		unsigned char* packet;
		packet = new unsigned char[size + HeaderSize];
		Address sender;
		int bytesRead = socket->Receive(sender, packet, size + HeaderSize);
		if (bytesRead == 0)
			return false;
		if (bytesRead <= HeaderSize)
			return 0;
		
		//check the packet header matches the protocolId
		if (packet[0] != (unsigned char)(protocolId >> 24) ||
			packet[1] != (unsigned char)((protocolId >> 16) & 0xFF) ||
			packet[2] != (unsigned char)((protocolId >> 8) & 0xFF) ||
			packet[3] != (unsigned char)(protocolId & 0xFF))
			return 0;

		if (mode == Server && !IsConnected())
		{
			printf("server accepts connection from client %d.%d.%d.%d:%d\n",
				sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), sender.GetPort());
			state = Connected;
			address = sender;
		}
		if (sender == address)
		{
			if (mode == Client && state == Connecting)
			{
				printf("client completes connection with server\n");
				state = Connected;
			}
			timeoutAccumulator = 0.0f;
			memcpy(data, &packet[HeaderSize], size - HeaderSize);
			return size - HeaderSize;
		}
		return 0;
	}
protected:
	void ClearData()
	{
		state = Disconnected;
		timeoutAccumulator = 0.0f;
		address = Address();
	}
private:
	enum State
	{
		Disconnected,
		Listening,
		Connecting,
		ConnectFail,
		Connected
	};

	unsigned int protocolId;
	float timeout;

	bool running;
	Mode mode;
	State state;
	ISocket* socket;
	float timeoutAccumulator;
	Address address;
};

