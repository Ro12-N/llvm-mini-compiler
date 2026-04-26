# Mini Compiler for a Tiny C-like Language

This project implements a small compiler pipeline in C++ for a tiny C-like language called `MiniC`. The compiler includes a hand-written lexer, a recursive-descent parser, a typed AST, scope-aware semantic analysis, three-address IR generation, an IR interpreter for validation, and pseudo x86-like code generation.

It is designed to mirror the structure of a real compiler in a compact, readable project.

## Features

- hand-written lexer
- recursive-descent parser
- typed AST with pretty-printing
- scope-aware semantic analysis
- readable compiler errors with line/column information
- three-address IR generation
- IR interpreter for end-to-end checking
- pseudo x86-like backend output

## Language Overview

`MiniC` supports:

- `int` variable declarations
- integer and boolean literals
- arithmetic expressions
- comparisons
- assignment
- `if / else`
- `while`
- `print(expr);`
- block scopes

Example:

```c
int n = 5;
int result = 1;

while (n > 1) {
    result = result * n;
    n = n - 1;
}

print(result);
```

## Folder Structure

```text
mini-compiler/
|-- CMakeLists.txt
|-- README.md
|-- build.bat
|-- docs/
|   `-- language_spec.md
|-- examples/
|   |-- branching.mini
|   |-- factorial.mini
|   |-- fibonacci.mini
|   |-- gcd.mini
|   `-- undeclared_error.mini
|-- include/
|   |-- AST.h
|   |-- AstPrinter.h
|   |-- CodeGenerator.h
|   |-- Compiler.h
|   |-- Error.h
|   |-- IR.h
|   |-- IRGenerator.h
|   |-- IRInterpreter.h
|   |-- Lexer.h
|   |-- Parser.h
|   |-- SemanticAnalyzer.h
|   `-- Token.h
|-- src/
|   |-- AST.cpp
|   |-- AstPrinter.cpp
|   |-- CodeGenerator.cpp
|   |-- Compiler.cpp
|   |-- Error.cpp
|   |-- IR.cpp
|   |-- IRGenerator.cpp
|   |-- IRInterpreter.cpp
|   |-- Lexer.cpp
|   |-- Parser.cpp
|   |-- SemanticAnalyzer.cpp
|   |-- Token.cpp
|   `-- main.cpp
`-- bin/
```

## Compiler Pipeline

### 1. Lexing

The lexer scans source text character by character and produces tokens such as:

- `TOK_INT`
- `TOK_IDENTIFIER`
- `TOK_ASSIGN`
- `TOK_NUMBER`
- `TOK_IF`
- `TOK_WHILE`

### 2. Parsing

The parser is a hand-written recursive-descent parser with separate functions for each grammar rule:

- `parseDeclaration()`
- `parseStatement()`
- `parseExpression()`
- `parseTerm()`
- `parseFactor()`
- `parseUnary()`

This keeps operator precedence explicit and easy to reason about.

### 3. AST Construction

The parser builds a typed AST with nodes such as:

- `VarDeclStmt`
- `AssignmentStmt`
- `IfStmt`
- `WhileStmt`
- `BinaryExpr`
- `UnaryExpr`
- `VariableExpr`

The AST can be pretty-printed for debugging and README screenshots.

### 4. Semantic Analysis

The semantic analyzer:

- checks for undeclared variables
- rejects redeclarations
- validates assignment types
- validates condition expressions

Errors include source locations, for example:

```text
error:2:1: undeclared variable 'y'
```

### 5. IR Generation

The AST is lowered into a small three-address IR with instructions like:

```text
%t0 = mul result, n
result = mov %t0
```

Control flow is represented using labels and jumps:

```text
while_begin_0:
jz %t0, while_end_1
jump while_begin_0
```

### 6. Backend

The project emits pseudo x86-like assembly to illustrate how high-level constructs map to lower-level operations.

This backend is meant for understanding code generation, not for direct assembly and linking.

## Build

### Option 1: `build.bat`

```bat
build.bat
```

### Option 2: `g++`

```powershell
g++ -std=c++17 -Iinclude `
  src\AST.cpp src\AstPrinter.cpp src\CodeGenerator.cpp src\Compiler.cpp `
  src\Error.cpp src\IR.cpp src\IRGenerator.cpp src\IRInterpreter.cpp `
  src\Lexer.cpp src\Parser.cpp src\SemanticAnalyzer.cpp src\Token.cpp src\main.cpp `
  -o bin\mini_compiler.exe
```

### Option 3: CMake

```powershell
cmake -S . -B build
cmake --build build
```

## Run

Compile a sample program:

```powershell
.\bin\mini_compiler.exe examples\factorial.mini
```

Try the other examples:

```powershell
.\bin\mini_compiler.exe examples\fibonacci.mini
.\bin\mini_compiler.exe examples\gcd.mini
.\bin\mini_compiler.exe examples\branching.mini
.\bin\mini_compiler.exe examples\undeclared_error.mini
```

## Example Output

For each program the compiler prints:

- tokens
- AST
- semantic analysis result
- IR
- simulated program output
- pseudo x86-like backend output

## Good Debugging Flow

If a program does not compile or behaves unexpectedly:

1. Check the token stream first.
2. Check the AST pretty-print for precedence or block-structure mistakes.
3. Check semantic analysis errors for undeclared or mismatched values.
4. Inspect IR to confirm control flow and temporary values.
5. Compare simulated output with the intended program behavior.

## Language Spec

See [docs/language_spec.md](docs/language_spec.md) for the grammar and feature list.

## Resume Bullet

Designed and implemented a mini compiler for a C-like DSL in C++, including a hand-written lexer, recursive-descent parser, typed AST, scope-aware semantic analysis, three-address IR generation, IR interpretation, and pseudo x86-style code generation.
