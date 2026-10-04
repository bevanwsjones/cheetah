#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <cstdint>
#include <vector>
#include <utility>
#include <thread>
#include <optional>
#include <filesystem>

namespace cheetah::platform {

using Envrionment = std::vector<std::pair<std::string, std::string> >;

enum class ProcessStatus {
    running, exited, killed, unknown
};

inline std::string to_string(const ProcessStatus& status) {
    switch(status){
        case ProcessStatus::running:
            return "running";
        case ProcessStatus::exited:
            return "exited";
        case ProcessStatus::killed:
            return "killed";
        case ProcessStatus::unknown:
        default:
            return "unknown";
    }
}

struct ProcessState{
    ProcessStatus status{ProcessStatus::unknown};
    int exit_code{0};
};

struct ProcessHandle{
    uint32_t pid{0};
    std::filesystem::path process_path;
};

namespace details {
    std::optional<ProcessHandle> launch(const std::filesystem::path& bin_path, const std::vector<std::string>& args, const Envrionment& env);
    bool kill(const ProcessHandle& handle, bool force);
    ProcessState get_state(const ProcessHandle& handle);
}

class Process {
    public:
    Process() = delete;
    Process(const std::filesystem::path& bin_path, const std::vector<std::string>& args, const Envrionment& env) {
        launch(bin_path, args, env);
    }
    ~Process() {        
        if(status() == ProcessStatus::running) {
            kill();
            if(status() == ProcessStatus::running) { //did not die
                // warn user
                std::cerr<<"Waiting for process to terminate "; ///
                std::this_thread::sleep_for(std::chrono::seconds(wait_time));
    
                if(status() == ProcessStatus::running) {
                    std::cerr<<"Waiting for terminate failed, killing"; ///
                    kill(true);
                } //did not die
            }
        }
    };

    Process(Process&) = delete;
    Process& operator=(Process&) = delete;

    Process(Process&& other) noexcept = default;
    Process& operator=(Process&& other) noexcept {
        if(this != &other){
            kill(true);           
            handle = std::exchange(other.handle, {});
            state = std::exchange(other.state, {});
        }
        return *this;
    };

    bool launch(const std::filesystem::path& bin_path, const std::vector<std::string>& args, const Envrionment& env){
        auto new_handle = details::launch(bin_path, args, env);
        
        if(!new_handle) {
            std::cerr<<"Failed to launch process"; //
            return false;
        }
        
        handle = new_handle.value();
        state = details::get_state(handle);

        return true;

    };

    bool kill(bool force = false){
        if(state.status != ProcessStatus::running) return true;

        bool died = details::kill(handle, force);
        if(died){
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // give chance to die        
            state = details::get_state(handle);
            return true;
        }
        else {
            std::cout<<"Failed to kill process"; //
            return false;
        }
        return true;
    };
    
    ProcessStatus status() {
        if(state.status != ProcessStatus::running) return state.status;
        state = details::get_state(handle);
        return state.status;
    }

    int exit_code() { 
        if(state.status != ProcessStatus::running) return state.exit_code;
        state = details::get_state(handle);
        return state.exit_code;
    } 
    
    private:
    ProcessHandle handle;
    ProcessState state;
    static constexpr std::chrono::seconds wait_time{60}; 
};

}//cheetah::platform

