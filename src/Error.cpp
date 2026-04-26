#include "Error.h"

#include <sstream>

namespace minicc {

CompileError::CompileError(const SourceLocation& location, const std::string& message)
    : std::runtime_error("error:" + sourceLocationToString(location) + ": " + message),
      location_(location) {}

const SourceLocation& CompileError::location() const noexcept {
    return location_;
}

[[noreturn]] void throwCompileError(const SourceLocation& location, const std::string& message) {
    throw CompileError(location, message);
}

} // namespace minicc
