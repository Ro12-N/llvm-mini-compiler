#include "CodeGenerator.h"

#include <map>
#include <sstream>

namespace minicc {
namespace {

bool needsStorage(const std::string& operand) {
    return !operand.empty() && !isImmediate(operand);
}

std::string slotName(const int index) {
    return "[rbp-" + std::to_string((index + 1) * 8) + "]";
}

std::string readOperand(const std::string& operand, const std::map<std::string, int>& slots) {
    if (operand.empty()) {
        return "0";
    }

    if (isImmediate(operand)) {
        return operand.substr(1);
    }

    return slotName(slots.at(operand));
}

} // namespace

std::string CodeGenerator::generate(const IRProgram& program) const {
    std::map<std::string, int> slots;
    auto ensureSlot = [&](const std::string& name) {
        if (!needsStorage(name)) {
            return;
        }
        if (slots.find(name) == slots.end()) {
            const int nextIndex = static_cast<int>(slots.size());
            slots.insert({name, nextIndex});
        }
    };

    for (const auto& instruction : program.instructions()) {
        switch (instruction.op) {
        case IROp::LoadConst:
        case IROp::Move:
        case IROp::Add:
        case IROp::Sub:
        case IROp::Mul:
        case IROp::Div:
        case IROp::Neg:
        case IROp::Not:
        case IROp::CmpEq:
        case IROp::CmpNe:
        case IROp::CmpLt:
        case IROp::CmpLe:
        case IROp::CmpGt:
        case IROp::CmpGe:
            ensureSlot(instruction.result);
            ensureSlot(instruction.arg1);
            ensureSlot(instruction.arg2);
            break;
        case IROp::Print:
            ensureSlot(instruction.arg1);
            break;
        case IROp::Label:
        case IROp::Jump:
        case IROp::JumpIfZero:
            ensureSlot(instruction.arg1);
            break;
        }
    }

    std::ostringstream out;
    out << "; pseudo x86-64 output (Intel-like syntax)\n";
    out << "section .text\n";
    out << "global _start\n\n";
    out << "_start:\n";
    out << "    push rbp\n";
    out << "    mov rbp, rsp\n";
    out << "    sub rsp, " << (slots.empty() ? 8 : static_cast<int>(slots.size()) * 8) << "\n\n";

    for (const auto& instruction : program.instructions()) {
        switch (instruction.op) {
        case IROp::LoadConst:
            out << "    mov eax, " << readOperand(instruction.arg1, slots) << "\n";
            out << "    mov " << slotName(slots.at(instruction.result)) << ", eax\n";
            break;

        case IROp::Move:
            out << "    mov eax, " << readOperand(instruction.arg1, slots) << "\n";
            out << "    mov " << slotName(slots.at(instruction.result)) << ", eax\n";
            break;

        case IROp::Add:
        case IROp::Sub:
        case IROp::Mul:
        case IROp::Div:
            out << "    mov eax, " << readOperand(instruction.arg1, slots) << "\n";
            if (instruction.op == IROp::Add) {
                out << "    add eax, " << readOperand(instruction.arg2, slots) << "\n";
            } else if (instruction.op == IROp::Sub) {
                out << "    sub eax, " << readOperand(instruction.arg2, slots) << "\n";
            } else if (instruction.op == IROp::Mul) {
                out << "    imul eax, " << readOperand(instruction.arg2, slots) << "\n";
            } else {
                out << "    cdq\n";
                out << "    mov ebx, " << readOperand(instruction.arg2, slots) << "\n";
                out << "    idiv ebx\n";
            }
            out << "    mov " << slotName(slots.at(instruction.result)) << ", eax\n";
            break;

        case IROp::Neg:
            out << "    mov eax, " << readOperand(instruction.arg1, slots) << "\n";
            out << "    neg eax\n";
            out << "    mov " << slotName(slots.at(instruction.result)) << ", eax\n";
            break;

        case IROp::Not:
            out << "    mov eax, " << readOperand(instruction.arg1, slots) << "\n";
            out << "    cmp eax, 0\n";
            out << "    sete al\n";
            out << "    movzx eax, al\n";
            out << "    mov " << slotName(slots.at(instruction.result)) << ", eax\n";
            break;

        case IROp::CmpEq:
        case IROp::CmpNe:
        case IROp::CmpLt:
        case IROp::CmpLe:
        case IROp::CmpGt:
        case IROp::CmpGe:
            out << "    mov eax, " << readOperand(instruction.arg1, slots) << "\n";
            out << "    cmp eax, " << readOperand(instruction.arg2, slots) << "\n";
            switch (instruction.op) {
            case IROp::CmpEq: out << "    sete al\n"; break;
            case IROp::CmpNe: out << "    setne al\n"; break;
            case IROp::CmpLt: out << "    setl al\n"; break;
            case IROp::CmpLe: out << "    setle al\n"; break;
            case IROp::CmpGt: out << "    setg al\n"; break;
            case IROp::CmpGe: out << "    setge al\n"; break;
            default: break;
            }
            out << "    movzx eax, al\n";
            out << "    mov " << slotName(slots.at(instruction.result)) << ", eax\n";
            break;

        case IROp::Label:
            out << instruction.result << ":\n";
            break;

        case IROp::Jump:
            out << "    jmp " << instruction.result << "\n";
            break;

        case IROp::JumpIfZero:
            out << "    mov eax, " << readOperand(instruction.arg1, slots) << "\n";
            out << "    cmp eax, 0\n";
            out << "    je " << instruction.result << "\n";
            break;

        case IROp::Print:
            out << "    mov eax, " << readOperand(instruction.arg1, slots) << "\n";
            out << "    call print_int ; runtime stub\n";
            break;
        }
        out << "\n";
    }

    out << "    mov rsp, rbp\n";
    out << "    pop rbp\n";
    out << "    ret\n\n";
    out << "print_int:\n";
    out << "    ; pseudo runtime hook for integer printing\n";
    out << "    ret\n";
    return out.str();
}

} // namespace minicc
