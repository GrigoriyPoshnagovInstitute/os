#pragma once

#include <string>
#include <cstdint>

class SysInfo {
public:
    std::string GetOSName() const;
    std::string GetOSVersion() const;
    uint64_t GetFreeMemory() const;
    uint64_t GetTotalMemory() const;
    unsigned GetProcessorCount() const;
};