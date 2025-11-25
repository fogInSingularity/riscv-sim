#ifndef ELF_LOADER_HPP_
#define ELF_LOADER_HPP_

#include <vector>
#include <string>

#include "sim/memory_segm.hpp"
#include "isa/isa_defs.hpp"

namespace sim {

struct ParsedElf {
    std::vector<MemorySegm> mem;
    isa::Address entry_point;
    isa::Address global_ptr;
};

[[nodiscard]] ParsedElf LoadElf(const std::string& elf_path);

} // namespace sim

#endif // ELF_LOADER_HPP_
