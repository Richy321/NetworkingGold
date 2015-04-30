#pragma once

#include "../../octet.h"
#include "NetworkServices.h"
#include "SharedNetworkConfiguration.h"
#include "FlowControl.h"
#include <chrono>

using namespace std::chrono;
using namespace networking;

namespace octet
{
	class ServerApp : public app
	{
	public:
		IConnection* connection;
		time_point<high_resolution_clock> startTime;
		time_point<high_resolution_clock> lastTime;
		FlowControl flowControl;

		ServerApp(int argc, char **argv) : app(argc, argv)
		{
		}

		void app_init()
		{
			startTime = high_resolution_clock::now();
			lastTime = startTime;
			if(isUseReliableConnection)
				connection = NetworkServices::GetInstance().CreateReliableConnection(ProtocolId, TimeOut);
			else
				connection = NetworkServices::GetInstance().CreateConnection(ProtocolId, TimeOut);

			if (!connection->Start(ServerPort))
			{
				printf("could not start connection on port %d\n", ServerPort);
				return;
			}
			connection->Listen();
		}

		void draw_world(int x, int y, int w, int h)
		{
			update();
		}

		void update()
		{
			time_point<high_resolution_clock> nowTime = high_resolution_clock::now();
			duration<float> deltaDuration = duration_cast<duration<float>>(nowTime - lastTime);

			if (nowTime > lastTime + std::chrono::duration_cast<milliseconds>(milliseconds(CommsTickDurationMs)))
			{
				updateComms(deltaDuration.count());
				lastTime = nowTime;
			}
		}

		void updateComms(float deltaTime)
		{
			if (connection->IsConnected())
			{
				if(isUseReliableConnection)
					flowControl.Update(deltaTime, ((ReliableConnection*)connection)->GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

				unsigned char packet[] = "server to client";
				
				connection->SendPacket(packet, sizeof(packet));

			}

			//receive until theres no more packets left
			while (true)
			{
				unsigned char packet[256];
				int bytesRead = connection->ReceivePacket(packet, sizeof(packet));
				if (bytesRead == 0)
					break;
				printf("received packet from client\n");
			}

			connection->Update(deltaTime);
		}
	};
}


