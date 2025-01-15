#include "headers/SysInfo.h"
#include <iostream>

const int BYTES_IN_KB = 1024;
const int KB_IN_MB = 1024;
const int BYTES_IN_MB = BYTES_IN_KB * KB_IN_MB;

int main() {
    SysInfo sysInfo;

    std::cout << "Operating System: " << sysInfo.GetOSName() << std::endl;
    std::cout << "OS Version: " << sysInfo.GetOSVersion() << std::endl;
    std::cout << "Processor Count: " << sysInfo.GetProcessorCount() << std::endl;
    std::cout << "Total Memory: " << sysInfo.GetTotalMemory() / BYTES_IN_MB << " MB" << std::endl;
    std::cout << "Free Memory: " << sysInfo.GetFreeMemory() / BYTES_IN_MB << " MB" << std::endl;

    return 0;
}