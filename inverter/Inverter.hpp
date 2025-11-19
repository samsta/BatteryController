/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _INVERTER_INVERTER_HPP
#define _INVERTER_INVERTER_HPP

namespace inverter {

class Inverter {

    public:
        virtual bool inverterCommsOk() = 0;
};

}

#endif // _INVERTER_INVERTER_HPP
