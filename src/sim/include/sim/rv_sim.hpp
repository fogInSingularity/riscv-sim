#ifndef RV_SIM_HPP_
#define RV_SIM_HPP_

#include <utility>
#include <vector>
#include <array>

#include "sim/memory_segm.hpp"
#include "sim/memory.hpp"
#include "sim/sim_defs.hpp"

namespace sim {

class RVSim { 
  private:
    Memory memory_;
    Address ip_;
    bool should_stop_;
    std::array<Register, kNumXRegisters> xregs;
    // std::array<Register, kNumFRegisters> fregs;
  public:
    RVSim(std::vector<MemorySegm> memory, Address entry_point)
        : memory_{std::move(memory)}, ip_{entry_point}, should_stop_{false} {}

    void Execute();

    bool& ShouldStop() {
        return should_stop_;
    }
};

} // namespace sim

#endif // RV_SIM_HPP_
