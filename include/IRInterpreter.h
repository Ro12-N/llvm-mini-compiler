#pragma once

#include "IR.h"

#include <vector>

namespace minicc {

class IRInterpreter {
public:
    std::vector<int> execute(const IRProgram& program) const;
};

} // namespace minicc
