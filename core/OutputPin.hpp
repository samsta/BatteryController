/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_OUTPUTPIN_HPP
#define _CORE_OUTPUTPIN_HPP

namespace core {

class OutputPin
{
public:
   enum State {
      LOW,
      HIGH
   };

   virtual ~OutputPin(){}
   virtual void set(State state) = 0;
};

class NullOutputPin: public OutputPin
{
public:
   virtual void set(State state);
};


}

#endif // _CORE_OUTPUTPIN_HPP
