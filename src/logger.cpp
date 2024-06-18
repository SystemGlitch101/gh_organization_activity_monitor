//    Logger source file
//    Description:
//      A simple logger that writes messages to a given filename.
//    Writer: Max
//    Reviewer: TBD
//    Finish date before review: 02/06/24
//    Finish date after review: 03/06/24
#include "logger.hpp"

namespace ilrd
{

Logger::Logger(const std::string& filename): 
	m_run_flag(true), m_output_fstream(filename, std::ios_base::app), 
	m_log_queue(), m_thread(&ilrd::Logger::LogThread, this)
{
	Log("Logger is now starting", INFO);
}

Logger::~Logger()
{
	m_run_flag = false;
	Log("Logger is now exiting", INFO);
	m_thread.join();
	m_output_fstream.close();
}

void Logger::Log(const std::string& message, Severity severity)
{
	m_log_queue.Push(
		std::make_tuple(std::chrono::system_clock::now(), severity, message));
}

void Logger::LogThread()
{
	while (m_run_flag)
	{
		LogTuple tuple;
		m_log_queue.Pop(tuple);
		
		m_output_fstream 
			<< "[" << TimePointToString(std::get<TIM>(tuple)) << "]"
			<< "[" << SeverityToString(std::get<SEV>(tuple))  << "]"
			<< "[" << std::get<MSG>(tuple)                    << "]\n";
	}
}

// END OF API FUNCTIONS


void Logger::FormatMilliseconds(std::string& out_str, size_t ms)
{

	//ctime format:
	//Day Mon dd hh:mm:ss yyyy\n
	//
	//my format:
	//Day Mon dd hh:MM:ss.mmm yyyy

	static const size_t msPerSecond = 1000;
	char msBuffer[4];
	
	ms %= msPerSecond;
	
	msBuffer[0] = '.';
	msBuffer[1] = '0' + (ms / 100);
	msBuffer[2] = '0' + (ms / 10) % 10;
	msBuffer[3] = '0' + ms % 10;

	out_str.insert(19, msBuffer, sizeof(msBuffer));
	out_str.at(28) = '\0';
}

std::string Logger::TimePointToString(const TimePoint& time_point)
{
	time_t cTime = std::chrono::system_clock::to_time_t(time_point);
	size_t ms = std::chrono::time_point_cast<std::chrono::milliseconds>(time_point).time_since_epoch().count();
	std::string retStr(std::ctime(&cTime));

	FormatMilliseconds(retStr, ms);

	return retStr;
}

const char *Logger::SeverityToString(Severity severity)
{
	switch (severity)
	{
	case INFO:
		return "INFO";
		break;
	case DEBUG:
		return "DEBUG";
		break;
	case WARN:
		return "WARN";
		break;
	case ERROR:
		return "ERROR";
		break;	
	default:
		break;
	}
	return nullptr;
}

} // ilrd