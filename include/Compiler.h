#pragma once

#include "AST.h"
#include "IR.h"
#include "Token.h"

#include <memory>
#include <string>
#include <vector>

namespace minicc {

struct CompilationResult {
    std::vector<Token> tokens;
    std::unique_ptr<Program> ast;
    IRProgram ir;
    std::vector<int> executionOutput;
    std::string assembly;
};

class Compiler {
public:
    CompilationResult compile(const std::string& source) const;
};

std::string formatTokens(const std::vector<Token>& tokens);

} // namespace minicc
