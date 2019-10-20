#ifndef _CAN_DATAFRAME_HPP
#define _CAN_DATAFRAME_HPP

#include <stdint.h>
#include "logging/stream.hpp"

namespace can {

class DataFrame
{
public:
   virtual ~DataFrame(){};

   virtual bool valid() const;
   unsigned size() const;
   virtual const uint8_t* data() const = 0;
   uint32_t id() const;
   uint64_t getBitField(unsigned start_bit, unsigned num_bits) const;
   int64_t  getSignedBitField(unsigned start_bit, unsigned num_bits) const;

   enum Constants {
      INVALID_ID = -1
   };

   typedef uint8_t value_type;
   typedef const uint8_t* const_iterator;
   const_iterator begin() const;
   const_iterator end() const;

protected:
   DataFrame();
   DataFrame(int32_t id, unsigned size);
   void setSize(unsigned size);
   void setId(int32_t id);

private:
   int32_t m_id;
   unsigned m_size;
};

logging::ostream& operator<<(logging::ostream&, const DataFrame&);

}

#endif // _CAN_DATAFRAME_HPP
