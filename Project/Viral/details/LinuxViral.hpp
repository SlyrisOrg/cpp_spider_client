//
// Created by doom on 03/11/17.
//

#ifndef SPIDER_CLIENT_LINUXVIRAL_HPP
#define SPIDER_CLIENT_LINUXVIRAL_HPP

#ifndef SPIDER_CLIENT_VIRAL_HPP
#error Do not include this file directly
#endif

#include <sys/ptrace.h>

namespace spi::details
{
    class ViralImpl
    {
    protected:
        void antiTrace() noexcept
        {
            /**
             * Most debugging tools use fork() + ptrace(PTRACE_TRACEME) + execve().
             * On Linux, a program can only ask to be traced once, so if the following call
             * fails, it means our process is already being traced
             */
            if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
                //Do stuff
            }
        }
    };
}

#endif //SPIDER_CLIENT_LINUXVIRAL_HPP
