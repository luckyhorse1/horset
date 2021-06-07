#pragma once
#include <sys/syscall.h> // SYS_gettid
#include <unistd.h> // pid_t ::syscall

namespace CurrentThread {
    extern __thread int cachedTid;
    inline int  tid() {
        if (cachedTid) {
            return cachedTid;
        }
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }
}
