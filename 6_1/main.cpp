#include "MemoryManager.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

int main() {
    {
        alignas(std::max_align_t) char buffer[1024];
        MemoryManager memoryManager(buffer, sizeof(buffer));

        constexpr size_t blockSize = 16;
        constexpr size_t blockCount = 600 / (blockSize + sizeof(size_t));

        void* blocks[blockCount];
        for (size_t i = 0; i < blockCount; ++i) {
            blocks[i] = memoryManager.Allocate(blockSize);
            if (blocks[i] == nullptr) {
                std::cout << "null \n";
            }
        }

        for (size_t i = 0; i < 1000; ++i) {
            alignas(std::max_align_t) char buffer[1024 * 1024];
            MemoryManager memoryManager(buffer, sizeof(buffer));

            std::vector<void*> arr;

            for (size_t j = 0; j < 1000; ++j) {
                size_t size = std::rand() % 100 + 16;
                void* ptr = memoryManager.Allocate(size, alignof(std::max_align_t));
                if (ptr == nullptr) {
                    std::cout << "Failed to allocate memory\n";
                    break;
                }
                arr.push_back(ptr);
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(arr.begin(), arr.end(), gen);

            for (size_t j = 0; j < arr.size(); ++j) {
                memoryManager.Free(arr[j]);
            }
        }
    }

    return 0;
}