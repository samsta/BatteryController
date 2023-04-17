///////////////////////////////////////////////////////////////////////////////
// @File Name:     Logger.cpp                                                //
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
#include <logging/logging.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

// Code Specific Header Files(s)

using namespace std;
using namespace logging;

// Log file name. File name should be change from here only
const string logFileName = "BatteryController.log";
const string dataFileName= "BatteryOneDataLog.txt";

Logger::Logger(LOG_LEVEL loglevel, core::Timer& timer, std::vector<monitor::Monitor*> vmonitor):
   m_timer(timer),
   m_vmonitor(vmonitor),
   m_datalog_callback(*this, &Logger::updateDataLog),
   m_prev_minute(0)
{
   m_File.open(logFileName.c_str(), ios::out|ios::app);
   m_LogLevel  = loglevel;
   m_LogType   = FILE_LOG;

   int ret=0;
   ret = pthread_mutexattr_settype(&m_Attr, PTHREAD_MUTEX_ERRORCHECK_NP);
   if(ret != 0)
   {
      printf("Logger::Logger() -- Mutex attribute did not initialize!!\n");
      exit(0);
   }
   ret = pthread_mutex_init(&m_Mutex,&m_Attr);
   if(ret != 0)
   {
      printf("Logger::Logger() -- Mutex did not initialize!!\n");
      exit(0);
   }

   m_timer.registerPeriodicCallback(&m_datalog_callback, DATALOG_CALLBACK_PERIOD);

}

Logger::~Logger()
{
   m_File.close();
   m_timer.deregisterCallback(&m_datalog_callback);

   pthread_mutexattr_destroy(&m_Attr);
   pthread_mutex_destroy(&m_Mutex);
}

//****************** lock/unlock ******************
// I believe these have been used when writing to the file because
// the code was written to allow multiple instances of the logger object,
// each writing to the same file. There had to be a method
// to stop two of them writing at the same time.
// I have changed the code so that it is not possible to have more than one
// instance so I guess these aren't needed but I will leave them. 

void Logger::lock()
{
   pthread_mutex_lock(&m_Mutex);
}

void Logger::unlock()
{
   pthread_mutex_unlock(&m_Mutex);
}

void Logger::setMonitor(std::vector<monitor::Monitor*> vmonitor)
{
   m_vmonitor = vmonitor;
   if (m_vmonitor.size() > MAX_BATTERIES)
   {
      error("MAX_BATTERIES exceeded in data logging, that's all, the program will likely crash but at least you've been warned.", __FILENAME__, __LINE__);
      // that's all, the program will likely crash but at least you've been warned.
   }
}

void Logger::updateDataLog()
{
   // called once per minute (DATALOG_CALLBACK_PERIOD)
   // every  minute take a reading, keep the average, min, max
   // every 10 minutes write the values to text file

   // take a data reading
   for (unsigned i=0; i<m_vmonitor.size(); i++)
   {
      // if (m_vmonitor[i]->getPackStatus() == monitor::Monitor::Pack_Status::NORMAL_OPERATION )
      {
         // m_bat_data[1][i].dataPoint(m_vmonitor[i]->getVoltage());
         // m_bat_data[2][i].dataPoint(m_vmonitor[i]->getCurrent());
         // m_bat_data[3][i].dataPoint(m_vmonitor[i]->getSocPercent());
         // m_bat_data[4][i].dataPoint(m_vmonitor[i]->getChargeCurrentLimit());
         // m_bat_data[5][i].dataPoint(m_vmonitor[i]->getDischargeCurrentLimit());
         // m_bat_data[6][i].dataPoint(m_vmonitor[i]->getEnergyRemainingKwh());
         float f = i+1;
         for (unsigned j=0; j<DATA_COUNT; j++)
         {
            float g = j+1;
            m_bat_data[j][i].dataPoint(g * (f+f/10.0));
         }

      }
   }

   // log data every 10 minutes
   std::time_t t = std::time(0);
   std::tm* now = std::localtime(&t);
   unsigned this_minute = now->tm_min;
   if (m_prev_minute != this_minute) 
   {
      m_prev_minute = this_minute;
      // on every minute evenly divisible by 10
      if (this_minute %2 == 0)
      {
         std::ofstream file;
         file.open(dataFileName.c_str(), ios::out|ios::app);
         // write the date and time in quotes            
         file << std::put_time(now, "\"%Y-%m-%d %H:%M:%S\",");
         // write the data in quotes
         for (unsigned i=0; i<m_vmonitor.size(); i++)
         {
            for (unsigned j=0; j<DATA_COUNT; j++)
            {
               file << "\"" << m_bat_data[j][i].getAverage() << "\",";
               file << "\"" << m_bat_data[j][i].getMin() << "\",";
               // no comma on the very last data point
               if ((j==(DATA_COUNT-1)) && (i==(m_vmonitor.size()-1)))
                  file << "\"" << m_bat_data[j][i].getMax() << "\"";
               else
                  file << "\"" << m_bat_data[j][i].getMax() << "\",";
               
               // reset the stats
               m_bat_data[j][i].reset();
            }
         }
         file << endl;
         file.close();
      }
   }
}

void Logger::logIntoFile(std::string& data)
{
   // unsigned long pos = m_File.tellp();
   // if (pos + data.size() > logSize)
   // {
	// 	rollLogFiles();	
   // }

   lock();
   m_File << getCurrentTime() << "  " << data << endl;
   unlock();
}

void Logger::logOnConsole(std::string& data)
{
   cout << getCurrentTime() << "  " << data << endl;
}

string Logger::getCurrentTime()
{
   string currTime;
   //Current date/time based on current time
   time_t now = time(0);
   // Convert current time to string
   currTime.assign(ctime(&now));

   // Last charactor of currentTime is "\n", so remove it
   string currentTime = currTime.substr(0, currTime.size()-1);
   return currentTime;
}

string Logger::getCurrentTimeForDataLog()
{
   string currTime;
   //Current date/time based on current time
   time_t now = time(0);
   // Convert current time to string
   currTime.assign(ctime(&now));

   // Last charactor of currentTime is "\n", so remove it
   string currentTime = currTime.substr(0, currTime.size()-1);
   return currentTime;
}


// Interface for Error Log
void Logger::error(const char* text) throw()
{
   string data;
   data.append("[ERROR]: ");
   data.append(text);

   // ERROR must be capture
   if(m_LogType == FILE_LOG)
   {
      logIntoFile(data);
   }
   else if(m_LogType == CONSOLE)
   {
      logOnConsole(data);
   }
}

void Logger::error(const char* text, const char* filename, int linenumber) throw()
{
   string data;
   string tmp = std::to_string(linenumber);
   data.append(text);
   data.append("  ");
   data.append(filename);
   data.append(":");
   data.append(tmp.c_str());
   error(data.c_str());
}

void Logger::error(std::string& text) throw()
{
   error(text.data());
}

void Logger::error(std::string& text, const char* filename, int linenumber) throw()
{
   error(text.data(),filename,linenumber);
}

void Logger::error(std::ostringstream& stream) throw()
{
   string text = stream.str();
   error(text.data());
}

void Logger::error(std::ostringstream& stream, const char* filename, int linenumber) throw()
{
   string text = stream.str();
   error(text.data(),filename,linenumber);
}

// Interface for Alarm Log
void Logger::alarm(const char* text) throw()
{
   string data;
   data.append("[ALARM]: ");
   data.append(text);

   // ALARM must be capture
   if(m_LogType == FILE_LOG)
   {
      logIntoFile(data);
   }
   else if(m_LogType == CONSOLE)
   {
      logOnConsole(data);
   }
}

void Logger::alarm(const char* text, const char* filename, int linenumber) throw()
{
   string data;
   string tmp = std::to_string(linenumber);
   data.append(text);
   data.append("  ");
   data.append(filename);
   data.append(":");
   data.append(tmp.c_str());
   alarm(data.c_str());
}

void Logger::alarm(std::string& text) throw()
{
   alarm(text.data());
}

void Logger::alarm(std::ostringstream& stream) throw()
{
   string text = stream.str();
   alarm(text.data());
}

void Logger::alarm(std::string& text, const char* filename, int linenumber) throw()
{
   alarm(text.data(),filename,linenumber);
}

void Logger::alarm(std::ostringstream& stream, const char* filename, int linenumber) throw()
{
   string text = stream.str();
   alarm(text.data(),filename,linenumber);
}

// Interface for Info Log
void Logger::info(const char* text) throw()
{
   string data;
   data.append("[INFO]: ");
   data.append(text);

   if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_INFO))
   {
      logIntoFile(data);
   }
   else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_INFO))
   {
      logOnConsole(data);
   }
}

void Logger::info(const char* text, const char* filename, int linenumber) throw()
{
   string data;
   string tmp = std::to_string(linenumber);
   data.append(text);
   data.append("  ");
   data.append(filename);
   data.append(":");
   data.append(tmp.c_str());
   info(data.c_str());
}

void Logger::info(std::string& text) throw()
{
   info(text.data());
}

void Logger::info(std::string& text, const char* filename, int linenumber) throw()
{
   info(text.data(), filename, linenumber);
}

void Logger::info(std::ostringstream& stream) throw()
{
   string text = stream.str();
   info(text.data());
}

void Logger::info(std::ostringstream& stream, const char* filename, int linenumber) throw()
{
   string text = stream.str();
   info(text.data(), filename,linenumber);
}

// Interface for Trace Log
void Logger::trace(const char* text) throw()
{
   string data;
   data.append("[TRACE]: ");
   data.append(text);

   if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_TRACE))
   {
      logIntoFile(data);
   }
   else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_TRACE))
   {
      logOnConsole(data);
   }
}

// Interface for Always Log
void Logger::always(const char* text) throw()
{
   string data;
   data.append("[ALWAYS]: ");
   data.append(text);

   // No check for ALWAYS logs
   if(m_LogType == FILE_LOG)
   {
      logIntoFile(data);
   }
   else if(m_LogType == CONSOLE)
   {
      logOnConsole(data);
   }
}

void Logger::always(std::string& text) throw()
{
   always(text.data());
}

void Logger::always(std::ostringstream& stream) throw()
{
   string text = stream.str();
   always(text.data());
}

// Interface for Buffer Log
void Logger::buffer(const char* text) throw()
{
   // Buffer is the special case. So don't add log level
   // and timestamp in the buffer message. Just log the raw bytes.
   if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_BUFFER))
   {
      lock();
      m_File << text << endl;
      unlock();
   }
   else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_BUFFER))
   {
      cout << text << endl;
   }
}

void Logger::buffer(std::string& text) throw()
{
   buffer(text.data());
}

void Logger::buffer(std::ostringstream& stream) throw()
{
   string text = stream.str();
   buffer(text.data());
}

void Logger::trace(std::string& text) throw()
{
   trace(text.data());
}

void Logger::trace(std::ostringstream& stream) throw()
{
   string text = stream.str();
   trace(text.data());
}

// Interface for Debug Log
void Logger::debug(const char* text) throw()
{
   string data;
   data.append("[DEBUG]: ");
   data.append(text);

   if((m_LogType == FILE_LOG) && (m_LogLevel >= LOG_LEVEL_DEBUG))
   {
      logIntoFile(data);
   }
   else if((m_LogType == CONSOLE) && (m_LogLevel >= LOG_LEVEL_DEBUG))
   {
      logOnConsole(data);
   }
}

void Logger::debug(std::string& text) throw()
{
   debug(text.data());
}

void Logger::debug(std::ostringstream& stream) throw()
{
   string text = stream.str();
   debug(text.data());
}

// Interfaces to control log levels
void Logger::updateLogLevel(LogLevel logLevel)
{
   m_LogLevel = logLevel;
}

// Enable all log levels
void Logger::enaleLog()
{
   m_LogLevel = ENABLE_LOG;
}

// Disable all log levels, except error and alarm
void Logger:: disableLog()
{
   m_LogLevel = DISABLE_LOG;
}

// Interfaces to control log Types
void Logger::updateLogType(LogType logType)
{
   m_LogType = logType;
}

void Logger::enableConsoleLogging()
{
   m_LogType = CONSOLE;
}

void Logger::enableFileLogging()
{
   m_LogType = FILE_LOG ;
}

// THIS CODE NEEDS A LOT OF WORK...

// // Interfaces to control roll over mechanism
// void Logger::updateMaxLogFiles(const ssize_t maxFiles)
// {
//    if(maxFiles > 0)
// 	   maxLogFiles = maxFiles;
//    else
// 	   maxLogFiles = MAX_LOG_FILES;
// }

// void Logger::updateLogSize(const ssize_t size)
// {
//    if(size > 0)
//    	logSize = size;
//    else
//       logSize = LOG_FILE_SIZE;
// }

// // Handle roll over mechanism
// void Logger::rollLogFiles()
// {
//     m_File.close();	

//     if(maxLogFiles > 1)
//     {
//          string logFile = logFileName.substr(0, logFileName.length()-4); // removing ".log" from file name
   
//          // To check if the maximum files have reached
//          if(logFilesCount >= maxLogFiles)
//          {
//              string deleteFileName = logFile + "_" + to_string(maxLogFiles-1) + ".tar.gz";
//              remove(deleteFileName.c_str());
    
//              logFilesCount--; 	
//          }

//          // Renaming the files 
//         for(int i = logFilesCount; i >= 2; --i)
//        {
//              string oldLogFileName = logFile + "_" + to_string(i-1) + ".tar.gz";
//              string newLogFileName = logFile + "_" + to_string(i) + ".tar.gz";

//              rename(oldLogFileName.c_str(), newLogFileName.c_str());
//          }

//          string cmd = "tar -cf " + logFile + "_1.tar.gz " + logFileName;

//         system(cmd.c_str()); // creating tar file
//     }

//     remove(logFileName.c_str());

//     m_File.open(logFileName.c_str(), ios::out|ios::app);

//     if(logFilesCount < maxLogFiles)
//     {
//         logFilesCount++;
//     }
// }
