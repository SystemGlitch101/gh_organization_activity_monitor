//    Logger header file
//    Description:
//      A simple logger that writes messages to a given filename.
//    Writer: Max
//    Reviewer: TBD
//    Finish date before review: 02/06/24
//    Finish date after review: TBD
#ifndef __RD_153_LOGGER_HPP__
#define __RD_153_LOGGER_HPP__

#include <string> // std::string
#include <utility> // std::tuple
#include <thread> // std::thread
#include <chrono> // std::chrono::milliseconds
#include <fstream> // std::ofstream

#include "handleton.hpp"
#include "waitqueue.hpp" // ilrd::WaitableQueue

namespace ilrd
{
class Logger
{
public:
	enum Severity
	{
		INFO = 0,
		DEBUG,
		WARN,
		ERROR
	};

	Logger(const Logger& rhs) = delete;
	Logger(const Logger&& rhs) = delete;
	Logger& operator=(const Logger& rhs) = delete;
	Logger& operator=(const Logger&& rhs) = delete;
	~Logger();

	void Log(const std::string& message, Severity severity);
private:
	explicit Logger(const std::string& filename = "log.txt");
	friend class Handleton<Logger>;

	using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
	using LogTuple = std::tuple<TimePoint, Severity, std::string>; 
	
	void LogThread();

	//replace with enum for tuple OR inline
	enum LogTupleMembers
	{
		TIM = 0,
		SEV,
		MSG
	};

	static void FormatMilliseconds(std::string& out_str, size_t ms);
	
	static std::string TimePointToString(const TimePoint& time_point);
	static const char *SeverityToString(Severity severity);

	bool m_run_flag;
	std::ofstream m_output_fstream;
	WaitableQueue<LogTuple> m_log_queue;
	std::thread m_thread;
};

}

#endif // __RD_153_LOGGER_HPP__