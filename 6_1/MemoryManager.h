#include <cstddef>
#include <cstdint>
#include <new>
#include <cassert>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <memory> // For std::construct_at and std::destroy_at

class MemoryManager
{
public:
    MemoryManager(void* start, size_t size) noexcept
            : m_start(static_cast<uint8_t*>(start)), m_size(size)
    {
        assert(reinterpret_cast<uintptr_t>(start) % alignof(std::max_align_t) == 0);
        assert(size >= sizeof(MemoryBlock));

        // Construct the initial MemoryBlock using placement new and std::construct_at
        m_freeList = std::construct_at<MemoryBlock>(reinterpret_cast<MemoryManager::MemoryBlock *>(m_start), size - sizeof(MemoryBlock), nullptr);
    }

    ~MemoryManager()
    {
        // Destroy all MemoryBlocks in the free list
        MemoryBlock* current = m_freeList;
        while (current)
        {
            MemoryBlock* next = current->nextBlock;
            std::destroy_at(current);
            current = next;
        }
    }

    void* Allocate(size_t size, size_t align = alignof(std::max_align_t)) noexcept
    {
        if (size == 0 || (align & (align - 1)) != 0)
        {
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(m_mutex);

        MemoryBlock* prevBlock = nullptr;
        MemoryBlock* currentBlock = m_freeList;

        while (currentBlock)
        {
            uintptr_t blockStart = reinterpret_cast<uintptr_t>(currentBlock + 1);
            uintptr_t alignedStart = (blockStart + (align - 1)) & ~(align - 1);
            size_t padding = alignedStart - blockStart;

            if (currentBlock->blockSize >= size + padding)
            {
                size_t remainingSize = currentBlock->blockSize - (size + padding);
                if (remainingSize >= sizeof(MemoryBlock))
                {
                    // Construct the new MemoryBlock using std::construct_at
                    auto newBlock = std::construct_at<MemoryBlock>(
                            reinterpret_cast<MemoryBlock*>(alignedStart + size),
                            remainingSize,
                            currentBlock->nextBlock
                    );
                    currentBlock->nextBlock = newBlock;
                }

                if (prevBlock)
                {
                    prevBlock->nextBlock = currentBlock->nextBlock;
                }
                else
                {
                    m_freeList = currentBlock->nextBlock;
                }

                currentBlock->blockSize = size + padding;
                return reinterpret_cast<void*>(alignedStart);
            }

            prevBlock = currentBlock;
            currentBlock = currentBlock->nextBlock;
        }

        return nullptr;
    }

    void Free(void* addr) noexcept
    {
        if (!addr)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);

        auto block = reinterpret_cast<MemoryBlock*>(
                static_cast<uint8_t*>(addr) - sizeof(MemoryBlock));

        assert(reinterpret_cast<uint8_t*>(block) >= m_start &&
               reinterpret_cast<uint8_t*>(block) < m_start + m_size);

        MemoryBlock* prevBlock = nullptr;
        MemoryBlock* currentBlock = m_freeList;

        while (currentBlock && currentBlock < block)
        {
            prevBlock = currentBlock;
            currentBlock = currentBlock->nextBlock;
        }

        block->nextBlock = currentBlock;
        if (prevBlock)
        {
            prevBlock->nextBlock = block;
        }
        else
        {
            m_freeList = block;
        }

        if (prevBlock)
        {
            MergeBlocks(prevBlock, block);
            MergeBlocks(prevBlock, currentBlock);
        }
        else
        {
            MergeBlocks(block, currentBlock);
        }

        // Destroy the MemoryBlock if it's no longer in use
        // Note: Destruction should be handled in the destructor
    }

private:
    struct MemoryBlock
    {
        size_t blockSize;
        MemoryBlock* nextBlock;

        // Default constructor for std::construct_at
        MemoryBlock(size_t size, MemoryBlock* next) noexcept
                : blockSize(size), nextBlock(next) {}
    };

    uint8_t* m_start;
    size_t m_size;
    MemoryBlock* m_freeList;
    std::mutex m_mutex;

    void MergeBlocks(MemoryBlock* prev, MemoryBlock* current) noexcept
    {
        if (prev && reinterpret_cast<uint8_t*>(prev) + prev->blockSize + sizeof(MemoryBlock) == reinterpret_cast<uint8_t*>(current))
        {
            prev->blockSize += current->blockSize + sizeof(MemoryBlock);
            prev->nextBlock = current->nextBlock;
            // Destroy the merged block
            std::destroy_at(current);
        }
    }
};