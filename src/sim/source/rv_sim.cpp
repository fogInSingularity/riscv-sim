#include "sim/rv_sim.hpp"

#include <cassert>

#include <spdlog/spdlog.h>

#include "helpers/traceable_exception.hpp"
#include "helpers/common.hpp"
#include "isa/mnemonics.hpp"
#include "sim/decode.hpp"
#include "sim/dispatch.hpp"
#include "isa/isa_defs.hpp"

namespace sim {
    
int RVSim::Execute() {
    while (!should_stop_) {
        // Trace();

        isa::UndecodedInsn undecoded_insn = memory_.Fetch(ip_);
        spdlog::trace("undecoded insn: {:#04x}", undecoded_insn);

        isa::InsnMnemonic mnem = Decode(undecoded_insn);
        spdlog::trace("Mnemonic: {}", isa::MnemonicToStr(mnem));
        if (mnem == isa::InsnMnemonic::kInvalid) {
            throw hlp::TraceableException{"Unkown instuction encountered"};
        }
        auto instr_callback = GetInsnCallback(mnem);
        
        instr_callback(this, undecoded_insn);
    }

    return xregs_[hlp::FromEnum(isa::XRegAlias::a0)];
}

} // namespace sim
