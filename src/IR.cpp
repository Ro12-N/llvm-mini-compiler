#include "IR.h"

#include "Error.h"

#include <sstream>

namespace minicc {

void IRProgram::add(const IROp op,
                    std::string result,
                    std::string arg1,
                    std::string arg2,
                    const SourceLocation& location) {
    instructions_.push_back(IRInstruction{op, std::move(result), std::move(arg1), std::move(arg2), location});
}

const std::vector<IRInstruction>& IRProgram::instructions() const noexcept {
    return instructions_;
}

std::string IRProgram::toString() const {
    std::ostringstream out;
    for (const auto& instruction : instructions_) {
        switch (instruction.op) {
        case IROp::LoadConst:
            out << instruction.result << " = const " << instruction.arg1;
            break;
        case IROp::Move:
            out << instruction.result << " = mov " << instruction.arg1;
            break;
        case IROp::Add:
        case IROp::Sub:
        case IROp::Mul:
        case IROp::Div:
        case IROp::CmpEq:
        case IROp::CmpNe:
        case IROp::CmpLt:
        case IROp::CmpLe:
        case IROp::CmpGt:
        case IROp::CmpGe:
            out << instruction.result << " = " << irOpName(instruction.op)
                << " " << instruction.arg1 << ", " << instruction.arg2;
            break;
        case IROp::Neg:
        case IROp::Not:
            out << instruction.result << " = " << irOpName(instruction.op)
                << " " << instruction.arg1;
            break;
        case IROp::Label:
            out << instruction.result << ":";
            break;
        case IROp::Jump:
            out << "jump " << instruction.result;
            break;
        case IROp::JumpIfZero:
            out << "jz " << instruction.arg1 << ", " << instruction.result;
            break;
        case IROp::Print:
            out << "print " << instruction.arg1;
            break;
        }
        out << "\n";
    }
    return out.str();
}

std::string irOpName(const IROp op) {
    switch (op) {
    case IROp::LoadConst: return "const";
    case IROp::Move: return "mov";
    case IROp::Add: return "add";
    case IROp::Sub: return "sub";
    case IROp::Mul: return "mul";
    case IROp::Div: return "div";
    case IROp::Neg: return "neg";
    case IROp::Not: return "not";
    case IROp::CmpEq: return "cmpeq";
    case IROp::CmpNe: return "cmpne";
    case IROp::CmpLt: return "cmplt";
    case IROp::CmpLe: return "cmple";
    case IROp::CmpGt: return "cmpgt";
    case IROp::CmpGe: return "cmpge";
    case IROp::Label: return "label";
    case IROp::Jump: return "jump";
    case IROp::JumpIfZero: return "jz";
    case IROp::Print: return "print";
    }

    return "unknown";
}

bool isImmediate(const std::string& operand) {
    return !operand.empty() && operand.front() == '#';
}

int parseImmediate(const std::string& operand) {
    if (!isImmediate(operand)) {
        throwCompileError(SourceLocation{0, 0}, "internal IR error: operand is not an immediate");
    }
    return std::stoi(operand.substr(1));
}

} // namespace minicc
