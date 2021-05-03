//
// Created by iancu on 03.05.2021.
//
#include <iostream>
#include <chrono>
#include "Point.h"
#include "PoolAllocator.h"
#include "test.h"


void allocator_stress_test(int obj_count) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::duration;
    using std::chrono::time_point;

    Point **points = new Point *[obj_count];
    PoolAllocator<Point> allocator(obj_count);
    auto start = high_resolution_clock::now();
    for (int i = 0; i < obj_count; i++) {
        points[i] = allocator.allocate();
        *points[i] = Point(i, i);
    }
    for (int i = 0; i < obj_count; i++) {
        allocator.deallocate(points[i]);
    }
    auto end = high_resolution_clock::now();
    auto millis = duration_cast<milliseconds>(end - start);
    std::cout << "We had " << obj_count << " allocations and " << obj_count << " deallocations " << "and it lasted "
              << millis.count()
              << std::endl;
    std::cout << "Max usage:" << allocator.get_stats().max_usage << std::endl
              << "System usage:" << allocator.get_stats().system_usage << std::endl
              << "Current usage:" << allocator.get_stats().current_usage << std::endl
              << "Total allocations:" << allocator.get_stats().allocations << std::endl
              << "Total deallocations:" << allocator.get_stats().deallocations << std::endl;


}