/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_INVERTERIDENTITY_HPP
#define _CAN_MESSAGES_SMA_INVERTERIDENTITY_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class InverterIdentity
{
public:

   InverterIdentity(const DataFrame&);
   InverterIdentity();

   uint32_t getSoftwareVersion() const;
   uint16_t getInverterId() const;
   uint8_t  getConfiguration() const;

   bool valid() const;
private:
   uint32_t m_software_version;
   uint16_t m_inverter_id;
   uint8_t  m_configuration;
   bool     m_valid;
};

logging::ostream& operator<<(logging::ostream&, const InverterIdentity&);

}
}
}

#endif // _CAN_MESSAGES_SMA_INVERTERIDENTITY_HPP
