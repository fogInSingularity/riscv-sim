#ifndef PROGRAM_ARGS_HPP_
#define PROGRAM_ARGS_HPP_

#include <cstdint>
#include <vector>
#include <string>
#include <cstddef>
#include <cstring>

#include "isa/isa_defs.hpp"

namespace sim {

// args passed to elf:
// argc (count with program name == test name)
// for simulator run: ./sim <test> <value> -> argv[0] should be <test>
// stack should be:
// [sp+(argc+1)] = NULL
// [sp+(argc)] = last arg
//  ... 
// [sp+0] = argc
struct ProgramArgs {
    int argc_;
    std::vector<std::string> argv_;

    ProgramArgs(int argc, const char* argv[]) : argc_{argc - 1} {
        static_assert(sizeof('0') == 1);

        argv_.reserve(argc_);
        for (int i = 0; i < argc_; i++) {
            argv_.emplace_back(argv[i + 1]);
        }
    }

    int Argc() const {
        return argc_;
    }

    uint32_t SizeOfArgv() const {
        uint32_t size = 0;
        for (auto&& str: argv_) {
            size += str.size() + sizeof('0');
        }

        return size;
    }

    std::vector<uint32_t> CopyArgv(isa::MemByte* copy_to) const {
        std::vector<uint32_t> segm_offset;
        segm_offset.reserve(argc_);

        size_t copy_offset = 0;
        for (auto&& str: argv_) {
            std::memcpy(copy_to + copy_offset, str.data(), str.size() + sizeof('0'));
            segm_offset.push_back(copy_offset);
            copy_offset += str.size() + sizeof('0');
        }

        return segm_offset;
    }
};

} // namespace sim

#endif // PROGRAM_ARGS_HPP_
