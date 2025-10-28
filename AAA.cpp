#include <iostream>
#include <vector>
#include <new>      // std::bad_alloc
#include <cstdint>
#include <chrono>
#include <thread>

int main() {
    std::vector<std::unique_ptr<char[]>> blocks;
    const std::size_t block_size = 99 * 1024 * 1024; // 10 MB
    std::size_t allocated_mb = 0;

    try {
        while (true) {
            auto block = std::make_unique<char[]>(block_size);
            // touch the memory to ensure OS actually commits pages
            for (std::size_t i = 0; i < block_size; i += 4096) block[i] = 0;
            blocks.push_back(std::move(block));
            allocated_mb += block_size / (1024 * 1024);
            std::cout << "Allocated: " << allocated_mb << " MB\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // slow it down
        }
    } catch (const std::bad_alloc&) {
        std::cerr << "Allocation failed after allocating ~" << allocated_mb << " MB. Exiting safely.\n";
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    // clean up and exit cleanly
    blocks.clear();
    std::cout << "Freed memory and exiting.\n";
    return 0;
}
