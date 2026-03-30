#include "SemanticVisitor.h"

#include <cctype>
#include <iostream>
#include <utility>

void SemanticVisitor::addBuiltinFunction(const std::string& name,
                                         const std::vector<std::string>& params,
                                         const std::string& returnType) {
    builtinFunctionNames_.insert(name);
    functionSigs_[name].push_back({params, returnType});
}

void SemanticVisitor::addBuiltInStruct(
    const std::string& name,
    const std::vector<std::pair<std::string, std::string>>& fields) {
    semantic::Symbol structSymbol;
    structSymbol.name = name;
    structSymbol.kind = semantic::SymbolKind::TypeName;
    for (const auto& [fieldType, fieldName] : fields) {
        structSymbol.members[fieldName] = {fieldName, semantic::SymbolKind::Variable, semantic::TypeInfo(fieldType)};
    }
    scopes.declare(structSymbol);
}

void SemanticVisitor::addBuiltFunctionsInSymbolTable() {
    for (const auto& [name, overloads] : functionSigs_) {
        if (overloads.empty()) continue;
        semantic::Symbol symbol;
        symbol.name = name;
        symbol.kind = semantic::SymbolKind::Function;
        symbol.type = semantic::TypeInfo(overloads.front().second);
        symbol.parameters = overloads;
        scopes.declare(symbol);
    }
}

void SemanticVisitor::initBuiltinFunctionSignatures() {
    addBuiltinFunction("sin", {"float"}, "float");
    addBuiltinFunction("sin", {"vec2"}, "vec2");
    addBuiltinFunction("sin", {"vec3"}, "vec3");
    addBuiltinFunction("sin", {"vec4"}, "vec4");

    addBuiltinFunction("cos", {"float"}, "float");
    addBuiltinFunction("cos", {"vec2"}, "vec2");
    addBuiltinFunction("cos", {"vec3"}, "vec3");
    addBuiltinFunction("cos", {"vec4"}, "vec4");

    addBuiltinFunction("normalize", {"vec2"}, "vec2");
    addBuiltinFunction("normalize", {"vec3"}, "vec3");
    addBuiltinFunction("normalize", {"vec4"}, "vec4");

    addBuiltinFunction("length", {"float"}, "float");
    addBuiltinFunction("length", {"vec2"}, "float");
    addBuiltinFunction("length", {"vec3"}, "float");
    addBuiltinFunction("length", {"vec4"}, "float");

    addBuiltinFunction("dot", {"vec2", "vec2"}, "float");
    addBuiltinFunction("dot", {"vec3", "vec3"}, "float");
    addBuiltinFunction("dot", {"vec4", "vec4"}, "float");

    addBuiltinFunction("mix", {"float", "float", "float"}, "float");
    addBuiltinFunction("mix", {"vec2", "vec2", "float"}, "vec2");
    addBuiltinFunction("mix", {"vec3", "vec3", "float"}, "vec3");
    addBuiltinFunction("mix", {"vec4", "vec4", "float"}, "vec4");

    addBuiltinFunction("min", {"float", "float"}, "float");
    addBuiltinFunction("min", {"int", "int"}, "int");
    addBuiltinFunction("min", {"vec2", "vec2"}, "vec2");
    addBuiltinFunction("min", {"vec3", "vec3"}, "vec3");
    addBuiltinFunction("min", {"vec4", "vec4"}, "vec4");

    addBuiltinFunction("max", {"float", "float"}, "float");
    addBuiltinFunction("max", {"int", "int"}, "int");
    addBuiltinFunction("max", {"vec2", "vec2"}, "vec2");
    addBuiltinFunction("max", {"vec3", "vec3"}, "vec3");
    addBuiltinFunction("max", {"vec4", "vec4"}, "vec4");

    addBuiltinFunction("clamp", {"float", "float", "float"}, "float");
    addBuiltinFunction("clamp", {"vec2", "vec2", "vec2"}, "vec2");
    addBuiltinFunction("clamp", {"vec3", "vec3", "vec3"}, "vec3");
    addBuiltinFunction("clamp", {"vec4", "vec4", "vec4"}, "vec4");

    addBuiltinFunction("texture", {"sampler2D", "vec2"}, "vec4");
    addBuiltinFunction("texture", {"samplerCube", "vec3"}, "vec4");

    addBuiltInStruct("Light", {
        {"vec4", "position"},
        {"float", "intensity"},
        {"vec4", "color"}
    });
    addBuiltInStruct("vec2", {
        {"float", "x"},
        {"float", "y"},
    });
    addBuiltInStruct("vec3", {
        {"float", "x"},
        {"float", "y"},
        {"float", "z"},
        {"vec2", "xy"},
        {"vec2", "yz"},
        {"vec2", "xz"}
    });
    addBuiltInStruct("vec4", {
        {"float", "x"},
        {"float", "y"},
        {"float", "z"},
        {"float", "w"},
        {"vec2", "xy"},
        {"vec2", "yz"},
        {"vec2", "zw"},
        {"vec2", "xz"},
        {"vec3", "xyz"}
    });
}

void SemanticVisitor::initConstructorSignatures() {
    constructorSigs_["vec2"] = {
        {"float"},
        {"float", "float"},
        {"vec2"}
    };
    constructorSigs_["vec3"] = {
        {"float"},
        {"float", "float", "float"},
        {"vec2", "float"},
        {"float", "vec2"},
        {"vec3"}
    };
    constructorSigs_["vec4"] = {
        {"float"},
        {"float", "float", "float", "float"},
        {"vec2", "vec2"},
        {"vec3", "float"},
        {"float", "vec3"},
        {"vec4"}
    };
    constructorSigs_["int"] = {{"int"}, {"float"}, {"bool"}};
    constructorSigs_["float"] = {{"float"}, {"int"}, {"bool"}};
    constructorSigs_["bool"] = {{"bool"}, {"int"}, {"float"}};
    constructorSigs_["mat4"] = {
        {"float"},
        {"float", "float", "float", "float",
         "float", "float", "float", "float",
         "float", "float", "float", "float",
         "float", "float", "float", "float"},
        {"vec4", "vec4", "vec4", "vec4"},
        {"mat4"}
    };
    constructorSigs_["sampler2D"] = {};
}

std::string SemanticVisitor::trim(const std::string& value) {
    size_t begin = 0;
    while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin]))) {
        ++begin;
    }

    size_t end = value.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(begin, end - begin);
}

bool SemanticVisitor::parseArrayType(const std::string& typeName, std::string& baseType, int& arraySize) {
    const std::string clean = trim(typeName);
    const size_t leftBracket = clean.find('[');
    const size_t rightBracket = clean.find(']');

    if (leftBracket == std::string::npos || rightBracket == std::string::npos || rightBracket < leftBracket) {
        baseType = clean;
        arraySize = -1;
        return false;
    }

    baseType = trim(clean.substr(0, leftBracket));
    const std::string sizeToken = trim(clean.substr(leftBracket + 1, rightBracket - leftBracket - 1));
    if (sizeToken.empty()) {
        arraySize = -1;
    } else {
        try {
            arraySize = std::stoi(sizeToken);
        } catch (...) {
            arraySize = -1;
        }
    }
    return true;
}

const semantic::Symbol* SemanticVisitor::resolveTypeSymbolFromName(const std::string& typeName) {
    std::string baseType;
    int arraySize = -1;
    parseArrayType(typeName, baseType, arraySize);
    if (baseType.empty()) baseType = typeName;

    const auto* symbol = scopes.resolve(baseType);
    if (!symbol) return nullptr;
    if (symbol->kind != semantic::SymbolKind::TypeName) return nullptr;
    return symbol;
}

bool SemanticVisitor::isCompatible(const std::string& actual, const std::string& expected) const {
    if (actual == "unknown" || expected == "unknown") return true;

    std::string actualBase;
    std::string expectedBase;
    int actualArraySize = -1;
    int expectedArraySize = -1;
    const bool actualIsArray = parseArrayType(actual, actualBase, actualArraySize);
    const bool expectedIsArray = parseArrayType(expected, expectedBase, expectedArraySize);

    if (actualIsArray != expectedIsArray) return false;
    if (actualIsArray && expectedIsArray) {
        if (actualBase != expectedBase) return false;
        if (actualArraySize >= 0 && expectedArraySize >= 0 && actualArraySize != expectedArraySize) {
            return false;
        }
        return true;
    }

    if (actualBase == expectedBase) return true;
    if (expectedBase == "float" && actualBase == "int") return true;
    if (expectedBase == "int" && actualBase == "float") return true;
    return false;
}

std::string SemanticVisitor::typeOf(Expression* expression) const {
    auto it = exprTypes_.find(expression);
    if (it == exprTypes_.end()) return "unknown";
    return it->second;
}

void SemanticVisitor::setType(Expression* expression, const std::string& typeName) {
    if (expression) exprTypes_[expression] = typeName;
}

bool SemanticVisitor::checkConstructorArgs(const std::string& ctorName,
                                           const std::vector<std::string>& argTypes) const {
    auto found = constructorSigs_.find(ctorName);
    if (found == constructorSigs_.end()) return false;

    for (const auto& signature : found->second) {
        if (signature.size() != argTypes.size()) continue;
        bool ok = true;
        for (size_t index = 0; index < signature.size(); ++index) {
            if (!isCompatible(argTypes[index], signature[index])) {
                ok = false;
                break;
            }
        }
        if (ok) return true;
    }
    return false;
}

bool SemanticVisitor::checkFunctionArgs(const std::string& functionName,
                                        const std::vector<std::string>& argTypes,
                                        std::string* inferredReturnType) const {
    auto found = functionSigs_.find(functionName);
    if (found == functionSigs_.end()) return false;

    for (const auto& signature : found->second) {
        const auto& params = signature.first;
        const auto& returnType = signature.second;
        if (params.size() != argTypes.size()) continue;

        bool ok = true;
        for (size_t index = 0; index < params.size(); ++index) {
            if (!isCompatible(argTypes[index], params[index])) {
                ok = false;
                break;
            }
        }

        if (ok) {
            if (inferredReturnType) *inferredReturnType = returnType;
            return true;
        }
    }
    return false;
}

std::string SemanticVisitor::joinTypes(const std::vector<std::string>& types) {
    std::string joined;
    for (size_t index = 0; index < types.size(); ++index) {
        joined += types[index];
        if (index + 1 < types.size()) joined += ", ";
    }
    return joined;
}

bool SemanticVisitor::isReservedFunctionName(const std::string& name) const {
    if (functionSigs_.find(name) != functionSigs_.end()) {
        return true;
    }

    if (const auto* symbol = scopes.resolve(name)) {
        return symbol->kind == semantic::SymbolKind::Function;
    }

    return false;
}

bool SemanticVisitor::isUniformLValue(const Expression* expression) const {
    if (!expression) return false;

    if (const auto* variableExpr = dynamic_cast<const VariableExpr*>(expression)) {
        return uniformNames_.find(variableExpr->name) != uniformNames_.end();
    }

    if (const auto* memberExpr = dynamic_cast<const MemberExpr*>(expression)) {
        return isUniformLValue(memberExpr->object.get());
    }

    if (const auto* indexExpr = dynamic_cast<const IndexExpr*>(expression)) {
        return isUniformLValue(indexExpr->object.get());
    }

    return false;
}

SemanticVisitor::SemanticVisitor() {
    initConstructorSignatures();
}

std::unordered_map<std::string, std::vector<std::vector<std::string>>> SemanticVisitor::getConstructorSignatures() const {
    return constructorSigs_;
}

void SemanticVisitor::error(const std::string& msg) {
    diagnostics.push_back({Diagnostic::Level::Error, msg});
}

void SemanticVisitor::warning(const std::string& msg) {
    diagnostics.push_back({Diagnostic::Level::Warning, msg});
}

void SemanticVisitor::analyze(const ShaderInfo& shader) {
    functionSigs_.clear();
    builtinFunctionNames_.clear();
    uniformNames_.clear();
    initBuiltinFunctionSignatures();

    for (auto& s : shader.structs) {
        std::unordered_map<std::string, semantic::Symbol> members;
        for (const auto& field : s.second.fields) {
            semantic::TypeInfo fieldType;
            fieldType.name = field.type;
            fieldType.isArray = field.arraySize != -1;
            fieldType.arraySize = field.arraySize;
            members.emplace(field.name, semantic::Symbol{field.name, semantic::SymbolKind::Variable, fieldType});
        }
        if (!scopes.declare({s.first, semantic::SymbolKind::TypeName, semantic::TypeInfo(s.first), {}, std::move(members)})) {
            error("Tipo/struct già dichiarato (o riservato): '" + s.first + "'");
        }
    }

    for (auto& v : shader.globalVariables) {
        semantic::TypeInfo variableType;
        variableType.name = v.type;
        variableType.isArray = v.arraySize != -1;
        variableType.arraySize = v.arraySize;
        if (!isValidType(v.type)) {
            error("Tipo sconosciuto per variabile '" + v.name + "': '" + v.type + "'");
            variableType.name = "unknown";
        }
        if (!scopes.declare({v.name, semantic::SymbolKind::Variable, variableType})) {
            error("Nome già dichiarato (o riservato): '" + v.name + "'");
        }
        if (!v.initializerExpr) continue;

        v.initializerExpr->accept(this);
        const std::string initType = typeOf(v.initializerExpr.get());
        if (!isCompatible(initType, variableType.name)) {
            error("Tipo dell'inizializzatore incompatibile per variabile '" + v.name + "': '" + initType +
                  "' non è compatibile con '" + variableType.name + "'");
        }
    }

    for (auto& u : shader.uniforms) {
        semantic::TypeInfo uniformType;
        uniformType.name = u.type;
        uniformType.isArray = u.arraySize != -1;
        uniformType.arraySize = u.arraySize;
        uniformNames_.insert(u.name);
        if (!u.initializer.empty()) {
            error("Uniform non inizializzabile: '" + u.name + "' (trovato inizializzatore '" + u.initializer + "')");
        }
        if (!isValidType(u.type)) {
            error("Tipo sconosciuto per uniform '" + u.name + "': '" + u.type + "'");
            uniformType.name = "unknown";
        }
        if (!scopes.declare({u.name, semantic::SymbolKind::Variable, uniformType})) {
            error("Nome già dichiarato (o riservato): '" + u.name + "'");
        }
    }

    for (auto& i : shader.inputs) {
        if (!scopes.declare({i.name, semantic::SymbolKind::Variable, {i.type}})) {
            error("Input già dichiarato (o riservato): '" + i.name + "'");
        }
    }
    for (auto& o : shader.outputs) {
        if (!scopes.declare({o.name, semantic::SymbolKind::Variable, {o.type}})) {
            error("Output già dichiarato (o riservato): '" + o.name + "'");
        }
    }

    for (auto& fn : shader.functions) {
        if (!scopes.declare({fn.name, semantic::SymbolKind::Function, {fn.returnType}})) {
            error("Symbol already declared: " + fn.name);
            continue;
        }
        std::vector<std::string> params;
        params.reserve(fn.params.size());
        for (const auto& parameter : fn.params) {
            params.push_back(parameter.type);
        }
        functionSigs_[fn.name].push_back({std::move(params), fn.returnType});
    }

    for (auto& fn : shader.functions) {
        analyzeFunction(fn);
    }
}

void SemanticVisitor::printDiagnostics() const {
    std::cout << "[Semantic] Diagnostics: " << diagnostics.size() << "\n";
    for (auto& d : diagnostics) {
        std::cerr << (d.level == Diagnostic::Level::Error ? "[ERROR] " : "[WARN]  ") << d.message << "\n";
    }
    if (diagnostics.empty()) {
        std::cout << "[Semantic] Nessun errore rilevato.\n";
    }
}

bool SemanticVisitor::hasErrors() const {
    for (auto& d : diagnostics) {
        if (d.level == Diagnostic::Level::Error) return true;
    }
    return false;
}

bool SemanticVisitor::isFunctionDeclared(const std::string& name) const {
    return functionSigs_.find(name) != functionSigs_.end();
}

std::string SemanticVisitor::getFunctionReturnType(const std::string& name,
                                                   const std::vector<std::string>& argTypes) const {
    auto found = functionSigs_.find(name);
    if (found == functionSigs_.end()) return "unknown";

    for (const auto& sig : found->second) {
        if (sig.first.size() != argTypes.size()) continue;
        bool match = true;
        for (size_t i = 0; i < sig.first.size(); i++) {
            if (!isCompatible(argTypes[i], sig.first[i])) {
                match = false;
                break;
            }
        }
        if (match) {
            return sig.second;
        }
    }
    return "unknown";
}

std::vector<std::string> SemanticVisitor::getAllFunctionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : functionSigs_) {
        names.push_back(pair.first);
    }
    return names;
}

std::unordered_map<std::string, std::vector<std::pair<std::vector<std::string>, std::string>>>
SemanticVisitor::getFunctionSignatures() const {
    return functionSigs_;
}

bool SemanticVisitor::isBuiltinFunction(const std::string& name) const {
    return builtinFunctionNames_.find(name) != builtinFunctionNames_.end();
}

void SemanticVisitor::analyzeFunction(const FunctionInfo& fn) {
    const std::string previousFunctionName = currentFunctionName_;
    const std::string previousFunctionReturnType = currentFunctionReturnType_;
    const bool previousFunctionHasReturnValue = currentFunctionHasReturnValue_;

    currentFunctionName_ = fn.name;
    currentFunctionReturnType_ = fn.returnType;
    currentFunctionHasReturnValue_ = false;

    scopes.push(semantic::ScopeKind::Function);
    for (auto& p : fn.params) {
        if (isReservedFunctionName(p.name)) {
            error("Parametro usa un nome riservato di funzione: '" + p.name + "'");
            continue;
        }
        scopes.declare({p.name, semantic::SymbolKind::Parameter, {p.type}});
        if (functionSigs_.find(p.name) != functionSigs_.end()) {
            error("Parametro già dichiarato in questa funzione: '" + p.name + "'");
        }
    }

    if (fn.body) {
        fn.body->accept(this);
    }

    scopes.pop();

    if (currentFunctionReturnType_ != "void" && !currentFunctionHasReturnValue_) {
        error("La funzione '" + currentFunctionName_ + "' dichiara return type '" + currentFunctionReturnType_ +
              "' ma non ritorna alcun valore");
    }

    currentFunctionName_ = previousFunctionName;
    currentFunctionReturnType_ = previousFunctionReturnType;
    currentFunctionHasReturnValue_ = previousFunctionHasReturnValue;
}

void SemanticVisitor::visit(BlockStmt* s) {
    scopes.push(semantic::ScopeKind::Block);
    for (auto& stmt : s->stmts) {
        stmt->accept(this);
    }
    scopes.pop();
}

bool SemanticVisitor::isValidType(const std::string& typeName) const {
    if (typeName.empty()) return false;
    if (const semantic::Symbol* symbol = scopes.resolve(typeName)) {
        if (symbol->kind == semantic::SymbolKind::TypeName) {
            return true;
        }
    }
    if (constructorSigs_.find(typeName) != constructorSigs_.end()) return true;
    return false;
}

void SemanticVisitor::visit(VarDeclStmt* s) {
    if (isReservedFunctionName(s->var.name)) {
        error("Variabile usa un nome riservato di funzione: '" + s->var.name + "'");
        return;
    }

    if (!isValidType(s->var.type)) {
        error("Tipo non valido per la variabile '" + s->var.name + "': '" + s->var.type + "'");
        return;
    }

    bool isArray = s->var.arraySize != -1;
    semantic::TypeInfo declaredType;
    declaredType.name = s->var.type;
    declaredType.isArray = isArray;
    declaredType.arraySize = s->var.arraySize;

    if (!scopes.declare({s->var.name, semantic::SymbolKind::Variable, declaredType})) {
        error("Variabile già dichiarata (o riservata): '" + s->var.name + "'");
        return;
    }

    if (s->initializerExpr) {
        s->initializerExpr->accept(this);
        const std::string initType = typeOf(s->initializerExpr.get());

        std::string declaredTypeName = declaredType.name;
        if (declaredType.isArray && declaredType.arraySize >= 0) {
            declaredTypeName += "[" + std::to_string(declaredType.arraySize) + "]";
        }

        if (!isCompatible(initType, declaredTypeName)) {
            error("Inizializzazione non valida per variabile '" + s->var.name +
                  "': atteso '" + declaredTypeName + "', trovato '" + initType + "'");
        }
    }
}

void SemanticVisitor::visit(ExprStmt* s) {
    if (s->expr) s->expr->accept(this);
}

void SemanticVisitor::visit(IfStmt* s) {
    if (s->condition) s->condition->accept(this);
    if (s->thenBranch) s->thenBranch->accept(this);
    if (s->elseBranch) s->elseBranch->accept(this);
}

void SemanticVisitor::visit(WhileStmt* s) {
    scopes.push(semantic::ScopeKind::Loop);
    if (s->condition) s->condition->accept(this);
    if (s->body) s->body->accept(this);
    scopes.pop();
}

void SemanticVisitor::visit(ForStmt* s) {
    scopes.push(semantic::ScopeKind::Loop);
    if (s->init) s->init->accept(this);
    if (s->condition) s->condition->accept(this);
    if (s->iteration) s->iteration->accept(this);
    if (s->body) s->body->accept(this);
    scopes.pop();
}

void SemanticVisitor::visit(SwitchStmt* s) {
    if (s->expr) s->expr->accept(this);
    scopes.push(semantic::ScopeKind::Switch);
    if (s->body) s->body->accept(this);
    scopes.pop();
}

void SemanticVisitor::visit(CaseLabel* s) {
    if (s->stmt) s->stmt->accept(this);
}

void SemanticVisitor::visit(JumpStmt* s) {
    if (s->kind != JumpStmt::Kind::Return) {
        if (s->value) s->value->accept(this);
        return;
    }

    if (currentFunctionReturnType_.empty()) {
        if (s->value) s->value->accept(this);
        return;
    }

    if (!s->value) {
        if (currentFunctionReturnType_ != "void") {
            error("La funzione '" + currentFunctionName_ + "' deve ritornare un valore di tipo '" +
                  currentFunctionReturnType_ + "'");
        }
        return;
    }

    s->value->accept(this);
    const std::string returnExprType = typeOf(s->value.get());

    if (currentFunctionReturnType_ == "void") {
        error("La funzione '" + currentFunctionName_ + "' è void ma ritorna un valore");
        return;
    }

    if (!isCompatible(returnExprType, currentFunctionReturnType_)) {
        error("Return non compatibile in '" + currentFunctionName_ + "': atteso '" +
              currentFunctionReturnType_ + "', trovato '" + returnExprType + "'");
        return;
    }

    currentFunctionHasReturnValue_ = true;
}

void SemanticVisitor::visit(FunctionDeclStmt* s) {
    analyzeFunction(s->fn);
}

void SemanticVisitor::visit(VariableExpr* e) {
    if (!scopes.resolve(e->name)) {
        error("Variabile non dichiarata: '" + e->name + "'");
        setType(e, "unknown");
        return;
    }

    if (auto* symbol = scopes.resolve(e->name)) {
        if (symbol->kind != semantic::SymbolKind::Variable && symbol->kind != semantic::SymbolKind::Parameter) {
            error("Simbolo non variabile: '" + e->name + "'");
            setType(e, "unknown");
        } else {
            std::string arrayType = symbol->type.arraySize != -1 ? "[" + std::to_string(symbol->type.arraySize) + "]" : "";
            setType(e, symbol->type.name.empty() ? "unknown" : symbol->type.name + arrayType);
        }
    }
}

void SemanticVisitor::visit(AssignExpr* e) {
    if (e->left) e->left->accept(this);
    if (e->right) e->right->accept(this);

    if (e->left && isUniformLValue(e->left.get())) {
        error("Assegnamento non consentito su uniform: '" + e->left->toString() + "'");
    }

    if (e->left && e->right) {
        const std::string leftType = typeOf(e->left.get());
        const std::string rightType = typeOf(e->right.get());
        if (leftType != "unknown" && rightType != "unknown" && !isCompatible(rightType, leftType)) {
            error("Assegnamento non valido: '" + rightType + "' -> '" + leftType + "'");
        }
        setType(e, leftType);
    }
}

void SemanticVisitor::visit(BinaryExpr* e) {
    if (e->left) e->left->accept(this);
    if (e->right) e->right->accept(this);
    const std::string leftType = e->left ? typeOf(e->left.get()) : "unknown";
    const std::string rightType = e->right ? typeOf(e->right.get()) : "unknown";
    if (leftType == rightType) {
        setType(e, leftType);
        return;
    }
    char op = e->op[0];
    switch (op) {
        case '+':
        case '-':
            if (isCompatible(leftType, rightType) || isCompatible(rightType, leftType)) {
                setType(e, isCompatible(leftType, rightType) ? rightType : leftType);
                return;
            }
            break;
        case '*':
        case '/':
            if (isCompatible(leftType, rightType) || isCompatible(rightType, leftType)) {
                setType(e, isCompatible(leftType, rightType) ? rightType : leftType);
                return;
            }
            if ((leftType == "float" && rightType == "int") || (leftType == "int" && rightType == "float")) {
                setType(e, "float");
                return;
            } else if ((leftType == "vec2" && rightType == "float") || (leftType == "float" && rightType == "vec2")) {
                setType(e, "vec2");
                return;
            } else if ((leftType == "vec3" && rightType == "float") || (leftType == "float" && rightType == "vec3")) {
                setType(e, "vec3");
                return;
            } else if ((leftType == "vec4" && rightType == "float") || (leftType == "float" && rightType == "vec4")) {
                setType(e, "vec4");
                return;
            } else if ((leftType == "vec2" && rightType == "int") || (leftType == "int" && rightType == "vec2")) {
                setType(e, "vec2");
                return;
            } else if ((leftType == "vec3" && rightType == "int") || (leftType == "int" && rightType == "vec3")) {
                setType(e, "vec3");
                return;
            } else if ((leftType == "vec4" && rightType == "int") || (leftType == "int" && rightType == "vec4")) {
                setType(e, "vec4");
                return;
            } else if ((leftType == "mat4" && rightType == "float") || (leftType == "float" && rightType == "mat4")) {
                setType(e, "mat4");
                return;
            } else if ((leftType == "mat4" && rightType == "int") || (leftType == "int" && rightType == "mat4")) {
                setType(e, "mat4");
                return;
            } else if ((leftType == "mat4" && rightType == "vec4") || (leftType == "vec4" && rightType == "mat4")) {
                setType(e, "vec4");
                return;
            }
            break;
        case '=':
        case '>':
        case '<':
            if ((leftType == "float" && rightType == "int") || (leftType == "int" && rightType == "float") ||
                (leftType == "bool" && rightType == "bool")) {
                setType(e, "bool");
                return;
            } else {
                setType(e, "unknown");
            }
            break;
        default:
            break;
    }

    if (leftType != "unknown" && rightType != "unknown") {
        error("Operazione binaria non valida: '" + leftType + " " + e->op + " " + rightType + "'");
    }
    setType(e, "unknown");
}

void SemanticVisitor::visit(UnaryExpr* e) {
    if (e->operand) {
        e->operand->accept(this);

        if ((e->op == "++" || e->op == "--") && isUniformLValue(e->operand.get())) {
            error("Operazione non consentita su uniform: '" + e->operand->toString() + e->op + "'");
        }

        setType(e, typeOf(e->operand.get()));
    }
}

void SemanticVisitor::visit(PostfixExpr* e) {
    if (e->operand) {
        e->operand->accept(this);

        if ((e->op == "++" || e->op == "--") && isUniformLValue(e->operand.get())) {
            error("Operazione non consentita su uniform: '" + e->operand->toString() + e->op + "'");
        }

        setType(e, typeOf(e->operand.get()));
    }
}

void SemanticVisitor::visit(MemberExpr* e) {
    if (e->object) e->object->accept(this);
    const std::string objectType = e->object ? typeOf(e->object.get()) : "unknown";
    if (objectType == "unknown") {
        setType(e, "unknown");
        error("Tipo sconosciuto per l'oggetto dell'accesso a membro: '" + objectType + "'");
        return;
    }
    if (const auto* typeSymbol = resolveTypeSymbolFromName(objectType)) {
        auto memberIt = typeSymbol->members.find(e->field);
        if (memberIt != typeSymbol->members.end()) {
            const auto& memberType = memberIt->second.type;
            if (memberType.isArray && memberType.arraySize >= 0) {
                setType(e, memberType.name + "[" + std::to_string(memberType.arraySize) + "]");
            } else {
                setType(e, memberType.name.empty() ? "unknown" : memberType.name);
            }
            return;
        }
    }

    error("Membro non valido: '" + objectType + "." + e->field + "'");
    setType(e, "unknown");
}

void SemanticVisitor::visit(IndexExpr* e) {
    if (e->object) e->object->accept(this);
    if (e->index) e->index->accept(this);

    const std::string objectType = e->object ? typeOf(e->object.get()) : "unknown";
    const std::string indexType = e->index ? typeOf(e->index.get()) : "unknown";

    if (indexType != "unknown" && indexType != "int" && indexType != "uint") {
        error("Indice array non valido: atteso tipo intero, trovato '" + indexType + "'");
    }

    std::string baseType;
    int arraySize = -1;
    if (parseArrayType(objectType, baseType, arraySize)) {
        setType(e, baseType.empty() ? "unknown" : baseType);
        return;
    }

    if (objectType != "unknown") {
        error("Tentativo di indicizzare un valore non array di tipo '" + objectType + "'");
    }
    setType(e, "unknown");
}

void SemanticVisitor::visit(TernaryExpr* e) {
    if (e->condition) e->condition->accept(this);
    if (e->trueExpr) e->trueExpr->accept(this);
    if (e->falseExpr) e->falseExpr->accept(this);
    if (e->trueExpr) setType(e, typeOf(e->trueExpr.get()));
}

void SemanticVisitor::visit(CommaExpr* e) {
    if (e->left) e->left->accept(this);
    if (e->right) e->right->accept(this);
    if (e->right) setType(e, typeOf(e->right.get()));
}

void SemanticVisitor::visit(CallExpr* e) {
    std::vector<std::string> argTypes;
    for (auto& arg : e->args) {
        arg->accept(this);
        argTypes.push_back(typeOf(arg.get()));
    }

    if (e->isConstructor && !e->callee.empty()) {
        if (!checkConstructorArgs(e->callee, argTypes)) {
            error("Costruttore '" + e->callee + "' non accetta argomenti: (" + joinTypes(argTypes) + ")");
            setType(e, "unknown");
        } else {
            setType(e, e->callee);
        }
        return;
    }

    if (!e->isConstructor && !e->callee.empty()) {
        if (functionSigs_.count(e->callee)) {
            std::string returnType;
            if (!checkFunctionArgs(e->callee, argTypes, &returnType)) {
                error("Chiamata funzione non valida: '" + e->callee + "(" + joinTypes(argTypes) + ")'");
                setType(e, "unknown");
            } else {
                setType(e, returnType.empty() ? "unknown" : returnType);
            }
            return;
        }

        if (const auto* symbol = scopes.resolve(e->callee)) {
            if (symbol->kind != semantic::SymbolKind::Function) {
                error("Simbolo non invocabile come funzione: '" + e->callee + "'");
            }
        } else {
            error("Funzione non dichiarata: '" + e->callee + "'");
        }
    }

    setType(e, "unknown");
}

void SemanticVisitor::visit(FunctionCallExpr* e) {
    std::vector<std::string> argTypes;
    for (auto& arg : e->args) {
        arg->accept(this);
        argTypes.push_back(typeOf(arg.get()));
    }

    if (!e->functionName.empty()) {
        if (functionSigs_.count(e->functionName)) {
            std::string returnType;
            if (!checkFunctionArgs(e->functionName, argTypes, &returnType)) {
                error("Chiamata funzione non valida: '" + e->functionName + "(" + joinTypes(argTypes) + ")'");
                setType(e, "unknown");
            } else {
                setType(e, returnType.empty() ? "unknown" : returnType);
            }
            return;
        }

        if (const auto* symbol = scopes.resolve(e->functionName)) {
            if (symbol->kind != semantic::SymbolKind::Function) {
                error("Simbolo non invocabile come funzione: '" + e->functionName + "'");
            }
        } else {
            error("Funzione non dichiarata: '" + e->functionName + "'");
        }
    }

    setType(e, "unknown");
}

void SemanticVisitor::visit(IntLiteralExpr* e) {
    setType(e, e->isUnsigned ? "uint" : "int");
}

void SemanticVisitor::visit(FloatLiteralExpr* e) {
    setType(e, e->isDouble ? "double" : "float");
}

void SemanticVisitor::visit(BoolLiteralExpr* e) {
    setType(e, "bool");
}
