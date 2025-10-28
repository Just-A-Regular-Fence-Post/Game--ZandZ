#include <algorithm>
#include <vector>
#include <random>
#include <iostream>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    std::random_device rd;
    std::mt19937 g(rd());

    while (true) {
        std::shuffle(v.begin(), v.end(), g);
        std::sort(v.begin(), v.end());
        std::cout << "Sorted: ";
        for (int num : v) {
            std::cout << num << " ";
    }
}
}