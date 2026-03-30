#pragma once
#include <string>
#include <queue>
#include <vector>
#include "Variable.h"
struct StatementVisitor;
struct ExpressionVisitor;
struct IfStmt;
struct SwitchStmt;
struct CaseLabel;
struct WhileStmt;
struct ForStmt;
struct JumpStmt;
struct VarDeclStmt;
struct BlockStmt;
struct ExprStmt;
struct FunctionDeclStmt;
struct VariableExpr;
struct IntLiteralExpr;
struct FloatLiteralExpr;
struct BoolLiteralExpr;
struct FunctionCallExpr;
struct IndexExpr;
struct MemberExpr;
struct PostfixExpr;
struct UnaryExpr;
struct BinaryExpr;
struct AssignExpr;
struct TernaryExpr;
struct CommaExpr;
struct CallExpr;
struct ParamInfo {
    std::string qualifier; // "in", "out", "inout", ""
    std::string type;
    std::string name;
};
struct Statement{
    virtual std::string toString() const { return "Statement"; }
    virtual void accept(StatementVisitor* visitor) = 0;
    virtual ~Statement() = default;
    
};
struct Expression {
    virtual std::string toString() const { return "Expression"; }
    virtual void accept(ExpressionVisitor* visitor) = 0;
    virtual ~Expression() = default;
};

// Definisci il visitor
struct StatementVisitor {
    virtual ~StatementVisitor() = default;
    virtual void visit(VarDeclStmt* stmt) = 0;
    virtual void visit(BlockStmt* stmt) = 0;
    virtual void visit(IfStmt* stmt) = 0;
    virtual void visit(WhileStmt* stmt) = 0;
    virtual void visit(ForStmt* stmt) = 0;
    virtual void visit(SwitchStmt* stmt) = 0;
    virtual void visit(CaseLabel* stmt) = 0;
    virtual void visit(JumpStmt* stmt) = 0;
    virtual void visit(ExprStmt* stmt) = 0;
    virtual void visit(FunctionDeclStmt* stmt) = 0;
};

struct ExpressionVisitor {
    virtual ~ExpressionVisitor() = default;
    virtual void visit(VariableExpr* expr) = 0;
    virtual void visit(IntLiteralExpr* expr) = 0;
    virtual void visit(FloatLiteralExpr* expr) = 0;
    virtual void visit(BoolLiteralExpr* expr) = 0;
    virtual void visit(BinaryExpr* expr) = 0;
    virtual void visit(AssignExpr* expr) = 0;
    virtual void visit(FunctionCallExpr* expr) = 0;
    virtual void visit(IndexExpr* expr) = 0;
    virtual void visit(MemberExpr* expr) = 0;
    virtual void visit(PostfixExpr* expr) = 0;
    virtual void visit(UnaryExpr* expr) = 0;
    virtual void visit(TernaryExpr* expr) = 0;
    virtual void visit(CommaExpr* expr) = 0;
    virtual void visit(CallExpr* expr) = 0;
};

using StatementPtr = std::shared_ptr<Statement>;
using ExpressionPtr = std::shared_ptr<Expression>;

struct FunctionInfo {
    std::string             returnType;
    std::string             name;
    std::vector<ParamInfo>  params;
    StatementPtr              body; // da costruire visitando il function body
     
};



struct VarDeclStmt : Statement {
    VariableDeclaration var;
    ExpressionPtr initializerExpr; // AST dell'inizializzatore, se presente
    virtual std::string toString() const override {
        return "VarDeclStmt: " + var.type + " " + var.name;
    }
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct FunctionDeclStmt : Statement {
    FunctionInfo fn;
    virtual std::string toString() const override {
        std::string result = "FunctionDeclStmt: " + fn.returnType + " " + fn.name + "(";
        for (size_t j = 0; j < fn.params.size(); ++j) {
            const auto& p = fn.params[j];
            if (!p.qualifier.empty()) result += p.qualifier + " ";
            result += p.type + " " + p.name;
            if (j < fn.params.size() - 1) result += ", ";
        }
        result += ")";
        result += "{\n" + fn.body->toString() + "}\n";
        return result;
    }
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct BlockStmt : Statement {
    VariableDeclaration var; // opzionale, se presente è una dichiarazione di variabile locale
    
    std::vector<StatementPtr> stmts;
    virtual std::string toString() const override {
        std::string result = "{";
        for (const auto& stmt : stmts) {
            result += "  " + stmt->toString() + "\n";
        }
        result += "}";
        return result;
    }
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct ExprStmt : Statement{
    ExpressionPtr expr; // nullptr per ";"
    virtual std::string toString() const override{
        return expr ? expr->toString() + ";" : ";";
        
    }
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct IfStmt : Statement {
    ExpressionPtr  condition;
    StatementPtr  thenBranch;
    StatementPtr  elseBranch; // nullptr se assente
    virtual std::string toString() const override {
        std::string result = "if (" + condition->toString() + ")\n" + thenBranch->toString();
        if (elseBranch) {
            result += "\nelse\n" + elseBranch->toString();
        }
        return result;
    }
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct SwitchStmt : Statement{
    ExpressionPtr              expr;
    StatementPtr              body; // blocco con case label e dichiarazioni
    virtual std::string toString() const override {
        return "switch (" + expr->toString() + ")\n" + body->toString();
    }

    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct CaseLabel : Statement {
    std::string label; // "case 1:", "default:"
    StatementPtr stmt; // il case label è seguito da una dichiarazione o da un'altra case label
    virtual std::string toString() const override {
        return label + stmt->toString();
    }
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct WhileStmt : Statement {
    ExpressionPtr condition;
    StatementPtr body;
    virtual std::string toString() const override {
        return "while (" + condition->toString() + ")\n" + body->toString();
    }
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct ForStmt : Statement {
    StatementPtr init; // dichiarazione o espressione, o nullptr se assente
    ExpressionPtr condition; // nullptr se assente
    ExpressionPtr iteration; // nullptr se assente
    StatementPtr body;
    virtual std::string toString() const override {
        std::string result = "for (";
        result += init ? init->toString() : "";
        result += ";";
        result += condition ? condition->toString() : "";
        result += ";";
        result += iteration ? iteration->toString() : "";
        result += ")\n" + body->toString();
        return result;
    }
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct JumpStmt : Statement {
    enum class Kind { Continue, Break, Return, Discard } kind;
    ExpressionPtr value; // solo per return con espressione
    virtual std::string toString() const override {
        switch (kind) {
            case Kind::Continue:
                return "continue;";
            case Kind::Break:
                return "break;";
            case Kind::Return:
                return "return " + (value ? value->toString() : "") + ";";
            case Kind::Discard:
                return "discard;";
        }
        return "";
    }
    void accept(StatementVisitor* visitor) override {
        visitor->visit(this);
    }
};


// Expression
struct VariableExpr : Expression {
    std::string name;
    virtual std::string toString() const override {
        return name;
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct IntLiteralExpr : Expression {
    long long value;
    bool      isUnsigned = false;
    virtual std::string toString() const override {
        return std::to_string(value) + (isUnsigned ? "u" : "");
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct FloatLiteralExpr : Expression {
    double value;
    bool   isDouble = false;
    virtual std::string toString() const override {
        return std::to_string(value) + (isDouble ? "lf" : "");
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct BoolLiteralExpr : Expression {
    bool value;
    virtual std::string toString() const override {
        return value ? "true" : "false";
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct FunctionCallExpr : Expression {
    std::string functionName;
    std::vector<ExpressionPtr> args;
    virtual std::string toString() const override {
        std::string result = functionName + "(";
        for (size_t i = 0; i < args.size(); ++i) {
            result += args[i]->toString();
            if (i < args.size() - 1) result += ", ";
        }
        result += ")";
        return result;
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct IndexExpr : Expression {
    ExpressionPtr object;
    ExpressionPtr index;
    virtual std::string toString() const override {
        return object->toString() + "[" + index->toString() + "]";
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct MemberExpr : Expression {
    ExpressionPtr object;
    std::string field;        // "xyz", "r", ecc.
    bool        isSwizzle = false;
    virtual std::string toString() const override {
        return object->toString() + "." + field;
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct PostfixExpr : Expression {
    ExpressionPtr operand;
    std::string op; // "++" | "--"
    virtual std::string toString() const override{
        return operand->toString() + op;
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct UnaryExpr : Expression {
    std::string op; // "++" | "--" | "+" | "-" | "!" | "~"
    ExpressionPtr operand;
    virtual std::string toString() const override{
        return operand->toString() + op;
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};
struct BinaryExpr : Expression {
    std::string op; // "+", "-", "Ptr", "/", "&&", "||", ecc.
    ExpressionPtr left;
    ExpressionPtr right;
    virtual std::string toString() const override {
        return left->toString() + " " + op + " " + right->toString();
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct AssignExpr : Expression {
    std::string op; // "=", "+=", "-=", ecc.
    ExpressionPtr left;
    ExpressionPtr right;
    virtual std::string toString() const override {
        return left->toString() + " " + op + " " + right->toString();
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct TernaryExpr : Expression {
    ExpressionPtr condition;
    ExpressionPtr trueExpr;
    ExpressionPtr falseExpr;
    virtual std::string toString() const override {
        return condition->toString() + " ? " + trueExpr->toString() + " : " + falseExpr->toString();
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct CommaExpr :  Expression { 
    ExpressionPtr left;
    ExpressionPtr right;
    virtual std::string toString() const override {
        return left->toString() + ", " + right->toString();
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};

struct CallExpr : Expression {
    // callee può essere un IdentifierExpr (funzione) o un TypeSpec (costruttore)
    // Per semplicità usiamo una stringa per i tipi built-in ("vec3(...)") e
    // un ExprPtr per le chiamate su espressioni ("obj.method(...)")
    std::string              callee;      // nome funzione / tipo costruttore
    ExpressionPtr              calleeExpr;  // usato per metodi su oggetti, nullptr altrimenti
    std::vector<ExpressionPtr>     args;
    bool                     isConstructor = false; // vec3(...), mat4(...)
    virtual std::string toString() const override {
        std::string result;
        if (calleeExpr) {
            result += calleeExpr->toString() + "." + callee;
        } else {
            result += callee;
        }
        result += "(";
        for (size_t i = 0; i < args.size(); ++i) {
            result += args[i]->toString();
            if (i < args.size() - 1) result += ", ";
        }
        result += ")";
        return result;
    }
    void accept(ExpressionVisitor* visitor) override {
        visitor->visit(this);
    }
};

