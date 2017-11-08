//
// Created by doom on 03/11/17.
//

#ifndef SPIDER_CLIENT_LINUXVIRAL_HPP
#define SPIDER_CLIENT_LINUXVIRAL_HPP

#ifndef SPIDER_CLIENT_VIRAL_HPP
#error Do not include this file directly
#endif

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>

namespace spi::details
{
    class ViralImpl
    {
    protected:
//        void antiTrace() noexcept
//        {
            /**
             * Most debugging tools use fork() + ptrace(PTRACE_TRACEME) + execve().
             * On Linux, a program can only ask to be traced once, so if the following call
             * fails, it means our process is already being traced.
             * However for Unknow reasons the following puts the program in -trace stop-
             * as if a SIGTRAP was caught, after receiving SIGINT
             * and sending SIGCONT don't solve the problem ¯\_(ツ)_/¯
             */
//            if ((ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1) ||
//                ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == 0) {
//                std::cout << "ALLEZ CIAO" << std::endl;
//                exit(EXIT_SUCCESS);
//            }
//        }

        bool antiPreload() noexcept
        {
            if (!checkMemoryMappings()) {
                _log(logging::Warning) << "Unexpected libraries detected: exiting" << std::endl;
                return false;
            }
            return true;
        }

    private:
        logging::Logger _log{"linux-spider-viral", logging::Level::Debug};

    private:
        static int syscall_open(const char *path, long oflag) noexcept
        {
            int fd = -1;
#ifdef __i386__
            __asm__ (
            "mov $5, %%eax;" // Open syscall number
            "mov %1, %%ebx;" // Address of our string
            "mov %2, %%ecx;" // Open mode
            "mov $0, %%edx;" // No create mode
            "int $0x80;"     // Straight to ring0
            "mov %%eax, %0;" // Returned file descriptor
            :"=r" (fd)
            :"m" (path), "m" (oflag)
            :"eax", "ebx", "ecx", "edx"
            );
#elif __amd64__
            __asm__ (
            "mov $2, %%rax;" // Open syscall number
                "mov %1, %%rdi;" // Address of our string
                "mov %2, %%rsi;" // Open mode
                "mov $0, %%rdx;" // No create mode
                "syscall;"       // Straight to ring0
                "mov %%eax, %0;" // Returned file descriptor
            :"=r" (fd)
            :"m" (path), "m" (oflag)
            :"rax", "rdi", "rsi", "rdx"
            );
#endif
            return fd;
        }

        // always return -1 ... weird
        static ssize_t syscall_read(char *buffer, ssize_t buffer_size, int fd) noexcept
        {
            ssize_t i;
            for (i = 0; i < buffer_size - 1; i++) {
                size_t nbytes;
#ifdef __i386__
                __asm__ (
                "mov $3, %%eax;" // Read syscall number
                "mov %1, %%ebx;" // File descriptor
                "mov %2, %%ecx;" // Address of our buffer
                "mov $1, %%edx;" // Read 1 byte
                "int $0x80;"     // Straight to ring0
                "mov %%eax, %0;" // Returned read byte number
                :"=r" (nbytes)
                :"m" (fd), "r" (&(buffer[i]))
                :"eax", "ebx", "ecx", "edx"
                );
#elif __amd64__
                __asm__ (
                "mov $0, %%rax;" // Read syscall number
                    "mov %1, %%rdi;" // File descriptor
                    "mov %2, %%rsi;" // Address of our buffer
                    "mov $1, %%rdx;" // Read 1 byte
                    "syscall;"       // Straight to ring0
                    "mov %%rax, %0;" // Returned read byte number
                :"=r" (nbytes)
                :"m" (fd), "r" (&(buffer[i]))
                :"rax", "rdi", "rsi", "rdx"
                );
#endif
                if (nbytes != 1)
                    return -1;
                if (buffer[i] == '\n') {
                    i++;
                    break;
                }
            }
            buffer[i] = '\0';
            return i;
        }

        static bool checkMemoryMappings() noexcept
        {
            utils::Guard<int> g(static_cast<int>(syscall_open("/proc/self/maps", O_RDONLY)), ::close);

            if (g.get() == -1) {
                /** We treat this as an error since a process should always have access to its own metadata */
                return false;
            }

            char tmp[4096];
            ssize_t readRet;
            bool preloaded = false;
            bool afterBoost = false;

            while ((readRet = syscall(SYS_read, g.get(), tmp, sizeof(tmp) - 1)) > 0) {
                tmp[readRet] = '\0';
                char *cur = tmp;
                if (!afterBoost) {
                    int ret = afterLibBoost(tmp, &cur);
                    if (ret == 2)
                        return false;
                    afterBoost = ret == 1;
                }
                if (afterBoost) {
                    cur = TRsubstr(cur, "\n");
                    cur += 1;
                    int isPreload = isPreloaded(cur);
                    if (isPreload == 1)
                        return false;
                    else if (isPreload == 2)
                        return true;
                }
            }
            return readRet != -1 && !preloaded;
        }

        static char *TRsubstr(char *str, const char *sub) noexcept
        {
            bool found = false;
            char *cur;
            for (cur = str; !found && *cur; cur++) {
                if (*cur == *sub) {
                    found = true;
                    for (int i = 0; found && sub[i]; i++) {
                        if (sub[i] != cur[i])
                            found = false;
                    }
                }
            }
            if (!found)
                return nullptr;
            return cur - 1;
        }

        static bool TRstrcmp(const char *str1, const char *str2) noexcept
        {
            bool found = true;
            while (*str2) {
                if (!*str1 || (*str1 != *str2))
                    found = false;
                str1 += 1;
                str2 += 1;
            }
            return found;
        }

        static int afterLibBoost(char *str, char **cur) noexcept
        {
            bool found = false;
            *cur = str;
            char *tmp;
            while ((*cur = TRsubstr(*cur, "libboost_")) != nullptr) {
                *cur += 9;
                tmp = *cur;
                if (!TRstrcmp(*cur, "filesystem.so") && !TRstrcmp(*cur, "program_options.so") &&
                    !TRstrcmp(*cur, "system.so"))
                    return 2;
                found = true;
            }
            *cur = tmp;
            return (found ? 1 : 0);
        }

        static int TRlen(const char *str) noexcept
        {
            int i = 0;
            while (*str) {
                str += 1;
                i += 1;
            }
            return i;
        }

        static bool isLd(char *str, char *end) noexcept
        {
            while (str < end && *str) {
                bool found = false;
                if (TRstrcmp(str, "ld-")) {
                    str += 3;
                    found = true;
                }
                if (found && '0' <= *str && *str <= '9') {
                    while (*str && str < end && '0' <= *str && *str <= '9')
                        str += 1;
                } else
                    found = false;
                if (found && *str == '.')
                    str += 1;
                else
                    found = false;
                if (found && '0' <= *str && *str <= '9') {
                    while (*str && str < end && '0' <= *str && *str <= '9')
                        str += 1;
                } else
                    found = false;
                if (found && TRstrcmp(str, ".so\n"))
                    return true;
                str += 1;
            }
            return false;
        }

        static int isPreloaded(char *str) noexcept
        {
            int len = TRlen(str);
            while (*str) {
                if ((len -= 31) <= 0)
                    return 0;
                str += 31;
                char *end = TRsubstr(str, "\n");
                if (!TRstrcmp(str, "00000000 00:00 0")) {
                    if (isLd(str, end)) {
                        return 2;
                    } else
                        return 1;
                }
                if (end)
                    str = end + 1;
                else
                    return 0;
            }
            return 0;
        }
    };
}

#endif //SPIDER_CLIENT_LINUXVIRAL_HPP
