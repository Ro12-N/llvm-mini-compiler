#pragma once

#include <string>

namespace minicc {

struct SourceLocation {
    int line = 1;
    int column = 1;
};

enum class TokenType {
    Int,
    If,
    Else,
    While,
    Print,
    True,
    False,
    Identifier,
    Number,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Semicolon,
    Comma,
    Assign,
    Plus,
    Minus,
    Star,
    Slash,
    Bang,
    EqualEqual,
    BangEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string lexeme;
    SourceLocation location;
};

std::string tokenTypeName(TokenType type);
std::string sourceLocationToString(const SourceLocation& location);

} // namespace minicc
