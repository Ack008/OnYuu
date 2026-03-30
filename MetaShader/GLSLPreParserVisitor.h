
// Generated from GLSLPreParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"
#include "GLSLPreParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by GLSLPreParser.
 */
class  GLSLPreParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by GLSLPreParser.
   */
    virtual std::any visitTranslation_unit(GLSLPreParser::Translation_unitContext *context) = 0;

    virtual std::any visitCompiler_directive(GLSLPreParser::Compiler_directiveContext *context) = 0;

    virtual std::any visitBehavior(GLSLPreParser::BehaviorContext *context) = 0;

    virtual std::any visitConstant_expression(GLSLPreParser::Constant_expressionContext *context) = 0;

    virtual std::any visitDefine_directive(GLSLPreParser::Define_directiveContext *context) = 0;

    virtual std::any visitElif_directive(GLSLPreParser::Elif_directiveContext *context) = 0;

    virtual std::any visitElse_directive(GLSLPreParser::Else_directiveContext *context) = 0;

    virtual std::any visitEndif_directive(GLSLPreParser::Endif_directiveContext *context) = 0;

    virtual std::any visitError_directive(GLSLPreParser::Error_directiveContext *context) = 0;

    virtual std::any visitError_message(GLSLPreParser::Error_messageContext *context) = 0;

    virtual std::any visitExtension_directive(GLSLPreParser::Extension_directiveContext *context) = 0;

    virtual std::any visitExtension_name(GLSLPreParser::Extension_nameContext *context) = 0;

    virtual std::any visitGroup_of_lines(GLSLPreParser::Group_of_linesContext *context) = 0;

    virtual std::any visitIf_directive(GLSLPreParser::If_directiveContext *context) = 0;

    virtual std::any visitIfdef_directive(GLSLPreParser::Ifdef_directiveContext *context) = 0;

    virtual std::any visitIfndef_directive(GLSLPreParser::Ifndef_directiveContext *context) = 0;

    virtual std::any visitLine_directive(GLSLPreParser::Line_directiveContext *context) = 0;

    virtual std::any visitLine_expression(GLSLPreParser::Line_expressionContext *context) = 0;

    virtual std::any visitMacro_esc_newline(GLSLPreParser::Macro_esc_newlineContext *context) = 0;

    virtual std::any visitMacro_identifier(GLSLPreParser::Macro_identifierContext *context) = 0;

    virtual std::any visitMacro_name(GLSLPreParser::Macro_nameContext *context) = 0;

    virtual std::any visitMacro_text(GLSLPreParser::Macro_textContext *context) = 0;

    virtual std::any visitMacro_text_(GLSLPreParser::Macro_text_Context *context) = 0;

    virtual std::any visitNumber(GLSLPreParser::NumberContext *context) = 0;

    virtual std::any visitOff(GLSLPreParser::OffContext *context) = 0;

    virtual std::any visitOn(GLSLPreParser::OnContext *context) = 0;

    virtual std::any visitPragma_debug(GLSLPreParser::Pragma_debugContext *context) = 0;

    virtual std::any visitPragma_directive(GLSLPreParser::Pragma_directiveContext *context) = 0;

    virtual std::any visitPragma_optimize(GLSLPreParser::Pragma_optimizeContext *context) = 0;

    virtual std::any visitProfile(GLSLPreParser::ProfileContext *context) = 0;

    virtual std::any visitProgram_text(GLSLPreParser::Program_textContext *context) = 0;

    virtual std::any visitStdgl(GLSLPreParser::StdglContext *context) = 0;

    virtual std::any visitUndef_directive(GLSLPreParser::Undef_directiveContext *context) = 0;

    virtual std::any visitVersion_directive(GLSLPreParser::Version_directiveContext *context) = 0;


};

