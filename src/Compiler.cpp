#include "Compiler.h"

#include "AstPrinter.h"
#include "CodeGenerator.h"
#include "IRGenerator.h"
#include "IRInterpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"

#include <iomanip>
#include <sstream>

namespace minicc {

CompilationResult Compiler::compile(const std::string& source) const {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    std::unique_ptr<Program> ast = parser.parseProgram();

    SemanticAnalyzer analyzer;
    analyzer.analyze(*ast);

    IRGenerator irGenerator;
    IRProgram ir = irGenerator.generate(*ast);

    IRInterpreter interpreter;
    std::vector<int> output = interpreter.execute(ir);

    CodeGenerator codeGenerator;
    std::string assembly = codeGenerator.generate(ir);

    CompilationResult result;
    result.tokens = std::move(tokens);
    result.ast = std::move(ast);
    result.ir = std::move(ir);
    result.executionOutput = std::move(output);
    result.assembly = std::move(assembly);
    return result;
}

std::string formatTokens(const std::vector<Token>& tokens) {
    std::ostringstream out;
    for (const auto& token : tokens) {
        out << std::setw(8) << sourceLocationToString(token.location)
            << "  " << std::setw(16) << tokenTypeName(token.type)
            << "  '" << token.lexeme << "'\n";
    }
    return out.str();
}

} // namespace minicc
