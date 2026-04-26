#include "AST.h"

namespace minicc {

std::string valueTypeName(const ValueType type) {
    switch (type) {
    case ValueType::Int: return "int";
    case ValueType::Bool: return "bool";
    case ValueType::Void: return "void";
    case ValueType::Unknown: return "unknown";
    }

    return "unknown";
}

} // namespace minicc
