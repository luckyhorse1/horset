//
// Created by xiaoma on 6/9/21.
//

#include "Timestamp.h"
#include <sys/time.h>

Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}