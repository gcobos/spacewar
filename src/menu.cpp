
#include "menu.h"
#include <string.h>

Menu::Menu (SpaceWar *game, CL_Font *font,CL_ResourceManager *rsc)
{
	this->game=game;
	this->font=font;
	this->rsc=rsc;

	soundMoving = new CL_SoundBuffer("Sound/MenuItem", rsc);
	soundSelect = new CL_SoundBuffer("Sound/MenuSelect", rsc);

	screenWidth=CL_Display::get_width();
	width=200;
	keyRepeatTime=0.2;
	lastKeySince=0;

	soundSession=soundMoving->prepare();
	soundSession.set_looping(false);

	setSubMenu(MENU_PRINCIPAL);
}

Menu::~Menu ()
{


}

void Menu::draw (int h)
{
	int i;
	int fw,fh;
	int fx,fy;

	for (i=0;i<numOptions;i++) {
		fw=font->get_width(options[i].title);
		fh=font->get_height(options[i].title);
		fx=screenWidth/2-fw/2;
		fy=h+i*40;

		if (i==active) {
			CL_Display::fill_rect(
				CL_Rect(fx-10,fy-4,fx+fw+10,fy+fh+4),CL_Color(64,128,128));
		}
		font->draw(fx,fy,options[i].title);
	}
}

void Menu::update (float elapsedTime)
{
	lastKeySince+=elapsedTime;
}

void Menu::moveUp ()
{
	if (lastKeySince<keyRepeatTime) {
		return;
	}
	lastKeySince=0;
	soundSession=soundMoving->prepare();
	soundSession.play();
	
	if (active==0) {
		active=numOptions-1;
	} else {
		active--;
	}
}

void Menu::moveDown ()
{
	if (lastKeySince<keyRepeatTime) {
		return;
	}
	lastKeySince=0;
	soundSession=soundMoving->prepare();
	soundSession.play();

	if (active==numOptions-1) {
		active=0;
	} else {
		active++;
	}
}

void Menu::select ()
{
	if (lastKeySince<keyRepeatTime) {
		return;
	}
	lastKeySince=0;
	soundSession=soundSelect->prepare();
	soundSession.play();

	switch (subMenu) {
	case MENU_PRINCIPAL:
		switch (active) {
		case 0:
			game->initGame();
			break;
		case 1:
			setSubMenu(MENU_RED);
			break;
		case 2:
			setSubMenu(MENU_OPCIONES);
			break;
		case 3:
			game->quit=1;
			break;
		}
		break;
	case MENU_RED:
		switch (active) {
		case 0:
			if (game->network->runServer()) {
				std::cout << "Iniciando servidor" << std::endl;
				game->initGame();
			}
			break;
		case 1:
			if (game->network->runClient()) {
				std::cout << "Conectando con el servidor" << std::endl;
				game->initGame();
			}
			break;
		case 2:
			setSubMenu(MENU_PRINCIPAL,1);
			break;
		}
		break;		
	case MENU_OPCIONES:
		switch (active) {
		case 0:
			setSubMenu(MENU_CONTROLES);
			break;
		case 1:
			if (game->options[WITH_PLANET]) {
				game->options[WITH_PLANET]=0;
			} else {
				game->options[WITH_PLANET]=1;
			}
			setSubMenu(MENU_OPCIONES,1);
			break;
		case 2:
			if (game->options[WITH_GRAVITY]) {
				game->options[WITH_GRAVITY]=0;
			} else {
				game->options[WITH_GRAVITY]=1;
			}
			setSubMenu(MENU_OPCIONES,2);
			break;
		case 3:
			if (game->options[FULL_SCREEN]) {
				game->options[FULL_SCREEN]=0;
				CL_Display::set_windowed();
			} else {
				game->options[FULL_SCREEN]=1;
				CL_Display::set_fullscreen(game->width,game->height,1);
			}
			setSubMenu(MENU_OPCIONES,3);
			break;
		case 4:
			setSubMenu(MENU_PRINCIPAL,2);
			break;
		}
		break;		
	case MENU_CONTROLES:
		switch (active) {
		case 0:
			//setSubMenu(MENU_PRINCIPAL);
			break;
		case 1:
			//setSubMenu(MENU_PRINCIPAL);
			break;
		case 2:
			setSubMenu(MENU_OPCIONES,0);
			break;
		}
		break;		
	}
}

void Menu::setSubMenu (int sub,int act)
{
	subMenu=sub;
	active=act;

	switch (sub) {
	case MENU_PRINCIPAL:
		strcpy(options[0].title,"JUGAR");
		strcpy(options[1].title,"RED");
		strcpy(options[2].title,"OPCIONES");
		strcpy(options[3].title,"SALIR");
		numOptions=4;
		break;
	case MENU_RED:
		strcpy(options[0].title,"INICIAR PARTIDA (NAVE DERECHA)");
		strcpy(options[1].title,"UNIRSE A LA PARTIDA (NAVE IZQUIERDA)");
		strcpy(options[2].title,"VOLVER");
		numOptions=3;
		break;
	case MENU_OPCIONES:
		strcpy(options[0].title,"CONTROLES");
		if (game->options[WITH_PLANET]) 
			 {	strcpy(options[1].title,"MOSTRAR PLANETA ( SI )"); } 
		else { 	strcpy(options[1].title,"MOSTRAR PLANETA ( NO )"); }		
		if (game->options[WITH_GRAVITY]) 
			 {	strcpy(options[2].title,"CON GRAVEDAD ( SI )");	}
		else {	strcpy(options[2].title,"CON GRAVEDAD ( NO )"); }		
		if (game->options[FULL_SCREEN]) 
			 {	strcpy(options[3].title,"PANTALLA COMPLETA ( SI )"); }
		else {	strcpy(options[3].title,"PANTALLA COMPLETA ( NO )"); }
		strcpy(options[4].title,"VOLVER");
		numOptions=5;
		break;
	case MENU_CONTROLES:
		strcpy(options[0].title,"JUGADOR IZQ");
		strcpy(options[1].title,"JUGADOR DER");
		strcpy(options[2].title,"VOLVER");
		numOptions=3;
		break;	
	
	}
}

