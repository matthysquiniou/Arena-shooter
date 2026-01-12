#pragma once
#include <chrono>
#include <unordered_map>
#include <string>
#include <iostream>

class Benchmark {
public:
    static void start(const std::string& tag) {
        startTimes[tag] = std::chrono::steady_clock::now();
    }

    static void end(const std::string& tag) {
        auto it = startTimes.find(tag);
        if (it == startTimes.end()) {
            std::cerr << "[Benchmark] Tag inconnu: " << tag << std::endl;
            return;
        }

        auto elapsed = std::chrono::steady_clock::now() - it->second;
        long long us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

        std::cout << "[Benchmark] " << tag << " : ";

        if (us < 1'000) {
            std::cout << us << " micros";
        }
        else if (us < 1'000'000) {
            std::cout << us / 1'000.0 << " ms";
        }
        else {
            std::cout << us / 1'000'000.0 << " s";
        }

        std::cout << std::endl;
        startTimes.erase(it);
    }


private:
    static std::unordered_map<std::string, std::chrono::steady_clock::time_point> startTimes;
};
