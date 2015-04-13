#include "NetworkServices.h"
#include "ISocket.h"
#include "Address.h"

namespace octet 
{
  /// Scene containing a box with octet.
  class networkingApp : public app 
  {
    // scene for drawing box
    ref<visual_scene> app_scene;
	networking::ISocket* socket;
	int port = 30000;

  public:
    /// this is called when we construct the class before everything is initialised.
	  networkingApp(int argc, char **argv) : app(argc, argv)
	{
    }

    /// this is called once OpenGL is initialized
    void app_init() 
	{
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();

      material *red = new material(vec4(1, 0, 0, 1));
      mesh_box *box = new mesh_box(vec3(4));
      scene_node *node = new scene_node();
      app_scene->add_child(node);
      app_scene->add_mesh_instance(new mesh_instance(node, box, red));

	  printf("creating socket on port %d\n", port);
	  socket = networking::NetworkServices::GetInstance().CreateSocket();
	  socket->Open(port);

    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);

      // tumble the box  (there is only one mesh instance)
      scene_node *node = app_scene->get_mesh_instance(0)->get_node();
      node->rotate(1, vec3(1, 0, 0));
      node->rotate(1, vec3(0, 1, 0));

	  UpdateCommunications();

    }


	void UpdateCommunications()
	{
		const char data[] = "hello world!";
		socket->Send(networking::Address(127, 0, 0, 1, port), data, sizeof(data));

		while (true)
		{
			unsigned char buffer[256];
			networking::Address sender;

			int bytesRead = socket->Receive(sender, &buffer, sizeof(buffer));
			if (bytesRead)
			{
				printf("received packet from %d.%d.%d.%d:%d (%d bytes) - %s\n",
					sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(),
					sender.GetPort(), bytesRead, buffer);
			}
			else
				break;
		}
	}
  };
}
