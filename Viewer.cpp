/*
 * Viewer.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#include "Viewer.h"
#include "Logger.h"
#include "Scheduler.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

namespace RobotGame {

unsigned int Viewer::colors[] = {
		0xFFF000FF,
		0xFF0F00FF,
		0xFF00F0FF,
		0xFF000FFF
};

const int Viewer::maxRobots = 4;

#define ARENA_WIDTH 1000
#define ARENA_HEIGHT 1000
#define STATUS_WIDTH 400
#define STATUS_HEIGHT 400
#define SCREEN_WIDTH  (ARENA_WIDTH + STATUS_WIDTH)
#define SCREEN_HEIGHT (std::max (ARENA_HEIGHT,STATUS_HEIGHT))

#define POS_TO_MAP_SCALE	(10) // Divide all x,y position with 10
// Singleton
Viewer& Viewer::getViewer() {
	static Viewer instance;
	return instance;
}


void Viewer::Start() {
	getViewer().Runner();
}

void Viewer::End() {
	Viewer& inst = getViewer();

	inst.DoDie();
}

void Viewer::DoDie() {
	goDie = true;
}

void Viewer::PostEvent(RobEvent* ev)
{
	Logger::Log("Event", ev->getName() + std::string("(") +
				std::to_string(ev->getId()) + std::string(")"));
	getViewer().evQueue.enqueue(ev);
}

void Viewer::tick(unsigned int tick)
{
	getViewer().eventProcess.unlock();
}

void Viewer::ClearArena()
{
	// Black - clear
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
	SDL_RenderClear( gRenderer );
}

void Viewer::CannonShow(int id, int x1, int y1, int x2, int y2, bool blasted)
{
	struct ShellPos shell;
	shell.x1 = x1/POS_TO_MAP_SCALE;
	shell.x2 = x2/POS_TO_MAP_SCALE;
	shell.y1 = y1/POS_TO_MAP_SCALE;
	shell.y2 = y2/POS_TO_MAP_SCALE;
	shell.blasted = blasted;
	getViewer()._CannonShow(shell);
}
void Viewer::_CannonShow(struct ShellPos shell)
{
	shells.push_back(shell);
}

void Viewer::RobotShow(int id, int x, int y)
{
	getViewer()._RobotShow(id, x, y);
}

void Viewer::_RobotShow(int id, int x, int y)
{
	if (id >= maxRobots)
		return;

	if (! robots.count(id)) {
		robots[id].color = colors[id];
		robots[id].nameTexture = 0;
	}

	robots[id].x = x/POS_TO_MAP_SCALE;
	robots[id].y = y/POS_TO_MAP_SCALE;
}

void Viewer::RobotDataShow(int id, std::string name, int armor, int energy)
{
	getViewer()._RobotDataShow(id, name, armor, energy);
}

void Viewer::_RobotDataShow(int id, std::string name, int armor, int energy)
{
	SDL_Color col = {
			static_cast<Uint8>((robots[id].color >> 24) & 0xff),
			static_cast<Uint8>((robots[id].color >> 16) & 0xff),
			static_cast<Uint8>((robots[id].color >> 8) & 0xff),
			static_cast<Uint8>((robots[id].color >> 0) & 0xff)
	};
	if (!robots[id].nameTexture) {
		SDL_Surface* surface = TTF_RenderText_Solid( gFont, name.c_str(), col );

		robots[id].nameTexture = SDL_CreateTextureFromSurface(gRenderer, surface);
		SDL_FreeSurface(surface);

		surface = TTF_RenderText_Solid( gFont, "E: ", col);
		robots[id].energyTexture = SDL_CreateTextureFromSurface(gRenderer, surface);
		SDL_FreeSurface(surface);

		surface = TTF_RenderText_Solid( gFont, "A: ", col);
		robots[id].armorTexture = SDL_CreateTextureFromSurface(gRenderer, surface);
		SDL_FreeSurface(surface);

	}
	robots[id].armor = armor;
	robots[id].energy = energy;
}

void Viewer::Runner()
{
	while(!goDie) {

		eventProcess.lock();
		Logger::LogDebug(std::string("Queue has ") + std::to_string(evQueue.size()) + std::string(" element(s)"));
		while (!evQueue.empty()) {
			RobEvent* ev = evQueue.dequeue();

			ev->execute();
			delete ev;
		}
		SetArenaViewPort();
		ClearArena();

		for (unsigned int i=0; i < robots.size(); i++) {
			PrintRobot(i);
		}
		for (unsigned int i=0; i < shells.size(); i++) {
			PrintShell(shells[i]);
		}
		shells.clear();
		SDL_RenderPresent( gRenderer );

		SetStatusViewPort();
		ClearStatus();
		for (unsigned int i=0; i < robots.size(); i++) {
			PrintRobotStatus(i);
		}
		SDL_RenderPresent( gRenderer );

		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if( event.type == SDL_QUIT )
			{
				goDie = true;
			}
		}

		Scheduler::end();
	}
}


Viewer::Viewer()
: goDie(false)
{
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		Logger::LogError("Viewer - Init fail - exiting");
		goDie = true;
		return;
	}
	//Set texture filtering to linear
	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
		Logger::LogWarning( "Linear texture filtering not enabled!" );
	}

	//Create window
	gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if( gWindow == NULL ) {
		Logger::LogError( std::string("Window could not be created! SDL Error: ") + SDL_GetError() );
		goDie = true;
		return;
	}

	//Create renderer for window
	gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
	if( gRenderer == NULL )	{
		Logger::LogError(std::string("Renderer could not be created! SDL Error: ") + SDL_GetError() );
		goDie = true;
		return;
	}
	//Initialize renderer color
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
		Logger::LogError(std::string("SDL_image could not initialize! SDL Error: ") + SDL_GetError() );
		goDie = true;
		return;
	}

	// font handling
	TTF_Init();
	gFont = TTF_OpenFont( "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 18 );

	// Wait for scheduler to make us run
	eventProcess.lock();
}

Viewer::~Viewer() {
	while (!evQueue.empty()) {
		RobEvent* ev = evQueue.dequeue();
		delete ev;
	}
}

void Viewer::StatusUpdate(int w, int h)
{
	//Render red filled quad
	SDL_Rect fillRect = { 0, 0, w, h };
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
	SDL_RenderFillRect( gRenderer, &fillRect );

}

void Viewer::SetArenaViewPort()
{
	arenaViewport.x = STATUS_WIDTH;
	arenaViewport.y = 0;
	arenaViewport.w = ARENA_WIDTH;
	arenaViewport.h = ARENA_HEIGHT;
	SDL_RenderSetViewport( gRenderer, &arenaViewport );
}

void Viewer::PrintShell(struct ShellPos shell)
{

	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xF0, 0x02, 0xFF );
	SDL_RenderDrawLine(gRenderer, shell.x1, shell.y1,shell.x2, shell.y2);
	if (shell.blasted ) {
		filledCircleColor(gRenderer, shell.x2, shell.y2, 40, 0xFFF002FF);
	}
}

void Viewer::PrintRobot(int id)
{
#define ROBOT_RADIUS 5


	filledCircleColor(gRenderer, robots[id].x, robots[id].y, ROBOT_RADIUS, robots[id].color);
}

void Viewer::SetStatusViewPort()
{
	statusViewPort.x = 0;
	statusViewPort.y = 0;
	statusViewPort.w = STATUS_WIDTH;
	statusViewPort.h = ARENA_HEIGHT;
	SDL_RenderSetViewport( gRenderer, &statusViewPort );
}

void Viewer::ClearStatus()
{
	// Black - clear
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
	SDL_RenderClear( gRenderer );

}
void Viewer::PrintRobotStatus(int id)
{
	int robStatusHeight = ARENA_HEIGHT / maxRobots;
	int robStatusY      = id * robStatusHeight;

	const int blockHeight = 40;
	const int prefixWidth = 40;

	SDL_Rect viewPort;
	viewPort.x = 0;
	viewPort.y = robStatusY;
	viewPort.w = STATUS_WIDTH;
	viewPort.h = robStatusHeight;
	SDL_RenderSetViewport(gRenderer, &viewPort);

	SDL_SetRenderDrawColor( gRenderer,
			(robots[id].color >> 24)& 0xFF, (robots[id].color >> 16) & 0xFF,
			(robots[id].color >> 8) & 0xFF, robots[id].color & 0xFF);

	SDL_Rect nameRect;
	nameRect.x = 1;
	nameRect.y = 1;
	nameRect.w = STATUS_WIDTH-2;
	nameRect.h = 2*blockHeight-2;
	if (SDL_RenderCopy( gRenderer, robots[id].nameTexture, 0, &nameRect)) {
		Logger::LogDebug("Error rendering name");
	}

	nameRect.x = 1;
	nameRect.y = 2*blockHeight + 1;
	nameRect.w = prefixWidth - 2;
	nameRect.h = blockHeight - 2;
	if (SDL_RenderCopy( gRenderer, robots[id].energyTexture, 0, &nameRect)) {
		Logger::LogDebug("Error rendering energy");
	}

	nameRect.x = 1;
	nameRect.y = 3*blockHeight + 1;
	nameRect.w = prefixWidth - 2;
	nameRect.h = blockHeight - 2;
	if (SDL_RenderCopy( gRenderer, robots[id].armorTexture, 0, &nameRect)) {
		Logger::LogDebug("Error rendering energy");
	}

	int energy = robots[id].energy * (STATUS_WIDTH - prefixWidth) / MAX_ENERGY;
	int armor = robots[id].armor * (STATUS_WIDTH - prefixWidth) / MAX_ARMOR;
	SDL_RenderDrawLine(gRenderer, prefixWidth, 2.5*blockHeight, 40+energy, 2.5*blockHeight);
	SDL_RenderDrawLine(gRenderer, prefixWidth, 3.5*blockHeight, 40+armor, 3.5*blockHeight);
}



}
/* namespace RobotGame */
