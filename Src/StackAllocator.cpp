
#include <cstddef>
#include <cstdlib>
#include "Allocator/StackAllocator.h"
#include "Util.hpp"

static const size_t MIN_BLOCK_SIZE = 128;

StackAllocator::StackAllocator(size_t minBlockSize, size_t defaultAlignment)
    : _defaultAlignment(Util::UpAlignmentPowerOfTwo(defaultAlignment))
    , _defaultBlockSize(minBlockSize < MIN_BLOCK_SIZE ? MIN_BLOCK_SIZE: minBlockSize)
    , _pFirst(nullptr)
    , _pStackTopBlock(nullptr)
{
    _defaultBlockSize = Util::UpAlignmentPowerOfTwo(_defaultBlockSize);
}

StackAllocator::~StackAllocator()
{
    BlockHeader* pItr = _pFirst;
    while (pItr != nullptr)
    {
        BlockHeader* pNeedToFree = pItr;
        pItr = pItr->pNext;
        ::free(pNeedToFree);
    }
}

void* StackAllocator::Allocate(size_t size)
{
    return Allocate(size, _defaultAlignment);
}

void* StackAllocator::Allocate(size_t size, size_t alignment)
{
    size_t requiredPaddedSize = Util::UpAlignment(size, alignment);
    size_t frameHeaderSize = Util::GetPaddedSize<FrameHeader>(_defaultAlignment);\

    if (_pFirst == nullptr)
        AddBlock(requiredPaddedSize);

    FrameHeader* pStackTopFrame = _pStackTopBlock->pLastFrame;
    size_t available = 0;

    // Nullptr means this is the first block
    if (pStackTopFrame == nullptr)
    {
        available = _pStackTopBlock->size - frameHeaderSize;

        if (    )
    }
    else
    {
        // Calculate available size
        size_t blockStartAddr = reinterpret_cast<size_t>(GetBlockStartPtr(_pStackTopBlock));
        size_t blockEndAddr = blockStartAddr + _pStackTopBlock->size;
        size_t topFrameStartAddr = reinterpret_cast<size_t>(GetFrameStartPtr(pStackTopFrame));
        available = blockEndAddr - topFrameStartAddr;
    }

    // Current block is insufficient, allocate another block
    if (available < requiredPaddedSize)
    {
        BlockHeader* pBlockHeader = AddBlock(requiredPaddedSize);

    }










    if (available < requiredPaddedSize)
    {
        BlockHeader* pBlockHeader = AddBlock(requiredPaddedSize);
        FrameHeader* pBlockFirstFrame = GetBlockFirstFrame(pBlockHeader);
        pBlockFirstFrame->pPrev = _pStackTopFrame;
        pBlockFirstFrame->used = true;
        _pStackTopFrame = pBlockFirstFrame;
        _pStackTopBlock = pBlockHeader;

        return GetFrameStartPtr(_pStackTopFrame);
    }
    else 
    {
        size_t leftSize = available - requiredPaddedSize;
        // Left size is not enough to place a new frame header, allocate another block
        if (leftSize > frameHeaderSize)
        {
            void* result = GetFrameStartPtr(_pStackTopFrame);
            FrameHeader* pNextFrame = reinterpret_cast<FrameHeader*>(reinterpret_cast<size_t>(result) + requiredPaddedSize);
            pNextFrame->pPrev = _pStackTopFrame;
            pNextFrame->used = false;

            _pStackTopFrame->used = true;
            _pStackTopFrame = pNextFrame;
            return result;
        }
        // C
        else
        {

        }
    }

    return nullptr;
}

void StackAllocator::Deallocate(void* p)
{

}

size_t StackAllocator::GetCurrentBlockNum() const
{
    size_t result = 0;

    BlockHeader* pItr = _pFirst;
    while (pItr != nullptr)
    {
        result++;
        pItr = pItr->pNext;
    }

    return result;
}

StackAllocator::BlockHeader* StackAllocator::AddBlock(size_t requiredSize)
{
    // Get the minimum size should be allocated = required size from outside + one node header.
    size_t minimumRequiredSize = requiredSize + Util::GetPaddedSize<FrameHeader>(_defaultAlignment);

    // The content size of block is at least Default Block Size.
    size_t blockContentSize = Util::UpAlignment(minimumRequiredSize > _defaultBlockSize
        ? minimumRequiredSize : _defaultBlockSize, _defaultAlignment);

    // Total allocate size = block header + block content
    size_t totalSize = blockContentSize + Util::GetPaddedSize<BlockHeader>(_defaultAlignment);

    void* pMemory = ::malloc(totalSize);

    BlockHeader* pBlock = reinterpret_cast<BlockHeader*>(pMemory);
    pBlock->pNext = nullptr;
    pBlock->size = blockContentSize;
    pBlock->pLastFrame = nullptr;

    if (_pFirst == nullptr)
         _pFirst = pBlock;
    else
    {
        auto pItr = _pFirst;
        while (pItr->pNext != nullptr)
            pItr = pItr->pNext;

        pItr->pNext = pBlock;
    }

    return pBlock;
}

void* StackAllocator::GetBlockStartPtr(const BlockHeader* pBlock) const
{
    size_t addrBlock = reinterpret_cast<size_t>(pBlock);
    return reinterpret_cast<void*>(addrBlock + Util::GetPaddedSize<BlockHeader>(_defaultAlignment));
}

void* StackAllocator::GetFrameStartPtr(const FrameHeader* pFrame) const
{
    size_t addrFrame = reinterpret_cast<size_t>(pFrame);
    return reinterpret_cast<void*>(addrFrame + Util::GetPaddedSize<FrameHeader>(_defaultAlignment));
}

StackAllocator::FrameHeader * StackAllocator::GetBlockFirstFrame(const BlockHeader *pBlock) const
{
    return reinterpret_cast<FrameHeader*>(GetBlockStartPtr(pBlock));
}
