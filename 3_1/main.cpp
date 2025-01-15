#include <iostream>
#include <fstream>
#include <filesystem>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

const int RUN_TIME = 100;

std::atomic_bool exitRequested(false);

void SignalHandler(int signal)
{
    try
    {
        if (signal == SIGINT || signal == SIGTERM)
        {
            std::cout << "Process " << getpid() << " has received signal #" << signal << "\n";
            exitRequested.store(true);
        }
    }
    catch (...)
    {}
}

class TempFile
{
public:
    explicit TempFile(const std::string &filename) : m_filename(filename)
    {
        m_ofstream.open(m_filename);
        if (!m_ofstream.is_open())
        {
            std::cerr << "Failed to create file\n";
        }
    }

    void Flush()
    {
        m_ofstream.flush();
    }
    // сделать деструктором
    void Delete()
    {
        try
        {
            if (m_ofstream.is_open())
            {
                m_ofstream.close();
            }
            if (std::filesystem::exists(m_filename))
            {
                std::filesystem::remove(m_filename);
                std::cout << "File " << m_filename << " has been removed.\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error removing file: " << e.what() << std::endl;
        }
    }

    void writeNumber(int number)
    {
        if (m_ofstream.is_open())
        {
            m_ofstream << number << std::endl;
        }
    }

    ~TempFile() {
        Delete();
    }

private:
    std::string m_filename;
    std::ofstream m_ofstream;
};

int main()
{
    std::cout << "pid " << getpid() << "\n";

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    try
    {
        TempFile tempFile("temp.txt");

        for (int i = 0; i <= RUN_TIME; ++i)
        {
            if (exitRequested.load())
            {
                std::cout << "Termination signal received, exiting...\n";
                break;
            }
            std::cout << i << std::endl;
            tempFile.writeNumber(i);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        tempFile.Flush();

        std::cout << "Done\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
