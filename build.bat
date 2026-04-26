@echo off
setlocal

if not exist bin mkdir bin

g++ -std=c++17 -Iinclude ^
  src\AST.cpp ^
  src\AstPrinter.cpp ^
  src\CodeGenerator.cpp ^
  src\Compiler.cpp ^
  src\Error.cpp ^
  src\IR.cpp ^
  src\IRGenerator.cpp ^
  src\IRInterpreter.cpp ^
  src\Lexer.cpp ^
  src\Parser.cpp ^
  src\SemanticAnalyzer.cpp ^
  src\Token.cpp ^
  src\main.cpp ^
  -o bin\mini_compiler.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful.
    echo.
    echo Running sample program: examples\factorial.mini
    echo.
    bin\mini_compiler.exe examples\factorial.mini
) else (
    echo Build failed.
)

endlocal
