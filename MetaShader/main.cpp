#include <iostream>
#include <fstream>
#include <string>

#include "MyGLSLVisitor.h"
#include "OpenGLVisitor.h"



std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Impossibile aprire il file: " + filename);

    std::string content((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));
    return content;
}



int main() {
    try {
        // 1. Leggi il file GLSL
        std::string shaderCode = readFile("shader.glsl");

        // 2. Crea uno stream di input ANTLR
        antlr4::ANTLRInputStream input(shaderCode);

        // 3. Crea il lexer
        GLSLLexer lexer(&input);

        // 4. Crea il token stream
        antlr4::CommonTokenStream tokens(&lexer);

        // 5. Crea il parser
        GLSLParser parser(&tokens);

        // 6. Esegui il parsing a partire dalla regola iniziale
        // Sostituisci 'translationUnit' con la tua regola iniziale della grammatica GLSL
        auto tree = parser.translation_unit();
        // 7. Crea un visitor e visita l'albero di parsing
        MyGLSLVisitor visitor;
        visitor.visit(tree);
        visitor.printAllShaderInfo();

        // Analisi semantica
        SemanticVisitor sem;
        sem.analyze(visitor.getShaderInfo());
        sem.printDiagnostics();
        // Generazione shader
        OpenGLVisitor glVisitor(&sem);
        glVisitor.produceShaders(visitor.getShaderInfo());
        glVisitor.printFragmentShader();
        glVisitor.printVertexShader();


    } catch (std::exception& e) {
        std::cerr << "Errore: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
