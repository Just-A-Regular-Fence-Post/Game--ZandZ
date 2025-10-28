#include <iostream>
#include <chrono>
#include <thread>
#include <stdlib.h>

int main() {
    std::string message = "Testy";
    int delay_ms = 150; // Delay in milliseconds for each character

    for (char c : message) {
        std::cout << c << std::flush;
        if (c != ' ') {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        }
        // if it's a space, print immediately without waiting
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    system("clear");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << std::endl;
    return 0;
}
