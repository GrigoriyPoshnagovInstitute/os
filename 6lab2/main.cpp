#include <cassert>
#include <mutex>
#include <vector>
#include <algorithm>

class MemoryManager
{
public:
    MemoryManager(void *start, size_t size) noexcept
            : m_start(static_cast<char *>(start)), m_size(size)
    {}

    // Удаляем конструктор копирования и оператор присваивания.
    MemoryManager(const MemoryManager &) = delete;

    MemoryManager &operator=(const MemoryManager &) = delete;

    void *Allocate(size_t size, size_t align = alignof(std::max_align_t)) noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        uintptr_t currentAddr = reinterpret_cast<uintptr_t>(m_start) + m_offset;
        uintptr_t alignedAddr = (currentAddr + (align - 1)) & ~(align - 1);

        size_t padding = alignedAddr - currentAddr;
        if (m_offset + padding + size > m_size)
        {
            return nullptr;
        }

        m_offset += padding + size;
        m_allocations.emplace_back(alignedAddr, size);

        return reinterpret_cast<void *>(alignedAddr);
    }

    void Free(void *addr) noexcept
    {
        if (!addr)
            return;

        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = std::find_if
                (
                        m_allocations.begin(),
                        m_allocations.end(),
                        [addr](const Allocation &alloc)
                        {
                            return alloc.address == reinterpret_cast<uintptr_t>(addr);
                        }
                );

        if (it != m_allocations.end())
        {
            m_allocations.erase(it);
        }
    }

private:
    // Структура для хранения информации о выделении.
    struct Allocation
    {
        uintptr_t address;
        size_t size;

        Allocation(uintptr_t addr, size_t sz) : address(addr), size(sz)
        {}
    };

    char *m_start;                  // Начальный адрес блока памяти.
    size_t m_size;                  // Размер блока памяти.
    size_t m_offset = 0;            // Текущее смещение внутри блока памяти.
    std::vector<Allocation> m_allocations; // Список выделений.
    std::mutex m_mutex;             // Мьютекс для обеспечения потокобезопасности.
};

int main()
{
    alignas(std::max_align_t) char buffer[1000];
    MemoryManager memoryManager(buffer, sizeof(buffer));

    auto ptr = memoryManager.Allocate(sizeof(double));
    assert(ptr != nullptr);

    auto value = std::construct_at(static_cast<double *>(ptr), 3.1415927);
    assert(*value == 3.1415927);

    memoryManager.Free(ptr);

    auto ptr1 = memoryManager.Allocate(100);
    auto ptr2 = memoryManager.Allocate(200);
    assert(ptr1 != nullptr && ptr2 != nullptr);

    memoryManager.Free(ptr1);
    memoryManager.Free(ptr2);

    auto largePtr = memoryManager.Allocate(2000);
    assert(largePtr == nullptr);

    return 0;
}
