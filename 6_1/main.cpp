#include "MemoryManager.h"
#include <iostream>

int main() {
    // Выделяем большой блок памяти для менеджера
    const size_t poolSize = 1024 * 1024; // 1 МБ
    void* memoryPool = std::malloc(poolSize);

    if (!memoryPool) {
        std::cerr << "Failed to allocate memory pool!" << std::endl;
        return 1;
    }

    // Создаем менеджер памяти
    MemoryManager memoryManager(memoryPool, poolSize);

    // Тест 1: Простое выделение и освобождение памяти
    void* ptr1 = memoryManager.Allocate(100);
    if (ptr1) {
        std::cout << "Test 1: Allocation of 100 bytes successful!" << std::endl;
    } else {
        std::cerr << "Test 1: Allocation failed!" << std::endl;
    }

    memoryManager.Free(ptr1);
    std::cout << "Test 1: Memory freed." << std::endl;

    // Тест 2: Выделение нескольких блоков
    void* ptr2 = memoryManager.Allocate(200);
    void* ptr3 = memoryManager.Allocate(300);
    if (ptr2 && ptr3) {
        std::cout << "Test 2: Allocation of 200 and 300 bytes successful!" << std::endl;
    } else {
        std::cerr << "Test 2: Allocation failed!" << std::endl;
    }

    memoryManager.Free(ptr2);
    memoryManager.Free(ptr3);
    std::cout << "Test 2: Memory freed." << std::endl;

    // Тест 3: Проверка выравнивания
    void* ptr4 = memoryManager.Allocate(256, 64);
    if (ptr4 && reinterpret_cast<uintptr_t>(ptr4) % 64 == 0) {
        std::cout << "Test 3: Allocation with alignment 64 successful!" << std::endl;
    } else {
        std::cerr << "Test 3: Allocation with alignment failed!" << std::endl;
    }

    memoryManager.Free(ptr4);
    std::cout << "Test 3: Memory freed." << std::endl;

    // Тест 4: Проверка на фрагментацию и повторное использование памяти
    void* ptr5 = memoryManager.Allocate(500);
    void* ptr6 = memoryManager.Allocate(500);
    if (ptr5 && ptr6) {
        std::cout << "Test 4: Allocation of two 500 bytes blocks successful!" << std::endl;
    } else {
        std::cerr << "Test 4: Allocation failed!" << std::endl;
    }

    memoryManager.Free(ptr5);
    void* ptr7 = memoryManager.Allocate(600);
    if (ptr7) {
        std::cout << "Test 4: Reuse of freed memory successful!" << std::endl;
    } else {
        std::cerr << "Test 4: Reuse of freed memory failed!" << std::endl;
    }

    memoryManager.Free(ptr6);
    memoryManager.Free(ptr7);
    std::cout << "Test 4: Memory freed." << std::endl;

    // Освобождаем пул памяти
    std::free(memoryPool);

    return 0;
}