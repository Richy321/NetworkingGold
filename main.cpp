////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Text overlay
//

#define OCTET_BULLET 1
#define OCTET_ANTTWEAKBAR 1

#include "../../octet.h"
#include "PlatformDetection.h"
#include "networkingApp.h"
#include "ServerApp.h"

/// Create a box with octet
int main(int argc, char **argv)
{
	// set up the platform.
	octet::app::init_all(argc, argv);

	#if _IS_SERVER
		//server application
		octet::ServerApp serverApp(argc, argv);
		serverApp.init();
	#else
		// client application.
		octet::networkingApp clientApp(argc, argv);
		clientApp.init();
	#endif

	// open windows
	octet::app::run_all_apps();
}


