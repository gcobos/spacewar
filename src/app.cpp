
#include <ClanLib/display.h>
#include <ClanLib/network.h>
#include <ClanLib/sound.h>
#include <ClanLib/mikmod.h>
#include <ClanLib/gl.h>
#include <stdlib.h>			// Para random

#include "app.h"
#include "spacewar.h"

Application app;

int Application::main(int argc, char** argv)
{
	int width;
	int height;
	bool fullscreen;

	try
	{
		// Init stuff
		CL_SetupCore::init();
		CL_SetupGL::init();
		CL_SetupDisplay::init();
		CL_SetupSound::init();
		CL_SetupMikMod::init();
		CL_SetupNetwork::init();

		// Sonido
		CL_SoundOutput output(44100);

		// Load resources
		CL_ResourceManager *resources=new CL_ResourceManager("rsc/general.xml");

		// Pilla el tamaño de la pantalla
		width=CL_Integer("Configuration/Screen/Width", resources);
		height=CL_Integer("Configuration/Screen/Height",resources);
		fullscreen=CL_Boolean("Configuration/Screen/FullScreen",resources);

		// Create a window
		CL_DisplayWindow window("SPACEWAR", width, height, fullscreen);

		// Connect the Window close event
		CL_Slot slot_quit = window.sig_window_close().connect(this, &Application::onWindowClose);

		// Connect a keyboard handler to on_key_up()
////	CL_Slot slot_input_up = (window.get_ic()->get_keyboard()).sig_key_up().connect(this, &Application::onInputUp);

		// Create game
		game=new SpaceWar(resources);

		window.hide_cursor();

		// No coments
		game->run();

		window.show_cursor();

		delete game;
		delete resources;

		std::cout << "May the farce be with you." << std::endl;
		
		// Deinit stuff
//		CL_SetupNetwork::deinit();
		CL_SetupMikMod::init();
		CL_SetupSound::deinit();
		CL_SetupDisplay::deinit();
		CL_SetupGL::deinit();
		CL_SetupCore::deinit();

	}
	catch (CL_Error e)
	{
		std::cout << e.message.c_str() << std::endl;
	}

	return 0;
}

void Application::onWindowClose ()
{
	std::cout << "Sale por cierre de la ventana" << std::endl;
	game->stop();
}

void Application::onInputUp (const CL_InputEvent &key)
{
	if (key.id == CL_KEY_ESCAPE) {
		std::cout << "Sale por ESCAPE" << std::endl;
		game->stop();
	}
}
