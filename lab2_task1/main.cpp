#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <vector>
#include <stdexcept>

const int BYTES_IN_KB = 1024;

class Handle
{
public:
    explicit Handle(HANDLE handle)
            : m_handle(handle)
    {
        if (m_handle == nullptr || m_handle == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("Failed to acquire process handle");
        }
    }

    ~Handle()
    {
        if (m_handle && m_handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_handle);
        }
    }

    //добавить перемещающий конструктор и пермещаюший оператор присваивания
    //и считать INVALID_HANDLE_VALUE Тоже валидным

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    HANDLE get() const
    {
        return m_handle;
    }

private:
    HANDLE m_handle;
};

void PrintProcessInfo(DWORD processId)
{
    try
    {
        //проверить на NULL
        Handle hProcess(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId));

        TCHAR processName[MAX_PATH] = TEXT("<unknown>");
        if (GetModuleBaseName(hProcess.get(), nullptr, processName, sizeof(processName) / sizeof(TCHAR)) == 0)
        {
            std::cerr << "Failed to get process name. Error: " << GetLastError() << std::endl;
            return;
        }

        PROCESS_MEMORY_COUNTERS pmc;
        if (!GetProcessMemoryInfo(hProcess.get(), &pmc, sizeof(pmc)))
        {
            std::cerr << "Failed to get memory info. Error: " << GetLastError() << std::endl;
            return;
        }

        std::wcout
                << "PID: " << processId
                << ", Process Name: " << processName
                << ", Memory Usage: " << pmc.WorkingSetSize / BYTES_IN_KB << " KB"
                << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error  process ID " << processId << ": " << e.what() << std::endl;
    }
}

int main()
{
    DWORD cbNeeded = 0;
    std::vector<DWORD> processes(1024); // Начальный размер

    while (true)
    {
        processes.resize(processes.size() * 2); // Увеличение размера в 2 раза

        if (!EnumProcesses(processes.data(), processes.size() * sizeof(DWORD), &cbNeeded))
        {
            std::cerr << "Failed to enumerate processes. Error: " << GetLastError() << std::endl;
            return 1;
        }

        if (cbNeeded < processes.size() * sizeof(DWORD))
        {
            break;
        }
    }

    size_t processCount = cbNeeded / sizeof(DWORD);

    for (size_t i = 0; i < processCount; i++)
    {
        if (processes[i] != 0)
        {
            PrintProcessInfo(processes[i]);
        }
    }

    return 0;
}
