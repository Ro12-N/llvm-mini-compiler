#pragma once

#include "IR.h"

#include <string>

namespace minicc {

class CodeGenerator {
public:
    std::string generate(const IRProgram& program) const;
};

} // namespace minicc
