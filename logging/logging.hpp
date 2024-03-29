#ifndef LOGGING_LOGGING_HPP_
#define LOGGING_LOGGING_HPP_

///////////////////////////////////////////////////////////////////////////////
// @File Name:     Logger.h                                                  //
// @Author:        Pankaj Choudhary                                          //
// @Version:       0.0.1                                                     //
// @L.M.D:         13th April 2015                                           //
// @Description:   For Logging into file                                     //
//                                                                           //
// Detail Description:                                                       //
// Implemented complete logging mechanism, Supporting multiple logging type  //
// like as file based logging, console base logging etc. It also supported   //
// for different log type.                                                   //
//                                                                           //
// Thread Safe logging mechanism. Compatible with VC++ (Windows platform)   //
// as well as G++ (Linux platform)                                           //
//                                                                           //
// Supported Log Type: ERROR, ALARM, ALWAYS, INFO, BUFFER, TRACE, DEBUG      //
//                                                                           //
// No control for ERROR, ALRAM and ALWAYS messages. These type of messages   //
// should be always captured.                                                //
//                                                                           //
// BUFFER log type should be use while logging raw buffer or raw messages    //
//                                                                           //
// Having direct interface as well as C++ Singleton inface. can use          //
// whatever interface want.                                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// C++ Header File(s)
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#include <vector>
#include <thread>
#include <curl/curl.h>

#include "core/Timer.hpp"
#include "monitor/Monitor.hpp"
#include "AvgMinMax.hpp"

// POSIX Socket Header File(s)
// #include <errno.h>
// #include <pthread.h>

#define __FILENAME__ (strrchr(__FILE__,47)+1)

namespace logging
{
   // Default value for maximum number of log files 
   #define MAX_LOG_FILES 10
   
   // Default size of a log file in bytes
   #define LOG_FILE_SIZE 3000

   // enum for LOG_LEVEL
   typedef enum LOG_LEVEL
   {
      DISABLE_LOG       = 1,
      LOG_LEVEL_INFO    = 2,
      LOG_LEVEL_BUFFER  = 3,
      LOG_LEVEL_TRACE   = 4,
      LOG_LEVEL_DEBUG   = 5,
      ENABLE_LOG        = 6,
   }LogLevel;

   // enum for LOG_TYPE
   typedef enum LOG_TYPE
   {
      NO_LOG            = 1,
      CONSOLE           = 2,
      FILE_LOG          = 3,
   }LogType;

   class Logger
   {
      public:
         Logger(LOG_LEVEL loglevel, core::Timer& timer, std::vector<monitor::Monitor*> vmonitor);
         ~Logger();

         void setMonitor(std::vector<monitor::Monitor*> vmonitor);

         // Interface for Error Log
         void error(const char* text) throw();
         void error(std::string& text) throw();
         void error(std::ostringstream& stream) throw();
         void error(const char* text, const char* filename, int linenumber) throw();
         void error(std::string& text, const char* filename, int linenumber) throw();
         void error(std::ostringstream&, const char* filename, int linenumber) throw();

         // Interface for Alarm Log
         void alarm(const char* text) throw();
         void alarm(std::string& text) throw();
         void alarm(std::ostringstream& stream) throw();
         void alarm(const char* text, const char* filename, int linenumber) throw();
         void alarm(std::string& text, const char* filename, int linenumber) throw();
         void alarm(std::ostringstream&, const char* filename, int linenumber) throw();

         // Interface for Info Log
         void info(const char* text) throw();
         void info(std::string& text) throw();
         void info(std::ostringstream& stream) throw();
         void info(const char* text, const char* filename, int linenumber) throw();
         void info(std::string& text, const char* filename, int linenumber) throw();
         void info(std::ostringstream&, const char* filename, int linenumber) throw();

         // Interface for Always Log
         void always(const char* text) throw();
         void always(std::string& text) throw();
         void always(std::ostringstream& stream) throw();

         // Interface for Buffer Log
         void buffer(const char* text) throw();
         void buffer(std::string& text) throw();
         void buffer(std::ostringstream& stream) throw();

         // Interface for Trace log
         void trace(const char* text) throw();
         void trace(std::string& text) throw();
         void trace(std::ostringstream& stream) throw();

         // Interface for Debug log
         void debug(const char* text) throw();
         void debug(std::string& text) throw();
         void debug(std::ostringstream& stream) throw();

         // Error and Alarm log must be always enable
         // Hence, there is no interfce to control error and alarm logs

         // Interfaces to control log levels
         void updateLogLevel(LogLevel logLevel);
         void enaleLog();  // Enable all log levels
         void disableLog(); // Disable all log levels, except error and alarm

         // Interfaces to control log Types
         void updateLogType(LogType logType);
         void enableConsoleLogging();
         void enableFileLogging();

         // Interfaces to control roll over mechanism
         // void updateMaxLogFiles(const ssize_t maxFiles);
         // void updateLogSize(const ssize_t size);

      protected:
         // Wrapper function for lock/unlock
         // For Extensible feature, lock and unlock should be in protected
         void lock();
         void unlock();

         std::string getCurrentTime();
         std::string getCurrentTimeForDataLog();

      private:
         void logIntoFile(std::string& data);
         void logOnConsole(std::string& data);
         // void rollLogFiles();

      private:
         std::thread httpPostThread;
         void updateDataLog();
         void httpPOSTstr(std::string str);

         std::ofstream           m_File;
         std::string floatToString(float f);

         pthread_mutexattr_t     m_Attr;
         pthread_mutex_t         m_Mutex;

         LogLevel                m_LogLevel;
         LogType                 m_LogType;

         core::Timer&                     m_timer;
         std::vector<monitor::Monitor*>   m_vmonitor;
         core::Callback<Logger> m_datalog_callback;

         #define DATALOG_CALLBACK_PERIOD 10 * 1000 // 60 seconds // set this to 10 seconds during testing
         bool resetCallback;
         unsigned m_prev_minute;
         #define MAX_BATTERIES 6
         #define DATA_COUNT 9
         AvgMinMax m_bat_data[DATA_COUNT][MAX_BATTERIES];

         // unsigned int		 logSize; // Size of a log file in bytes
         // unsigned int		 maxLogFiles; // Maximum number of log files
         // unsigned int		 logFilesCount; // Count of existing log files 
   };

} // End of namespace

#endif /* LOGGING_LOGGING_HPP_ */
