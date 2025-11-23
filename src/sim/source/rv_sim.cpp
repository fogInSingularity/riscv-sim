#include "sim/rv_sim.hpp"

#include <cassert>

#include <spdlog/spdlog.h>

#include "helpers/traceable_exception.hpp"
#include "isa/mnemonics.hpp"
#include "sim/decode.hpp"
#include "sim/dispatch.hpp"
#include "sim/sim_defs.hpp"

namespace sim {
    
void RVSim::Execute() {

    while (!should_stop_) {
        sim::UndecodedInsn undecoded_insn = memory_.Fetch(ip_);
        isa::InsnMnemonic mnem = Decode(undecoded_insn);
        spdlog::trace("Mnemonic: {}", static_cast<int>(mnem));
        if (mnem == isa::InsnMnemonic::kInvalid) {
            throw hlp::TraceableException{"Unkown instuction encountered"};
        }
        auto instr_callback = GetInsnCallback(mnem);
        
        instr_callback(this, undecoded_insn);
    }
}

} // namespace sim
