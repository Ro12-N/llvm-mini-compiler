#include "Lexer.h"

#include "Error.h"

#include <cctype>
#include <unordered_map>

namespace minicc {
namespace {

bool isIdentifierStart(const char ch) {
    return std::isalpha(static_cast<unsigned char>(ch)) != 0 || ch == '_';
}

bool isIdentifierPart(const char ch) {
    return std::isalnum(static_cast<unsigned char>(ch)) != 0 || ch == '_';
}

} // namespace

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skipWhitespaceAndComments();
        if (isAtEnd()) {
            tokens.push_back(makeToken(TokenType::EndOfFile, "", {line_, column_}));
            break;
        }

        const SourceLocation start{line_, column_};
        const char ch = advance();

        if (isIdentifierStart(ch)) {
            --current_;
            --column_;
            tokens.push_back(identifierOrKeyword());
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(ch)) != 0) {
            --current_;
            --column_;
            tokens.push_back(number());
            continue;
        }

        switch (ch) {
        case '(': tokens.push_back(makeToken(TokenType::LeftParen, "(", start)); break;
        case ')': tokens.push_back(makeToken(TokenType::RightParen, ")", start)); break;
        case '{': tokens.push_back(makeToken(TokenType::LeftBrace, "{", start)); break;
        case '}': tokens.push_back(makeToken(TokenType::RightBrace, "}", start)); break;
        case ';': tokens.push_back(makeToken(TokenType::Semicolon, ";", start)); break;
        case ',': tokens.push_back(makeToken(TokenType::Comma, ",", start)); break;
        case '+': tokens.push_back(makeToken(TokenType::Plus, "+", start)); break;
        case '-': tokens.push_back(makeToken(TokenType::Minus, "-", start)); break;
        case '*': tokens.push_back(makeToken(TokenType::Star, "*", start)); break;
        case '/': tokens.push_back(makeToken(TokenType::Slash, "/", start)); break;
        case '!': {
            const bool isBangEqual = match('=');
            tokens.push_back(makeToken(isBangEqual ? TokenType::BangEqual : TokenType::Bang, isBangEqual ? "!=" : "!", start));
            break;
        }
        case '=': {
            const bool isDoubleEqual = match('=');
            tokens.push_back(makeToken(isDoubleEqual ? TokenType::EqualEqual : TokenType::Assign, isDoubleEqual ? "==" : "=", start));
            break;
        }
        case '<': {
            const bool isLessEqual = match('=');
            tokens.push_back(makeToken(isLessEqual ? TokenType::LessEqual : TokenType::Less, isLessEqual ? "<=" : "<", start));
            break;
        }
        case '>': {
            const bool isGreaterEqual = match('=');
            tokens.push_back(makeToken(isGreaterEqual ? TokenType::GreaterEqual : TokenType::Greater, isGreaterEqual ? ">=" : ">", start));
            break;
        }
        default:
            throwCompileError(start, std::string("unexpected character '") + ch + "'");
        }
    }

    return tokens;
}

bool Lexer::isAtEnd() const noexcept {
    return current_ >= source_.size();
}

char Lexer::peek() const noexcept {
    if (isAtEnd()) {
        return '\0';
    }
    return source_[current_];
}

char Lexer::peekNext() const noexcept {
    if (current_ + 1 >= source_.size()) {
        return '\0';
    }
    return source_[current_ + 1];
}

char Lexer::advance() noexcept {
    if (isAtEnd()) {
        return '\0';
    }

    const char ch = source_[current_++];
    if (ch == '\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    return ch;
}

bool Lexer::match(const char expected) noexcept {
    if (isAtEnd() || source_[current_] != expected) {
        return false;
    }
    advance();
    return true;
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        const char ch = peek();
        if (std::isspace(static_cast<unsigned char>(ch)) != 0) {
            advance();
            continue;
        }

        if (ch == '/' && peekNext() == '/') {
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
            continue;
        }

        break;
    }
}

Token Lexer::identifierOrKeyword() {
    static const std::unordered_map<std::string, TokenType> keywords{
        {"int", TokenType::Int},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"while", TokenType::While},
        {"print", TokenType::Print},
        {"true", TokenType::True},
        {"false", TokenType::False}
    };

    const std::size_t startIndex = current_;
    const SourceLocation start{line_, column_};
    while (!isAtEnd() && isIdentifierPart(peek())) {
        advance();
    }

    const std::string lexeme = source_.substr(startIndex, current_ - startIndex);
    const auto found = keywords.find(lexeme);
    const TokenType type = found != keywords.end() ? found->second : TokenType::Identifier;
    return makeToken(type, lexeme, start);
}

Token Lexer::number() {
    const std::size_t startIndex = current_;
    const SourceLocation start{line_, column_};
    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek())) != 0) {
        advance();
    }

    return makeToken(TokenType::Number, source_.substr(startIndex, current_ - startIndex), start);
}

Token Lexer::makeToken(const TokenType type, const std::string& lexeme, const SourceLocation& location) const {
    return Token{type, lexeme, location};
}

} // namespace minicc
