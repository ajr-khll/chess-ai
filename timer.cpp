#include <chrono>
#include <iostream>

struct Timer {
  std::chrono::time_point<std::chrono::steady_clock> start;

  Timer() : start(std::chrono::steady_clock::now()) {}

  ~Timer() {
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "Elapsed: " << ms << " ms\n";
  }
};
