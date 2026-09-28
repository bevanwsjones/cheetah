#include "process.hpp"

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <string>
#include <iostream>
#include <memory>

using namespace cheetah::platform;

TEST(ProcessPosix, LaunchProcessNominal)
{   
    std::filesystem::path bin_path{"./build/bin/platform_test_binary"}; 
    std::vector<std::string> args; 
    Envrionment env;
    
    std::unique_ptr<Process> child;

    EXPECT_NO_FATAL_FAILURE([&](){
        child = std::make_unique<Process>(bin_path, args, env);
    }());
    
    std::cout<<"\n"<<to_string(child->status())<<std::endl;
    child->kill();
    EXPECT_EQ(child->status(), ProcessStatus::exited);
}

TEST(ProcessPosix, LaunchProcessBinPathDoesNotExist)
{
}

TEST(ProcessPosix, LaunchProcessNoArgs)
{
}

TEST(ProcessPosix, LaunchProcessPosixSpawnFail)
{
}