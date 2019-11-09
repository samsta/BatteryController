/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CONTACTOR_CONTACTOR_HPP
#define _CONTACTOR_CONTACTOR_HPP

namespace contactor {

class Contactor
{
public:
   virtual ~Contactor(){}

   virtual void setSafeToOperate(bool) = 0;
};

}

#endif // _CONTACTOR_CONTACTOR_HPP
