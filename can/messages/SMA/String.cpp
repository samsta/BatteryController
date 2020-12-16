/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "String.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "can/FrameSink.hpp"
#include "util.hpp"
#include <string.h>

using namespace util;

namespace can {
namespace messages {
namespace SMA {
namespace {
const unsigned CHARS_PER_MESSAGE = 7;
}

String::String(Ids id, const DataFrame& f):
      StandardDataFrame(f.id(), f.data()),
      Message(f.id())
{
   if (id == f.id())
   {
      setValid();
   }
}


String::String(Ids id):
      StandardDataFrame(id, "0000000000000000"),
      Message(id)
{
}

String::String(Ids id, uint8_t index, const char* string):
      StandardDataFrame(id, "0000000000000000"),
      Message(id)
{
   setByte(0, index);
   strncpy(reinterpret_cast<char*>(data()+1), string, CHARS_PER_MESSAGE);
   setValid();
}

void String::toStream(logging::ostream& os) const
{
   os << "String: ";
   if (Message::valid())
   {
      char str[CHARS_PER_MESSAGE + 1];
      strncpy(str, reinterpret_cast<const char*>(data() + 1), CHARS_PER_MESSAGE);
      str[CHARS_PER_MESSAGE] = 0;
      
      os << "Index=" << unsigned(getByte(0)) << " String=" << str;
   }
   else
   {
      os << "invalid";
   }
}


void sendString(FrameSink& sender, Ids id, const char* string)
{
   if (string == nullptr) return;

   const char* end = string + strlen(string);
   int index = 0;
   while (string < end)
   {
      sender.sink(String(id, index, string));
      string += CHARS_PER_MESSAGE;
      index++;
   }
   // terminate with a string with all zeros
   sender.sink(String(id, index, ""));   
}

}
}
}
