# Tema Alocator
This project was built using [CMake](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) 3.17.

Building the project:
```
cmake .
cmake --build .
```

The Object pool allocator serves as a memory preallocation to allow faster work with memory (rather than using `malloc()` or `new`). \
It's especially good in use cases where we need to allocate a large number of objects of the same type (games for example). \
It has an `O(1)` complexity for both allocation and deallocation.

#### The mechanism works the following way: 
We allocate a pool of memory where we hold a pointer to the next memory block followed by the object itself. \
When we want to allocate new memory we advance the allocation pointer to the next block of memory, \
if there is no more memory in the current pool, we allocate a new one. 

When we deallocate a block we move the allocation pointer to the respective block and we set the next pointer of the block to the previous allocation pointer. \
This way, no matter what we do, the allocator will return the right block of memory when we allocate and thus, no defragmentation is needed.

### Improvement Ideas

One disadvantage of the current implementation is that the object user cannot free memory to the operating system unless the whole allocator is destroyed. \
This can be solved introducing a defragmentation process to allow us freeing unnecesary memory pools and improve resource consumption.
