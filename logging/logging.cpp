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

#include <logging/logging.hpp>

using namespace std;
using namespace logging;

// Log file name. File name should be change from here only
const string logFileName = "BatteryController.log";
const string dataFileName= "BatteryOneDataLog.txt";
// const string httpPostURL = "http://jimster.ca/BatteryOne/TEST-data-receiver.php";
const string httpPostURL = "http://jimster.ca/BatteryOne/BatteryOne-data-receiver.php";

Logger::Logger(LOG_LEVEL loglevel, core::Timer& timer, std::vector<monitor::Monitor*> vmonitor):
   m_timer(timer),
   m_vmonitor(vmonitor),
   m_datalog_callback(*this, &Logger::updateDataLog),
   resetCallback(true),
   m_prev_minute(0)
{
   m_File.open(logFileName.c_str(), ios::out|ios::app);
   m_LogLevel  = loglevel;
   m_LogType   = FILE_LOG;

   // *** mutex not needed, see lock/unlock below ***
   // int ret=0;
   // ret = pthread_mutexattr_settype(&m_Attr, PTHREAD_MUTEX_ERRORCHECK_NP);
   // if(ret != 0)
   // {
   //    std::cout << "Logger::Logger() -- Mutex did not initialize! ret=" << ret << std::endl;
   //    // perror("pthread_mutex_init");
   //    // printf("Logger::Logger() -- Mutex attribute did not initialize!!\n");
   //    exit(0);
   // }
   // ret = pthread_mutex_init(&m_Mutex,&m_Attr);
   // if(ret != 0)
   // {
   //    printf("Logger::Logger() -- Mutex did not initialize!!\n");
   //    exit(0);
   // }

   // set callback to 5 secs in order to sync up to the start of a minute
   m_timer.registerPeriodicCallback(&m_datalog_callback, 5000, "LogBatteryData5sec");
}

Logger::~Logger()
{
   m_File.close();
   m_timer.deregisterCallback(&m_datalog_callback);

   // pthread_mutexattr_destroy(&m_Attr);
   // pthread_mutex_destroy(&m_Mutex);
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
   // pthread_mutex_lock(&m_Mutex);
}

void Logger::unlock()
{
   // pthread_mutex_unlock(&m_Mutex);
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
   // called once per DATALOG_CALLBACK_PERIOD
   // take a reading, keep the average, min, max
   // every 1 minute write the values to text file

   std:stringstream strstm;
   std::string str,strt;
   struct stat file_info;
   unsigned long fsize;
   char msgbuf[1024];

   std::time_t t = std::time(0);
   std::tm* now = std::localtime(&t);

   // change the callback to DATALOG_CALLBACK_PERIOD interval when it is the first 5 sec of the minute
   if (resetCallback) {
         // info("reset", __FILENAME__, __LINE__);
      if (now->tm_sec < 6)
      {
         resetCallback = false;
         m_timer.deregisterCallback(&m_datalog_callback);
         m_timer.registerPeriodicCallback(&m_datalog_callback, DATALOG_CALLBACK_PERIOD, "LogBatteryData1min");
         info("Data logging callback reset to 1 minute", __FILENAME__, __LINE__);
      }
      return;
   }
   
   // take a data reading
   // i is the battery
   for (unsigned i=0; i<m_vmonitor.size(); i++)
   {
      m_bat_data[0][i].dataPoint(m_vmonitor[i]->getMinCellVolts());
      m_bat_data[1][i].dataPoint(m_vmonitor[i]->getMaxCellVolts());
      m_bat_data[2][i].dataPoint(m_vmonitor[i]->getVoltage());
      m_bat_data[3][i].dataPoint(m_vmonitor[i]->getCurrent());
      m_bat_data[4][i].dataPoint(m_vmonitor[i]->getSocPercent());
      m_bat_data[5][i].dataPoint(m_vmonitor[i]->getChargeCurrentLimit());
      m_bat_data[6][i].dataPoint(m_vmonitor[i]->getDischargeCurrentLimit());
      m_bat_data[7][i].dataPoint(m_vmonitor[i]->getEnergyRemainingKwh());
      m_bat_data[8][i].dataPoint(m_vmonitor[i]->getTemperature());
      
      // generate fake data
      // j is the data type (V, I, SOC, etc)
      // for (unsigned j=0; j<DATA_COUNT; j++)
      // {
      //    m_bat_data[j][i].dataPoint((i*10 + now->tm_min + j ));
      // }
   }

   // log data when appropriate
   unsigned this_minute = now->tm_min;
   if (m_prev_minute != this_minute) 
   {
      m_prev_minute = this_minute;
      // LOG IT EVERY MINUTE during testing
      // on every minute evenly divisible by 5
      // if (this_minute %5 == 0)  
      {
         // write the date and time in quotes            
         strstm << std::put_time(now, "\"%Y-%m-%d %H:%M:%S\",");
         // write the data in quotes
         for (unsigned i=0; i<m_vmonitor.size(); i++)
         {
            // datetime
            str += strstm.str();
            // battery number
            // add one to packes numbered < size-1
            // size-1 is the multipack which will be assigned index 0
            if (i == (m_vmonitor.size()-1))
            {
               str += "\"" + to_string(0) + "\",";
            }
            else
            {
               str += "\"" + to_string(i+1) + "\",";
            }
            // loop over each data value for a pack
            for (unsigned j=0; j<DATA_COUNT; j++)
            {
               // special cases for cell voltages
               if (j == 0) {
                  // min cell voltage
                  str += + "\"" + floatToString(m_bat_data[j][i].getMin()) + "\",";
               }
               else if (j == 1) {
                  // max cell voltage
                  str += + "\"" + floatToString(m_bat_data[j][i].getMax()) + "\",";
               }
               else {
                  str += + "\"" + floatToString(m_bat_data[j][i].getAverage()) + "\",";
               }
               
               // no min on these indexes
               if (j!=0 and j!=1 and j!=4 and j!=7) {
                  str += + "\"" + floatToString(m_bat_data[j][i].getMin()) + "\",";
               }
               // no comma on the very last data point, but must have a newline character
               if (j==(DATA_COUNT-1)) {
                  str += + "\"" + floatToString(m_bat_data[j][i].getMax()) + "\"\n";
               }
               // no max on these indexes
               else if (j!=0 and j!=1 and j !=4 and j != 7) {
                  str += + "\"" + floatToString(m_bat_data[j][i].getMax()) + "\",";
               }
               
               // reset the stats
               m_bat_data[j][i].reset();
            }
         }

         // write the pertanent data to a file
         std::ofstream file;
         file.open(dataFileName.c_str(), ios::out|ios::app);
         file << str;
         file.close();

         // see if filesize is same as str size, in which case
         // we don't need to read the file
         if(stat(dataFileName.c_str(), &file_info)) {
            sprintf(msgbuf, "Couldn't open '%s': %s", dataFileName.c_str(), strerror(errno));
            error(msgbuf, __FILENAME__, __LINE__);
            return;
         }
         fsize = (unsigned long)file_info.st_size;

         if (fsize != str.length())
         {
            sprintf(msgbuf, "Reading datalog file from disk.  file size: %lu != string size: %lu", fsize, str.length());
            info(msgbuf, __FILENAME__, __LINE__);
            // read the file into str
            str.clear();
            std::stringstream buffer;
            // Open the file for reading
            std::ifstream file(dataFileName);
            // Read the file contents into the stringstream
            buffer << file.rdbuf();
            str = buffer.str();
         }

         // transfer the file to web host in a separate thread
         // because internet access can block and take some time to complete
         try {
            httpPostThread =  std::thread(&Logger::httpPOSTstr, this, str);
            httpPostThread.detach();
         }
         catch (const std::system_error& e)
         {
            std::ostringstream ss;
            ss << "std::thread failed, failed to transfer battery log data to web host. error:" << e.what();
            error(ss, __FILENAME__, __LINE__);
         }
      }
   }
}

// Callback to collect the response body for curl call
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

void Logger::httpPOSTstr(std::string str)
{
   // TODO ***********************CHANGE LOGGER CONTRUCTION BACK  SO THAT THE MUTEX WORKS ON RPI
   // info("httpPOSTstr now running.", __FILENAME__, __LINE__);

   CURL *curl;
   CURLcode res;
   unsigned long fsize;
   char msgbuf[1024];

   fsize = str.length();
   // sprintf(msgbuf, "Passed string size: %lu bytes.", fsize);
   // info(msgbuf, __FILENAME__, __LINE__);

   /* get a curl handle */
   curl = curl_easy_init();
   if (!curl) {
      snprintf(msgbuf, sizeof(msgbuf), "Failed to initialize CURL.");
      error(msgbuf, __FILENAME__, __LINE__);
      return;
   }

   // Set options
   curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str.c_str());
   curl_easy_setopt(curl, CURLOPT_URL, httpPostURL.c_str());

   struct curl_slist* headers = nullptr;
   std::string responseString;
   headers = curl_slist_append(headers, "Content-Type: text/plain");
   curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

   curl_easy_setopt(curl, CURLOPT_USERAGENT, "MyClient/1.0");

   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

   // Perform the request
   res = curl_easy_perform(curl);
   if (res != CURLE_OK) {
      snprintf(msgbuf, sizeof(msgbuf), "curl_easy_perform() failed: %s. Most likely due to loss of internet connection.", curl_easy_strerror(res));
      error(msgbuf, __FILENAME__, __LINE__);
      curl_easy_cleanup(curl);
      if (headers) curl_slist_free_all(headers);
      curl_global_cleanup();
      return;
   }

   // Check HTTP response code
   long http_code = 0;
   curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
   if (http_code != 200) {
      snprintf(msgbuf, sizeof(msgbuf),"HTTP POST failed with status code %ld. Server response: %s", http_code, responseString.c_str());
      error(msgbuf, __FILENAME__, __LINE__);

      curl_easy_cleanup(curl);
      if (headers) curl_slist_free_all(headers);
      curl_global_cleanup();
      return;
   }

   // Final cleanup
   curl_easy_cleanup(curl);
   if (headers) curl_slist_free_all(headers);
   curl_global_cleanup();

   // delete data file
   int result = remove(dataFileName.c_str());
   if (result != 0) {
      sprintf(msgbuf, "Error deleting datalog file.");
      error(msgbuf, __FILENAME__, __LINE__);
   }
   // else {
   //    sprintf(msgbuf, "File deleted successfully.");
   //    info(msgbuf, __FILENAME__, __LINE__);
   // }

   // info("httpPOSTstr finished.", __FILENAME__, __LINE__);

   // kludge city!
   // write the SOC to a file for OEMS to read
   int soc = 0.5 + m_vmonitor[m_vmonitor.size()-1]->getSocPercent();
   std::ofstream outFile("/home/pi/openems/SOC.txt");
   if (outFile.is_open()) {
      outFile << soc; // Write the integer as text
      outFile.close();
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

std::string Logger::floatToString(float f)
{
  char buf[50];
  std::snprintf(buf, sizeof(buf), "%.2f", f);
  std::string result = std::string(buf);
  // remove trailing zeros
  size_t pos = result.find_last_not_of('0');
  if (pos != std::string::npos && result[pos] == '.') {
    pos++;  // keep one trailing zero after the decimal point
  }
  if (pos != std::string::npos) {
    result.erase(pos + 1);
  }
  return result;
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
