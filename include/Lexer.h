#pragma once

#include "Token.h"

#include <string>
#include <vector>

namespace minicc {

class Lexer {
public:
    explicit Lexer(std::string source);

    std::vector<Token> tokenize();

private:
    bool isAtEnd() const noexcept;
    char peek() const noexcept;
    char peekNext() const noexcept;
    char advance() noexcept;
    bool match(char expected) noexcept;
    void skipWhitespaceAndComments();
    Token identifierOrKeyword();
    Token number();
    Token makeToken(TokenType type, const std::string& lexeme, const SourceLocation& location) const;

    std::string source_;
    std::size_t current_ = 0;
    int line_ = 1;
    int column_ = 1;
};

} // namespace minicc
