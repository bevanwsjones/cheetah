#include <cxxopts.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
    cxxopts::Options options("test_binary", "Helper process for PAL tests");
    options.add_options()
        ("e,exit-code", "Exit with this code", cxxopts::value<int>()->default_value("0"))
        ("s,sleep-ms",  "Sleep before exiting", cxxopts::value<int>()->default_value("0"))
        ("m,message",   "Print to stdout",     cxxopts::value<std::string>())
        ("h,help",      "Print usage");

    try {
        auto result = options.parse(argc, argv);
        if (result.count("help")) { std::cout << options.help() << '\n'; return 0; }
        if (result.count("message"))
            std::cout << result["message"].as<std::string>() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(result["sleep-ms"].as<int>()));
        return result["exit-code"].as<int>();
    }
    catch (const cxxopts::exceptions::exception& e) {
        std::cerr << e.what() << '\n' << options.help();
        return 2;
    }
    return 0;
}