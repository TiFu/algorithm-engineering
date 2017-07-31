#pragma once

#include <iostream>

#ifndef NDEBUG
#define dout(obj) do { \
   std::cerr << "" <<  __FILE__ << ":" << __LINE__ << ": " << obj; \
} while(false)
#else
/**
 * for debug statements
 */
#define dout(obj) do {} while(false)
#endif
