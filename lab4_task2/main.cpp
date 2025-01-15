#include <iostream>
#include <thread>
#include <syncstream>
#include <stop_token>
#include <string>

using namespace std::literals;
using Clock = std::chrono::system_clock;

void PrintMessage(std::string message, std::chrono::seconds interval, std::stop_token stopToken)
{
    while (!stopToken.stop_requested())
    {
        std::this_thread::sleep_for(interval);
        // чем stopToken от stopCallback
        if (!stopToken.stop_requested())
        {
            std::osyncstream(std::cout) << Clock::to_time_t(Clock::now()) << " " << message << "\n";
        }
    }
    std::osyncstream(std::cout) << message << " thread completed\n";
}

int main()
{
    try
    {
        //слушать избавиться от stopSource использовать stopToken который внутри jthreat-а
        std::stop_source stopSource;

        std::jthread tic(PrintMessage, "Tic", 2s, stopSource.get_token());
        std::jthread tac(PrintMessage, "Tac", 3s, stopSource.get_token());

        std::string input;
        std::getline(std::cin, input);

        stopSource.request_stop();
    }
    catch (std::exception const &e)
    {
        std::cout << e.what();
    }

    return 0;
}
