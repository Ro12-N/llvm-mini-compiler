#include "IRInterpreter.h"

#include "Error.h"

#include <unordered_map>

namespace minicc {
namespace {

int resolveOperand(const std::string& operand, const std::unordered_map<std::string, int>& values) {
    if (operand.empty()) {
        return 0;
    }

    if (isImmediate(operand)) {
        return parseImmediate(operand);
    }

    const auto found = values.find(operand);
    if (found == values.end()) {
        throwCompileError(SourceLocation{0, 0}, "internal interpreter error: unknown value '" + operand + "'");
    }
    return found->second;
}

} // namespace

std::vector<int> IRInterpreter::execute(const IRProgram& program) const {
    std::unordered_map<std::string, int> values;
    std::unordered_map<std::string, std::size_t> labels;
    std::vector<int> output;

    const auto& instructions = program.instructions();
    for (std::size_t index = 0; index < instructions.size(); ++index) {
        if (instructions[index].op == IROp::Label) {
            labels[instructions[index].result] = index;
        }
    }

    std::size_t pc = 0;
    while (pc < instructions.size()) {
        const IRInstruction& instruction = instructions[pc];

        switch (instruction.op) {
        case IROp::LoadConst:
            values[instruction.result] = resolveOperand(instruction.arg1, values);
            ++pc;
            break;

        case IROp::Move:
            values[instruction.result] = resolveOperand(instruction.arg1, values);
            ++pc;
            break;

        case IROp::Add:
            values[instruction.result] = resolveOperand(instruction.arg1, values) + resolveOperand(instruction.arg2, values);
            ++pc;
            break;

        case IROp::Sub:
            values[instruction.result] = resolveOperand(instruction.arg1, values) - resolveOperand(instruction.arg2, values);
            ++pc;
            break;

        case IROp::Mul:
            values[instruction.result] = resolveOperand(instruction.arg1, values) * resolveOperand(instruction.arg2, values);
            ++pc;
            break;

        case IROp::Div:
            values[instruction.result] = resolveOperand(instruction.arg1, values) / resolveOperand(instruction.arg2, values);
            ++pc;
            break;

        case IROp::Neg:
            values[instruction.result] = -resolveOperand(instruction.arg1, values);
            ++pc;
            break;

        case IROp::Not:
            values[instruction.result] = resolveOperand(instruction.arg1, values) == 0 ? 1 : 0;
            ++pc;
            break;

        case IROp::CmpEq:
            values[instruction.result] = resolveOperand(instruction.arg1, values) == resolveOperand(instruction.arg2, values) ? 1 : 0;
            ++pc;
            break;

        case IROp::CmpNe:
            values[instruction.result] = resolveOperand(instruction.arg1, values) != resolveOperand(instruction.arg2, values) ? 1 : 0;
            ++pc;
            break;

        case IROp::CmpLt:
            values[instruction.result] = resolveOperand(instruction.arg1, values) < resolveOperand(instruction.arg2, values) ? 1 : 0;
            ++pc;
            break;

        case IROp::CmpLe:
            values[instruction.result] = resolveOperand(instruction.arg1, values) <= resolveOperand(instruction.arg2, values) ? 1 : 0;
            ++pc;
            break;

        case IROp::CmpGt:
            values[instruction.result] = resolveOperand(instruction.arg1, values) > resolveOperand(instruction.arg2, values) ? 1 : 0;
            ++pc;
            break;

        case IROp::CmpGe:
            values[instruction.result] = resolveOperand(instruction.arg1, values) >= resolveOperand(instruction.arg2, values) ? 1 : 0;
            ++pc;
            break;

        case IROp::Label:
            ++pc;
            break;

        case IROp::Jump:
            pc = labels.at(instruction.result);
            break;

        case IROp::JumpIfZero:
            if (resolveOperand(instruction.arg1, values) == 0) {
                pc = labels.at(instruction.result);
            } else {
                ++pc;
            }
            break;

        case IROp::Print:
            output.push_back(resolveOperand(instruction.arg1, values));
            ++pc;
            break;
        }
    }

    return output;
}

} // namespace minicc
