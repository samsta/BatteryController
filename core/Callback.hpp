/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_CALLBACK_HPP
#define _CORE_CALLBACK_HPP

namespace core{

class Invokable
{
public:
   virtual ~Invokable(){}
   virtual void invoke() = 0;
};

template<class T>
class Callback: public Invokable
{
public:
   Callback(T& owner, void (T::*callback)()):
         m_owner(owner),
         m_callback(callback)
   {}

   virtual void invoke()
   {
      (m_owner.*m_callback)();
   }
   
private:
   T& m_owner;
   void (T::*m_callback)();
};

   
}

#endif // _CORE_CALLBACK_HPP
