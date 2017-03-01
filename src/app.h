#ifndef _APP_H_
#define _APP_H_

#include <ClanLib/application.h>
#include <ClanLib/core.h>

class SpaceWar;

class Application : public CL_ClanApplication
{
public:

	SpaceWar *game;

	Application() {};

	virtual int main(int argc, char** argv);

private:

	void onWindowClose ();
	void onInputUp (const CL_InputEvent &key);
};

extern Application app;

#endif

