#include "NetworkServices.h"
#include "ISocket.h"
#include "Address.h"
#include "SharedNetworkConfiguration.h"

#include <chrono>

namespace octet
{
	using namespace networking;
	using namespace std::chrono;

	/// Scene containing a box with octet.
	class networkingApp : public app
	{
		// scene for drawing box
		ref<visual_scene> app_scene;
		IConnection* serverConnection;
		bool connected = false;
		time_point<high_resolution_clock> startTime;
		time_point<high_resolution_clock> lastTime;

	public:
		/// this is called when we construct the class before everything is initialised.
		networkingApp(int argc, char **argv) : app(argc, argv)
		{
		}

		/// this is called once OpenGL is initialized
		void app_init()
		{
			startTime = high_resolution_clock::now();
			lastTime = startTime;

			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();

			material *red = new material(vec4(1, 0, 0, 1));
			mesh_box *box = new mesh_box(vec3(4));
			scene_node *node = new scene_node();
			app_scene->add_child(node);
			app_scene->add_mesh_instance(new mesh_instance(node, box, red));

			InitialiseConnectionToServer();
		}

		void InitialiseConnectionToServer()
		{
			serverConnection = NetworkServices::GetInstance().CreateConnection(ProtocolId, TimeOut);

			if (!serverConnection->Start(ClientPort))
			{
				printf("could not start connection on port %d\n", ClientPort);
				return;
			}

			serverConnection->Connect(Address(127, 0, 0, 1, ServerPort));
		}


		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h)
		{
			update();

			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);

			// tumble the box  (there is only one mesh instance)
			scene_node *node = app_scene->get_mesh_instance(0)->get_node();
			node->rotate(1, vec3(1, 0, 0));
			node->rotate(1, vec3(0, 1, 0));
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
			if (!connected && serverConnection->IsConnected())
			{
				printf("client connected to server\n");
				connected = true;
			}

			if (!connected && serverConnection->ConnectFailed())
			{
				printf("connection failed\n");
			}

			unsigned char packet[] = "client to server";
			serverConnection->SendPacket(packet, sizeof(packet));

			while (true)
			{
				unsigned char packet[256];
				int bytes_read = serverConnection->ReceivePacket(packet, sizeof(packet));
				if (bytes_read == 0)
					break;
				printf("received packet from server\n");
			}

			serverConnection->Update(deltaTime);
		}
	};
}
