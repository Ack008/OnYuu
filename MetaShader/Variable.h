#pragma once 
#include <string>
#include <memory>
#include <vector>

struct Expression;
struct UniformVar {
    std::string type;       // "vec3", "mat4", ecc.
    std::string name;       // "u_color", "u_mvp"
    bool isSampler = false; // sampler2D, samplerCube, ecc.
    int arraySize = -1;     // -1 = non è un array
    std::string initializer; // deve essere vuoto: le uniform non sono inizializzabili
};

struct InOutVar {
    std::string type;
    std::string name;
    int location = -1;      // da layout(location=N)
};



struct VariableDeclaration {
    std::string type;
    std::string name;
    std::string initializer; // es. "1.0" oppure "" se assente
    int arraySize = -1;
    std::shared_ptr<Expression> initializerExpr;
};
