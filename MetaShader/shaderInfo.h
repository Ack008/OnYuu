#pragma once
#include <string>
#include <vector>
#include "FunctionInfo.h"
struct StructInfo {
    std::vector<VariableDeclaration> fields;

};


struct ShaderInfo {
    std::vector<UniformVar> uniforms;
    std::vector<VariableDeclaration> globalVariables;
    std::vector<InOutVar>   inputs;
    std::vector<InOutVar>   outputs;
    std::vector<FunctionInfo> functions;
    std::unordered_map<std::string, StructInfo> structs;
};