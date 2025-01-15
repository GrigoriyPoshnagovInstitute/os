#include <iostream>
#include <unistd.h>  // Для fork(), getpid()
#include <sys/wait.h> // Для waitpid()
#include <signal.h>   // Для kill()
#include <cerrno>     // Для errno
#include <cstring>    // Для strerror()

void DisplayExecutablePath(int argc, char *argv[])
{
    if (argc > 0)
    {
        std::cout << "Executable path: " << argv[0] << std::endl;
    }
    else
    {
        std::cerr << "Error: No executable path found." << std::endl;
    }
}

pid_t CreateChildProcess()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        std::cerr << "Error: Failed to fork process. " << strerror(errno) << std::endl;
        return -1;
    }

    if (pid == 0)
    {
        std::cout << "Child process created with PID: " << getpid() << std::endl;
        return 0;
    }

    return pid;
}

void WaitForChildProcess(pid_t childPid)
{
    pid_t inputPid;
    int status;

    std::cout << "Enter the PID of the child process to wait for: ";

    while (true)
    {
        std::cin >> inputPid;

        if (inputPid != childPid)
        {
            std::cerr << "Invalid PID. Please try again: ";
            continue;
        }

        pid_t result = waitpid(inputPid, &status, 0);

        // ПОрпобовать именно с 256, 512 и сделать проанализировать полученные результаты
        if (result == -1 || !WIFEXITED(status))
        {
            std::cerr << "Error: waitpid failed. " << strerror(errno) << std::endl;
        }
        else
        {
            std::cout << "Child process with PID " << inputPid << " finished." << std::endl;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    DisplayExecutablePath(argc, argv);

    pid_t childPid = CreateChildProcess();

    if (childPid > 0)
    {
        WaitForChildProcess(childPid);
    }

    return 0;
}
