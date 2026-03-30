
// Generated from GLSLPreParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"
#include "GLSLPreParserVisitor.h"


/**
 * This class provides an empty implementation of GLSLPreParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  GLSLPreParserBaseVisitor : public GLSLPreParserVisitor {
public:

  virtual std::any visitTranslation_unit(GLSLPreParser::Translation_unitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCompiler_directive(GLSLPreParser::Compiler_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBehavior(GLSLPreParser::BehaviorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstant_expression(GLSLPreParser::Constant_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDefine_directive(GLSLPreParser::Define_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElif_directive(GLSLPreParser::Elif_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElse_directive(GLSLPreParser::Else_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEndif_directive(GLSLPreParser::Endif_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitError_directive(GLSLPreParser::Error_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitError_message(GLSLPreParser::Error_messageContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExtension_directive(GLSLPreParser::Extension_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExtension_name(GLSLPreParser::Extension_nameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGroup_of_lines(GLSLPreParser::Group_of_linesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIf_directive(GLSLPreParser::If_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfdef_directive(GLSLPreParser::Ifdef_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIfndef_directive(GLSLPreParser::Ifndef_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLine_directive(GLSLPreParser::Line_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLine_expression(GLSLPreParser::Line_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMacro_esc_newline(GLSLPreParser::Macro_esc_newlineContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMacro_identifier(GLSLPreParser::Macro_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMacro_name(GLSLPreParser::Macro_nameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMacro_text(GLSLPreParser::Macro_textContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMacro_text_(GLSLPreParser::Macro_text_Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumber(GLSLPreParser::NumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOff(GLSLPreParser::OffContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOn(GLSLPreParser::OnContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPragma_debug(GLSLPreParser::Pragma_debugContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPragma_directive(GLSLPreParser::Pragma_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPragma_optimize(GLSLPreParser::Pragma_optimizeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProfile(GLSLPreParser::ProfileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProgram_text(GLSLPreParser::Program_textContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStdgl(GLSLPreParser::StdglContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUndef_directive(GLSLPreParser::Undef_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVersion_directive(GLSLPreParser::Version_directiveContext *ctx) override {
    return visitChildren(ctx);
  }


};

