#include <iostream>
#include <chrono>
#include <thread>

int main() {
    // Clear screen initially
    std::cout << "\x1b[2J";

    // Print 5 lines with initial content
    for (int i = 0; i < 5; i++) {
        std::cout << "Line " << i + 1 << ": Original content\n";
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Move cursor up 5 lines to start overwriting
    std::cout << "\x1b[5A";

    // Overwrite lines
    for (int i = 0; i < 5; i++) {
        // Clear the current line
        std::cout << "\x1b[2K";

        // Write new content
        std::cout << "Line " << i + 1 << ": Updated content" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Demonstrate moving cursor to a specific line (line 3)
    std::cout << "\x1b[3;1H"; // Move to row 3, column 1
    std::cout << "\x1b[2K";    // Clear that line
    std::cout << "Line 3: Changed individually!" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
