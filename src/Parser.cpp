#include "Parser.h"

#include "Error.h"

namespace minicc {

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

std::unique_ptr<Program> Parser::parseProgram() {
    std::vector<std::unique_ptr<Statement>> statements;
    while (!isAtEnd()) {
        statements.push_back(parseDeclaration());
    }

    return std::make_unique<Program>(std::move(statements), SourceLocation{1, 1});
}

bool Parser::isAtEnd() const noexcept {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::peek() const noexcept {
    return tokens_[current_];
}

const Token& Parser::previous() const noexcept {
    return tokens_[current_ - 1];
}

const Token& Parser::advance() noexcept {
    if (!isAtEnd()) {
        ++current_;
    }
    return previous();
}

bool Parser::check(const TokenType type) const noexcept {
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

bool Parser::checkNext(const TokenType type) const noexcept {
    if (current_ + 1 >= tokens_.size()) {
        return false;
    }
    return tokens_[current_ + 1].type == type;
}

bool Parser::match(const TokenType type) noexcept {
    if (!check(type)) {
        return false;
    }
    advance();
    return true;
}

const Token& Parser::consume(const TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throwCompileError(peek().location, message);
}

std::unique_ptr<Statement> Parser::parseDeclaration() {
    if (match(TokenType::Int)) {
        return parseVarDeclaration(previous().location);
    }
    return parseStatement();
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (match(TokenType::LeftBrace)) {
        return parseBlockStatement(previous().location);
    }
    if (match(TokenType::If)) {
        return parseIfStatement(previous().location);
    }
    if (match(TokenType::While)) {
        return parseWhileStatement(previous().location);
    }
    if (match(TokenType::Print)) {
        return parsePrintStatement(previous().location);
    }
    if (check(TokenType::Identifier) && checkNext(TokenType::Assign)) {
        return parseAssignmentStatement();
    }

    throwCompileError(peek().location, "expected a statement or declaration");
}

std::unique_ptr<Statement> Parser::parseBlockStatement(const SourceLocation& location) {
    std::vector<std::unique_ptr<Statement>> statements;
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        statements.push_back(parseDeclaration());
    }
    consume(TokenType::RightBrace, "expected '}' after block");
    return std::make_unique<BlockStmt>(std::move(statements), location);
}

std::unique_ptr<Statement> Parser::parseVarDeclaration(const SourceLocation& location) {
    const Token& name = consume(TokenType::Identifier, "expected variable name after 'int'");

    std::unique_ptr<Expression> initializer;
    if (match(TokenType::Assign)) {
        initializer = parseExpression();
    }

    consume(TokenType::Semicolon, "expected ';' after variable declaration");
    return std::make_unique<VarDeclStmt>(name.lexeme, ValueType::Int, std::move(initializer), location);
}

std::unique_ptr<Statement> Parser::parseAssignmentStatement() {
    const Token name = consume(TokenType::Identifier, "expected variable name");
    consume(TokenType::Assign, "expected '=' after variable name");
    auto value = parseExpression();
    consume(TokenType::Semicolon, "expected ';' after assignment");
    return std::make_unique<AssignmentStmt>(name.lexeme, std::move(value), name.location);
}

std::unique_ptr<Statement> Parser::parsePrintStatement(const SourceLocation& location) {
    consume(TokenType::LeftParen, "expected '(' after 'print'");
    auto expression = parseExpression();
    consume(TokenType::RightParen, "expected ')' after print expression");
    consume(TokenType::Semicolon, "expected ';' after print statement");
    return std::make_unique<PrintStmt>(std::move(expression), location);
}

std::unique_ptr<Statement> Parser::parseIfStatement(const SourceLocation& location) {
    consume(TokenType::LeftParen, "expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "expected ')' after if condition");
    auto thenBranch = parseStatement();

    std::unique_ptr<Statement> elseBranch;
    if (match(TokenType::Else)) {
        elseBranch = parseStatement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch), location);
}

std::unique_ptr<Statement> Parser::parseWhileStatement(const SourceLocation& location) {
    consume(TokenType::LeftParen, "expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "expected ')' after while condition");
    auto body = parseStatement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body), location);
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseEquality();
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto expression = parseComparison();

    while (match(TokenType::EqualEqual) || match(TokenType::BangEqual)) {
        const TokenType op = previous().type;
        const SourceLocation location = previous().location;
        auto right = parseComparison();
        expression = std::make_unique<BinaryExpr>(op, std::move(expression), std::move(right), location);
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parseComparison() {
    auto expression = parseTerm();

    while (match(TokenType::Less) || match(TokenType::LessEqual) ||
           match(TokenType::Greater) || match(TokenType::GreaterEqual)) {
        const TokenType op = previous().type;
        const SourceLocation location = previous().location;
        auto right = parseTerm();
        expression = std::make_unique<BinaryExpr>(op, std::move(expression), std::move(right), location);
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto expression = parseFactor();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        const TokenType op = previous().type;
        const SourceLocation location = previous().location;
        auto right = parseFactor();
        expression = std::make_unique<BinaryExpr>(op, std::move(expression), std::move(right), location);
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    auto expression = parseUnary();

    while (match(TokenType::Star) || match(TokenType::Slash)) {
        const TokenType op = previous().type;
        const SourceLocation location = previous().location;
        auto right = parseUnary();
        expression = std::make_unique<BinaryExpr>(op, std::move(expression), std::move(right), location);
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parseUnary() {
    if (match(TokenType::Minus) || match(TokenType::Bang)) {
        const TokenType op = previous().type;
        const SourceLocation location = previous().location;
        return std::make_unique<UnaryExpr>(op, parseUnary(), location);
    }

    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (match(TokenType::Number)) {
        return std::make_unique<IntegerLiteral>(std::stoi(previous().lexeme), previous().location);
    }

    if (match(TokenType::True)) {
        return std::make_unique<BooleanLiteral>(true, previous().location);
    }

    if (match(TokenType::False)) {
        return std::make_unique<BooleanLiteral>(false, previous().location);
    }

    if (match(TokenType::Identifier)) {
        return std::make_unique<VariableExpr>(previous().lexeme, previous().location);
    }

    if (match(TokenType::LeftParen)) {
        auto expression = parseExpression();
        consume(TokenType::RightParen, "expected ')' after expression");
        return expression;
    }

    throwCompileError(peek().location, "expected expression");
}

} // namespace minicc
