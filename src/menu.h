#ifndef _MENU_H_
#define _MENU_H_

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include "spacewar.h"

#define	MENU_PRINCIPAL	0
#define	MENU_RED		1
#define	MENU_OPCIONES	2
#define	MENU_CONTROLES	3

class SpaceWar;

class Menu {

// Construction
public:
	Menu (SpaceWar *game, CL_Font *font, CL_ResourceManager *rsc);
	~Menu ();

	void draw (int height);
	void update (float elapsedTime);
	
	void setSubMenu (int sub=0, int act=0);
	
	void moveUp ();
	void moveDown ();
	void select ();
	
// Attributes
public:

	int screenWidth;
	int width;
	float lastKeySince,keyRepeatTime;

	struct option_t {
		char title[40];
	} options[10];
	
	int subMenu;
	int numOptions;
	int active;
	
	SpaceWar *game;
	CL_Font *font;
	CL_ResourceManager *rsc;

	CL_SoundBuffer *soundMoving;
	CL_SoundBuffer *soundSelect;
	CL_SoundBuffer_Session soundSession;

// Operations
public:

// Implementation:
private:
	

};


#endif


