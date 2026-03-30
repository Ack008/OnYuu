
// Generated from GLSLPreParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"
#include "GLSLPreParserListener.h"


/**
 * This class provides an empty implementation of GLSLPreParserListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  GLSLPreParserBaseListener : public GLSLPreParserListener {
public:

  virtual void enterTranslation_unit(GLSLPreParser::Translation_unitContext * /*ctx*/) override { }
  virtual void exitTranslation_unit(GLSLPreParser::Translation_unitContext * /*ctx*/) override { }

  virtual void enterCompiler_directive(GLSLPreParser::Compiler_directiveContext * /*ctx*/) override { }
  virtual void exitCompiler_directive(GLSLPreParser::Compiler_directiveContext * /*ctx*/) override { }

  virtual void enterBehavior(GLSLPreParser::BehaviorContext * /*ctx*/) override { }
  virtual void exitBehavior(GLSLPreParser::BehaviorContext * /*ctx*/) override { }

  virtual void enterConstant_expression(GLSLPreParser::Constant_expressionContext * /*ctx*/) override { }
  virtual void exitConstant_expression(GLSLPreParser::Constant_expressionContext * /*ctx*/) override { }

  virtual void enterDefine_directive(GLSLPreParser::Define_directiveContext * /*ctx*/) override { }
  virtual void exitDefine_directive(GLSLPreParser::Define_directiveContext * /*ctx*/) override { }

  virtual void enterElif_directive(GLSLPreParser::Elif_directiveContext * /*ctx*/) override { }
  virtual void exitElif_directive(GLSLPreParser::Elif_directiveContext * /*ctx*/) override { }

  virtual void enterElse_directive(GLSLPreParser::Else_directiveContext * /*ctx*/) override { }
  virtual void exitElse_directive(GLSLPreParser::Else_directiveContext * /*ctx*/) override { }

  virtual void enterEndif_directive(GLSLPreParser::Endif_directiveContext * /*ctx*/) override { }
  virtual void exitEndif_directive(GLSLPreParser::Endif_directiveContext * /*ctx*/) override { }

  virtual void enterError_directive(GLSLPreParser::Error_directiveContext * /*ctx*/) override { }
  virtual void exitError_directive(GLSLPreParser::Error_directiveContext * /*ctx*/) override { }

  virtual void enterError_message(GLSLPreParser::Error_messageContext * /*ctx*/) override { }
  virtual void exitError_message(GLSLPreParser::Error_messageContext * /*ctx*/) override { }

  virtual void enterExtension_directive(GLSLPreParser::Extension_directiveContext * /*ctx*/) override { }
  virtual void exitExtension_directive(GLSLPreParser::Extension_directiveContext * /*ctx*/) override { }

  virtual void enterExtension_name(GLSLPreParser::Extension_nameContext * /*ctx*/) override { }
  virtual void exitExtension_name(GLSLPreParser::Extension_nameContext * /*ctx*/) override { }

  virtual void enterGroup_of_lines(GLSLPreParser::Group_of_linesContext * /*ctx*/) override { }
  virtual void exitGroup_of_lines(GLSLPreParser::Group_of_linesContext * /*ctx*/) override { }

  virtual void enterIf_directive(GLSLPreParser::If_directiveContext * /*ctx*/) override { }
  virtual void exitIf_directive(GLSLPreParser::If_directiveContext * /*ctx*/) override { }

  virtual void enterIfdef_directive(GLSLPreParser::Ifdef_directiveContext * /*ctx*/) override { }
  virtual void exitIfdef_directive(GLSLPreParser::Ifdef_directiveContext * /*ctx*/) override { }

  virtual void enterIfndef_directive(GLSLPreParser::Ifndef_directiveContext * /*ctx*/) override { }
  virtual void exitIfndef_directive(GLSLPreParser::Ifndef_directiveContext * /*ctx*/) override { }

  virtual void enterLine_directive(GLSLPreParser::Line_directiveContext * /*ctx*/) override { }
  virtual void exitLine_directive(GLSLPreParser::Line_directiveContext * /*ctx*/) override { }

  virtual void enterLine_expression(GLSLPreParser::Line_expressionContext * /*ctx*/) override { }
  virtual void exitLine_expression(GLSLPreParser::Line_expressionContext * /*ctx*/) override { }

  virtual void enterMacro_esc_newline(GLSLPreParser::Macro_esc_newlineContext * /*ctx*/) override { }
  virtual void exitMacro_esc_newline(GLSLPreParser::Macro_esc_newlineContext * /*ctx*/) override { }

  virtual void enterMacro_identifier(GLSLPreParser::Macro_identifierContext * /*ctx*/) override { }
  virtual void exitMacro_identifier(GLSLPreParser::Macro_identifierContext * /*ctx*/) override { }

  virtual void enterMacro_name(GLSLPreParser::Macro_nameContext * /*ctx*/) override { }
  virtual void exitMacro_name(GLSLPreParser::Macro_nameContext * /*ctx*/) override { }

  virtual void enterMacro_text(GLSLPreParser::Macro_textContext * /*ctx*/) override { }
  virtual void exitMacro_text(GLSLPreParser::Macro_textContext * /*ctx*/) override { }

  virtual void enterMacro_text_(GLSLPreParser::Macro_text_Context * /*ctx*/) override { }
  virtual void exitMacro_text_(GLSLPreParser::Macro_text_Context * /*ctx*/) override { }

  virtual void enterNumber(GLSLPreParser::NumberContext * /*ctx*/) override { }
  virtual void exitNumber(GLSLPreParser::NumberContext * /*ctx*/) override { }

  virtual void enterOff(GLSLPreParser::OffContext * /*ctx*/) override { }
  virtual void exitOff(GLSLPreParser::OffContext * /*ctx*/) override { }

  virtual void enterOn(GLSLPreParser::OnContext * /*ctx*/) override { }
  virtual void exitOn(GLSLPreParser::OnContext * /*ctx*/) override { }

  virtual void enterPragma_debug(GLSLPreParser::Pragma_debugContext * /*ctx*/) override { }
  virtual void exitPragma_debug(GLSLPreParser::Pragma_debugContext * /*ctx*/) override { }

  virtual void enterPragma_directive(GLSLPreParser::Pragma_directiveContext * /*ctx*/) override { }
  virtual void exitPragma_directive(GLSLPreParser::Pragma_directiveContext * /*ctx*/) override { }

  virtual void enterPragma_optimize(GLSLPreParser::Pragma_optimizeContext * /*ctx*/) override { }
  virtual void exitPragma_optimize(GLSLPreParser::Pragma_optimizeContext * /*ctx*/) override { }

  virtual void enterProfile(GLSLPreParser::ProfileContext * /*ctx*/) override { }
  virtual void exitProfile(GLSLPreParser::ProfileContext * /*ctx*/) override { }

  virtual void enterProgram_text(GLSLPreParser::Program_textContext * /*ctx*/) override { }
  virtual void exitProgram_text(GLSLPreParser::Program_textContext * /*ctx*/) override { }

  virtual void enterStdgl(GLSLPreParser::StdglContext * /*ctx*/) override { }
  virtual void exitStdgl(GLSLPreParser::StdglContext * /*ctx*/) override { }

  virtual void enterUndef_directive(GLSLPreParser::Undef_directiveContext * /*ctx*/) override { }
  virtual void exitUndef_directive(GLSLPreParser::Undef_directiveContext * /*ctx*/) override { }

  virtual void enterVersion_directive(GLSLPreParser::Version_directiveContext * /*ctx*/) override { }
  virtual void exitVersion_directive(GLSLPreParser::Version_directiveContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

