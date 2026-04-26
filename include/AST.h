#pragma once

#include "Token.h"

#include <memory>
#include <string>
#include <vector>

namespace minicc {

enum class ValueType {
    Int,
    Bool,
    Void,
    Unknown
};

std::string valueTypeName(ValueType type);

struct AstNode {
    explicit AstNode(const SourceLocation& location) : location(location) {}
    virtual ~AstNode() = default;

    SourceLocation location;
};

struct Expression : AstNode {
    explicit Expression(const SourceLocation& location) : AstNode(location) {}
    ~Expression() override = default;

    ValueType resolvedType = ValueType::Unknown;
};

struct Statement : AstNode {
    explicit Statement(const SourceLocation& location) : AstNode(location) {}
    ~Statement() override = default;
};

struct IntegerLiteral final : Expression {
    IntegerLiteral(int value, const SourceLocation& location)
        : Expression(location), value(value) {}

    int value;
};

struct BooleanLiteral final : Expression {
    BooleanLiteral(bool value, const SourceLocation& location)
        : Expression(location), value(value) {}

    bool value;
};

struct VariableExpr final : Expression {
    VariableExpr(std::string name, const SourceLocation& location)
        : Expression(location), name(std::move(name)) {}

    std::string name;
};

struct UnaryExpr final : Expression {
    UnaryExpr(TokenType op, std::unique_ptr<Expression> operand, const SourceLocation& location)
        : Expression(location), op(op), operand(std::move(operand)) {}

    TokenType op;
    std::unique_ptr<Expression> operand;
};

struct BinaryExpr final : Expression {
    BinaryExpr(TokenType op,
               std::unique_ptr<Expression> left,
               std::unique_ptr<Expression> right,
               const SourceLocation& location)
        : Expression(location),
          op(op),
          left(std::move(left)),
          right(std::move(right)) {}

    TokenType op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

struct BlockStmt final : Statement {
    BlockStmt(std::vector<std::unique_ptr<Statement>> statements, const SourceLocation& location)
        : Statement(location), statements(std::move(statements)) {}

    std::vector<std::unique_ptr<Statement>> statements;
};

struct VarDeclStmt final : Statement {
    VarDeclStmt(std::string name,
                ValueType declaredType,
                std::unique_ptr<Expression> initializer,
                const SourceLocation& location)
        : Statement(location),
          name(std::move(name)),
          declaredType(declaredType),
          initializer(std::move(initializer)) {}

    std::string name;
    ValueType declaredType;
    std::unique_ptr<Expression> initializer;
};

struct AssignmentStmt final : Statement {
    AssignmentStmt(std::string name,
                   std::unique_ptr<Expression> value,
                   const SourceLocation& location)
        : Statement(location),
          name(std::move(name)),
          value(std::move(value)) {}

    std::string name;
    std::unique_ptr<Expression> value;
};

struct PrintStmt final : Statement {
    PrintStmt(std::unique_ptr<Expression> expression, const SourceLocation& location)
        : Statement(location), expression(std::move(expression)) {}

    std::unique_ptr<Expression> expression;
};

struct IfStmt final : Statement {
    IfStmt(std::unique_ptr<Expression> condition,
           std::unique_ptr<Statement> thenBranch,
           std::unique_ptr<Statement> elseBranch,
           const SourceLocation& location)
        : Statement(location),
          condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;
};

struct WhileStmt final : Statement {
    WhileStmt(std::unique_ptr<Expression> condition,
              std::unique_ptr<Statement> body,
              const SourceLocation& location)
        : Statement(location),
          condition(std::move(condition)),
          body(std::move(body)) {}

    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
};

struct Program final : AstNode {
    Program(std::vector<std::unique_ptr<Statement>> statements, const SourceLocation& location)
        : AstNode(location), statements(std::move(statements)) {}

    std::vector<std::unique_ptr<Statement>> statements;
};

} // namespace minicc
