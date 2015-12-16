/*
 * RobotGame.cpp
 *
 *  Created on: Oct 24, 2015
 *      Author: kbp
 */



#include <string>
#include <iostream>
#include <vector>
#include <dlfcn.h>
#include "Scheduler.h"
#include "Logger.h"
#include <thread>
#include "Viewer.h"
#include <unistd.h>

#define SIZEOF_ARRAY(arr)       (sizeof(arr) / sizeof(arr[0]))

extern "C" {
typedef RobotGame::Robot* ((*getRobotFunc)(RobotGame::RobCtrl* robCtrl));
}

getRobotFunc loadPlugin(const char* plugin)
{
	void* lib_handle;
	lib_handle = dlopen(plugin, RTLD_LAZY);
	if (!lib_handle) {
		std::cout << "Error loading plugin " << plugin << std::endl;
		return NULL;
	}

	getRobotFunc fn = reinterpret_cast<getRobotFunc>(dlsym(lib_handle, "getRobot"));
	char* error = dlerror();
	if (error != NULL) {
		std::cout << "Error finding getRobot() in " << plugin << std::endl;
		return NULL;
	}
	return fn;
}

int main(int argc, char** argv) {
	std::thread logger(RobotGame::Logger::Start );

	std::thread viewer(RobotGame::Viewer::Start);

	RobotGame::Scheduler& scheduler = RobotGame::Scheduler::getScheduler();

	int c;
	while ( ( c = ::getopt(argc, argv, "" ) ) != -1 ) {

	}

	while (--argc) {
		getRobotFunc fn = loadPlugin(argv[argc]);
		if (NULL == fn) {
			exit(1);
		}
		scheduler.addRobot(fn);
	}

	scheduler.run();

	RobotGame::Viewer::End();
	RobotGame::Logger::End();

	logger.join();
	viewer.join();

	return 0;
}
