#include "IRGenerator.h"

#include "Error.h"

namespace minicc {
namespace {

IROp binaryOpToIrOp(const TokenType op) {
    switch (op) {
    case TokenType::Plus: return IROp::Add;
    case TokenType::Minus: return IROp::Sub;
    case TokenType::Star: return IROp::Mul;
    case TokenType::Slash: return IROp::Div;
    case TokenType::EqualEqual: return IROp::CmpEq;
    case TokenType::BangEqual: return IROp::CmpNe;
    case TokenType::Less: return IROp::CmpLt;
    case TokenType::LessEqual: return IROp::CmpLe;
    case TokenType::Greater: return IROp::CmpGt;
    case TokenType::GreaterEqual: return IROp::CmpGe;
    default:
        throwCompileError(SourceLocation{0, 0}, "internal IR generation error: unsupported binary operator");
    }
}

} // namespace

IRProgram IRGenerator::generate(const Program& program) {
    program_ = IRProgram{};
    tempCounter_ = 0;
    labelCounter_ = 0;

    for (const auto& statement : program.statements) {
        emitStatement(*statement);
    }

    return program_;
}

void IRGenerator::emitStatement(const Statement& statement) {
    if (const auto* block = dynamic_cast<const BlockStmt*>(&statement)) {
        for (const auto& item : block->statements) {
            emitStatement(*item);
        }
        return;
    }

    if (const auto* declaration = dynamic_cast<const VarDeclStmt*>(&statement)) {
        if (declaration->initializer != nullptr) {
            program_.add(IROp::Move,
                         declaration->name,
                         emitExpression(*declaration->initializer),
                         "",
                         declaration->location);
        } else {
            program_.add(IROp::LoadConst, declaration->name, "#0", "", declaration->location);
        }
        return;
    }

    if (const auto* assignment = dynamic_cast<const AssignmentStmt*>(&statement)) {
        program_.add(IROp::Move,
                     assignment->name,
                     emitExpression(*assignment->value),
                     "",
                     assignment->location);
        return;
    }

    if (const auto* printStmt = dynamic_cast<const PrintStmt*>(&statement)) {
        program_.add(IROp::Print, "", emitExpression(*printStmt->expression), "", printStmt->location);
        return;
    }

    if (const auto* ifStmt = dynamic_cast<const IfStmt*>(&statement)) {
        const std::string elseLabel = nextLabel("else");
        const std::string endLabel = nextLabel("endif");
        program_.add(IROp::JumpIfZero, elseLabel, emitExpression(*ifStmt->condition), "", ifStmt->location);
        emitStatement(*ifStmt->thenBranch);
        if (ifStmt->elseBranch != nullptr) {
            program_.add(IROp::Jump, endLabel, "", "", ifStmt->location);
        }
        program_.add(IROp::Label, elseLabel, "", "", ifStmt->location);
        if (ifStmt->elseBranch != nullptr) {
            emitStatement(*ifStmt->elseBranch);
            program_.add(IROp::Label, endLabel, "", "", ifStmt->location);
        }
        return;
    }

    if (const auto* whileStmt = dynamic_cast<const WhileStmt*>(&statement)) {
        const std::string loopLabel = nextLabel("while_begin");
        const std::string endLabel = nextLabel("while_end");
        program_.add(IROp::Label, loopLabel, "", "", whileStmt->location);
        program_.add(IROp::JumpIfZero, endLabel, emitExpression(*whileStmt->condition), "", whileStmt->location);
        emitStatement(*whileStmt->body);
        program_.add(IROp::Jump, loopLabel, "", "", whileStmt->location);
        program_.add(IROp::Label, endLabel, "", "", whileStmt->location);
        return;
    }

    throwCompileError(statement.location, "internal IR generation error: unsupported statement");
}

std::string IRGenerator::emitExpression(const Expression& expression) {
    if (const auto* integer = dynamic_cast<const IntegerLiteral*>(&expression)) {
        return "#" + std::to_string(integer->value);
    }

    if (const auto* boolean = dynamic_cast<const BooleanLiteral*>(&expression)) {
        return boolean->value ? "#1" : "#0";
    }

    if (const auto* variable = dynamic_cast<const VariableExpr*>(&expression)) {
        return variable->name;
    }

    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
        const std::string operand = emitExpression(*unary->operand);
        const std::string temp = nextTemp();
        const IROp op = unary->op == TokenType::Minus ? IROp::Neg : IROp::Not;
        program_.add(op, temp, operand, "", unary->location);
        return temp;
    }

    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
        const std::string left = emitExpression(*binary->left);
        const std::string right = emitExpression(*binary->right);
        const std::string temp = nextTemp();
        program_.add(binaryOpToIrOp(binary->op), temp, left, right, binary->location);
        return temp;
    }

    throwCompileError(expression.location, "internal IR generation error: unsupported expression");
}

std::string IRGenerator::nextTemp() {
    return "%t" + std::to_string(tempCounter_++);
}

std::string IRGenerator::nextLabel(const std::string& prefix) {
    return prefix + "_" + std::to_string(labelCounter_++);
}

} // namespace minicc
