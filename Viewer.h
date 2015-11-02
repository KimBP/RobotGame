/*
 * Viewer.h
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#ifndef VIEWER_H_
#define VIEWER_H_

#include <SDL2/SDL.h>
#include "RobEvent.h"
#include "safequeue.h"

namespace RobotGame {

class Viewer {
public:
	Viewer();
	virtual ~Viewer();

	void Runner();

	static void Start();

private:
	static Viewer& getViewer();
	bool goDie;
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;

	SafeQueue<RobEvent*> evQueue;
};

} /* namespace RobotGame */
#endif /* VIEWER_H_ */
