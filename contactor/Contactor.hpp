/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CONTACTOR_CONTACTOR_HPP
#define _CONTACTOR_CONTACTOR_HPP

#include "logging/stream.hpp"

namespace contactor {

class Contactor
{
public:
   virtual ~Contactor(){}

   virtual void setSafeToOperate(bool) = 0;
   virtual bool isClosed() const = 0;
   virtual bool isSafeToOperate() const = 0;
   virtual void close() = 0;
   virtual void open() = 0;
};

logging::ostream& operator<<(logging::ostream& os, const Contactor& contactor);

}

#endif // _CONTACTOR_CONTACTOR_HPP
