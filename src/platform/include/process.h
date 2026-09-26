#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <cstdint>
#include <vector>
#include <utility>
#include <thread>
#include <optional>

namespace cheetah::platform {

using Envrionment = std::vector<std::pair<std::string, std::string> >;

enum class ProcessStatus {
    running, exited, killed, unknown
};

struct ProcessHandle{
    uint32_t pid{0};
    std::string name;
};

namespace details {
    std::optional<ProcessHandle> launch_proces(const std::vector<std::string>& args, const Envrionment& env);
    bool kill_process(ProcessHandle& handle, bool force);
}

class Process {
    public:
    Process() = delete;
    Process(const std::string& name, const std::vector<std::string>& args, const Envrionment& env) {}
    ~Process() {
        if(process_status == ProcessStatus::running) {
            kill_process();
            std::this_thread::sleep_for(std::chrono::seconds(1)); // give chance to die
            if(process_status == ProcessStatus::running) { //did not die
                // warn user
                std::cerr<<"Waiting for process to terminate "; ///
                std::this_thread::sleep_for(std::chrono::seconds(wait_time));
    
                if(process_status == ProcessStatus::running) {
                    std::cerr<<"Waiting for terminate failed, killing"; ///
                    kill_process(true);
                } //did not die
            }
        }
    };

    Process(Process&) = delete;
    Process& operator=(Process&) = delete;

    Process(Process&&) noexcept = default;
    Process& operator=(Process&&) noexcept = default;

    bool launch_process(const std::string& name, const std::vector<std::string>& args, const Envrionment& env){
        auto new_handle = details::launch_proces(args, env);
        
        if(new_handle) {
            handle = new_handle.value();
            process_status = ProcessStatus::running;
        }
        else {
            std::cerr<<"Failed to launch process"; ///
        }

    };

    bool kill_process(bool force = false){
        if(process_status != ProcessStatus::running) return true;

        bool died = details::kill_process(handle, force);
        if(died){
            if(force) process_status = ProcessStatus::killed;
            else process_status = ProcessStatus::exited;
            return true;
        }
        else {
            std::cout<<"Failed to kill process"; // //
            return false;
        }
        return true;
    };
    
    ProcessStatus status() const {return process_status;}
    int exit_code() const {return process_exit_code; } 
    
    private:
    int process_exit_code{0}; // NEED TO GET EXIT CODE
    ProcessHandle handle;
    ProcessStatus process_status{ProcessStatus::unknown};
    static constexpr std::chrono::seconds wait_time{60}; 
};

}//cheetah::platform

