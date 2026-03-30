#pragma once

#include <memory>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// Minimal semantic scope model for AST passes (name resolution + type checks).
namespace semantic {

enum class ScopeKind {
    Global,
    Function,
    Block,
    Loop,
    Switch
};

enum class SymbolKind {
    Variable,
    Parameter,
    Function,
    TypeName
};

struct SourceLocation {
    int line = -1;
    int column = -1;
};

struct TypeInfo {
    std::string name;      // e.g. "float", "vec3", "mat4"
    bool isArray = false;
    int arraySize = -1;
    
    TypeInfo() = default;
    TypeInfo(const std::string& n) : name(n) {}
    TypeInfo(const std::string& n, bool isArr, int arrSize) : name(n), isArray(isArr), arraySize(arrSize) {}
};

struct Symbol {
    std::string name;
    SymbolKind kind = SymbolKind::Variable;
    TypeInfo type;
    SourceLocation location;
    std::unordered_map<std::string, Symbol> members; // for struct types
    std::vector<std::pair<std::vector<std::string>, std::string>> parameters; // for function symbols/signatures
    
};



class Scope {
private:
public:
    Scope(ScopeKind kind, Scope* parent = nullptr)
        : kind_(kind), parent_(parent) {}

    ScopeKind kind() const { return kind_; }
    Scope* parent() const { return parent_; }

    // Returns false if the symbol already exists in the current scope.
    bool declare(const Symbol& symbol) {
        auto [_, inserted] = symbols_.emplace(symbol.name, symbol);
        return inserted;
    }

    bool containsLocal(const std::string& name) const {
        return symbols_.find(name) != symbols_.end();
    }

    const Symbol* resolveLocal(const std::string& name) const {
        auto it = symbols_.find(name);
        return it == symbols_.end() ? nullptr : &it->second;
    }

    // Resolves from current scope up to ancestors.
    const Symbol* resolve(const std::string& name) const {
        for (const Scope* current = this; current != nullptr; current = current->parent_) {
            if (const Symbol* symbol = current->resolveLocal(name)) {
                return symbol;
            }
        }
        return nullptr;
    }

    const std::unordered_map<std::string, Symbol>& symbols() const {
        return symbols_;
    }

private:
    ScopeKind kind_;
    Scope* parent_ = nullptr;
    std::unordered_map<std::string, Symbol> symbols_;
};

class ScopeStack {
public:
    ScopeStack() {
        push(ScopeKind::Global);
        for(auto &symbol : builtIns){
            declare(symbol);
        }
    }

    Scope& current() {
        return *scopes_.back();
    }

    const Scope& current() const {
        return *scopes_.back();
    }
    void printAllSymbols() const {
        std::cout << "Scope Stack:\n";
        for (size_t i = 0; i < scopes_.size(); ++i) {
            const Scope* scope = scopes_[i].get();
            std::cout << "  Scope " << i << " (kind: " << static_cast<int>(scope->kind()) << "):\n";
            for (const auto& pair : scope->symbols()) {
                const Symbol& sym = pair.second;
                std::cout << "    " << sym.name << " (kind: " << static_cast<int>(sym.kind) << ", type: " << sym.type.name;
                if (sym.type.isArray) {
                    std::cout << "[" << sym.type.arraySize << "]";
                }
                std::cout << ")\n";
            }
        }
    }
    Scope& global() const {
        return *scopes_.front();
    }

    void push(ScopeKind kind) {
        Scope* parent = scopes_.empty() ? nullptr : scopes_.back().get();
        scopes_.push_back(std::make_unique<Scope>(kind, parent));
    }

    void pop() {
        if (scopes_.size() > 1) {
            scopes_.pop_back();
        }
    }

    bool declare(const Symbol& symbol) {
        return current().declare(symbol);
    }

    const Symbol* resolve(const std::string& name) const {
        return current().resolve(name);
    }
    std::vector<Symbol> &getBuiltIns(){
        return builtIns;
    }


private:
    std::vector<std::unique_ptr<Scope>> scopes_;
    
    std::vector<Symbol> builtIns = {
        {"V_WORLD_POS", SymbolKind::Variable, TypeInfo("vec3")},
        {"V_NORMAL", SymbolKind::Variable, TypeInfo("vec3")},
        {"V_UV", SymbolKind::Variable, TypeInfo("vec2")},
        {"V_COLOR", SymbolKind::Variable, TypeInfo("vec4")},
        {"POSITION", SymbolKind::Variable, TypeInfo("vec4")},
        {"COLOR", SymbolKind::Variable, TypeInfo("vec4")},
        {"gl_PointCoord", SymbolKind::Variable, TypeInfo("vec2")},
        {"gl_FrontFacing", SymbolKind::Variable, TypeInfo("bool")},
        {"CAMERA_POS", SymbolKind::Variable, TypeInfo("vec3")},
        {"CAMERA_PROJ", SymbolKind::Variable, TypeInfo("mat4")},
        {"CAMERA_VIEW", SymbolKind::Variable, TypeInfo("mat4")},
        {"LIGHTS_COUNT", SymbolKind::Variable, TypeInfo("int")},
        {"LIGHTS", SymbolKind::Variable, TypeInfo("Light", true, 128)},
        {"texture", SymbolKind::Function, TypeInfo("vec4")},
        {"sin", SymbolKind::Function, TypeInfo("float")},
        {"cos", SymbolKind::Function, TypeInfo("float")},
        {"normalize", SymbolKind::Function, TypeInfo("vec3")},
        {"length", SymbolKind::Function, TypeInfo("float")},
        {"dot", SymbolKind::Function, TypeInfo("float")},
        {"mix", SymbolKind::Function, TypeInfo("vec3")},
        {"min", SymbolKind::Function, TypeInfo("vec3")},
        {"max", SymbolKind::Function, TypeInfo("vec3")},
        {"clamp", SymbolKind::Function, TypeInfo("vec3")}
    };
};

} // namespace semantic
