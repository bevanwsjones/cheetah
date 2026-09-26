#include "process.h"

#include <spawn.h>
#include <filesystem>

namespace cheetah::platform::details {

    
std::vector<char*> to_char_array(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    argv.reserve(args.size());
    for(auto str : args) argv.push_back(const_cast<char*>(str.c_str()));
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
        environ.push_back(const_cast<char*>(str.c_str()));
    }
    return {environ_concat, environ};
};

std::optional<ProcessHandle> launch_proces(const std::filesystem::path& bin_path, const std::vector<std::string>& args, const Envrionment& env){
    ProcessHandle handle;
    int pid;
    std::vector<char*> argv = to_char_array(args);
    auto [environ_stro, environ] = to_char_array(env);

    posix_spawnattr_t attr;
    posix_spawnattr_init(&attr);

    int spawned_return = posix_spawn(&pid, bin_path.c_str(), NULL, &attr, argv.data(), environ.data());

    // free argv and environ char**

    if(spawned_return == 0) {
        std::cerr<<"Failed to spawn new process: " + bin_path.string();
        return{};
    }

    handle.pid = static_cast<uint32_t>(pid);
    handle.process_path = bin_path; 
    return {handle};
}

bool kill_process(ProcessHandle& handle, bool force){
    return true;
}

}//cheetah::platform::details