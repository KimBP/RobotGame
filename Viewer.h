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
#include <mutex>

namespace RobotGame {

class Viewer {
private:
	struct RobPos {
		int x;
		int y;
		unsigned int color;
	};
	struct ShellPos {
		int x1;
		int y1;
		int x2;
		int y2;
		bool blasted;
	};

public:
	Viewer();
	virtual ~Viewer();

	void Runner();

	static void tick(unsigned int tick);
	static void Start();
	static void PostEvent(RobEvent* ev);

	static void RobotShow(int id, int x, int y);
	static void CannonShow(int id, int x1, int y1, int x2, int y2, bool blasted);
private:
	void _RobotShow(int id, int x, int y);
	void _CannonShow(struct ShellPos shell);
	void ClearArena();
	void RenderArena();
	static unsigned int colors[];
	static Viewer& getViewer();
	std::mutex eventProcess;

	void SetArenaViewPort();
	void PrintRobot(unsigned int color, int x, int y);
	void PrintShell(struct ShellPos shell);
	void ArenaUpdate(int w, int h);
	void StatusUpdate(int w, int h);

	bool goDie;
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;
	SDL_Rect arenaViewport;

	SafeQueue<RobEvent*> evQueue;
	std::map<int, struct RobPos> robots;
	std::vector<struct ShellPos> shells;
};

} /* namespace RobotGame */
#endif /* VIEWER_H_ */
