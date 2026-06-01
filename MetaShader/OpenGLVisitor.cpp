#include "OpenGLVisitor.h"

#include "SemanticVisitor.h"

#include <iostream>
#include <memory>

OpenGLVisitor::OpenGLVisitor(const SemanticVisitor* analyzer)
    : semanticAnalyzer(analyzer) {
    fragmentTypeMapping = {
        {"V_COLOR", "vColor"},
        {"V_NORMAL", "vNormal"},
        {"V_UV", "vUV"},
        {"V_WORLD_POS", "vWorldPos"},
        {"COLOR", "fragColor"},
        {"POSITION", "gl_Position"},
        {"CAMERA_POS", "u_position"},
        {"CAMERA_PROJ", "u_projection"},
        {"CAMERA_VIEW", "u_view"},
        {"LIGHTS_COUNT", "count"},
        {"LIGHTS", "lights"},
    };
}

std::string OpenGLVisitor::getTransformedType(const std::string& type) {
    if (semanticAnalyzer) {
        const auto& constructorSigs = semanticAnalyzer->getConstructorSignatures();
        if (constructorSigs.count(type)) {
            return type;
        }
    }
    return getOrCreateRandomName(type);
}

std::string OpenGLVisitor::getTransformedFunctionName(const std::string& name) {
    if (semanticAnalyzer &&
        (semanticAnalyzer->isBuiltinFunction(name) ||
         semanticAnalyzer->getConstructorSignatures().count(name))) {
        return name;
    }
    return getOrCreateRandomName(name);
}

std::string OpenGLVisitor::getOrCreateRandomName(const std::string& variableName) {
    auto existing = randomizedNames.find(variableName);
    if (existing != randomizedNames.end()) {
        return existing->second;
    }

    static constexpr char alphabet[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    std::uniform_int_distribution<int> alphabetDistribution(0, static_cast<int>(sizeof(alphabet) - 2));
    std::uniform_int_distribution<int> lengthDistribution(10, 14);

    std::string generated;
    do {
        generated.clear();
        generated += 'v';
        generated += '_';

        int randomLength = lengthDistribution(randomGenerator);
        for (int i = 0; i < randomLength; ++i) {
            generated += alphabet[alphabetDistribution(randomGenerator)];
        }
    } while (usedRandomizedNames.count(generated) > 0);

    randomizedNames[variableName] = generated;
    usedRandomizedNames.insert(generated);
    return generated;
}

bool OpenGLVisitor::isUserFunction(const std::string& functionName) const {
    return fragmentFunctionTable.find(functionName) != fragmentFunctionTable.end();
}

void OpenGLVisitor::collectCalledFunctionsFromExpr(
    const ExpressionPtr& expr,
    std::unordered_set<std::string>& calledFunctions) {
    if (!expr) return;

    if (auto call = std::dynamic_pointer_cast<FunctionCallExpr>(expr)) {
        if (isUserFunction(call->functionName)) {
            calledFunctions.insert(call->functionName);
        }
        for (const auto& arg : call->args) {
            collectCalledFunctionsFromExpr(arg, calledFunctions);
        }
        return;
    }

    if (auto call = std::dynamic_pointer_cast<CallExpr>(expr)) {
        if (!call->isConstructor && isUserFunction(call->callee)) {
            calledFunctions.insert(call->callee);
        }
        if (call->calleeExpr) {
            collectCalledFunctionsFromExpr(call->calleeExpr, calledFunctions);
        }
        for (const auto& arg : call->args) {
            collectCalledFunctionsFromExpr(arg, calledFunctions);
        }
        return;
    }

    if (auto binary = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        collectCalledFunctionsFromExpr(binary->left, calledFunctions);
        collectCalledFunctionsFromExpr(binary->right, calledFunctions);
        return;
    }

    if (auto assign = std::dynamic_pointer_cast<AssignExpr>(expr)) {
        collectCalledFunctionsFromExpr(assign->left, calledFunctions);
        collectCalledFunctionsFromExpr(assign->right, calledFunctions);
        return;
    }

    if (auto index = std::dynamic_pointer_cast<IndexExpr>(expr)) {
        collectCalledFunctionsFromExpr(index->object, calledFunctions);
        collectCalledFunctionsFromExpr(index->index, calledFunctions);
        return;
    }

    if (auto member = std::dynamic_pointer_cast<MemberExpr>(expr)) {
        collectCalledFunctionsFromExpr(member->object, calledFunctions);
        return;
    }

    if (auto postfix = std::dynamic_pointer_cast<PostfixExpr>(expr)) {
        collectCalledFunctionsFromExpr(postfix->operand, calledFunctions);
        return;
    }

    if (auto unary = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
        collectCalledFunctionsFromExpr(unary->operand, calledFunctions);
        return;
    }

    if (auto ternary = std::dynamic_pointer_cast<TernaryExpr>(expr)) {
        collectCalledFunctionsFromExpr(ternary->condition, calledFunctions);
        collectCalledFunctionsFromExpr(ternary->trueExpr, calledFunctions);
        collectCalledFunctionsFromExpr(ternary->falseExpr, calledFunctions);
        return;
    }

    if (auto comma = std::dynamic_pointer_cast<CommaExpr>(expr)) {
        collectCalledFunctionsFromExpr(comma->left, calledFunctions);
        collectCalledFunctionsFromExpr(comma->right, calledFunctions);
        return;
    }
}

void OpenGLVisitor::collectCalledFunctionsFromStmt(
    const StatementPtr& stmt,
    std::unordered_set<std::string>& calledFunctions) {
    if (!stmt) return;

    if (auto varDecl = std::dynamic_pointer_cast<VarDeclStmt>(stmt)) {
        if (varDecl->initializerExpr) {
            collectCalledFunctionsFromExpr(varDecl->initializerExpr, calledFunctions);
        }
        return;
    }

    if (auto block = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
        for (const auto& child : block->stmts) {
            collectCalledFunctionsFromStmt(child, calledFunctions);
        }
        return;
    }

    if (auto exprStmt = std::dynamic_pointer_cast<ExprStmt>(stmt)) {
        collectCalledFunctionsFromExpr(exprStmt->expr, calledFunctions);
        return;
    }

    if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
        collectCalledFunctionsFromExpr(ifStmt->condition, calledFunctions);
        collectCalledFunctionsFromStmt(ifStmt->thenBranch, calledFunctions);
        collectCalledFunctionsFromStmt(ifStmt->elseBranch, calledFunctions);
        return;
    }

    if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
        collectCalledFunctionsFromExpr(whileStmt->condition, calledFunctions);
        collectCalledFunctionsFromStmt(whileStmt->body, calledFunctions);
        return;
    }

    if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
        collectCalledFunctionsFromStmt(forStmt->init, calledFunctions);
        collectCalledFunctionsFromExpr(forStmt->condition, calledFunctions);
        collectCalledFunctionsFromExpr(forStmt->iteration, calledFunctions);
        collectCalledFunctionsFromStmt(forStmt->body, calledFunctions);
        return;
    }

    if (auto switchStmt = std::dynamic_pointer_cast<SwitchStmt>(stmt)) {
        collectCalledFunctionsFromExpr(switchStmt->expr, calledFunctions);
        collectCalledFunctionsFromStmt(switchStmt->body, calledFunctions);
        return;
    }

    if (auto caseLabel = std::dynamic_pointer_cast<CaseLabel>(stmt)) {
        collectCalledFunctionsFromStmt(caseLabel->stmt, calledFunctions);
        return;
    }

    if (auto jumpStmt = std::dynamic_pointer_cast<JumpStmt>(stmt)) {
        collectCalledFunctionsFromExpr(jumpStmt->value, calledFunctions);
        return;
    }
}

void OpenGLVisitor::collectReachableFunctionsDfs(
    const std::string& functionName,
    std::unordered_map<std::string, const FunctionInfo*> functionTable,
    std::unordered_map<std::string, int>& visitState,
    std::vector<const FunctionInfo*>& orderedFunctions) {
    auto it = functionTable.find(functionName);
    if (it == functionTable.end()) return;

    const int state = visitState[functionName];
    if (state == 2) return;
    if (state == 1) return;

    visitState[functionName] = 1;

    std::unordered_set<std::string> calledFunctions;
    collectCalledFunctionsFromStmt(it->second->body, calledFunctions);
    for (const auto& called : calledFunctions) {
        collectReachableFunctionsDfs(called, functionTable, visitState, orderedFunctions);
    }

    visitState[functionName] = 2;
    orderedFunctions.push_back(it->second);
}

std::string OpenGLVisitor::formatFunctionSignature(const FunctionInfo& fn) {
    std::string signature;
    signature += fn.returnType + " ";
    signature += (fn.name == "fragmentMain" || fn.name == "vertexMain"
                      ? "main"
                      : getTransformedFunctionName(fn.name));
    signature += "(";
    for (size_t j = 0; j < fn.params.size(); ++j) {
        const auto& p = fn.params[j];
        if (!p.qualifier.empty()) signature += p.qualifier + " ";
        signature += p.type + " " + getOrCreateRandomName(p.name);
        if (j < fn.params.size() - 1) signature += ", ";
    }
    signature += ")";
    return signature;
}

void OpenGLVisitor::emitFunctionPrototype(const FunctionInfo& fn) {
    output += formatFunctionSignature(fn) + ";\n";
}

void OpenGLVisitor::emitFunctionDefinition(const FunctionInfo& fn) {
    output += formatFunctionSignature(fn);
    if (fn.body) {
        fn.body->accept(this);
        output += "\n";
    }
}

void OpenGLVisitor::visit(VarDeclStmt* stmt) {
    if (!stmt) return;
    output += tabulation + getTransformedType(stmt->var.type) + " " + getOrCreateRandomName(stmt->var.name);
    if (stmt->var.arraySize != -1) output += "[" + std::to_string(stmt->var.arraySize) + "]";
    if (stmt->initializerExpr) {
        output += " = ";
        stmt->initializerExpr->accept(this);
    } else if (!stmt->var.initializer.empty()) {
        output += " = " + stmt->var.initializer;
    }
    output += ";\n";
}

void OpenGLVisitor::visit(BlockStmt* stmt) {
    if (!stmt) return;
    output += " {\n";
    tabulation += "\t";
    for (const auto& childStmt : stmt->stmts) {
        if (childStmt) {
            childStmt->accept(this);
        }
    }
    tabulation.pop_back();
    output += tabulation + "}\n";
}

void OpenGLVisitor::visit(IfStmt* stmt) {
    output += tabulation + "if (";
    if (stmt->condition) {
        stmt->condition->accept(this);
    }
    output += ")";
    if (stmt->thenBranch) {
        stmt->thenBranch->accept(this);
    }
    if (stmt->elseBranch) {
        output += tabulation + "else";
        stmt->elseBranch->accept(this);
    }
}

void OpenGLVisitor::visit(WhileStmt* stmt) {
    output += tabulation + "while (";
    if (stmt->condition) {
        stmt->condition->accept(this);
    }
    output += ")";
    if (stmt->body) {
        stmt->body->accept(this);
    }
}

void OpenGLVisitor::visit(ForStmt* stmt) {
    output += tabulation + "for (";
    if (stmt->init) {
        stmt->init->accept(this);
    } else {
        output += ";";
    }
    if (stmt->condition) {
        stmt->condition->accept(this);
    }
    output += ";";
    if (stmt->iteration) {
        stmt->iteration->accept(this);
    }
    output += ")";
    if (stmt->body) {
        stmt->body->accept(this);
    }
}

void OpenGLVisitor::visit(SwitchStmt* stmt) {
    output += tabulation + "switch (";
    if (stmt->expr) {
        stmt->expr->accept(this);
    }
    output += ")";
    if (stmt->body) {
        stmt->body->accept(this);
    }
}

void OpenGLVisitor::visit(CaseLabel* stmt) {
    output += tabulation;
    if (stmt->label == "default:") {
        output += "default:\n";
    } else {
        output += stmt->label + "\n";
    }
    if (!stmt->stmt) return;
    auto block = std::dynamic_pointer_cast<BlockStmt>(stmt->stmt);
    if (block && block->stmts.empty()) return;
    stmt->stmt->accept(this);
}

void OpenGLVisitor::visit(JumpStmt* stmt) {
    if (!stmt) return;
    if (stmt->kind == JumpStmt::Kind::Return) {
        output += tabulation + "return";
        if (stmt->value) {
            output += " ";
            stmt->value->accept(this);
        }
        output += ";\n";
    } else if (stmt->kind == JumpStmt::Kind::Break) {
        output += tabulation + "break;\n";
    } else if (stmt->kind == JumpStmt::Kind::Continue) {
        output += tabulation + "continue;\n";
    } else if (stmt->kind == JumpStmt::Kind::Discard) {
        output += tabulation + "discard;\n";
    }
}

void OpenGLVisitor::visit(ExprStmt* stmt) {
    output += tabulation;
    if (stmt->expr) {
        stmt->expr->accept(this);
    }
    output += ";\n";
}

void OpenGLVisitor::visit(FunctionDeclStmt* stmt) {
    (void)stmt;
}

void OpenGLVisitor::visit(VariableExpr* expr) {
    const auto mapped = fragmentTypeMapping.find(expr->name);
    output += (mapped != fragmentTypeMapping.end() ? mapped->second : getOrCreateRandomName(expr->name));
}

void OpenGLVisitor::visit(IntLiteralExpr* expr) {
    output += std::to_string(expr->value) + (expr->isUnsigned ? "u" : "");
}

void OpenGLVisitor::visit(FloatLiteralExpr* expr) {
    output += std::to_string(expr->value) + (expr->isDouble ? "lf" : "");
}

void OpenGLVisitor::visit(BoolLiteralExpr* expr) {
    output += (expr->value ? "true" : "false");
}

void OpenGLVisitor::visit(BinaryExpr* expr) {
    expr->left->accept(this);
    output += " " + expr->op + " ";
    expr->right->accept(this);
}

void OpenGLVisitor::visit(AssignExpr* expr) {
    expr->left->accept(this);
    output += " " + expr->op + " ";
    expr->right->accept(this);
}

void OpenGLVisitor::visit(FunctionCallExpr* expr) {
    output += getTransformedFunctionName(expr->functionName) + "(";
    for (size_t i = 0; i < expr->args.size(); ++i) {
        expr->args[i]->accept(this);
        if (i < expr->args.size() - 1) {
            output += ", ";
        }
    }
    output += ")";
}

void OpenGLVisitor::visit(IndexExpr* expr) {
    expr->object->accept(this);
    output += "[";
    expr->index->accept(this);
    output += "]";
}

void OpenGLVisitor::visit(MemberExpr* expr) {
    expr->object->accept(this);
    output += "." + expr->field;
}

void OpenGLVisitor::visit(PostfixExpr* expr) {
    expr->operand->accept(this);
    output += expr->op;
}

void OpenGLVisitor::visit(UnaryExpr* expr) {
    output += expr->op;
    expr->operand->accept(this);
}

void OpenGLVisitor::visit(TernaryExpr* expr) {
    expr->condition->accept(this);
    output += " ? ";
    expr->trueExpr->accept(this);
    output += " : ";
    expr->falseExpr->accept(this);
}

void OpenGLVisitor::visit(CommaExpr* expr) {
    (void)expr;
}

void OpenGLVisitor::visit(CallExpr* expr) {
    if (expr->calleeExpr) {
        expr->calleeExpr->accept(this);
        output += "." + getTransformedFunctionName(expr->callee);
    } else {
        output += getTransformedFunctionName(expr->callee);
    }
    output += "(";
    for (size_t i = 0; i < expr->args.size(); ++i) {
        expr->args[i]->accept(this);
        if (i < expr->args.size() - 1) {
            output += ", ";
        }
    }
    output += ")";
}

void OpenGLVisitor::produceShaders(const ShaderInfo& shader) {
    info = shader;
    produceFragmentShader(shader);
    fragmentShaderCode_ = output;
    output = "";
    produceVertexShader(shader);
    vertexShaderCode_ = output;
    injectVertexVaryingInitialization();
}

void OpenGLVisitor::produceVertexInputInfo(const ShaderInfo& shader) {
    output += R"(#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;
out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;
out vec4 vColor;
struct Light {
    vec4 position;
    vec3 color;
    float intensity;
};

layout(std140, binding = 1) uniform lightsInfo {
    int count;
    Light lights[128];
};

layout(std140, binding = 2) uniform CameraInfo {
    mat4 u_view;
    mat4 u_projection;
    vec4 u_position; // camera world position
};
uniform mat4 u_model;
)";

    for (const auto& structPair : shader.structs) {
        output += "struct " + getOrCreateRandomName(structPair.first) + " {\n";
        for (const auto& field : structPair.second.fields) {
            output += "    " + getTransformedType(field.type) + " " + getOrCreateRandomName(field.name);
            if (field.arraySize != -1) {
                output += "[" + std::to_string(field.arraySize) + "]";
            }
            output += ";\n";
        }
        output += "};\n";
    }

    for (const auto& uniform : shader.uniforms) {
        output += "uniform " + getTransformedType(uniform.type) + " " + getOrCreateRandomName(uniform.name) +
                  (uniform.arraySize >= 0 ? "[" + std::to_string(uniform.arraySize) + "]" : "") + ";\n";
    }
    for (const auto& global : shader.globalVariables) {
        output += getTransformedType(global.type) + " " + getOrCreateRandomName(global.name);
        if (global.arraySize != -1) output += "[" + std::to_string(global.arraySize) + "]";
        output += ";\n";
    }
}

void OpenGLVisitor::produceFragmentInputInfo(const ShaderInfo& shader) {
    output += "#version 450 core\n";
    output += "in vec3 vWorldPos;\n";
    output += "in vec3 vNormal;\n";
    output += "in vec2 vUV;\n";
    output += "in vec4 vColor;\n";
    output += "out vec4 fragColor;\n";
    output += R"(
struct Light {
    vec4 position;
    vec3 color;
    float intensity;
};

layout(std140, binding = 1) uniform lightsInfo {
    int count;
    Light lights[128];
};

layout(std140, binding = 2) uniform CameraInfo {
    mat4 u_view;
    mat4 u_projection;
    vec4 u_position; // camera world position
};
)";

    for (const auto& structPair : shader.structs) {
        output += "struct " + getOrCreateRandomName(structPair.first) + " {\n";
        for (const auto& field : structPair.second.fields) {
            output += "    " + getTransformedType(field.type) + " " + getOrCreateRandomName(field.name);
            if (field.arraySize != -1) {
                output += "[" + std::to_string(field.arraySize) + "]";
            }
            output += ";\n";
        }
        output += "};\n";
    }

    for (const auto& uniform : shader.uniforms) {
        output += "uniform " + getTransformedType(uniform.type) + " " + getOrCreateRandomName(uniform.name) +
                  (uniform.arraySize >= 0 ? "[" + std::to_string(uniform.arraySize) + "]" : "") + ";\n";
    }
    for (const auto& global : shader.globalVariables) {
        output += getTransformedType(global.type) + " " + getOrCreateRandomName(global.name);
        if (global.arraySize != -1) output += "[" + std::to_string(global.arraySize) + "]";
        output += ";\n";
    }
}

void OpenGLVisitor::produceFragmentShader(const ShaderInfo& shader) {
    fragmentFunctionTable.clear();
    fragmentVisitState.clear();
    orderedFragmentFunctions.clear();
    visited.clear();

    for (const auto& func : shader.functions) {
        fragmentFunctionTable[func.name] = &func;
    }

    if (!isUserFunction("fragmentMain")) {
        std::cerr << "Error: No fragmentMain function found in shader." << std::endl;
        return;
    }

    produceFragmentInputInfo(shader);

    collectReachableFunctionsDfs("fragmentMain", fragmentFunctionTable, fragmentVisitState, orderedFragmentFunctions);

    if (!orderedFragmentFunctions.empty()) {
        output += "\n";
    }

    for (const auto* fn : orderedFragmentFunctions) {
        if (fn->name != "fragmentMain") {
            emitFunctionPrototype(*fn);
        }
    }

    if (!orderedFragmentFunctions.empty()) {
        output += "\n";
    }

    for (const auto* fn : orderedFragmentFunctions) {
        emitFunctionDefinition(*fn);
    }
}

void OpenGLVisitor::produceStandardVertexShader() {
    output += R"(#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;
out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;
out vec4 vColor;
struct Light {
    vec4 position;
    vec3 color;
    float intensity;
};

layout(std140, binding = 1) uniform lightsInfo {
    int count;
    Light lights[128];
};

layout(std140, binding = 2) uniform CameraInfo {
    mat4 u_view;
    mat4 u_projection;
    vec4 u_position; // camera world position
};
uniform mat4 u_model;
void main() {
    vWorldPos = (u_model * vec4(aPosition, 1.0)).xyz;
    vNormal = mat3(transpose(inverse(u_model))) * normalize(aNormal);
    vUV = aTexCoord;
    vColor = aColor;
    gl_Position = u_projection * u_view * vec4(vWorldPos, 1.0);
})";
}

void OpenGLVisitor::produceVertexShader(const ShaderInfo& shader) {
    vertexFunctionTable.clear();
    vertexVisitState.clear();
    orderedVertexFunctions.clear();
    visited.clear();
    for (const auto& func : shader.functions) {
        vertexFunctionTable[func.name] = &func;
    }
    if (!isUserFunction("vertexMain")) {
        produceStandardVertexShader();
        return;
    }
    produceVertexInputInfo(shader);
    collectReachableFunctionsDfs("vertexMain", vertexFunctionTable, vertexVisitState, orderedVertexFunctions);

    if (!orderedVertexFunctions.empty()) {
        output += "\n";
    }

    for (const auto* fn : orderedVertexFunctions) {
        if (fn->name != "vertexMain") {
            emitFunctionPrototype(*fn);
        }
    }

    if (!orderedVertexFunctions.empty()) {
        output += "\n";
    }

    for (const auto* fn : orderedVertexFunctions) {
        emitFunctionDefinition(*fn);
    }
}

void OpenGLVisitor::injectVertexVaryingInitialization() {
    size_t mainPos = vertexShaderCode_.find("void main()");
    if (mainPos == std::string::npos) {
        mainPos = vertexShaderCode_.find("main() {");
        if (mainPos == std::string::npos) return;
    }

    size_t bracePos = vertexShaderCode_.find("{", mainPos);
    if (bracePos == std::string::npos) return;

    bool hasWorldPosInit = vertexShaderCode_.find("vWorldPos =") != std::string::npos;
    bool hasNormalInit = vertexShaderCode_.find("vNormal =") != std::string::npos;
    bool hasUVInit = vertexShaderCode_.find("vUV =") != std::string::npos;
    bool hasColorInit = vertexShaderCode_.find("vColor =") != std::string::npos;

    std::string injectedCode;
    if (!hasWorldPosInit) {
        injectedCode += "\n    vWorldPos = (u_model * vec4(aPosition, 1.0)).xyz;";
    }
    if (!hasNormalInit) {
        injectedCode += "\n    vNormal = mat3(transpose(inverse(u_model))) * normalize(aNormal);";
    }
    if (!hasUVInit) {
        injectedCode += "\n    vUV = aTexCoord;";
    }
    if (!hasColorInit) {
        injectedCode += "\n    vColor = aColor;";
    }

    if (!injectedCode.empty()) {
        vertexShaderCode_.insert(bracePos + 1, injectedCode);
    }
}

void OpenGLVisitor::printFragmentShader() const {
    std::cout << "\n\nGenerated Fragment Shader:\n";
    std::cout << fragmentShaderCode_ << std::endl;
}

void OpenGLVisitor::printVertexShader() const {
    std::cout << "\n\nGenerated Vertex Shader:\n";
    std::cout << vertexShaderCode_ << std::endl;
}
