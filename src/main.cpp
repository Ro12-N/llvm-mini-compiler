#include "AstPrinter.h"
#include "Compiler.h"
#include "Error.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string readFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("unable to open source file: " + path);
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void printUsage(const char* programName) {
    std::cout
        << "Usage: " << programName << " <source-file>\n"
        << "Example: " << programName << " examples\\factorial.mini\n";
}

} // namespace

int main(const int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    try {
        const std::string path = argv[1];
        const std::string source = readFile(path);

        minicc::Compiler compiler;
        minicc::CompilationResult result = compiler.compile(source);
        minicc::AstPrinter printer;

        std::cout << "=== Mini Compiler ===\n";
        std::cout << "Source file: " << path << "\n\n";

        std::cout << "--- Tokens ---\n";
        std::cout << minicc::formatTokens(result.tokens) << "\n";

        std::cout << "--- AST ---\n";
        std::cout << printer.print(*result.ast) << "\n";

        std::cout << "--- Semantic Analysis ---\n";
        std::cout << "OK\n\n";

        std::cout << "--- IR ---\n";
        std::cout << result.ir.toString() << "\n";

        std::cout << "--- Simulated Program Output ---\n";
        if (result.executionOutput.empty()) {
            std::cout << "(no output)\n";
        } else {
            for (const int value : result.executionOutput) {
                std::cout << value << "\n";
            }
        }
        std::cout << "\n";

        std::cout << "--- Pseudo x86 ---\n";
        std::cout << result.assembly;
    } catch (const minicc::CompileError& error) {
        std::cerr << error.what() << "\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "fatal: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
