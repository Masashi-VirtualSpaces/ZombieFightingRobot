#include <sys/time.h>

long time_diff(struct timeval end , struct timeval start)
{
  return (long)end.tv_sec*1000000 + (long)end.tv_usec - (long)start.tv_sec*1000000 - (long)start.tv_usec;
}
