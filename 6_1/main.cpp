#include <cassert>
#include <mutex>
#include <vector>
#include <algorithm>

// Класс MemoryManager управляет блоком памяти, предоставленным извне.
class MemoryManager
{
public:
    // Конструктор инициализирует менеджер памяти переданным блоком памяти.
    MemoryManager(void* start, size_t size) noexcept
            : m_start(static_cast<char*>(start)), m_size(size), m_freeMemory(size)
    {
        // Убеждаемся, что начальный адрес выровнен корректно.
        assert(reinterpret_cast<uintptr_t>(start) % alignof(std::max_align_t) == 0);
    }

    // Удаляем конструктор копирования и оператор присваивания.
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    // Метод Allocate выделяет блок памяти заданного размера и выравнивания.
    void* Allocate(size_t size, size_t align = alignof(std::max_align_t)) noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex); // Обеспечиваем потокобезопасность.

        // Проверяем, что выравнивание является степенью двойки.
        if ((align & (align - 1)) != 0 || size == 0)
            return nullptr;

        // Находим первый подходящий свободный блок памяти.
        uintptr_t currentAddr = reinterpret_cast<uintptr_t>(m_start) + m_offset;
        uintptr_t alignedAddr = (currentAddr + (align - 1)) & ~(align - 1);

        size_t padding = alignedAddr - currentAddr;
        if (m_offset + padding + size > m_size)
            return nullptr; // Недостаточно памяти.

        m_offset += padding + size; // Обновляем смещение.
        m_allocations.emplace_back(alignedAddr, size); // Сохраняем информацию о выделении.

        return reinterpret_cast<void*>(alignedAddr);
    }

    // Метод Free освобождает ранее выделенный блок памяти.
    void Free(void* addr) noexcept
    {
        if (!addr)
            return; // Если указатель нулевой, ничего не делаем.

        std::lock_guard<std::mutex> lock(m_mutex); // Обеспечиваем потокобезопасность.

        // Ищем указанный блок в списке выделений.
        auto it = std::find_if(m_allocations.begin(), m_allocations.end(),
                               [addr](const Allocation& alloc) {
                                   return alloc.address == reinterpret_cast<uintptr_t>(addr);
                               });

        if (it != m_allocations.end())
        {
            m_freeMemory += it->size; // Увеличиваем количество свободной памяти.
            m_allocations.erase(it);  // Удаляем блок из списка выделений.
        }
    }

private:
    // Структура для хранения информации о выделении.
    struct Allocation
    {
        uintptr_t address;
        size_t size;
        Allocation(uintptr_t addr, size_t sz) : address(addr), size(sz) {}
    };

    char* m_start;                  // Начальный адрес блока памяти.
    size_t m_size;                  // Размер блока памяти.
    size_t m_offset = 0;            // Текущее смещение внутри блока памяти.
    size_t m_freeMemory;            // Количество свободной памяти.
    std::vector<Allocation> m_allocations; // Список выделений.
    std::mutex m_mutex;             // Мьютекс для обеспечения потокобезопасности.
};

// Юнит-тесты.
int main()
{
    alignas(std::max_align_t) char buffer[1000];
    MemoryManager memoryManager(buffer, sizeof(buffer));

    // Тест: Выделение и освобождение памяти.
    auto ptr = memoryManager.Allocate(sizeof(double));
    assert(ptr != nullptr);

    auto value = std::construct_at(static_cast<double*>(ptr), 3.1415927);
    assert(*value == 3.1415927);

    memoryManager.Free(ptr);

    // Тест: Несколько выделений.
    auto ptr1 = memoryManager.Allocate(100);
    auto ptr2 = memoryManager.Allocate(200);
    assert(ptr1 != nullptr && ptr2 != nullptr);

    memoryManager.Free(ptr1);
    memoryManager.Free(ptr2);

    // Тест: Попытка выделить слишком большой блок.
    auto largePtr = memoryManager.Allocate(2000);
    assert(largePtr == nullptr);

    return 0;
}
