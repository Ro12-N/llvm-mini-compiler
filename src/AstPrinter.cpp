#include "AstPrinter.h"

#include <sstream>

namespace minicc {

std::string AstPrinter::print(const Program& program) const {
    std::string out;
    appendLine("Program", 0, out);
    for (const auto& statement : program.statements) {
        printStatement(*statement, 2, out);
    }
    return out;
}

void AstPrinter::printStatement(const Statement& statement, const int indent, std::string& out) const {
    if (const auto* block = dynamic_cast<const BlockStmt*>(&statement)) {
        appendLine("Block", indent, out);
        for (const auto& item : block->statements) {
            printStatement(*item, indent + 2, out);
        }
        return;
    }

    if (const auto* decl = dynamic_cast<const VarDeclStmt*>(&statement)) {
        appendLine("VarDecl(" + valueTypeName(decl->declaredType) + " " + decl->name + ")", indent, out);
        if (decl->initializer != nullptr) {
            appendLine("Initializer", indent + 2, out);
            printExpression(*decl->initializer, indent + 4, out);
        }
        return;
    }

    if (const auto* assignment = dynamic_cast<const AssignmentStmt*>(&statement)) {
        appendLine("Assignment(" + assignment->name + ")", indent, out);
        printExpression(*assignment->value, indent + 2, out);
        return;
    }

    if (const auto* printStmt = dynamic_cast<const PrintStmt*>(&statement)) {
        appendLine("Print", indent, out);
        printExpression(*printStmt->expression, indent + 2, out);
        return;
    }

    if (const auto* ifStmt = dynamic_cast<const IfStmt*>(&statement)) {
        appendLine("If", indent, out);
        appendLine("Condition", indent + 2, out);
        printExpression(*ifStmt->condition, indent + 4, out);
        appendLine("Then", indent + 2, out);
        printStatement(*ifStmt->thenBranch, indent + 4, out);
        if (ifStmt->elseBranch != nullptr) {
            appendLine("Else", indent + 2, out);
            printStatement(*ifStmt->elseBranch, indent + 4, out);
        }
        return;
    }

    if (const auto* whileStmt = dynamic_cast<const WhileStmt*>(&statement)) {
        appendLine("While", indent, out);
        appendLine("Condition", indent + 2, out);
        printExpression(*whileStmt->condition, indent + 4, out);
        appendLine("Body", indent + 2, out);
        printStatement(*whileStmt->body, indent + 4, out);
    }
}

void AstPrinter::printExpression(const Expression& expression, const int indent, std::string& out) const {
    if (const auto* integer = dynamic_cast<const IntegerLiteral*>(&expression)) {
        appendLine("Int(" + std::to_string(integer->value) + ")", indent, out);
        return;
    }

    if (const auto* boolean = dynamic_cast<const BooleanLiteral*>(&expression)) {
        appendLine(std::string("Bool(") + (boolean->value ? "true" : "false") + ")", indent, out);
        return;
    }

    if (const auto* variable = dynamic_cast<const VariableExpr*>(&expression)) {
        appendLine("Var(" + variable->name + ")", indent, out);
        return;
    }

    if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expression)) {
        appendLine("Unary(" + tokenTypeName(unary->op) + ")", indent, out);
        printExpression(*unary->operand, indent + 2, out);
        return;
    }

    if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expression)) {
        appendLine("Binary(" + tokenTypeName(binary->op) + ")", indent, out);
        printExpression(*binary->left, indent + 2, out);
        printExpression(*binary->right, indent + 2, out);
    }
}

void AstPrinter::appendIndent(const int indent, std::string& out) const {
    out.append(static_cast<std::size_t>(indent), ' ');
}

void AstPrinter::appendLine(const std::string& text, const int indent, std::string& out) const {
    appendIndent(indent, out);
    out += text;
    out += '\n';
}

} // namespace minicc
