#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <dirent.h>
#include <unistd.h>

const int BYTES_IN_KB = 1024;
const std::string PROC_DIR_NAME = "/proc";
// не директория а файл std::filesystem path для создания путей
const std::string COMM_DIR_NAME = "/comm";
const std::string STATM_DIR_NAME = "/statm";

class CDir
{
public:
    //посмотреть видео и сделать по феншую
    explicit CDir(const std::string path)
    {
        m_dir = opendir(path.c_str());
        if (!m_dir) {
            throw std::runtime_error("Failed to open directory");
        }
    }

    ~CDir()
    {
        if (m_dir) {
            closedir(m_dir);
        }
    }

    DIR* Get() const
    {
        return m_dir;
    }

private:
    DIR* m_dir;
};

// пид не строка
std::string GetProcessName(const std::string &pid)
{
    std::string cmdlinePath = PROC_DIR_NAME + '/' + pid + COMM_DIR_NAME;
    std::ifstream cmdlineFile(cmdlinePath);
    std::string processName;

    if (cmdlineFile.is_open())
    {
        std::getline(cmdlineFile, processName);
        cmdlineFile.close();
    }
    else
    {
        processName = "Unknown";
    }

    return processName;
}

//сюда должна передоваться не строка
unsigned long GetProcessMemory(const std::string &pid)
{
    std::string statmPath = PROC_DIR_NAME + '/' + pid + STATM_DIR_NAME;
    std::ifstream statmFile(statmPath);
    unsigned long memory = 0;

    if (statmFile.is_open())
    {
        statmFile >> memory;
        memory *= getpagesize() / BYTES_IN_KB;
    }
    else
    {
        std::cerr << "Error: Unable to open memory info for PID " << pid << std::endl;
    }

    return memory;
}

void ListProcesses()
{
    CDir procDir(PROC_DIR_NAME);
    if (procDir.Get() == nullptr)
    {
        std::cerr << "Error: Unable to open /proc directory" << std::endl;
        return;
    }
    //вывести в консоль информацию о процессах используя команды линукса

    struct dirent *entry;
    std::cout << std::setw(10) << "PID"
              << std::setw(30) << "Process Name"
              << std::setw(20) << "Memory (KB)" << std::endl;

    while ((entry = readdir(procDir.Get())) != nullptr)
    {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0]))
        {
            std::string pid = entry->d_name;
            std::string processName = GetProcessName(pid);
            unsigned long memory = GetProcessMemory(pid);
            std::cout << std::setw(10) << pid
                      << std::setw(30) << processName
                      << std::setw(20) << memory << std::endl;
        }
    }
}

int main()
{
    ListProcesses();
    return 0;
}
