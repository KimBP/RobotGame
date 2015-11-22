/*
 * Logger.h
 *
 *  Created on: Oct 25, 2015
 *      Author: kbp
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "safequeue.h"
#include <string>
#include "Robot.h"

namespace RobotGame {

class Logger {
public:
	Logger() : goDie(false), showDebug(true) {};
	virtual ~Logger();

	void Runner();
	void DoDie();

	static void Log(std::string name, std::string msg);
	static void Log(const Robot* robot, std::string msg);
	static void LogHead(std::string msg);
	static void LogError(std::string error);
	static void LogWarning(std::string warning);
	static void LogDebug(std::string msg);
	static void Start();
	static void End();
	static void EnableDebug();
	static void DisableDebug();
private:
	static Logger& getLogger();

	bool goDie;
	SafeQueue<std::string> queue;
	bool showDebug;
};

} /* namespace RobotGame */
#endif /* LOGGER_H_ */
