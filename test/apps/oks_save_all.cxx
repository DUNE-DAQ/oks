#include "CLI/CLI.hpp"
#include <iostream>
#include "oks/kernel.hpp"
#include <fmt/core.h>
#include <deque>

namespace dunedaq {
namespace oks {

} // namespace oks
} // namespace dunedaq

int main(int argc, char const *argv[])
{
    using namespace dunedaq::oks;

    CLI::App app{"App description"};

    std::string file;
    app.add_option("-f,--file", file, "Data file")
        ->required()
        ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    fmt::print("Data file: {}\n", file);

    OksKernel k;
    k.load_data(file);
    k.save_all_data();
    k.close_all_data();

    /* code */
    return 0;
}
