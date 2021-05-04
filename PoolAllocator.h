//
// Created by iancu on 02.05.2021.
//

#ifndef TEMA_ALOCATOR_POOLALLOCATOR_H
#define TEMA_ALOCATOR_POOLALLOCATOR_H

#include "MemoryBlock.h"
#include <vector>
#include <cstdlib>

struct MemoryStats {
    size_t allocations = 0;
    size_t deallocations = 0;
    size_t max_usage = 0;
    size_t current_usage = 0;
    size_t system_usage = 0;
    int pools = 0;
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
}

template<typename T>
PoolAllocator<T>::~PoolAllocator() {
    for (MemoryBlock *pool : pools) {
        free(pool);
    }
}
/**
 * When we allocate a new object, we just advance the allocation pointer to the next object
 * @tparam T
 * @return
 */
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
/**
 * When we deallocate an object, we set the allocation pointer to the current object
 * and set the next pointer to the previous allocation pointer
 * @tparam T
 * @param obj
 */
template<typename T>
void PoolAllocator<T>::deallocate(T *obj) {
    MemoryBlock *block = reinterpret_cast<MemoryBlock *>(obj) - 1;
    block->next = allocation;
    allocation = block;
    stats.deallocations++;
    stats.current_usage -= sizeof(T);
}
/**
 * Allocates a new pool memory for our objects
 * In this pool we keep the memory for the object preceded by a pointer to the next block.
 * So a memory pool of size N objects will span N * (size_t + sizeof(T)) bytes
 *
 * @tparam T the type of object we want to allocate memory for
 * @return
 */
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
