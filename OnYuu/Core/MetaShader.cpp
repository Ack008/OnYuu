#include "MetaShader.h"
#include "Platform/OpenGL/OpenGLMetaShader.h"
#include "Platform/Vulkan/VulkanMetaShader.h"
#include "Platform/API.h"
#include "Render/Renderer.h"
namespace OnYuu {
	MetaShader::MetaShader(const std::string& filename)
	{
		try {
			// 1. Leggi il file GLSL
			std::string shaderCode = readFile(filename);
			// 2. Crea uno stream di input ANTLR
			antlr4::ANTLRInputStream input(shaderCode);
			// 3. Crea il lexer
			GLSLLexer lexer(&input);
			// 4. Crea il token stream
			antlr4::CommonTokenStream tokens(&lexer);
			// 5. Crea il parser
			GLSLParser parser(&tokens);
			// 6. Esegui il parsing a partire dalla regola iniziale
			auto tree = parser.translation_unit();
			// 7. Crea un visitor e visita l'albero di parsing
			MyGLSLVisitor visitor;
			visitor.visit(tree);
			visitor.printAllShaderInfo();
			glslVisitor = std::move(visitor);
			// Analisi semantica
			SemanticVisitor sem;
			sem.analyze(glslVisitor.getShaderInfo());
			sem.printDiagnostics();
			code = shaderCode;
			this->sem = std::move(sem);
		}
		catch (std::exception& e) {
			std::cerr << "Errore: " << e.what() << std::endl;
		}
	}


	std::string OnYuu::MetaShader::readFile(const std::string& filename)
	{
		std::ifstream file(filename);
		if (!file) throw std::runtime_error("Impossibile aprire il file: " + filename);
		std::string content((std::istreambuf_iterator<char>(file)),
			(std::istreambuf_iterator<char>()));
		return content;
	}

	std::shared_ptr<MetaShader> OnYuu::MetaShader::create(const std::string& filename)
	{
		switch (Render::getAPI())
		{
		case API::OpenGL:
			return std::make_shared<OpenGLMetaShader>(filename);
		case API::Vulkan:
			return std::make_shared<VulkanMetaShader>(filename);
		default:
			break;
		}
		return std::make_shared<OpenGLMetaShader>(filename);
	}
}