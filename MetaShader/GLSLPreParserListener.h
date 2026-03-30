
// Generated from GLSLPreParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"
#include "GLSLPreParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by GLSLPreParser.
 */
class  GLSLPreParserListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterTranslation_unit(GLSLPreParser::Translation_unitContext *ctx) = 0;
  virtual void exitTranslation_unit(GLSLPreParser::Translation_unitContext *ctx) = 0;

  virtual void enterCompiler_directive(GLSLPreParser::Compiler_directiveContext *ctx) = 0;
  virtual void exitCompiler_directive(GLSLPreParser::Compiler_directiveContext *ctx) = 0;

  virtual void enterBehavior(GLSLPreParser::BehaviorContext *ctx) = 0;
  virtual void exitBehavior(GLSLPreParser::BehaviorContext *ctx) = 0;

  virtual void enterConstant_expression(GLSLPreParser::Constant_expressionContext *ctx) = 0;
  virtual void exitConstant_expression(GLSLPreParser::Constant_expressionContext *ctx) = 0;

  virtual void enterDefine_directive(GLSLPreParser::Define_directiveContext *ctx) = 0;
  virtual void exitDefine_directive(GLSLPreParser::Define_directiveContext *ctx) = 0;

  virtual void enterElif_directive(GLSLPreParser::Elif_directiveContext *ctx) = 0;
  virtual void exitElif_directive(GLSLPreParser::Elif_directiveContext *ctx) = 0;

  virtual void enterElse_directive(GLSLPreParser::Else_directiveContext *ctx) = 0;
  virtual void exitElse_directive(GLSLPreParser::Else_directiveContext *ctx) = 0;

  virtual void enterEndif_directive(GLSLPreParser::Endif_directiveContext *ctx) = 0;
  virtual void exitEndif_directive(GLSLPreParser::Endif_directiveContext *ctx) = 0;

  virtual void enterError_directive(GLSLPreParser::Error_directiveContext *ctx) = 0;
  virtual void exitError_directive(GLSLPreParser::Error_directiveContext *ctx) = 0;

  virtual void enterError_message(GLSLPreParser::Error_messageContext *ctx) = 0;
  virtual void exitError_message(GLSLPreParser::Error_messageContext *ctx) = 0;

  virtual void enterExtension_directive(GLSLPreParser::Extension_directiveContext *ctx) = 0;
  virtual void exitExtension_directive(GLSLPreParser::Extension_directiveContext *ctx) = 0;

  virtual void enterExtension_name(GLSLPreParser::Extension_nameContext *ctx) = 0;
  virtual void exitExtension_name(GLSLPreParser::Extension_nameContext *ctx) = 0;

  virtual void enterGroup_of_lines(GLSLPreParser::Group_of_linesContext *ctx) = 0;
  virtual void exitGroup_of_lines(GLSLPreParser::Group_of_linesContext *ctx) = 0;

  virtual void enterIf_directive(GLSLPreParser::If_directiveContext *ctx) = 0;
  virtual void exitIf_directive(GLSLPreParser::If_directiveContext *ctx) = 0;

  virtual void enterIfdef_directive(GLSLPreParser::Ifdef_directiveContext *ctx) = 0;
  virtual void exitIfdef_directive(GLSLPreParser::Ifdef_directiveContext *ctx) = 0;

  virtual void enterIfndef_directive(GLSLPreParser::Ifndef_directiveContext *ctx) = 0;
  virtual void exitIfndef_directive(GLSLPreParser::Ifndef_directiveContext *ctx) = 0;

  virtual void enterLine_directive(GLSLPreParser::Line_directiveContext *ctx) = 0;
  virtual void exitLine_directive(GLSLPreParser::Line_directiveContext *ctx) = 0;

  virtual void enterLine_expression(GLSLPreParser::Line_expressionContext *ctx) = 0;
  virtual void exitLine_expression(GLSLPreParser::Line_expressionContext *ctx) = 0;

  virtual void enterMacro_esc_newline(GLSLPreParser::Macro_esc_newlineContext *ctx) = 0;
  virtual void exitMacro_esc_newline(GLSLPreParser::Macro_esc_newlineContext *ctx) = 0;

  virtual void enterMacro_identifier(GLSLPreParser::Macro_identifierContext *ctx) = 0;
  virtual void exitMacro_identifier(GLSLPreParser::Macro_identifierContext *ctx) = 0;

  virtual void enterMacro_name(GLSLPreParser::Macro_nameContext *ctx) = 0;
  virtual void exitMacro_name(GLSLPreParser::Macro_nameContext *ctx) = 0;

  virtual void enterMacro_text(GLSLPreParser::Macro_textContext *ctx) = 0;
  virtual void exitMacro_text(GLSLPreParser::Macro_textContext *ctx) = 0;

  virtual void enterMacro_text_(GLSLPreParser::Macro_text_Context *ctx) = 0;
  virtual void exitMacro_text_(GLSLPreParser::Macro_text_Context *ctx) = 0;

  virtual void enterNumber(GLSLPreParser::NumberContext *ctx) = 0;
  virtual void exitNumber(GLSLPreParser::NumberContext *ctx) = 0;

  virtual void enterOff(GLSLPreParser::OffContext *ctx) = 0;
  virtual void exitOff(GLSLPreParser::OffContext *ctx) = 0;

  virtual void enterOn(GLSLPreParser::OnContext *ctx) = 0;
  virtual void exitOn(GLSLPreParser::OnContext *ctx) = 0;

  virtual void enterPragma_debug(GLSLPreParser::Pragma_debugContext *ctx) = 0;
  virtual void exitPragma_debug(GLSLPreParser::Pragma_debugContext *ctx) = 0;

  virtual void enterPragma_directive(GLSLPreParser::Pragma_directiveContext *ctx) = 0;
  virtual void exitPragma_directive(GLSLPreParser::Pragma_directiveContext *ctx) = 0;

  virtual void enterPragma_optimize(GLSLPreParser::Pragma_optimizeContext *ctx) = 0;
  virtual void exitPragma_optimize(GLSLPreParser::Pragma_optimizeContext *ctx) = 0;

  virtual void enterProfile(GLSLPreParser::ProfileContext *ctx) = 0;
  virtual void exitProfile(GLSLPreParser::ProfileContext *ctx) = 0;

  virtual void enterProgram_text(GLSLPreParser::Program_textContext *ctx) = 0;
  virtual void exitProgram_text(GLSLPreParser::Program_textContext *ctx) = 0;

  virtual void enterStdgl(GLSLPreParser::StdglContext *ctx) = 0;
  virtual void exitStdgl(GLSLPreParser::StdglContext *ctx) = 0;

  virtual void enterUndef_directive(GLSLPreParser::Undef_directiveContext *ctx) = 0;
  virtual void exitUndef_directive(GLSLPreParser::Undef_directiveContext *ctx) = 0;

  virtual void enterVersion_directive(GLSLPreParser::Version_directiveContext *ctx) = 0;
  virtual void exitVersion_directive(GLSLPreParser::Version_directiveContext *ctx) = 0;


};

