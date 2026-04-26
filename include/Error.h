#pragma once

#include "Token.h"

#include <stdexcept>
#include <string>

namespace minicc {

class CompileError : public std::runtime_error {
public:
    CompileError(const SourceLocation& location, const std::string& message);

    const SourceLocation& location() const noexcept;

private:
    SourceLocation location_;
};

[[noreturn]] void throwCompileError(const SourceLocation& location, const std::string& message);

} // namespace minicc
