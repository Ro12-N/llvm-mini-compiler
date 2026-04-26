#include "Token.h"

#include <sstream>

namespace minicc {

std::string tokenTypeName(const TokenType type) {
    switch (type) {
    case TokenType::Int: return "TOK_INT";
    case TokenType::If: return "TOK_IF";
    case TokenType::Else: return "TOK_ELSE";
    case TokenType::While: return "TOK_WHILE";
    case TokenType::Print: return "TOK_PRINT";
    case TokenType::True: return "TOK_TRUE";
    case TokenType::False: return "TOK_FALSE";
    case TokenType::Identifier: return "TOK_IDENTIFIER";
    case TokenType::Number: return "TOK_NUMBER";
    case TokenType::LeftParen: return "TOK_LPAREN";
    case TokenType::RightParen: return "TOK_RPAREN";
    case TokenType::LeftBrace: return "TOK_LBRACE";
    case TokenType::RightBrace: return "TOK_RBRACE";
    case TokenType::Semicolon: return "TOK_SEMICOLON";
    case TokenType::Comma: return "TOK_COMMA";
    case TokenType::Assign: return "TOK_ASSIGN";
    case TokenType::Plus: return "TOK_PLUS";
    case TokenType::Minus: return "TOK_MINUS";
    case TokenType::Star: return "TOK_STAR";
    case TokenType::Slash: return "TOK_SLASH";
    case TokenType::Bang: return "TOK_BANG";
    case TokenType::EqualEqual: return "TOK_EQEQ";
    case TokenType::BangEqual: return "TOK_NEQ";
    case TokenType::Less: return "TOK_LT";
    case TokenType::LessEqual: return "TOK_LTE";
    case TokenType::Greater: return "TOK_GT";
    case TokenType::GreaterEqual: return "TOK_GTE";
    case TokenType::EndOfFile: return "TOK_EOF";
    }

    return "TOK_UNKNOWN";
}

std::string sourceLocationToString(const SourceLocation& location) {
    std::ostringstream out;
    out << location.line << ":" << location.column;
    return out.str();
}

} // namespace minicc
