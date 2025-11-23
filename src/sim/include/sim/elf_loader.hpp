#ifndef ELF_LOADER_HPP_
#define ELF_LOADER_HPP_

#include <vector>
#include <string>

#include "sim/memory_segm.hpp"
#include "sim/sim_defs.hpp"

namespace sim {

[[nodiscard]] std::vector<MemorySegm> LoadElf(const std::string& elf_path, Address* entry_point);

} // namespace sim

#endif // ELF_LOADER_HPP_
