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

// Code Specific Header Files(s)

using namespace std;
using namespace logging;

Logger* Logger::m_Instance = 0;

// Log file name. File name should be change from here only
const string logFileName = "BatteryController.log";
const string dataFileName= "BatteryOneDataLog.txt";

Logger::Logger(LOG_LEVEL loglevel, core::Timer& timer, std::vector<monitor::Monitor*> vmonitor):
   m_timer(timer),
   m_vmonitor(vmonitor),
   m_datalog_callback(*this, &Logger::updateDataLog)
{
   m_File.open(logFileName.c_str(), ios::out|ios::app);
   m_LogLevel  = loglevel;
   m_LogType   = FILE_LOG;

   int ret=0;
   ret = pthread_mutexattr_settype(&m_Attr, PTHREAD_MUTEX_ERRORCHECK_NP);
   if(ret != 0)
   {
      printf("Logger::Logger() -- Mutex attribute not initialize!!\n");
      exit(0);
   }
   ret = pthread_mutex_init(&m_Mutex,&m_Attr);
   if(ret != 0)
   {
      printf("Logger::Logger() -- Mutex not initialize!!\n");
      exit(0);
   }

   m_timer.registerPeriodicCallback(&m_datalog_callback, 1000);

}

Logger::~Logger()
{
   m_File.close();
   m_timer.deregisterCallback(&m_datalog_callback);

   pthread_mutexattr_destroy(&m_Attr);
   pthread_mutex_destroy(&m_Mutex);
}

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
      error("MAX_BATTERIES exceeded in data logging", __FILENAME__, __LINE__);
      // that's all, the program will likely crash but at least you've been warned.
   }
}

void Logger::updateDataLog()
{
   for (unsigned i=0; i<m_vmonitor.size(); i++)
   {
      if (m_vmonitor[i]->getPackStatus() == monitor::Monitor::Pack_Status::NORMAL_OPERATION )
      {
         m_voltage_ra[i].dataPoint(m_vmonitor[i]->getVoltage());
      }
   }

//   iThisMinute = timeClient.getMinutes();
//   if ((iPrevMinute != iThisMinute) and (timeClient.getSeconds() >= 5)) {
//     iPrevMinute = iThisMinute;
//      //on every minute divisible by 10
//      if (iThisMinute % 10 == 0) {
//       // disable interrupt to get readings
//       detachInterrupt(digitalPinToInterrupt(interruptPin1));
//       detachInterrupt(digitalPinToInterrupt(interruptPin2));
//       detachInterrupt(digitalPinToInterrupt(interruptPin3));
//       // get current readings
//       instpowerftp1 = instpower1;
//       instpower1 = 0;
//       intervalpowerftp1 = intervalpower1;
//       intervalpower1 = 0;
//       pulseintervalbegin1 = pulsebegin1;
//       pulsecountinterval1 = 1;
//       totalWhftp1 = totalWh1;

//       instpowerftp2 = instpower2;
//       instpower2 = 0;
//       intervalpowerftp2 = intervalpower2;
//       intervalpower2 = 0;
//       pulseintervalbegin2 = pulsebegin2;
//       pulsecountinterval2 = 1;
//       totalWhftp2 = totalWh2;

//       instpowerftp3 = instpower3;
//       instpower3 = 0;
//       intervalpowerftp3 = intervalpower3;
//       intervalpower3 = 0;
//       pulseintervalbegin3 = pulsebegin3;
//       pulsecountinterval3 = 1;
//       totalWhftp3 = totalWh3;

//       WriteData();
//       FTPFileAndDelete(tempdatafile,"jimster.ca/HeatChart"); 

//       // enable interrupt
//       attachInterrupt(digitalPinToInterrupt(interruptPin1), onPulse1, FALLING);
//       attachInterrupt(digitalPinToInterrupt(interruptPin2), onPulse2, FALLING);
//       attachInterrupt(digitalPinToInterrupt(interruptPin3), onPulse3, FALLING);
//   }
//   }







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
   // NEED TO GET MINUTES SO WE CAN TRIGGER EVERY 10 MINUTES
   // NEED TO GET MINUTES SO WE CAN TRIGGER EVERY 10 MINUTES
   // NEED TO GET MINUTES SO WE CAN TRIGGER EVERY 10 MINUTES

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
