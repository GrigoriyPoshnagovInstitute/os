#include "headers/SysInfo.h"
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <versionhelpers.h>
#include <sysinfoapi.h>

std::string SysInfo::GetOSName() const {
    return "Windows";
}

std::string SysInfo::GetOSVersion() const {
    if (IsWindows10OrGreater())
    {
        return "Windows version is equal or greater than 10";
    }
    if (IsWindows8OrGreater())
    {
        return "Windows version is 8";
    }
    if (IsWindows7OrGreater())
    {
        return "Windows version is 7";
    }
    return "Windows version is lower than 7";
}

uint64_t SysInfo::GetFreeMemory() const {
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memoryStatus))
    {
        return memoryStatus.ullAvailPhys;
    }
    else
    {
        perror("GlobalMemoryStatusEx() error");
    }
}

uint64_t SysInfo::GetTotalMemory() const {
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memoryStatus))
    {
        return memoryStatus.ullTotalPhys;
    }
    else
    {
        perror("GlobalMemoryStatusEx() error");
    }
}

unsigned SysInfo::GetProcessorCount() const {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}

#elif __linux__
#include <sys/utsname.h>
#include <sys/sysinfo.h>

std::string SysInfo::GetOSName() const {
    utsname buffer{};
    if (uname(&buffer) < 0)
    {
        perror("uname() error");
    }
    else {
        return buffer.sysname;
    }
}

std::string SysInfo::GetOSVersion() const {
    utsname buffer{};
    if (uname(&buffer) < 0)
    {
        perror("uname() error");
    }
    else {
        return buffer.release;
    }
}

uint64_t SysInfo::GetFreeMemory() const {
    struct sysinfo memInfo{};
    if (sysinfo(&memInfo) < 0)
    {
        perror("sysinfo() error");
    }
    else {
        return memInfo.freeram;
    }
}

uint64_t SysInfo::GetTotalMemory() const {
    struct sysinfo memInfo{};
    sysinfo(&memInfo);
    if (sysinfo(&memInfo) < 0)
    {
        perror("sysinfo() error");
    }
    else
    {
        return memInfo.totalram;
    }
}

unsigned SysInfo::GetProcessorCount() const {
    return get_nprocs();
}

#endif