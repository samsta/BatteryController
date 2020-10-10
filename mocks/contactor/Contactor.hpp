#ifndef _MOCKS_CONTACTOR_CONTACTOR_HPP
#define _MOCKS_CONTACTOR_CONTACTOR_HPP

#include "contactor/Contactor.hpp"

namespace mocks {
namespace contactor {

class Contactor: public ::contactor::Contactor
{
public:
   MOCK_METHOD(void, setSafeToOperate,(bool));
   MOCK_METHOD(bool, isClosed,(), (const));
   MOCK_METHOD(void, close,());
   MOCK_METHOD(void, open,());
};

}
}

#endif // _MOCKS_CONTACTOR_CONTACTOR_HPP
