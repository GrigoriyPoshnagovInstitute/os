#include <cstddef>
#include <cstdint>
#include <new>
#include <cassert>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <memory>

class MemoryManager
{
public:
    MemoryManager(void *start, size_t size) noexcept
            : m_start(static_cast<uint8_t *>(start)), m_size(size)
    {
        assert(reinterpret_cast<uintptr_t>(start) % alignof(std::max_align_t) == 0);
        assert(size >= sizeof(MemoryBlock));

        uintptr_t startAddress = reinterpret_cast<uintptr_t>(m_start);
        size_t alignment = alignof(MemoryBlock);
        size_t padding = (alignment - (startAddress % alignment)) % alignment;

        uint8_t *alignedStart = m_start + padding;
        size_t alignedSize = size - padding;

        assert(alignedSize >= sizeof(MemoryBlock));

        m_freeList = std::construct_at<MemoryBlock>(
                reinterpret_cast<MemoryBlock *>(alignedStart),
                alignedSize - sizeof(MemoryBlock),
                nullptr
        );
    }

    ~MemoryManager()
    {
        MemoryBlock *current = m_freeList;
        while (current)
        {
            MemoryBlock *next = current->nextBlock;
            std::destroy_at(current);
            current = next;
        }
    }

    void *Allocate(size_t size, size_t align = alignof(std::max_align_t)) noexcept
    {
        if (size == 0 || (align & (align - 1)) != 0)
            return nullptr;

        std::lock_guard<std::mutex> lock(m_mutex);

        MemoryBlock *prevBlock = nullptr;
        MemoryBlock *currentBlock = m_freeList;

        while (currentBlock)
        {
            uintptr_t blockStart = reinterpret_cast<uintptr_t>(currentBlock + 1);
            uintptr_t dataAddress = (blockStart + (align - 1)) & ~(align - 1);

            size_t dataPadding = dataAddress - blockStart;

            if (currentBlock->blockSize >= size + dataPadding)
            {
                // тут, если остается место, вставляем новый пустой блок
                size_t blockAlign = alignof(MemoryBlock);
                size_t blockPadding = ((blockAlign - (size + dataPadding) % blockAlign) % blockAlign);
                size_t remainingSize = currentBlock->blockSize - (size + dataPadding) - blockPadding;

                if (remainingSize >= sizeof(MemoryBlock) + blockPadding)
                {
                    auto blockAddress = dataAddress + size + blockPadding;
                    assert(blockAddress % alignof(MemoryBlock) == 0);

                    auto newBlock = std::construct_at<MemoryBlock>(
                            reinterpret_cast<MemoryBlock *>(blockAddress),
                            remainingSize,
                            currentBlock->nextBlock
                    );

                    currentBlock->nextBlock = newBlock;
                }
                // дальше идет основная логика

                if (prevBlock)
                    prevBlock->nextBlock = currentBlock->nextBlock;
                else
                    m_freeList = currentBlock->nextBlock;

                currentBlock->blockSize = size + dataPadding;
                assert(dataAddress % align == 0);
                return reinterpret_cast<void *>(dataAddress);
            }

            prevBlock = currentBlock;
            currentBlock = currentBlock->nextBlock;
        }

        return nullptr;
    }

    void Free(void *addr) noexcept
    {
        if (!addr)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);

        auto block = reinterpret_cast<MemoryBlock *>(
                static_cast<uint8_t *>(addr) - sizeof(MemoryBlock));

        assert(reinterpret_cast<uint8_t *>(block) >= m_start &&
               reinterpret_cast<uint8_t *>(block) < m_start + m_size);

        MemoryBlock *prevBlock = nullptr;
        MemoryBlock *currentBlock = m_freeList;

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
    }

private:
    struct MemoryBlock
    {
        size_t blockSize;
        MemoryBlock *nextBlock;

        MemoryBlock(size_t size, MemoryBlock *next) noexcept
                : blockSize(size), nextBlock(next)
        {}
    };

    uint8_t *m_start;
    size_t m_size;
    MemoryBlock *m_freeList;
    std::mutex m_mutex;

    static void MergeBlocks(MemoryBlock *prev, MemoryBlock *current) noexcept
    {
        if (prev && reinterpret_cast<uint8_t *>(prev) + prev->blockSize + sizeof(MemoryBlock) ==
                    reinterpret_cast<uint8_t *>(current))
        {
            prev->blockSize += current->blockSize + sizeof(MemoryBlock);
            prev->nextBlock = current->nextBlock;

            std::destroy_at(current);
        }
    }
};