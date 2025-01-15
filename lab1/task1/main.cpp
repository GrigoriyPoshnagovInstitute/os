#include <iostream>
#include <windows.h>
#include <versionhelpers.h>
#include <cstdint>

const int BYTES_IN_KB = 1024;
const int KB_IN_MB = 1024;
const int BYTES_IN_MB = BYTES_IN_KB * KB_IN_MB;

struct MemoryInfo
{
    uint64_t available;
    uint64_t total;

    MemoryInfo(uint64_t avail, uint64_t tot) : available(avail), total(tot) {}
};

std::string GetOSInformation()
{
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

MemoryInfo GetMemoryStatusInMb()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo))
    {
        return MemoryInfo(
            memInfo.ullAvailPhys / (BYTES_IN_MB),
            memInfo.ullTotalPhys / (BYTES_IN_MB)
        );
    }
    else
    {
        throw std::runtime_error("GlobalMemoryStatusEx call error");
    }
}

int GetProcessorCount()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}

int main()
{
    std::cout << GetOSInformation() << std::endl;
    try
    {
        MemoryInfo memoryInfo = GetMemoryStatusInMb();
        std::cout << "RAM: "
            << memoryInfo.available
            << "/"
            << memoryInfo.total
            << " MB" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Произошла ошибка получения информации о памяти: " << e.what() << std::endl;
    }
    std::cout << "Processors: " << GetProcessorCount() << std::endl;

    return 0;
}