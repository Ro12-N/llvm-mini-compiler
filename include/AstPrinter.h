#pragma once

#include "AST.h"

#include <string>

namespace minicc {

class AstPrinter {
public:
    std::string print(const Program& program) const;

private:
    void printStatement(const Statement& statement, int indent, std::string& out) const;
    void printExpression(const Expression& expression, int indent, std::string& out) const;
    void appendIndent(int indent, std::string& out) const;
    void appendLine(const std::string& text, int indent, std::string& out) const;
};

} // namespace minicc
