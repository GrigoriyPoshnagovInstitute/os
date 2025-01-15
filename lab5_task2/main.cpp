#include <iostream>

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <atomic>
#include <vector>
#include <iostream>
#include <syncstream>
#include "BgThreadDispatcher.h"

//int main()
//{
//    using osync = std::osyncstream;
//    BgThreadDispatcher dispatcher;
//    dispatcher.Dispatch([]{ osync(std::cout) << "1"; }); // Выводим 1 в фоновом потоке
//    osync(std::cout) << "!"; // Выводим ! в основном потоке
//    // Вывод 1 не гарантируется, так как мы не вызвали Wait перед вызовом деструктора.
//}
