#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <random>
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */

static int random_0_to_n(int n)
{
  static std::default_random_engine e(time(0));
  std::uniform_int_distribution<int> u(0,n-1);
 
  auto ret = u(e);
  return ret;
}
#endif // UTILS_H_INCLUDED