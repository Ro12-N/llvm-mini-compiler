#pragma once

#include "Token.h"

#include <string>
#include <vector>

namespace minicc {

enum class IROp {
    LoadConst,
    Move,
    Add,
    Sub,
    Mul,
    Div,
    Neg,
    Not,
    CmpEq,
    CmpNe,
    CmpLt,
    CmpLe,
    CmpGt,
    CmpGe,
    Label,
    Jump,
    JumpIfZero,
    Print
};

struct IRInstruction {
    IROp op;
    std::string result;
    std::string arg1;
    std::string arg2;
    SourceLocation location;
};

class IRProgram {
public:
    void add(IROp op,
             std::string result,
             std::string arg1,
             std::string arg2,
             const SourceLocation& location);

    const std::vector<IRInstruction>& instructions() const noexcept;
    std::string toString() const;

private:
    std::vector<IRInstruction> instructions_;
};

std::string irOpName(IROp op);
bool isImmediate(const std::string& operand);
int parseImmediate(const std::string& operand);

} // namespace minicc
