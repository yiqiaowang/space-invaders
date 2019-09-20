#include "spdlog/spdlog.h"
#include "dbg.h"

int main() 
{
    spdlog::info("Welcome to spdlog!");

    std::string message = "Hello World";
    dbg(message);

    return 0;
}
