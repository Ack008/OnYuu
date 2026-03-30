
// Generated from GLSLPreParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"




class  GLSLPreParser : public antlr4::Parser {
public:
  enum {
    ATOMIC_UINT = 1, ATTRIBUTE = 2, BOOL = 3, BREAK = 4, BUFFER = 5, BVEC2 = 6, 
    BVEC3 = 7, BVEC4 = 8, CASE = 9, CENTROID = 10, COHERENT = 11, CONST = 12, 
    CONTINUE = 13, DEFAULT = 14, DISCARD = 15, DMAT2 = 16, DMAT2X2 = 17, 
    DMAT2X3 = 18, DMAT2X4 = 19, DMAT3 = 20, DMAT3X2 = 21, DMAT3X3 = 22, 
    DMAT3X4 = 23, DMAT4 = 24, DMAT4X2 = 25, DMAT4X3 = 26, DMAT4X4 = 27, 
    DO = 28, DOUBLE = 29, DVEC2 = 30, DVEC3 = 31, DVEC4 = 32, ELSE = 33, 
    FALSE = 34, FLAT = 35, FLOAT = 36, FOR = 37, HIGHP = 38, IF = 39, IIMAGE1D = 40, 
    IIMAGE1DARRAY = 41, IIMAGE2D = 42, IIMAGE2DARRAY = 43, IIMAGE2DMS = 44, 
    IIMAGE2DMSARRAY = 45, IIMAGE2DRECT = 46, IIMAGE3D = 47, IIMAGEBUFFER = 48, 
    IIMAGECUBE = 49, IIMAGECUBEARRAY = 50, IMAGE1D = 51, IMAGE1DARRAY = 52, 
    IMAGE2D = 53, IMAGE2DARRAY = 54, IMAGE2DMS = 55, IMAGE2DMSARRAY = 56, 
    IMAGE2DRECT = 57, IMAGE3D = 58, IMAGEBUFFER = 59, IMAGECUBE = 60, IMAGECUBEARRAY = 61, 
    IN = 62, INOUT = 63, INT = 64, INVARIANT = 65, ISAMPLER1D = 66, ISAMPLER1DARRAY = 67, 
    ISAMPLER2D = 68, ISAMPLER2DARRAY = 69, ISAMPLER2DMS = 70, ISAMPLER2DMSARRAY = 71, 
    ISAMPLER2DRECT = 72, ISAMPLER3D = 73, ISAMPLERBUFFER = 74, ISAMPLERCUBE = 75, 
    ISAMPLERCUBEARRAY = 76, ISUBPASSINPUT = 77, ISUBPASSINPUTMS = 78, ITEXTURE1D = 79, 
    ITEXTURE1DARRAY = 80, ITEXTURE2D = 81, ITEXTURE2DARRAY = 82, ITEXTURE2DMS = 83, 
    ITEXTURE2DMSARRAY = 84, ITEXTURE2DRECT = 85, ITEXTURE3D = 86, ITEXTUREBUFFER = 87, 
    ITEXTURECUBE = 88, ITEXTURECUBEARRAY = 89, IVEC2 = 90, IVEC3 = 91, IVEC4 = 92, 
    LAYOUT = 93, LOWP = 94, MAT2 = 95, MAT2X2 = 96, MAT2X3 = 97, MAT2X4 = 98, 
    MAT3 = 99, MAT3X2 = 100, MAT3X3 = 101, MAT3X4 = 102, MAT4 = 103, MAT4X2 = 104, 
    MAT4X3 = 105, MAT4X4 = 106, MEDIUMP = 107, NOPERSPECTIVE = 108, OUT = 109, 
    PATCH = 110, PRECISE = 111, PRECISION = 112, READONLY = 113, RESTRICT = 114, 
    RETURN = 115, SAMPLE = 116, SAMPLER = 117, SAMPLER1D = 118, SAMPLER1DARRAY = 119, 
    SAMPLER1DARRAYSHADOW = 120, SAMPLER1DSHADOW = 121, SAMPLER2D = 122, 
    SAMPLER2DARRAY = 123, SAMPLER2DARRAYSHADOW = 124, SAMPLER2DMS = 125, 
    SAMPLER2DMSARRAY = 126, SAMPLER2DRECT = 127, SAMPLER2DRECTSHADOW = 128, 
    SAMPLER2DSHADOW = 129, SAMPLER3D = 130, SAMPLERBUFFER = 131, SAMPLERCUBE = 132, 
    SAMPLERCUBEARRAY = 133, SAMPLERCUBEARRAYSHADOW = 134, SAMPLERCUBESHADOW = 135, 
    SAMPLERSHADOW = 136, SHARED = 137, SMOOTH = 138, STRUCT = 139, SUBPASSINPUT = 140, 
    SUBPASSINPUTMS = 141, SUBROUTINE = 142, SWITCH = 143, TEXTURE1D = 144, 
    TEXTURE1DARRAY = 145, TEXTURE2D = 146, TEXTURE2DARRAY = 147, TEXTURE2DMS = 148, 
    TEXTURE2DMSARRAY = 149, TEXTURE2DRECT = 150, TEXTURE3D = 151, TEXTUREBUFFER = 152, 
    TEXTURECUBE = 153, TEXTURECUBEARRAY = 154, TRUE = 155, UIMAGE1D = 156, 
    UIMAGE1DARRAY = 157, UIMAGE2D = 158, UIMAGE2DARRAY = 159, UIMAGE2DMS = 160, 
    UIMAGE2DMSARRAY = 161, UIMAGE2DRECT = 162, UIMAGE3D = 163, UIMAGEBUFFER = 164, 
    UIMAGECUBE = 165, UIMAGECUBEARRAY = 166, UINT = 167, UNIFORM = 168, 
    USAMPLER1D = 169, USAMPLER1DARRAY = 170, USAMPLER2D = 171, USAMPLER2DARRAY = 172, 
    USAMPLER2DMS = 173, USAMPLER2DMSARRAY = 174, USAMPLER2DRECT = 175, USAMPLER3D = 176, 
    USAMPLERBUFFER = 177, USAMPLERCUBE = 178, USAMPLERCUBEARRAY = 179, USUBPASSINPUT = 180, 
    USUBPASSINPUTMS = 181, UTEXTURE1D = 182, UTEXTURE1DARRAY = 183, UTEXTURE2D = 184, 
    UTEXTURE2DARRAY = 185, UTEXTURE2DMS = 186, UTEXTURE2DMSARRAY = 187, 
    UTEXTURE2DRECT = 188, UTEXTURE3D = 189, UTEXTUREBUFFER = 190, UTEXTURECUBE = 191, 
    UTEXTURECUBEARRAY = 192, UVEC2 = 193, UVEC3 = 194, UVEC4 = 195, VARYING = 196, 
    VEC2 = 197, VEC3 = 198, VEC4 = 199, VOID = 200, VOLATILE = 201, WHILE = 202, 
    WRITEONLY = 203, ADD_ASSIGN = 204, AMPERSAND = 205, AND_ASSIGN = 206, 
    AND_OP = 207, BANG = 208, CARET = 209, COLON = 210, COMMA = 211, DASH = 212, 
    DEC_OP = 213, DIV_ASSIGN = 214, DOT = 215, EQ_OP = 216, EQUAL = 217, 
    GE_OP = 218, INC_OP = 219, LE_OP = 220, LEFT_ANGLE = 221, LEFT_ASSIGN = 222, 
    LEFT_BRACE = 223, LEFT_BRACKET = 224, LEFT_OP = 225, LEFT_PAREN = 226, 
    MOD_ASSIGN = 227, MUL_ASSIGN = 228, NE_OP = 229, NUMBER_SIGN = 230, 
    OR_ASSIGN = 231, OR_OP = 232, PERCENT = 233, PLUS = 234, QUESTION = 235, 
    RIGHT_ANGLE = 236, RIGHT_ASSIGN = 237, RIGHT_BRACE = 238, RIGHT_BRACKET = 239, 
    RIGHT_OP = 240, RIGHT_PAREN = 241, SEMICOLON = 242, SLASH = 243, STAR = 244, 
    SUB_ASSIGN = 245, TILDE = 246, VERTICAL_BAR = 247, XOR_ASSIGN = 248, 
    XOR_OP = 249, DOUBLECONSTANT = 250, FLOATCONSTANT = 251, INTCONSTANT = 252, 
    UINTCONSTANT = 253, BLOCK_COMMENT = 254, LINE_COMMENT = 255, LINE_CONTINUATION = 256, 
    IDENTIFIER = 257, WHITE_SPACE = 258, DEFINE_DIRECTIVE = 259, ELIF_DIRECTIVE = 260, 
    ELSE_DIRECTIVE = 261, ENDIF_DIRECTIVE = 262, ERROR_DIRECTIVE = 263, 
    EXTENSION_DIRECTIVE = 264, IF_DIRECTIVE = 265, IFDEF_DIRECTIVE = 266, 
    IFNDEF_DIRECTIVE = 267, LINE_DIRECTIVE = 268, PRAGMA_DIRECTIVE = 269, 
    UNDEF_DIRECTIVE = 270, VERSION_DIRECTIVE = 271, SPACE_TAB_0 = 272, NEWLINE_0 = 273, 
    MACRO_NAME = 274, NEWLINE_1 = 275, SPACE_TAB_1 = 276, CONSTANT_EXPRESSION = 277, 
    NEWLINE_2 = 278, ERROR_MESSAGE = 279, NEWLINE_3 = 280, BEHAVIOR = 281, 
    EXTENSION_NAME = 282, NEWLINE_4 = 283, SPACE_TAB_2 = 284, NEWLINE_5 = 285, 
    MACRO_IDENTIFIER = 286, NEWLINE_6 = 287, SPACE_TAB_3 = 288, LINE_EXPRESSION = 289, 
    NEWLINE_7 = 290, MACRO_ESC_NEWLINE = 291, MACRO_TEXT = 292, NEWLINE_8 = 293, 
    DEBUG = 294, NEWLINE_9 = 295, OFF = 296, ON = 297, OPTIMIZE = 298, SPACE_TAB_5 = 299, 
    STDGL = 300, PROGRAM_TEXT = 301, NEWLINE_10 = 302, SPACE_TAB_6 = 303, 
    NEWLINE_11 = 304, NUMBER = 305, PROFILE = 306, SPACE_TAB_7 = 307
  };

  enum {
    RuleTranslation_unit = 0, RuleCompiler_directive = 1, RuleBehavior = 2, 
    RuleConstant_expression = 3, RuleDefine_directive = 4, RuleElif_directive = 5, 
    RuleElse_directive = 6, RuleEndif_directive = 7, RuleError_directive = 8, 
    RuleError_message = 9, RuleExtension_directive = 10, RuleExtension_name = 11, 
    RuleGroup_of_lines = 12, RuleIf_directive = 13, RuleIfdef_directive = 14, 
    RuleIfndef_directive = 15, RuleLine_directive = 16, RuleLine_expression = 17, 
    RuleMacro_esc_newline = 18, RuleMacro_identifier = 19, RuleMacro_name = 20, 
    RuleMacro_text = 21, RuleMacro_text_ = 22, RuleNumber = 23, RuleOff = 24, 
    RuleOn = 25, RulePragma_debug = 26, RulePragma_directive = 27, RulePragma_optimize = 28, 
    RuleProfile = 29, RuleProgram_text = 30, RuleStdgl = 31, RuleUndef_directive = 32, 
    RuleVersion_directive = 33
  };

  explicit GLSLPreParser(antlr4::TokenStream *input);

  GLSLPreParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~GLSLPreParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class Translation_unitContext;
  class Compiler_directiveContext;
  class BehaviorContext;
  class Constant_expressionContext;
  class Define_directiveContext;
  class Elif_directiveContext;
  class Else_directiveContext;
  class Endif_directiveContext;
  class Error_directiveContext;
  class Error_messageContext;
  class Extension_directiveContext;
  class Extension_nameContext;
  class Group_of_linesContext;
  class If_directiveContext;
  class Ifdef_directiveContext;
  class Ifndef_directiveContext;
  class Line_directiveContext;
  class Line_expressionContext;
  class Macro_esc_newlineContext;
  class Macro_identifierContext;
  class Macro_nameContext;
  class Macro_textContext;
  class Macro_text_Context;
  class NumberContext;
  class OffContext;
  class OnContext;
  class Pragma_debugContext;
  class Pragma_directiveContext;
  class Pragma_optimizeContext;
  class ProfileContext;
  class Program_textContext;
  class StdglContext;
  class Undef_directiveContext;
  class Version_directiveContext; 

  class  Translation_unitContext : public antlr4::ParserRuleContext {
  public:
    Translation_unitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Compiler_directiveContext *> compiler_directive();
    Compiler_directiveContext* compiler_directive(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Translation_unitContext* translation_unit();

  class  Compiler_directiveContext : public antlr4::ParserRuleContext {
  public:
    Compiler_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Define_directiveContext *define_directive();
    Elif_directiveContext *elif_directive();
    Else_directiveContext *else_directive();
    Endif_directiveContext *endif_directive();
    Error_directiveContext *error_directive();
    Extension_directiveContext *extension_directive();
    If_directiveContext *if_directive();
    Ifdef_directiveContext *ifdef_directive();
    Ifndef_directiveContext *ifndef_directive();
    Line_directiveContext *line_directive();
    Pragma_directiveContext *pragma_directive();
    Undef_directiveContext *undef_directive();
    Version_directiveContext *version_directive();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Compiler_directiveContext* compiler_directive();

  class  BehaviorContext : public antlr4::ParserRuleContext {
  public:
    BehaviorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BEHAVIOR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BehaviorContext* behavior();

  class  Constant_expressionContext : public antlr4::ParserRuleContext {
  public:
    Constant_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONSTANT_EXPRESSION();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constant_expressionContext* constant_expression();

  class  Define_directiveContext : public antlr4::ParserRuleContext {
  public:
    Define_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *DEFINE_DIRECTIVE();
    Macro_nameContext *macro_name();
    Macro_textContext *macro_text();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Define_directiveContext* define_directive();

  class  Elif_directiveContext : public antlr4::ParserRuleContext {
  public:
    Elif_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *ELIF_DIRECTIVE();
    Constant_expressionContext *constant_expression();
    Group_of_linesContext *group_of_lines();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Elif_directiveContext* elif_directive();

  class  Else_directiveContext : public antlr4::ParserRuleContext {
  public:
    Else_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *ELSE_DIRECTIVE();
    Group_of_linesContext *group_of_lines();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Else_directiveContext* else_directive();

  class  Endif_directiveContext : public antlr4::ParserRuleContext {
  public:
    Endif_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *ENDIF_DIRECTIVE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Endif_directiveContext* endif_directive();

  class  Error_directiveContext : public antlr4::ParserRuleContext {
  public:
    Error_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *ERROR_DIRECTIVE();
    Error_messageContext *error_message();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Error_directiveContext* error_directive();

  class  Error_messageContext : public antlr4::ParserRuleContext {
  public:
    Error_messageContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ERROR_MESSAGE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Error_messageContext* error_message();

  class  Extension_directiveContext : public antlr4::ParserRuleContext {
  public:
    Extension_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *EXTENSION_DIRECTIVE();
    Extension_nameContext *extension_name();
    antlr4::tree::TerminalNode *COLON();
    BehaviorContext *behavior();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Extension_directiveContext* extension_directive();

  class  Extension_nameContext : public antlr4::ParserRuleContext {
  public:
    Extension_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EXTENSION_NAME();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Extension_nameContext* extension_name();

  class  Group_of_linesContext : public antlr4::ParserRuleContext {
  public:
    Group_of_linesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Program_textContext *> program_text();
    Program_textContext* program_text(size_t i);
    std::vector<Compiler_directiveContext *> compiler_directive();
    Compiler_directiveContext* compiler_directive(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Group_of_linesContext* group_of_lines();

  class  If_directiveContext : public antlr4::ParserRuleContext {
  public:
    If_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *IF_DIRECTIVE();
    Constant_expressionContext *constant_expression();
    Group_of_linesContext *group_of_lines();
    Endif_directiveContext *endif_directive();
    std::vector<Elif_directiveContext *> elif_directive();
    Elif_directiveContext* elif_directive(size_t i);
    Else_directiveContext *else_directive();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  If_directiveContext* if_directive();

  class  Ifdef_directiveContext : public antlr4::ParserRuleContext {
  public:
    Ifdef_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *IFDEF_DIRECTIVE();
    Macro_identifierContext *macro_identifier();
    Group_of_linesContext *group_of_lines();
    Endif_directiveContext *endif_directive();
    std::vector<Elif_directiveContext *> elif_directive();
    Elif_directiveContext* elif_directive(size_t i);
    Else_directiveContext *else_directive();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Ifdef_directiveContext* ifdef_directive();

  class  Ifndef_directiveContext : public antlr4::ParserRuleContext {
  public:
    Ifndef_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *IFNDEF_DIRECTIVE();
    Macro_identifierContext *macro_identifier();
    Group_of_linesContext *group_of_lines();
    Endif_directiveContext *endif_directive();
    std::vector<Elif_directiveContext *> elif_directive();
    Elif_directiveContext* elif_directive(size_t i);
    Else_directiveContext *else_directive();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Ifndef_directiveContext* ifndef_directive();

  class  Line_directiveContext : public antlr4::ParserRuleContext {
  public:
    Line_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *LINE_DIRECTIVE();
    Line_expressionContext *line_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Line_directiveContext* line_directive();

  class  Line_expressionContext : public antlr4::ParserRuleContext {
  public:
    Line_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LINE_EXPRESSION();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Line_expressionContext* line_expression();

  class  Macro_esc_newlineContext : public antlr4::ParserRuleContext {
  public:
    Macro_esc_newlineContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MACRO_ESC_NEWLINE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Macro_esc_newlineContext* macro_esc_newline();

  class  Macro_identifierContext : public antlr4::ParserRuleContext {
  public:
    Macro_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MACRO_IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Macro_identifierContext* macro_identifier();

  class  Macro_nameContext : public antlr4::ParserRuleContext {
  public:
    Macro_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MACRO_NAME();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Macro_nameContext* macro_name();

  class  Macro_textContext : public antlr4::ParserRuleContext {
  public:
    Macro_textContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Macro_text_Context *> macro_text_();
    Macro_text_Context* macro_text_(size_t i);
    std::vector<Macro_esc_newlineContext *> macro_esc_newline();
    Macro_esc_newlineContext* macro_esc_newline(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Macro_textContext* macro_text();

  class  Macro_text_Context : public antlr4::ParserRuleContext {
  public:
    Macro_text_Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MACRO_TEXT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Macro_text_Context* macro_text_();

  class  NumberContext : public antlr4::ParserRuleContext {
  public:
    NumberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NumberContext* number();

  class  OffContext : public antlr4::ParserRuleContext {
  public:
    OffContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OFF();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OffContext* off();

  class  OnContext : public antlr4::ParserRuleContext {
  public:
    OnContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OnContext* on();

  class  Pragma_debugContext : public antlr4::ParserRuleContext {
  public:
    Pragma_debugContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DEBUG();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    OnContext *on();
    OffContext *off();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pragma_debugContext* pragma_debug();

  class  Pragma_directiveContext : public antlr4::ParserRuleContext {
  public:
    Pragma_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *PRAGMA_DIRECTIVE();
    StdglContext *stdgl();
    Pragma_debugContext *pragma_debug();
    Pragma_optimizeContext *pragma_optimize();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pragma_directiveContext* pragma_directive();

  class  Pragma_optimizeContext : public antlr4::ParserRuleContext {
  public:
    Pragma_optimizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPTIMIZE();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    OnContext *on();
    OffContext *off();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pragma_optimizeContext* pragma_optimize();

  class  ProfileContext : public antlr4::ParserRuleContext {
  public:
    ProfileContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PROFILE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProfileContext* profile();

  class  Program_textContext : public antlr4::ParserRuleContext {
  public:
    Program_textContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PROGRAM_TEXT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Program_textContext* program_text();

  class  StdglContext : public antlr4::ParserRuleContext {
  public:
    StdglContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STDGL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StdglContext* stdgl();

  class  Undef_directiveContext : public antlr4::ParserRuleContext {
  public:
    Undef_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *UNDEF_DIRECTIVE();
    Macro_identifierContext *macro_identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Undef_directiveContext* undef_directive();

  class  Version_directiveContext : public antlr4::ParserRuleContext {
  public:
    Version_directiveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER_SIGN();
    antlr4::tree::TerminalNode *VERSION_DIRECTIVE();
    NumberContext *number();
    ProfileContext *profile();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Version_directiveContext* version_directive();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

