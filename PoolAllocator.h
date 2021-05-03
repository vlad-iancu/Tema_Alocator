//
// Created by iancu on 02.05.2021.
//

#ifndef TEMA_ALOCATOR_POOLALLOCATOR_H
#define TEMA_ALOCATOR_POOLALLOCATOR_H

#include "MemoryBlock.h"
#include <vector>
#include <cstdlib>

struct MemoryStats {
    size_t allocations;
    size_t deallocations;
    size_t max_usage;
    size_t current_usage;
    size_t system_usage;
    int pools;
};


template<typename T>
class PoolAllocator {
private:
    MemoryBlock *allocation;
    std::vector<MemoryBlock *> pools;
    MemoryStats stats;
    int pool_size;
public:
    PoolAllocator();

    ~PoolAllocator();

    explicit PoolAllocator(int pool_size);

    T *allocate();

    void deallocate(T *obj);

    MemoryBlock *allocate_pool();

    MemoryStats get_stats();

};

template<typename T>
PoolAllocator<T>::PoolAllocator(): PoolAllocator<T>(1024) {}

template<typename T>
PoolAllocator<T>::PoolAllocator(int _pool_size) {
    pool_size = _pool_size;
    allocation = nullptr;

    stats.max_usage = 0;
    stats.pools = 0;
    stats.allocations = 0;
    stats.deallocations = 0;
    stats.current_usage = 0;
    stats.system_usage = 0;
}

template<typename T>
PoolAllocator<T>::~PoolAllocator() {
    for (MemoryBlock *pool : pools) {
        free(pool);
    }
}

template<typename T>
T *PoolAllocator<T>::allocate() {
    if (allocation == nullptr) {
        allocation = allocate_pool();
    }
    T *memory = reinterpret_cast<T *>(allocation + 1);
    allocation = allocation->next;
    stats.allocations++;
    stats.current_usage += sizeof(T);
    if (stats.current_usage > stats.max_usage) stats.max_usage = stats.current_usage;
    return memory;
}

template<typename T>
void PoolAllocator<T>::deallocate(T *obj) {
    MemoryBlock *block = reinterpret_cast<MemoryBlock *>(obj) - 1;
    block->next = allocation;
    allocation = block;
    stats.deallocations++;
    stats.current_usage -= sizeof(T);
}

template<typename T>
MemoryBlock *PoolAllocator<T>::allocate_pool() {
    size_t total_memory = pool_size * (sizeof(T) + sizeof(MemoryBlock));
    auto pool = reinterpret_cast<MemoryBlock *>(malloc(total_memory));
    MemoryBlock *current_block = pool;
    for (int i = 0; i < pool_size - 1; i++) {
        T *obj = reinterpret_cast<T *>(current_block + 1);
        current_block->next = reinterpret_cast<MemoryBlock *>(obj + 1);
        current_block = current_block->next;
    }
    current_block->next = nullptr;
    pools.push_back(pool);
    stats.system_usage += total_memory;
    stats.pools = pools.size();

    return pool;
}

template<typename T>
MemoryStats PoolAllocator<T>::get_stats() { return stats; }


#endif //TEMA_ALOCATOR_POOLALLOCATOR_H
