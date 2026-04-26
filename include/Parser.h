#pragma once

#include "AST.h"
#include "Token.h"

#include <memory>
#include <vector>

namespace minicc {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    std::unique_ptr<Program> parseProgram();

private:
    bool isAtEnd() const noexcept;
    const Token& peek() const noexcept;
    const Token& previous() const noexcept;
    const Token& advance() noexcept;
    bool check(TokenType type) const noexcept;
    bool checkNext(TokenType type) const noexcept;
    bool match(TokenType type) noexcept;
    const Token& consume(TokenType type, const std::string& message);

    std::unique_ptr<Statement> parseDeclaration();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseBlockStatement(const SourceLocation& location);
    std::unique_ptr<Statement> parseVarDeclaration(const SourceLocation& location);
    std::unique_ptr<Statement> parseAssignmentStatement();
    std::unique_ptr<Statement> parsePrintStatement(const SourceLocation& location);
    std::unique_ptr<Statement> parseIfStatement(const SourceLocation& location);
    std::unique_ptr<Statement> parseWhileStatement(const SourceLocation& location);

    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseComparison();
    std::unique_ptr<Expression> parseTerm();
    std::unique_ptr<Expression> parseFactor();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parsePrimary();

    std::vector<Token> tokens_;
    std::size_t current_ = 0;
};

} // namespace minicc
