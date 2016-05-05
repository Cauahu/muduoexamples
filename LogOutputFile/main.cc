#include <muduo/base/Logging.h>
#include <muduo/base/LogFile.h>
#include <iostream>

using namespace muduo;


void OutputFile(const char* msg, int len)
{
	std::cout << msg ;
	string filename("huzhuang-logfile");
	LogFile logFile_(filename, 1024);
	logFile_.append(msg, len);
}

int main(int argc, char* argv[])
{
	Logger::setOutput(OutputFile);
	LOG_INFO << "this is loggng class test";
}
