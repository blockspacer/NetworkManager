/*
 * Common header
 * Li Ao
 * hi@leeleo.me
 */

#ifndef __NETWORKSERVICE__COMMON__
#define __NETWORKSERVICE__COMMON__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <string>
#include <stdint.h>

#include "Enum.hpp"
#include "Log.hpp"

#define MEM_FN(x) boost::bind(&SelfType::x, shared_from_this())
#define MEM_FN1(x, y) boost::bind(&SelfType::x, shared_from_this(), y)
#define MEM_FN2(x, y, z) boost::bind(&SelfType::x, shared_from_this(), y, z)

typedef boost::system::error_code ErrorCode;

#endif
