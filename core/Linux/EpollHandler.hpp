/* SPDX-License-Identifier: GPL-3.0-or-later */


#ifndef _CORE_LINUX_EPOLLHANDLER_HPP_
#define _CORE_LINUX_EPOLLHANDLER_HPP_

namespace core {

class EpollHandler
{
public:
   virtual ~EpollHandler(){}

   virtual void handle() = 0;
};

}

#endif /* _CORE_LINUX_EPOLLHANDLER_HPP_ */
