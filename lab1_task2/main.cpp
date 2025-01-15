#include <iostream>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <cstdio>

const int BYTES_IN_KB = 1024;
const int KB_IN_MB = 1024;
const int BYTES_IN_MB = BYTES_IN_KB * KB_IN_MB;


int main()
{
    utsname buffer{};
    if (uname(&buffer) < 0)
        perror("uname() error");
    else {
        std::cout <<"Os: "
                  << buffer.sysname
                  << ' '
                  << buffer.release
                  << std::endl;
    }

    struct sysinfo systemInfo;
    if (sysinfo(&systemInfo) < 0)
        perror("sysinfo() error");
    else {
        std::cout << "RAM: "
                  << systemInfo.freeram / BYTES_IN_MB
                  << "/"
                  << systemInfo.totalram / BYTES_IN_MB
                  << " MB" << std::endl;
    }
    std::cout << "Processors: " << get_nprocs() << std::endl;

    return 0;
}