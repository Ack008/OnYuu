#pragma once

#include "FunctionInfo.h"
#include "SemanticScope.h"
#include "shaderInfo.h"

#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class SemanticVisitor;

class VulkanVisitor : public StatementVisitor, public ExpressionVisitor {
private:
    std::string output = "";
    std::string vertexShaderCode_ = "";
    std::string fragmentShaderCode_ = "";
    ShaderInfo info;
    std::unordered_map<std::string, UniformVar> uniformNames;
    std::unordered_map<std::string, bool> visited;
    std::unordered_map<std::string, const FunctionInfo*> fragmentFunctionTable;
    std::unordered_map<std::string, int> fragmentVisitState;
    std::vector<const FunctionInfo*> orderedFragmentFunctions;

    std::unordered_map<std::string, const FunctionInfo*> vertexFunctionTable;
    std::unordered_map<std::string, int> vertexVisitState;
    std::vector<const FunctionInfo*> orderedVertexFunctions;
    std::string tabulation = "";
    std::unordered_map<std::string, std::string> fragmentTypeMapping;
    std::unordered_map<std::string, std::string> randomizedNames;
    std::unordered_set<std::string> usedRandomizedNames;
    std::mt19937 randomGenerator{ std::random_device{}() };
    const SemanticVisitor* semanticAnalyzer = nullptr;

    std::string getTransformedType(const std::string& type);
    std::string getTransformedFunctionName(const std::string& name);
    std::string getOrCreateRandomName(const std::string& variableName);
    bool isUserFunction(const std::string& functionName) const;
    bool isUniform(const std::string& name);
    bool isSampler(const std::string& name);

    void collectCalledFunctionsFromExpr(const ExpressionPtr& expr, std::unordered_set<std::string>& calledFunctions);
    void collectCalledFunctionsFromStmt(const StatementPtr& stmt, std::unordered_set<std::string>& calledFunctions);
    void collectReachableFunctionsDfs(
        const std::string& functionName,
        std::unordered_map<std::string, const FunctionInfo*> functionTable,
        std::unordered_map<std::string, int>& visitState,
        std::vector<const FunctionInfo*>& orderedFunctions);

    std::string formatFunctionSignature(const FunctionInfo& fn);
    void emitFunctionPrototype(const FunctionInfo& fn);
    void emitFunctionDefinition(const FunctionInfo& fn);

public:
    VulkanVisitor(const SemanticVisitor* analyzer = nullptr);
	std::unordered_set<std::string> getUsedRandomizedNames() const { return usedRandomizedNames; }
	std::unordered_map<std::string, std::string> getRandomizedNames() const { return randomizedNames; }
    void visit(VarDeclStmt* stmt) override;
    void visit(BlockStmt* stmt) override;
    void visit(IfStmt* stmt) override;
    void visit(WhileStmt* stmt) override;
    void visit(ForStmt* stmt) override;
    void visit(SwitchStmt* stmt) override;
    void visit(CaseLabel* stmt) override;
    void visit(JumpStmt* stmt) override;
    void visit(ExprStmt* stmt) override;
    void visit(FunctionDeclStmt* stmt) override;

    void visit(VariableExpr* expr) override;
    void visit(IntLiteralExpr* expr) override;
    void visit(FloatLiteralExpr* expr) override;
    void visit(BoolLiteralExpr* expr) override;
    void visit(BinaryExpr* expr) override;
    void visit(AssignExpr* expr) override;
    void visit(FunctionCallExpr* expr) override;
    void visit(IndexExpr* expr) override;
    void visit(MemberExpr* expr) override;
    void visit(PostfixExpr* expr) override;
    void visit(UnaryExpr* expr) override;
    void visit(TernaryExpr* expr) override;
    void visit(CommaExpr* expr) override;
    void visit(CallExpr* expr) override;

    void produceShaders(const ShaderInfo& shader);
    void produceVertexInputInfo(const ShaderInfo& shader);
    void produceFragmentInputInfo(const ShaderInfo& shader);
    void produceFragmentShader(const ShaderInfo& shader);
    void produceStandardVertexShader();
    void produceVertexShader(const ShaderInfo& shader);
    void injectVertexVaryingInitialization();

    void printFragmentShader() const;
    void printVertexShader() const;
    std::string getFragmentShaderCode() const { return fragmentShaderCode_; }
    std::string getVertexShaderCode() const { return vertexShaderCode_; }
};
