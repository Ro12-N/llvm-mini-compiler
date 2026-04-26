#pragma once

#include "AST.h"
#include "IR.h"

namespace minicc {

class IRGenerator {
public:
    IRProgram generate(const Program& program);

private:
    void emitStatement(const Statement& statement);
    std::string emitExpression(const Expression& expression);
    std::string nextTemp();
    std::string nextLabel(const std::string& prefix);

    IRProgram program_;
    int tempCounter_ = 0;
    int labelCounter_ = 0;
};

} // namespace minicc
