#include "CurrentThread.h"

namespace CurrentThread {
    __thread int cachedTid = 0;
}