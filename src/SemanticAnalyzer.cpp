#include "SemanticAnalyzer.h"

#include "Error.h"

namespace minicc {
namespace {

bool isConditionType(const ValueType type) {
    return type == ValueType::Int || type == ValueType::Bool;
}

} // namespace

void SemanticAnalyzer::analyze(Program& program) {
    scopes_.clear();
    beginScope();

    for (const auto& statement : program.statements) {
        analyzeStatement(*statement);
    }

    endScope();
}

void SemanticAnalyzer::beginScope() {
    scopes_.push_back({});
}

void SemanticAnalyzer::endScope() {
    scopes_.pop_back();
}

void SemanticAnalyzer::analyzeStatement(Statement& statement) {
    if (auto* block = dynamic_cast<BlockStmt*>(&statement)) {
        beginScope();
        for (const auto& item : block->statements) {
            analyzeStatement(*item);
        }
        endScope();
        return;
    }

    if (auto* declaration = dynamic_cast<VarDeclStmt*>(&statement)) {
        if (isDeclaredInAnyActiveScope(declaration->name)) {
            throwCompileError(declaration->location, "redeclared variable '" + declaration->name + "'");
        }

        if (declaration->initializer != nullptr) {
            const ValueType initType = analyzeExpression(*declaration->initializer);
            if (!isAssignable(declaration->declaredType, initType)) {
                throwCompileError(declaration->initializer->location,
                                  "cannot initialize variable '" + declaration->name +
                                      "' of type " + valueTypeName(declaration->declaredType) +
                                      " with " + valueTypeName(initType));
            }
        }

        scopes_.back().insert({declaration->name, Symbol{declaration->declaredType, declaration->location}});
        return;
    }

    if (auto* assignment = dynamic_cast<AssignmentStmt*>(&statement)) {
        const Symbol& symbol = lookup(assignment->name, assignment->location);
        const ValueType valueType = analyzeExpression(*assignment->value);
        if (!isAssignable(symbol.type, valueType)) {
            throwCompileError(assignment->value->location,
                              "cannot assign " + valueTypeName(valueType) +
                                  " to variable '" + assignment->name +
                                  "' of type " + valueTypeName(symbol.type));
        }
        return;
    }

    if (auto* printStmt = dynamic_cast<PrintStmt*>(&statement)) {
        const ValueType exprType = analyzeExpression(*printStmt->expression);
        if (exprType != ValueType::Int && exprType != ValueType::Bool) {
            throwCompileError(printStmt->expression->location, "print() expects an int or bool expression");
        }
        return;
    }

    if (auto* ifStmt = dynamic_cast<IfStmt*>(&statement)) {
        const ValueType conditionType = analyzeExpression(*ifStmt->condition);
        if (!isConditionType(conditionType)) {
            throwCompileError(ifStmt->condition->location, "if condition must be int or bool");
        }
        analyzeStatement(*ifStmt->thenBranch);
        if (ifStmt->elseBranch != nullptr) {
            analyzeStatement(*ifStmt->elseBranch);
        }
        return;
    }

    if (auto* whileStmt = dynamic_cast<WhileStmt*>(&statement)) {
        const ValueType conditionType = analyzeExpression(*whileStmt->condition);
        if (!isConditionType(conditionType)) {
            throwCompileError(whileStmt->condition->location, "while condition must be int or bool");
        }
        analyzeStatement(*whileStmt->body);
    }
}

ValueType SemanticAnalyzer::analyzeExpression(Expression& expression) {
    if (auto* integer = dynamic_cast<IntegerLiteral*>(&expression)) {
        integer->resolvedType = ValueType::Int;
        return integer->resolvedType;
    }

    if (auto* boolean = dynamic_cast<BooleanLiteral*>(&expression)) {
        boolean->resolvedType = ValueType::Bool;
        return boolean->resolvedType;
    }

    if (auto* variable = dynamic_cast<VariableExpr*>(&expression)) {
        const Symbol& symbol = lookup(variable->name, variable->location);
        variable->resolvedType = symbol.type;
        return variable->resolvedType;
    }

    if (auto* unary = dynamic_cast<UnaryExpr*>(&expression)) {
        const ValueType operandType = analyzeExpression(*unary->operand);
        if (unary->op == TokenType::Minus) {
            if (operandType != ValueType::Int) {
                throwCompileError(unary->location, "unary '-' expects an int operand");
            }
            unary->resolvedType = ValueType::Int;
            return unary->resolvedType;
        }

        if (!isConditionType(operandType)) {
            throwCompileError(unary->location, "unary '!' expects an int or bool operand");
        }
        unary->resolvedType = ValueType::Bool;
        return unary->resolvedType;
    }

    if (auto* binary = dynamic_cast<BinaryExpr*>(&expression)) {
        const ValueType leftType = analyzeExpression(*binary->left);
        const ValueType rightType = analyzeExpression(*binary->right);

        switch (binary->op) {
        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Star:
        case TokenType::Slash:
            if (leftType != ValueType::Int || rightType != ValueType::Int) {
                throwCompileError(binary->location, "arithmetic operators require int operands");
            }
            binary->resolvedType = ValueType::Int;
            return binary->resolvedType;

        case TokenType::Less:
        case TokenType::LessEqual:
        case TokenType::Greater:
        case TokenType::GreaterEqual:
            if (leftType != ValueType::Int || rightType != ValueType::Int) {
                throwCompileError(binary->location, "comparison operators require int operands");
            }
            binary->resolvedType = ValueType::Bool;
            return binary->resolvedType;

        case TokenType::EqualEqual:
        case TokenType::BangEqual:
            if (leftType != rightType) {
                throwCompileError(binary->location, "equality operands must have matching types");
            }
            binary->resolvedType = ValueType::Bool;
            return binary->resolvedType;

        default:
            break;
        }
    }

    throwCompileError(expression.location, "internal semantic analyzer error: unknown expression kind");
}

bool SemanticAnalyzer::isDeclaredInAnyActiveScope(const std::string& name) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

const SemanticAnalyzer::Symbol& SemanticAnalyzer::lookup(const std::string& name, const SourceLocation& usageLocation) const {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        const auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }

    throwCompileError(usageLocation, "undeclared variable '" + name + "'");
}

bool SemanticAnalyzer::isAssignable(const ValueType target, const ValueType value) const {
    return target == value;
}

} // namespace minicc
