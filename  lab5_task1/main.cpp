#include <iostream>
#include <unordered_set>
#include <vector>
#include <shared_mutex>
#include <thread>
#include <chrono>
#include <mutex>

////префиксы m_
////узнать что делает 2 последних аргумента
////добавить получение
////сравнивать результаты
//// чем shared_mutex от mutex Отличается почему выбрал shred_mutex
//// почему бы не добавить begin end аргумементирвоать

////некоторые значеня выгоднее перемещать нежели копировать, например строки добавить версию, которая перегружена для r value ссылок

template<typename T, typename H = std::hash<T>, typename Comp = std::equal_to<T>>
class ThreadSafeSet
{
public:
    void Insert(const T &value)
    {
        std::unique_lock lock(m_mutex);
        m_set.insert(value);
    }

    bool Contains(const T& value) const {
        std::shared_lock lock(m_mutex);
        return m_set.find(value) != m_set.end();
    }

    //почему нельлзя вернуть константуню ссылку
    std::unordered_set<T, H, Comp> get() const
    {
        std::shared_lock lock(m_mutex);
        return m_set;
    }

private:
    mutable std::shared_mutex m_mutex;
    std::unordered_set<T, H, Comp> m_set;
};

bool IsPrime(uint64_t n)
{
    if (n < 2) return false;
//// n / i >= или что-то переписать
    for (uint64_t i = 2; n / i >= i; ++i)
    {
        if (n % i == 0) return false;
    }
    return true;
}

std::unordered_set<uint64_t> FindPrimesSingleThread(uint64_t maxNumber)
{
//// зациклится если end равен верхней границе
    std::unordered_set<uint64_t> primes;
    for (uint64_t i = 2; i <= maxNumber; ++i)
    {
        if (IsPrime(i))
        {
            primes.insert(i);
        }
    }
    return primes;
}

void FindPrimesMultiThread(uint64_t start, uint64_t end, ThreadSafeSet<uint64_t> &primes)
{
    for (uint64_t i = start; i <= end; ++i)
    {
        if (IsPrime(i))
        {
            primes.Insert(i);
        }
    }
}

template<typename Func, typename... Args>
double MeasureExecutionTime(Func func, Args &&... args)
{
    auto start = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

void InitThreads(uint64_t maxNumber, ThreadSafeSet<uint64_t> &primes, unsigned int threadCount)
{
    std::vector<std::jthread> threads;
    uint64_t range = maxNumber / threadCount;

    for (unsigned int i = 0; i < threadCount; ++i)
    {
        uint64_t start = i * range + 1;
        uint64_t end = (i == threadCount - 1)
                ? maxNumber
                : (i + 1) * range;
        threads.emplace_back(FindPrimesMultiThread, start, end, std::ref(primes));
    }
}

bool CompareOrders(std::unordered_set<uint64_t> &order1, ThreadSafeSet<uint64_t> &order2)
{
    if (order1.size() != order2.get().size())
    {
        return false;
    }
    for (auto element: order1)
    {
        if (!order2.Contains(element))
        {
            return false;
        }
    }
    return true;
}

int main()
{
    uint64_t maxNumber;
    std::cout << "Enter the upper limit for prime number search: ";
    std::cin >> maxNumber;

    std::unordered_set<uint64_t> primes;
    double singleThreadTime = MeasureExecutionTime([&]()
    {
        primes = FindPrimesSingleThread(maxNumber);
    });
    std::cout << "Execution time in single-threaded mode: " << singleThreadTime << " seconds\n";

    for (int threadAmount = 1; threadAmount <= 16; threadAmount++)
    {
        ThreadSafeSet<uint64_t> primesMulti;
        double multiThreadTime = MeasureExecutionTime(InitThreads, maxNumber, primesMulti, threadAmount);
        std::cout
                << "Execution time in multi-threaded mode ("
                << threadAmount
                << " threads): "
                << multiThreadTime
                << " seconds\n";
        if (!CompareOrders(primes, primesMulti))
        {
            std::cerr << "Getting primes error";
        }
        else
        {
            std::cout << "succ\n";
        }
    }

    return 0;
}
