# MiniC Language Specification

`MiniC` is a tiny, integer-focused C-like language designed for learning compiler construction.

## Supported Features

- `int` variable declarations
- integer literals
- boolean literals: `true`, `false`
- arithmetic: `+`, `-`, `*`, `/`
- comparisons: `==`, `!=`, `<`, `<=`, `>`, `>=`
- unary operators: `-`, `!`
- assignment
- block scopes with `{ ... }`
- `if / else`
- `while`
- `print(expr);`

## Deliberate Simplifications

- only `int` variables may be declared
- comparisons produce boolean values internally as `0` or `1`
- no functions
- no arrays
- no floating-point values
- no user-defined types
- no variable shadowing across active scopes

## Grammar (EBNF)

```text
program        := declaration* EOF ;

declaration    := var_decl | statement ;

var_decl       := "int" IDENT ( "=" expression )? ";" ;

statement      := block
                | if_stmt
                | while_stmt
                | print_stmt
                | assignment_stmt ;

block          := "{" declaration* "}" ;

if_stmt        := "if" "(" expression ")" statement ( "else" statement )? ;

while_stmt     := "while" "(" expression ")" statement ;

print_stmt     := "print" "(" expression ")" ";" ;

assignment_stmt:= IDENT "=" expression ";" ;

expression     := equality ;

equality       := comparison ( ( "==" | "!=" ) comparison )* ;

comparison     := term ( ( "<" | "<=" | ">" | ">=" ) term )* ;

term           := factor ( ( "+" | "-" ) factor )* ;

factor         := unary ( ( "*" | "/" ) unary )* ;

unary          := ( "-" | "!" ) unary | primary ;

primary        := NUMBER
                | "true"
                | "false"
                | IDENT
                | "(" expression ")" ;
```

## Compilation Pipeline

1. Source code is tokenized by the lexer.
2. Tokens are parsed by a recursive-descent parser.
3. The parser builds a typed AST.
4. Semantic analysis validates declarations, assignments, and conditions.
5. The backend lowers the AST into three-address IR.
6. IR is executed by a lightweight interpreter for end-to-end verification.
7. IR is also rendered as pseudo x86-like assembly.
