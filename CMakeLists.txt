cmake_minimum_required(VERSION 3.16)

project(mini_compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(mini_compiler
    src/AST.cpp
    src/AstPrinter.cpp
    src/CodeGenerator.cpp
    src/Compiler.cpp
    src/Error.cpp
    src/IR.cpp
    src/IRGenerator.cpp
    src/IRInterpreter.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/SemanticAnalyzer.cpp
    src/Token.cpp
    src/main.cpp
)

target_include_directories(mini_compiler PRIVATE include)
