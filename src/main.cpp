#include <cstdlib>
#include <iostream>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "helpers/traceable_exception.hpp"
#include "sim/elf_loader.hpp"
#include "sim/memory_segm.hpp"
#include "sim/rv_sim.hpp"
#include "sim/sim_defs.hpp"

int main(int argc, const char* argv[]) {
    auto logger = spdlog::basic_logger_mt("sim", "sim.log", true);
    spdlog::set_default_logger(logger);

#if defined (NDEBUG)
    spdlog::set_level(spdlog::level::info);
#else // NDEBUG
    spdlog::flush_on(spdlog::level::trace);
    spdlog::set_level(spdlog::level::trace);
#endif // NDEBUG

    // log argv
    for (int i = 0; i < argc; i++) {
        spdlog::info("argv[{}]: {}", i, argv[i]);
    }

    if (argc < 2) {
        std::cerr 
            << "elf file not passed\n" 
            << "Usage: " << argv[0] << " <elf file>\n";
        spdlog::error("argc < 2: {}", argc);

        return EXIT_FAILURE;
    }

    sim::Address entry_point = 0;
    std::vector<sim::MemorySegm> segms;
    
    try {
        segms = sim::LoadElf(argv[1], &entry_point);
    } catch (const hlp::TraceableException& e) {
        std::cerr << e.what() << std::endl;
        spdlog::debug("Catch exception in main: {}", e.what());

        return EXIT_FAILURE;
    }

    sim::RVSim sim{std::move(segms), entry_point};

    sim.Execute();
}

