#include <iostream>
#include <thread>
#include <vector>
#include "lru_cache.h"
#include "lfu_cache.h"

void testLRU() {
    LRUCache<int, int> cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    std::cout << "LRU Get 1: " << cache.get(1) << std::endl; // 1
    cache.put(3, 3);
    try { cache.get(2); } 
    catch(...) { std::cout << "LRU Key 2 evicted!" << std::endl; }
}

void testLFU() {
    LFUCache<int, int> cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    std::cout << "LFU Get 1: " << cache.get(1) << std::endl; // 1
    cache.put(3, 3);
    try { cache.get(2); } 
    catch(...) { std::cout << "LFU Key 2 evicted!" << std::endl; }
}

void concurrentTest() {
    LRUCache<int, int> cache(10);
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&cache, i]() {
            cache.put(i, i);
            try { std::cout << cache.get(i) << " "; } 
            catch(...) {}
        });
    }
    for (auto& t : threads) t.join();
}

int main() {
    std::cout << "=== Testing LRU ===" << std::endl;
    testLRU();
    std::cout << "\n=== Testing LFU ===" << std::endl;
    testLFU();
    std::cout << "\n=== Concurrent Test ===" << std::endl;
    concurrentTest();
    return 0;
}