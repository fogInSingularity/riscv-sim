#include "sim/elf_loader.hpp"

#include <cstdint>
#include <execution>
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
#include "elfio/elfio_symbols.hpp"
#include "helpers/traceable_exception.hpp"
#include "sim/memory_segm.hpp"
#include "isa/isa_defs.hpp"

namespace sim {

namespace {

bool IsTargetCorrect(const ELFIO::elfio& elf_loader);

} // namespace

ParsedElf LoadElf(const std::string& elf_path) {
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
        spdlog::debug("Found loadable segment, virtual address {}, memory size {}", segm->get_virtual_address(), segm->get_memory_size());
        isa::Address start_addr = segm->get_virtual_address();
        isa::MemSize mem_size = segm->get_memory_size();
        isa::MemSize file_size = segm->get_file_size();
        const auto* segm_data = reinterpret_cast<const isa::MemByte*>(segm->get_data());

        auto perms = segm->get_flags();

        MemorySegm mem_segm = {
            .start_addr = start_addr,
            .end_addr = start_addr + mem_size,
            .data = decltype(MemorySegm::data)(mem_size),

            .r_permission = (perms & ELFIO::PF_R) == ELFIO::PF_R,
            .w_permission = (perms & ELFIO::PF_W) == ELFIO::PF_W,
            .x_permission = (perms & ELFIO::PF_X) == ELFIO::PF_X,
        };

        std::copy(segm_data, segm_data + file_size, mem_segm.data.data());
        std::fill(mem_segm.data.data() + file_size, mem_segm.data.data() + mem_size, 0);

        memory_segms.push_back(std::move(mem_segm));
    }

    int sect_num = elf_loader.sections.size();

    ParsedElf parsed_elf{};

    for (int i = 0; i < sect_num; i++) {
        auto* sect = elf_loader.sections[i];
        if (sect->get_type() != ELFIO::SHT_SYMTAB) { continue; }
        // .symtab
        
        const ELFIO::symbol_section_accessor symtab{elf_loader, sect};
        ELFIO::Elf64_Addr sym_addr{};
        ELFIO::Elf_Xword sym_size{};
        uint8_t sym_bind{};
        uint8_t sym_type{};
        ELFIO::Elf_Half sym_ind{};
        uint8_t sym_other{};

        bool found_gp = symtab.get_symbol(
            "__global_pointer$", 
            sym_addr, 
            sym_size, 
            sym_bind, 
            sym_type, 
            sym_ind, 
            sym_other
        );

        if (!found_gp) { continue; }

        parsed_elf.global_ptr = sym_addr;
    }

    parsed_elf.entry_point = elf_loader.get_entry();
    parsed_elf.mem = std::move(memory_segms);

    return parsed_elf;
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
