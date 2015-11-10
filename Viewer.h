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
#include <map>

namespace RobotGame {

class Viewer {
private:
	struct RobPos {
		int x;
		int y;
		unsigned int color;
	};

public:
	Viewer();
	virtual ~Viewer();

	void Runner();

	static void Start();
	static void PostEvent(RobEvent* ev);

	static void RobotShow(int id, int x, int y);

private:
	void _RobotShow(int id, int x, int y);
	void ClearArena();
	void RenderArena();
	static unsigned int colors[];
	static Viewer& getViewer();

	void SetArenaViewPort();
	void PrintRobot(unsigned int color, int x, int y);

	void ArenaUpdate(int w, int h);
	void StatusUpdate(int w, int h);

	bool goDie;
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;
	SDL_Rect arenaViewport;

	SafeQueue<RobEvent*> evQueue;
	std::map<int, struct RobPos> robots;
};

} /* namespace RobotGame */
#endif /* VIEWER_H_ */
