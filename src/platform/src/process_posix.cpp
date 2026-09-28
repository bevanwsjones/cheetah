#include "process.hpp"

#include <spawn.h>
#include <filesystem>

#include <sys/types.h>
#include <sys/wait.h>

namespace cheetah::platform::details {

    
std::vector<char*> to_char_array(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    argv.reserve(args.size());
    for(auto str : args) argv.push_back(const_cast<char*>(str.data()));
    return argv;
};

std::pair<std::vector<std::string>, std::vector<char*>> to_char_array(const Envrionment& env) {
    std::vector<std::string> environ_concat;
    std::vector<char*> environ;
    environ_concat.reserve(env.size());
    environ.reserve(env.size());
    
    for(auto str : env) {
        environ_concat.push_back((str.first + "=" + str.second));
    }
    for(auto str : environ_concat) {
        environ.push_back(const_cast<char*>(str.data()));
    }
    return {environ_concat, environ};
};

std::optional<ProcessHandle> launch(const std::filesystem::path& bin_path, const std::vector<std::string>& args, const Envrionment& env){
    if(!std::filesystem::exists(bin_path)){
        std::cerr<<"Cannot launch process, binary does not exist "<<bin_path.string();
        return {};
    }
    
    std::vector<std::string> arg_strs;
    arg_strs.push_back(bin_path.string());              // argv[0]
    arg_strs.insert(arg_strs.end(), args.begin(), args.end());
   
   std::vector<std::string> env_strs;
   for (const auto& [k, v] : env) env_strs.push_back(k + "=" + v);
    
    std::vector<char*> argv, envp;
    for (auto& s : arg_strs) argv.push_back(s.data());
    for (auto& s : env_strs) envp.push_back(s.data());
    argv.push_back(nullptr);
    envp.push_back(nullptr);

    ProcessHandle handle;
    int pid;
    // std::vector<char*> argv = to_char_array(args);
    
    auto [environ_stro, environ] = to_char_array(env);

    posix_spawnattr_t attr;
    posix_spawnattr_init(&attr);

    int spawned_return = posix_spawn(&pid, bin_path.c_str(), NULL, &attr, argv.data(), envp.data());

    if(spawned_return != 0) {
        std::cerr<<"Failed to spawn new process: " + bin_path.string();
        return{};
    }

    handle.pid = static_cast<uint32_t>(pid);   
    handle.process_path = bin_path; 
    return {handle};
}

bool kill(const ProcessHandle& handle, bool force){
    return true;
}

ProcessState get_state(const ProcessHandle& handle){
    
    int status;
    auto result = waitpid(static_cast<pid_t>(handle.pid), &status, WNOHANG);

    if(result == 0) return {ProcessStatus::running, 0};
    
    if(result == -1){
        std::cerr<<"Failed to get status for "<<handle.process_path.string();
        return {ProcessStatus::unknown, -1};
    }

    if (WIFEXITED(status)) return {ProcessStatus::exited, WEXITSTATUS(status)};
    if (WIFSIGNALED(status)) return {ProcessStatus::killed, WTERMSIG(status)};
    
    std::cerr<<"Obtained unknown state for process "<<handle.process_path.string();
    return  {ProcessStatus::unknown, -1}; 
}

}//cheetah::platform::details