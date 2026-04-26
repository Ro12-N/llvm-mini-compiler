#pragma once

#include "AST.h"

#include <unordered_map>
#include <vector>

namespace minicc {

class SemanticAnalyzer {
public:
    void analyze(Program& program);

private:
    struct Symbol {
        ValueType type = ValueType::Unknown;
        SourceLocation location;
    };

    void beginScope();
    void endScope();
    void analyzeStatement(Statement& statement);
    ValueType analyzeExpression(Expression& expression);
    bool isDeclaredInAnyActiveScope(const std::string& name) const;
    const Symbol& lookup(const std::string& name, const SourceLocation& usageLocation) const;
    bool isAssignable(ValueType target, ValueType value) const;

    std::vector<std::unordered_map<std::string, Symbol>> scopes_;
};

} // namespace minicc
