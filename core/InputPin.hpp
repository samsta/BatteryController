/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_INPUTPIN_HPP
#define _CORE_INPUTPIN_HPP

namespace core {

class InputPin
{
public:
   // enum State {
   //    LOW,
   //    HIGH
   // };

   virtual ~InputPin(){}
   virtual bool get() = 0;
};

class NullInputPin: public InputPin
{
public:
   virtual bool get();

private:
   bool input_state;
};


}

#endif // _CORE_INPUTPIN_HPP
