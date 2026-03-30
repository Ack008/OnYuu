#pragma once

#include "FunctionInfo.h"
#include "SemanticScope.h"
#include "shaderInfo.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Diagnostic {
    enum class Level { Error, Warning };
    Level level;
    std::string message;
};

class SemanticVisitor : public StatementVisitor, public ExpressionVisitor {
private:
    std::unordered_map<Expression*, std::string> exprTypes_;
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> constructorSigs_;
    std::unordered_map<std::string, std::vector<std::pair<std::vector<std::string>, std::string>>> functionSigs_;
    std::unordered_set<std::string> builtinFunctionNames_;
    std::unordered_set<std::string> uniformNames_;
    std::string currentFunctionName_;
    std::string currentFunctionReturnType_;
    bool currentFunctionHasReturnValue_ = false;

    void addBuiltinFunction(const std::string& name,
                            const std::vector<std::string>& params,
                            const std::string& returnType);
    void addBuiltInStruct(const std::string& name,
                          const std::vector<std::pair<std::string, std::string>>& fields);
    void addBuiltFunctionsInSymbolTable();
    void initBuiltinFunctionSignatures();
    void initConstructorSignatures();

    static std::string trim(const std::string& value);
    static bool parseArrayType(const std::string& typeName, std::string& baseType, int& arraySize);

    const semantic::Symbol* resolveTypeSymbolFromName(const std::string& typeName);
    bool isCompatible(const std::string& actual, const std::string& expected) const;
    std::string typeOf(Expression* expression) const;
    void setType(Expression* expression, const std::string& typeName);

    bool checkConstructorArgs(const std::string& ctorName, const std::vector<std::string>& argTypes) const;
    bool checkFunctionArgs(const std::string& functionName,
                           const std::vector<std::string>& argTypes,
                           std::string* inferredReturnType = nullptr) const;
    static std::string joinTypes(const std::vector<std::string>& types);
    bool isReservedFunctionName(const std::string& name) const;
    bool isUniformLValue(const Expression* expression) const;

public:
    semantic::ScopeStack scopes;
    std::vector<Diagnostic> diagnostics;

    SemanticVisitor();

    std::unordered_map<std::string, std::vector<std::vector<std::string>>> getConstructorSignatures() const;

    void error(const std::string& msg);
    void warning(const std::string& msg);
    void analyze(const ShaderInfo& shader);
    void printDiagnostics() const;
    bool hasErrors() const;

    bool isFunctionDeclared(const std::string& name) const;
    std::string getFunctionReturnType(const std::string& name,
                                      const std::vector<std::string>& argTypes) const;
    std::vector<std::string> getAllFunctionNames() const;
    std::unordered_map<std::string, std::vector<std::pair<std::vector<std::string>, std::string>>> getFunctionSignatures() const;
    bool isBuiltinFunction(const std::string& name) const;

private:
    void analyzeFunction(const FunctionInfo& fn);

    void visit(BlockStmt* s) override;
    bool isValidType(const std::string& typeName) const;
    void visit(VarDeclStmt* s) override;
    void visit(ExprStmt* s) override;
    void visit(IfStmt* s) override;
    void visit(WhileStmt* s) override;
    void visit(ForStmt* s) override;
    void visit(SwitchStmt* s) override;
    void visit(CaseLabel* s) override;
    void visit(JumpStmt* s) override;
    void visit(FunctionDeclStmt* s) override;

    void visit(VariableExpr* e) override;
    void visit(AssignExpr* e) override;
    void visit(BinaryExpr* e) override;
    void visit(UnaryExpr* e) override;
    void visit(PostfixExpr* e) override;
    void visit(MemberExpr* e) override;
    void visit(IndexExpr* e) override;
    void visit(TernaryExpr* e) override;
    void visit(CommaExpr* e) override;
    void visit(CallExpr* e) override;
    void visit(FunctionCallExpr* e) override;
    void visit(IntLiteralExpr* e) override;
    void visit(FloatLiteralExpr* e) override;
    void visit(BoolLiteralExpr* e) override;
};
