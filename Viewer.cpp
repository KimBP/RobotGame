/*
 * Viewer.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#include "Viewer.h"
#include "Logger.h"
#include <SDL2/SDL_image.h>

namespace RobotGame {

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

// Singleton
Viewer& Viewer::getViewer() {
	static Viewer instance;
	return instance;
}


void Viewer::Start() {
	Viewer& inst = getViewer();

	inst.Runner();
}

void Viewer::Runner() {
	while(!goDie) {
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( gRenderer );

		//Render red filled quad
		SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
		SDL_RenderFillRect( gRenderer, &fillRect );

		//Render green outlined quad
		SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
		SDL_RenderDrawRect( gRenderer, &outlineRect );

		//Draw blue horizontal line
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );
		SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );

		//Draw vertical line of yellow dots
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0x00, 0xFF );
		for( int i = 0; i < SCREEN_HEIGHT; i += 4 )
		{
			SDL_RenderDrawPoint( gRenderer, SCREEN_WIDTH / 2, i );
		}

		//Update screen
		SDL_RenderPresent( gRenderer );

		RobEvent* ev = evQueue.dequeue();

	}
}



Viewer::Viewer() :
	goDie(false)
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
	gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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
}

Viewer::~Viewer() {
	// TODO Auto-generated destructor stub
}

} /* namespace RobotGame */
