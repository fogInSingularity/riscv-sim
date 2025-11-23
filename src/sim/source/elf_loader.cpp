#include "sim/elf_loader.hpp"

#include <memory>
#include <string>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <utility>
#include <cassert>

#include <spdlog/spdlog.h>
#include <elfio/elfio.hpp>

#include "elfio/elf_types.hpp"
#include "helpers/traceable_exception.hpp"
#include "sim/memory_segm.hpp"
#include "sim/sim_defs.hpp"

namespace sim {

namespace {

bool IsTargetCorrect(const ELFIO::elfio& elf_loader);

} // namespace

std::vector<MemorySegm> LoadElf(const std::string& elf_path, Address* entry_point) {
    ELFIO::elfio elf_loader;

    if (!elf_loader.load(elf_path)) {
        throw hlp::TraceableException{"Unable to load elf file"};
    }

    if (!IsTargetCorrect(elf_loader)) {
        throw hlp::TraceableException{"Unsupported target"};
    }

    std::vector<MemorySegm> memory_segms;

    int segm_num = elf_loader.segments.size();

    for (int i = 0; i < segm_num; i++) {
        const auto* segm = elf_loader.segments[i];
        if (segm->get_type() != ELFIO::PT_LOAD) { continue; }
        // should load
        spdlog::debug("Found loadable segment, virtual address: {}", segm->get_virtual_address());
        Address start_addr = segm->get_virtual_address();
        MemSize mem_size = segm->get_memory_size();
        MemSize file_size = segm->get_file_size();
        const auto* segm_data = reinterpret_cast<const MemByte*>(segm->get_data());

        auto perms = segm->get_flags();

        MemorySegm mem_segm = {
            .start_addr = start_addr,
            .end_addr = start_addr + mem_size,
            .data = std::make_unique<MemByte[]>(mem_size),

            .r_permission = (perms & ELFIO::PF_R) == ELFIO::PF_R,
            .w_permission = (perms & ELFIO::PF_W) == ELFIO::PF_W,
            .x_permission = (perms & ELFIO::PF_X) == ELFIO::PF_X,
        };

        std::copy(segm_data, segm_data + file_size, mem_segm.data.get());
        std::fill(mem_segm.data.get() + file_size, mem_segm.data.get() + mem_size, 0);

        memory_segms.push_back(std::move(mem_segm));
    }

    *entry_point = elf_loader.get_entry();

    return memory_segms;
}

namespace {

bool IsTargetCorrect(const ELFIO::elfio& elf_loader) {
    if (elf_loader.get_machine() != ELFIO::EM_RISCV) { 
        spdlog::error("Unsupported target arch");
        return false; 
    }

    if (elf_loader.get_class() != ELFIO::ELFCLASS32) { 
        spdlog::error("Unsupported target bitness");
        return false; 
    }

    return true;
}

} // namespace 

} // namespace sim
