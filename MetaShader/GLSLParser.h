
// Generated from GLSLParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"




class  GLSLParser : public antlr4::Parser {
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
    RuleTranslation_unit = 0, RuleVariable_identifier = 1, RulePrimary_expression = 2, 
    RulePostfix_expression = 3, RuleField_selection = 4, RuleInteger_expression = 5, 
    RuleFunction_call = 6, RuleFunction_identifier = 7, RuleFunction_call_parameters = 8, 
    RuleUnary_expression = 9, RuleUnary_operator = 10, RuleAssignment_expression = 11, 
    RuleAssignment_operator = 12, RuleBinary_expression = 13, RuleExpression = 14, 
    RuleConstant_expression = 15, RuleDeclaration = 16, RuleIdentifier_list = 17, 
    RuleFunction_prototype = 18, RuleFunction_parameters = 19, RuleParameter_declarator = 20, 
    RuleParameter_declaration = 21, RuleParameter_type_specifier = 22, RuleInit_declarator_list = 23, 
    RuleSingle_declaration = 24, RuleTypeless_declaration = 25, RuleFully_specified_type = 26, 
    RuleInvariant_qualifier = 27, RuleInterpolation_qualifier = 28, RuleLayout_qualifier = 29, 
    RuleLayout_qualifier_id_list = 30, RuleLayout_qualifier_id = 31, RulePrecise_qualifier = 32, 
    RuleType_qualifier = 33, RuleSingle_type_qualifier = 34, RuleStorage_qualifier = 35, 
    RuleType_name_list = 36, RuleType_name = 37, RuleType_specifier = 38, 
    RuleArray_specifier = 39, RuleDimension = 40, RuleType_specifier_nonarray = 41, 
    RulePrecision_qualifier = 42, RuleStruct_specifier = 43, RuleStruct_declaration_list = 44, 
    RuleStruct_declaration = 45, RuleStruct_declarator_list = 46, RuleStruct_declarator = 47, 
    RuleInitializer = 48, RuleInitializer_list = 49, RuleDeclaration_statement = 50, 
    RuleStatement = 51, RuleSimple_statement = 52, RuleCompound_statement = 53, 
    RuleStatement_no_new_scope = 54, RuleCompound_statement_no_new_scope = 55, 
    RuleStatement_list = 56, RuleExpression_statement = 57, RuleSelection_statement = 58, 
    RuleSelection_rest_statement = 59, RuleCondition = 60, RuleSwitch_statement = 61, 
    RuleCase_label = 62, RuleIteration_statement = 63, RuleFor_init_statement = 64, 
    RuleFor_rest_statement = 65, RuleJump_statement = 66, RuleExternal_declaration = 67, 
    RuleFunction_definition = 68
  };

  explicit GLSLParser(antlr4::TokenStream *input);

  GLSLParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~GLSLParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class Translation_unitContext;
  class Variable_identifierContext;
  class Primary_expressionContext;
  class Postfix_expressionContext;
  class Field_selectionContext;
  class Integer_expressionContext;
  class Function_callContext;
  class Function_identifierContext;
  class Function_call_parametersContext;
  class Unary_expressionContext;
  class Unary_operatorContext;
  class Assignment_expressionContext;
  class Assignment_operatorContext;
  class Binary_expressionContext;
  class ExpressionContext;
  class Constant_expressionContext;
  class DeclarationContext;
  class Identifier_listContext;
  class Function_prototypeContext;
  class Function_parametersContext;
  class Parameter_declaratorContext;
  class Parameter_declarationContext;
  class Parameter_type_specifierContext;
  class Init_declarator_listContext;
  class Single_declarationContext;
  class Typeless_declarationContext;
  class Fully_specified_typeContext;
  class Invariant_qualifierContext;
  class Interpolation_qualifierContext;
  class Layout_qualifierContext;
  class Layout_qualifier_id_listContext;
  class Layout_qualifier_idContext;
  class Precise_qualifierContext;
  class Type_qualifierContext;
  class Single_type_qualifierContext;
  class Storage_qualifierContext;
  class Type_name_listContext;
  class Type_nameContext;
  class Type_specifierContext;
  class Array_specifierContext;
  class DimensionContext;
  class Type_specifier_nonarrayContext;
  class Precision_qualifierContext;
  class Struct_specifierContext;
  class Struct_declaration_listContext;
  class Struct_declarationContext;
  class Struct_declarator_listContext;
  class Struct_declaratorContext;
  class InitializerContext;
  class Initializer_listContext;
  class Declaration_statementContext;
  class StatementContext;
  class Simple_statementContext;
  class Compound_statementContext;
  class Statement_no_new_scopeContext;
  class Compound_statement_no_new_scopeContext;
  class Statement_listContext;
  class Expression_statementContext;
  class Selection_statementContext;
  class Selection_rest_statementContext;
  class ConditionContext;
  class Switch_statementContext;
  class Case_labelContext;
  class Iteration_statementContext;
  class For_init_statementContext;
  class For_rest_statementContext;
  class Jump_statementContext;
  class External_declarationContext;
  class Function_definitionContext; 

  class  Translation_unitContext : public antlr4::ParserRuleContext {
  public:
    Translation_unitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<External_declarationContext *> external_declaration();
    External_declarationContext* external_declaration(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Translation_unitContext* translation_unit();

  class  Variable_identifierContext : public antlr4::ParserRuleContext {
  public:
    Variable_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Variable_identifierContext* variable_identifier();

  class  Primary_expressionContext : public antlr4::ParserRuleContext {
  public:
    Primary_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Variable_identifierContext *variable_identifier();
    antlr4::tree::TerminalNode *TRUE();
    antlr4::tree::TerminalNode *FALSE();
    antlr4::tree::TerminalNode *INTCONSTANT();
    antlr4::tree::TerminalNode *UINTCONSTANT();
    antlr4::tree::TerminalNode *FLOATCONSTANT();
    antlr4::tree::TerminalNode *DOUBLECONSTANT();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RIGHT_PAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Primary_expressionContext* primary_expression();

  class  Postfix_expressionContext : public antlr4::ParserRuleContext {
  public:
    Postfix_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Primary_expressionContext *primary_expression();
    Type_specifierContext *type_specifier();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    Function_call_parametersContext *function_call_parameters();
    Postfix_expressionContext *postfix_expression();
    antlr4::tree::TerminalNode *LEFT_BRACKET();
    Integer_expressionContext *integer_expression();
    antlr4::tree::TerminalNode *RIGHT_BRACKET();
    antlr4::tree::TerminalNode *DOT();
    Field_selectionContext *field_selection();
    antlr4::tree::TerminalNode *INC_OP();
    antlr4::tree::TerminalNode *DEC_OP();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Postfix_expressionContext* postfix_expression();
  Postfix_expressionContext* postfix_expression(int precedence);
  class  Field_selectionContext : public antlr4::ParserRuleContext {
  public:
    Field_selectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Variable_identifierContext *variable_identifier();
    Function_callContext *function_call();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Field_selectionContext* field_selection();

  class  Integer_expressionContext : public antlr4::ParserRuleContext {
  public:
    Integer_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Integer_expressionContext* integer_expression();

  class  Function_callContext : public antlr4::ParserRuleContext {
  public:
    Function_callContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_identifierContext *function_identifier();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    Function_call_parametersContext *function_call_parameters();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_callContext* function_call();

  class  Function_identifierContext : public antlr4::ParserRuleContext {
  public:
    Function_identifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_specifierContext *type_specifier();
    Postfix_expressionContext *postfix_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_identifierContext* function_identifier();

  class  Function_call_parametersContext : public antlr4::ParserRuleContext {
  public:
    Function_call_parametersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Assignment_expressionContext *> assignment_expression();
    Assignment_expressionContext* assignment_expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *VOID();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_call_parametersContext* function_call_parameters();

  class  Unary_expressionContext : public antlr4::ParserRuleContext {
  public:
    Unary_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Postfix_expressionContext *postfix_expression();
    antlr4::tree::TerminalNode *INC_OP();
    Unary_expressionContext *unary_expression();
    antlr4::tree::TerminalNode *DEC_OP();
    Unary_operatorContext *unary_operator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Unary_expressionContext* unary_expression();

  class  Unary_operatorContext : public antlr4::ParserRuleContext {
  public:
    Unary_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *DASH();
    antlr4::tree::TerminalNode *BANG();
    antlr4::tree::TerminalNode *TILDE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Unary_operatorContext* unary_operator();

  class  Assignment_expressionContext : public antlr4::ParserRuleContext {
  public:
    Assignment_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_expressionContext *constant_expression();
    Unary_expressionContext *unary_expression();
    Assignment_operatorContext *assignment_operator();
    Assignment_expressionContext *assignment_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assignment_expressionContext* assignment_expression();

  class  Assignment_operatorContext : public antlr4::ParserRuleContext {
  public:
    Assignment_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQUAL();
    antlr4::tree::TerminalNode *MUL_ASSIGN();
    antlr4::tree::TerminalNode *DIV_ASSIGN();
    antlr4::tree::TerminalNode *MOD_ASSIGN();
    antlr4::tree::TerminalNode *ADD_ASSIGN();
    antlr4::tree::TerminalNode *SUB_ASSIGN();
    antlr4::tree::TerminalNode *LEFT_ASSIGN();
    antlr4::tree::TerminalNode *RIGHT_ASSIGN();
    antlr4::tree::TerminalNode *AND_ASSIGN();
    antlr4::tree::TerminalNode *XOR_ASSIGN();
    antlr4::tree::TerminalNode *OR_ASSIGN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assignment_operatorContext* assignment_operator();

  class  Binary_expressionContext : public antlr4::ParserRuleContext {
  public:
    Binary_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Unary_expressionContext *unary_expression();
    std::vector<Binary_expressionContext *> binary_expression();
    Binary_expressionContext* binary_expression(size_t i);
    antlr4::tree::TerminalNode *STAR();
    antlr4::tree::TerminalNode *SLASH();
    antlr4::tree::TerminalNode *PERCENT();
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *DASH();
    antlr4::tree::TerminalNode *LEFT_OP();
    antlr4::tree::TerminalNode *RIGHT_OP();
    antlr4::tree::TerminalNode *LEFT_ANGLE();
    antlr4::tree::TerminalNode *RIGHT_ANGLE();
    antlr4::tree::TerminalNode *LE_OP();
    antlr4::tree::TerminalNode *GE_OP();
    antlr4::tree::TerminalNode *EQ_OP();
    antlr4::tree::TerminalNode *NE_OP();
    antlr4::tree::TerminalNode *AMPERSAND();
    antlr4::tree::TerminalNode *CARET();
    antlr4::tree::TerminalNode *VERTICAL_BAR();
    antlr4::tree::TerminalNode *AND_OP();
    antlr4::tree::TerminalNode *XOR_OP();
    antlr4::tree::TerminalNode *OR_OP();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Binary_expressionContext* binary_expression();
  Binary_expressionContext* binary_expression(int precedence);
  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Assignment_expressionContext *assignment_expression();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionContext* expression();
  ExpressionContext* expression(int precedence);
  class  Constant_expressionContext : public antlr4::ParserRuleContext {
  public:
    Constant_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Binary_expressionContext *binary_expression();
    antlr4::tree::TerminalNode *QUESTION();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *COLON();
    Assignment_expressionContext *assignment_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constant_expressionContext* constant_expression();

  class  DeclarationContext : public antlr4::ParserRuleContext {
  public:
    DeclarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_prototypeContext *function_prototype();
    antlr4::tree::TerminalNode *SEMICOLON();
    Init_declarator_listContext *init_declarator_list();
    antlr4::tree::TerminalNode *PRECISION();
    Precision_qualifierContext *precision_qualifier();
    Type_specifierContext *type_specifier();
    Type_qualifierContext *type_qualifier();
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    antlr4::tree::TerminalNode *LEFT_BRACE();
    Struct_declaration_listContext *struct_declaration_list();
    antlr4::tree::TerminalNode *RIGHT_BRACE();
    Array_specifierContext *array_specifier();
    Identifier_listContext *identifier_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DeclarationContext* declaration();

  class  Identifier_listContext : public antlr4::ParserRuleContext {
  public:
    Identifier_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> IDENTIFIER();
    antlr4::tree::TerminalNode* IDENTIFIER(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Identifier_listContext* identifier_list();

  class  Function_prototypeContext : public antlr4::ParserRuleContext {
  public:
    Function_prototypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Fully_specified_typeContext *fully_specified_type();
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    Function_parametersContext *function_parameters();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_prototypeContext* function_prototype();

  class  Function_parametersContext : public antlr4::ParserRuleContext {
  public:
    Function_parametersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Parameter_declarationContext *> parameter_declaration();
    Parameter_declarationContext* parameter_declaration(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_parametersContext* function_parameters();

  class  Parameter_declaratorContext : public antlr4::ParserRuleContext {
  public:
    Parameter_declaratorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_specifierContext *type_specifier();
    antlr4::tree::TerminalNode *IDENTIFIER();
    Array_specifierContext *array_specifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parameter_declaratorContext* parameter_declarator();

  class  Parameter_declarationContext : public antlr4::ParserRuleContext {
  public:
    Parameter_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_qualifierContext *type_qualifier();
    Parameter_declaratorContext *parameter_declarator();
    Parameter_type_specifierContext *parameter_type_specifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parameter_declarationContext* parameter_declaration();

  class  Parameter_type_specifierContext : public antlr4::ParserRuleContext {
  public:
    Parameter_type_specifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_specifierContext *type_specifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parameter_type_specifierContext* parameter_type_specifier();

  class  Init_declarator_listContext : public antlr4::ParserRuleContext {
  public:
    Init_declarator_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Single_declarationContext *single_declaration();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<Typeless_declarationContext *> typeless_declaration();
    Typeless_declarationContext* typeless_declaration(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Init_declarator_listContext* init_declarator_list();

  class  Single_declarationContext : public antlr4::ParserRuleContext {
  public:
    Single_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Fully_specified_typeContext *fully_specified_type();
    Typeless_declarationContext *typeless_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Single_declarationContext* single_declaration();

  class  Typeless_declarationContext : public antlr4::ParserRuleContext {
  public:
    Typeless_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    Array_specifierContext *array_specifier();
    antlr4::tree::TerminalNode *EQUAL();
    InitializerContext *initializer();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Typeless_declarationContext* typeless_declaration();

  class  Fully_specified_typeContext : public antlr4::ParserRuleContext {
  public:
    Fully_specified_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_specifierContext *type_specifier();
    Type_qualifierContext *type_qualifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Fully_specified_typeContext* fully_specified_type();

  class  Invariant_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Invariant_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INVARIANT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Invariant_qualifierContext* invariant_qualifier();

  class  Interpolation_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Interpolation_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SMOOTH();
    antlr4::tree::TerminalNode *FLAT();
    antlr4::tree::TerminalNode *NOPERSPECTIVE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interpolation_qualifierContext* interpolation_qualifier();

  class  Layout_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Layout_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LAYOUT();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    Layout_qualifier_id_listContext *layout_qualifier_id_list();
    antlr4::tree::TerminalNode *RIGHT_PAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Layout_qualifierContext* layout_qualifier();

  class  Layout_qualifier_id_listContext : public antlr4::ParserRuleContext {
  public:
    Layout_qualifier_id_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Layout_qualifier_idContext *> layout_qualifier_id();
    Layout_qualifier_idContext* layout_qualifier_id(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Layout_qualifier_id_listContext* layout_qualifier_id_list();

  class  Layout_qualifier_idContext : public antlr4::ParserRuleContext {
  public:
    Layout_qualifier_idContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *EQUAL();
    Constant_expressionContext *constant_expression();
    antlr4::tree::TerminalNode *SHARED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Layout_qualifier_idContext* layout_qualifier_id();

  class  Precise_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Precise_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PRECISE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Precise_qualifierContext* precise_qualifier();

  class  Type_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Type_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Single_type_qualifierContext *> single_type_qualifier();
    Single_type_qualifierContext* single_type_qualifier(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_qualifierContext* type_qualifier();

  class  Single_type_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Single_type_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Storage_qualifierContext *storage_qualifier();
    Layout_qualifierContext *layout_qualifier();
    Precision_qualifierContext *precision_qualifier();
    Interpolation_qualifierContext *interpolation_qualifier();
    Invariant_qualifierContext *invariant_qualifier();
    Precise_qualifierContext *precise_qualifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Single_type_qualifierContext* single_type_qualifier();

  class  Storage_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Storage_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONST();
    antlr4::tree::TerminalNode *IN();
    antlr4::tree::TerminalNode *OUT();
    antlr4::tree::TerminalNode *INOUT();
    antlr4::tree::TerminalNode *CENTROID();
    antlr4::tree::TerminalNode *PATCH();
    antlr4::tree::TerminalNode *SAMPLE();
    antlr4::tree::TerminalNode *UNIFORM();
    antlr4::tree::TerminalNode *BUFFER();
    antlr4::tree::TerminalNode *SHARED();
    antlr4::tree::TerminalNode *COHERENT();
    antlr4::tree::TerminalNode *VOLATILE();
    antlr4::tree::TerminalNode *RESTRICT();
    antlr4::tree::TerminalNode *READONLY();
    antlr4::tree::TerminalNode *WRITEONLY();
    antlr4::tree::TerminalNode *SUBROUTINE();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    Type_name_listContext *type_name_list();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    antlr4::tree::TerminalNode *ATTRIBUTE();
    antlr4::tree::TerminalNode *VARYING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Storage_qualifierContext* storage_qualifier();

  class  Type_name_listContext : public antlr4::ParserRuleContext {
  public:
    Type_name_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Type_nameContext *> type_name();
    Type_nameContext* type_name(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_name_listContext* type_name_list();

  class  Type_nameContext : public antlr4::ParserRuleContext {
  public:
    Type_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_nameContext* type_name();

  class  Type_specifierContext : public antlr4::ParserRuleContext {
  public:
    Type_specifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_specifier_nonarrayContext *type_specifier_nonarray();
    Array_specifierContext *array_specifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_specifierContext* type_specifier();

  class  Array_specifierContext : public antlr4::ParserRuleContext {
  public:
    Array_specifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<DimensionContext *> dimension();
    DimensionContext* dimension(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Array_specifierContext* array_specifier();

  class  DimensionContext : public antlr4::ParserRuleContext {
  public:
    DimensionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LEFT_BRACKET();
    antlr4::tree::TerminalNode *RIGHT_BRACKET();
    Constant_expressionContext *constant_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DimensionContext* dimension();

  class  Type_specifier_nonarrayContext : public antlr4::ParserRuleContext {
  public:
    Type_specifier_nonarrayContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VOID();
    antlr4::tree::TerminalNode *FLOAT();
    antlr4::tree::TerminalNode *DOUBLE();
    antlr4::tree::TerminalNode *INT();
    antlr4::tree::TerminalNode *UINT();
    antlr4::tree::TerminalNode *BOOL();
    antlr4::tree::TerminalNode *VEC2();
    antlr4::tree::TerminalNode *VEC3();
    antlr4::tree::TerminalNode *VEC4();
    antlr4::tree::TerminalNode *DVEC2();
    antlr4::tree::TerminalNode *DVEC3();
    antlr4::tree::TerminalNode *DVEC4();
    antlr4::tree::TerminalNode *BVEC2();
    antlr4::tree::TerminalNode *BVEC3();
    antlr4::tree::TerminalNode *BVEC4();
    antlr4::tree::TerminalNode *IVEC2();
    antlr4::tree::TerminalNode *IVEC3();
    antlr4::tree::TerminalNode *IVEC4();
    antlr4::tree::TerminalNode *UVEC2();
    antlr4::tree::TerminalNode *UVEC3();
    antlr4::tree::TerminalNode *UVEC4();
    antlr4::tree::TerminalNode *MAT2();
    antlr4::tree::TerminalNode *MAT3();
    antlr4::tree::TerminalNode *MAT4();
    antlr4::tree::TerminalNode *MAT2X2();
    antlr4::tree::TerminalNode *MAT2X3();
    antlr4::tree::TerminalNode *MAT2X4();
    antlr4::tree::TerminalNode *MAT3X2();
    antlr4::tree::TerminalNode *MAT3X3();
    antlr4::tree::TerminalNode *MAT3X4();
    antlr4::tree::TerminalNode *MAT4X2();
    antlr4::tree::TerminalNode *MAT4X3();
    antlr4::tree::TerminalNode *MAT4X4();
    antlr4::tree::TerminalNode *DMAT2();
    antlr4::tree::TerminalNode *DMAT3();
    antlr4::tree::TerminalNode *DMAT4();
    antlr4::tree::TerminalNode *DMAT2X2();
    antlr4::tree::TerminalNode *DMAT2X3();
    antlr4::tree::TerminalNode *DMAT2X4();
    antlr4::tree::TerminalNode *DMAT3X2();
    antlr4::tree::TerminalNode *DMAT3X3();
    antlr4::tree::TerminalNode *DMAT3X4();
    antlr4::tree::TerminalNode *DMAT4X2();
    antlr4::tree::TerminalNode *DMAT4X3();
    antlr4::tree::TerminalNode *DMAT4X4();
    antlr4::tree::TerminalNode *ATOMIC_UINT();
    antlr4::tree::TerminalNode *SAMPLER2D();
    antlr4::tree::TerminalNode *SAMPLER3D();
    antlr4::tree::TerminalNode *SAMPLERCUBE();
    antlr4::tree::TerminalNode *SAMPLER2DSHADOW();
    antlr4::tree::TerminalNode *SAMPLERCUBESHADOW();
    antlr4::tree::TerminalNode *SAMPLER2DARRAY();
    antlr4::tree::TerminalNode *SAMPLER2DARRAYSHADOW();
    antlr4::tree::TerminalNode *SAMPLERCUBEARRAY();
    antlr4::tree::TerminalNode *SAMPLERCUBEARRAYSHADOW();
    antlr4::tree::TerminalNode *ISAMPLER2D();
    antlr4::tree::TerminalNode *ISAMPLER3D();
    antlr4::tree::TerminalNode *ISAMPLERCUBE();
    antlr4::tree::TerminalNode *ISAMPLER2DARRAY();
    antlr4::tree::TerminalNode *ISAMPLERCUBEARRAY();
    antlr4::tree::TerminalNode *USAMPLER2D();
    antlr4::tree::TerminalNode *USAMPLER3D();
    antlr4::tree::TerminalNode *USAMPLERCUBE();
    antlr4::tree::TerminalNode *USAMPLER2DARRAY();
    antlr4::tree::TerminalNode *USAMPLERCUBEARRAY();
    antlr4::tree::TerminalNode *SAMPLER1D();
    antlr4::tree::TerminalNode *SAMPLER1DSHADOW();
    antlr4::tree::TerminalNode *SAMPLER1DARRAY();
    antlr4::tree::TerminalNode *SAMPLER1DARRAYSHADOW();
    antlr4::tree::TerminalNode *ISAMPLER1D();
    antlr4::tree::TerminalNode *ISAMPLER1DARRAY();
    antlr4::tree::TerminalNode *USAMPLER1D();
    antlr4::tree::TerminalNode *USAMPLER1DARRAY();
    antlr4::tree::TerminalNode *SAMPLER2DRECT();
    antlr4::tree::TerminalNode *SAMPLER2DRECTSHADOW();
    antlr4::tree::TerminalNode *ISAMPLER2DRECT();
    antlr4::tree::TerminalNode *USAMPLER2DRECT();
    antlr4::tree::TerminalNode *SAMPLERBUFFER();
    antlr4::tree::TerminalNode *ISAMPLERBUFFER();
    antlr4::tree::TerminalNode *USAMPLERBUFFER();
    antlr4::tree::TerminalNode *SAMPLER2DMS();
    antlr4::tree::TerminalNode *ISAMPLER2DMS();
    antlr4::tree::TerminalNode *USAMPLER2DMS();
    antlr4::tree::TerminalNode *SAMPLER2DMSARRAY();
    antlr4::tree::TerminalNode *ISAMPLER2DMSARRAY();
    antlr4::tree::TerminalNode *USAMPLER2DMSARRAY();
    antlr4::tree::TerminalNode *IMAGE2D();
    antlr4::tree::TerminalNode *IIMAGE2D();
    antlr4::tree::TerminalNode *UIMAGE2D();
    antlr4::tree::TerminalNode *IMAGE3D();
    antlr4::tree::TerminalNode *IIMAGE3D();
    antlr4::tree::TerminalNode *UIMAGE3D();
    antlr4::tree::TerminalNode *IMAGECUBE();
    antlr4::tree::TerminalNode *IIMAGECUBE();
    antlr4::tree::TerminalNode *UIMAGECUBE();
    antlr4::tree::TerminalNode *IMAGEBUFFER();
    antlr4::tree::TerminalNode *IIMAGEBUFFER();
    antlr4::tree::TerminalNode *UIMAGEBUFFER();
    antlr4::tree::TerminalNode *IMAGE1D();
    antlr4::tree::TerminalNode *IIMAGE1D();
    antlr4::tree::TerminalNode *UIMAGE1D();
    antlr4::tree::TerminalNode *IMAGE1DARRAY();
    antlr4::tree::TerminalNode *IIMAGE1DARRAY();
    antlr4::tree::TerminalNode *UIMAGE1DARRAY();
    antlr4::tree::TerminalNode *IMAGE2DRECT();
    antlr4::tree::TerminalNode *IIMAGE2DRECT();
    antlr4::tree::TerminalNode *UIMAGE2DRECT();
    antlr4::tree::TerminalNode *IMAGE2DARRAY();
    antlr4::tree::TerminalNode *IIMAGE2DARRAY();
    antlr4::tree::TerminalNode *UIMAGE2DARRAY();
    antlr4::tree::TerminalNode *IMAGECUBEARRAY();
    antlr4::tree::TerminalNode *IIMAGECUBEARRAY();
    antlr4::tree::TerminalNode *UIMAGECUBEARRAY();
    antlr4::tree::TerminalNode *IMAGE2DMS();
    antlr4::tree::TerminalNode *IIMAGE2DMS();
    antlr4::tree::TerminalNode *UIMAGE2DMS();
    antlr4::tree::TerminalNode *IMAGE2DMSARRAY();
    antlr4::tree::TerminalNode *IIMAGE2DMSARRAY();
    antlr4::tree::TerminalNode *UIMAGE2DMSARRAY();
    Struct_specifierContext *struct_specifier();
    Type_nameContext *type_name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_specifier_nonarrayContext* type_specifier_nonarray();

  class  Precision_qualifierContext : public antlr4::ParserRuleContext {
  public:
    Precision_qualifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HIGHP();
    antlr4::tree::TerminalNode *MEDIUMP();
    antlr4::tree::TerminalNode *LOWP();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Precision_qualifierContext* precision_qualifier();

  class  Struct_specifierContext : public antlr4::ParserRuleContext {
  public:
    Struct_specifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRUCT();
    antlr4::tree::TerminalNode *LEFT_BRACE();
    Struct_declaration_listContext *struct_declaration_list();
    antlr4::tree::TerminalNode *RIGHT_BRACE();
    antlr4::tree::TerminalNode *IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_specifierContext* struct_specifier();

  class  Struct_declaration_listContext : public antlr4::ParserRuleContext {
  public:
    Struct_declaration_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Struct_declarationContext *> struct_declaration();
    Struct_declarationContext* struct_declaration(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_declaration_listContext* struct_declaration_list();

  class  Struct_declarationContext : public antlr4::ParserRuleContext {
  public:
    Struct_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_specifierContext *type_specifier();
    Struct_declarator_listContext *struct_declarator_list();
    antlr4::tree::TerminalNode *SEMICOLON();
    Type_qualifierContext *type_qualifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_declarationContext* struct_declaration();

  class  Struct_declarator_listContext : public antlr4::ParserRuleContext {
  public:
    Struct_declarator_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Struct_declaratorContext *> struct_declarator();
    Struct_declaratorContext* struct_declarator(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_declarator_listContext* struct_declarator_list();

  class  Struct_declaratorContext : public antlr4::ParserRuleContext {
  public:
    Struct_declaratorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IDENTIFIER();
    Array_specifierContext *array_specifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Struct_declaratorContext* struct_declarator();

  class  InitializerContext : public antlr4::ParserRuleContext {
  public:
    InitializerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Assignment_expressionContext *assignment_expression();
    antlr4::tree::TerminalNode *LEFT_BRACE();
    Initializer_listContext *initializer_list();
    antlr4::tree::TerminalNode *RIGHT_BRACE();
    antlr4::tree::TerminalNode *COMMA();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InitializerContext* initializer();

  class  Initializer_listContext : public antlr4::ParserRuleContext {
  public:
    Initializer_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<InitializerContext *> initializer();
    InitializerContext* initializer(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Initializer_listContext* initializer_list();

  class  Declaration_statementContext : public antlr4::ParserRuleContext {
  public:
    Declaration_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    DeclarationContext *declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Declaration_statementContext* declaration_statement();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Compound_statementContext *compound_statement();
    Simple_statementContext *simple_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StatementContext* statement();

  class  Simple_statementContext : public antlr4::ParserRuleContext {
  public:
    Simple_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Declaration_statementContext *declaration_statement();
    Expression_statementContext *expression_statement();
    Selection_statementContext *selection_statement();
    Switch_statementContext *switch_statement();
    Case_labelContext *case_label();
    Iteration_statementContext *iteration_statement();
    Jump_statementContext *jump_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_statementContext* simple_statement();

  class  Compound_statementContext : public antlr4::ParserRuleContext {
  public:
    Compound_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LEFT_BRACE();
    antlr4::tree::TerminalNode *RIGHT_BRACE();
    Statement_listContext *statement_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Compound_statementContext* compound_statement();

  class  Statement_no_new_scopeContext : public antlr4::ParserRuleContext {
  public:
    Statement_no_new_scopeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Compound_statement_no_new_scopeContext *compound_statement_no_new_scope();
    Simple_statementContext *simple_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_no_new_scopeContext* statement_no_new_scope();

  class  Compound_statement_no_new_scopeContext : public antlr4::ParserRuleContext {
  public:
    Compound_statement_no_new_scopeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LEFT_BRACE();
    antlr4::tree::TerminalNode *RIGHT_BRACE();
    Statement_listContext *statement_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Compound_statement_no_new_scopeContext* compound_statement_no_new_scope();

  class  Statement_listContext : public antlr4::ParserRuleContext {
  public:
    Statement_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Statement_listContext* statement_list();

  class  Expression_statementContext : public antlr4::ParserRuleContext {
  public:
    Expression_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMICOLON();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Expression_statementContext* expression_statement();

  class  Selection_statementContext : public antlr4::ParserRuleContext {
  public:
    Selection_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IF();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    Selection_rest_statementContext *selection_rest_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Selection_statementContext* selection_statement();

  class  Selection_rest_statementContext : public antlr4::ParserRuleContext {
  public:
    Selection_rest_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<StatementContext *> statement();
    StatementContext* statement(size_t i);
    antlr4::tree::TerminalNode *ELSE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Selection_rest_statementContext* selection_rest_statement();

  class  ConditionContext : public antlr4::ParserRuleContext {
  public:
    ConditionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    Fully_specified_typeContext *fully_specified_type();
    antlr4::tree::TerminalNode *IDENTIFIER();
    antlr4::tree::TerminalNode *EQUAL();
    InitializerContext *initializer();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConditionContext* condition();

  class  Switch_statementContext : public antlr4::ParserRuleContext {
  public:
    Switch_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SWITCH();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    antlr4::tree::TerminalNode *LEFT_BRACE();
    antlr4::tree::TerminalNode *RIGHT_BRACE();
    Statement_listContext *statement_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Switch_statementContext* switch_statement();

  class  Case_labelContext : public antlr4::ParserRuleContext {
  public:
    Case_labelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CASE();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *DEFAULT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_labelContext* case_label();

  class  Iteration_statementContext : public antlr4::ParserRuleContext {
  public:
    Iteration_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WHILE();
    antlr4::tree::TerminalNode *LEFT_PAREN();
    ConditionContext *condition();
    antlr4::tree::TerminalNode *RIGHT_PAREN();
    Statement_no_new_scopeContext *statement_no_new_scope();
    antlr4::tree::TerminalNode *DO();
    StatementContext *statement();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *SEMICOLON();
    antlr4::tree::TerminalNode *FOR();
    For_init_statementContext *for_init_statement();
    For_rest_statementContext *for_rest_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Iteration_statementContext* iteration_statement();

  class  For_init_statementContext : public antlr4::ParserRuleContext {
  public:
    For_init_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Expression_statementContext *expression_statement();
    Declaration_statementContext *declaration_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  For_init_statementContext* for_init_statement();

  class  For_rest_statementContext : public antlr4::ParserRuleContext {
  public:
    For_rest_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMICOLON();
    ConditionContext *condition();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  For_rest_statementContext* for_rest_statement();

  class  Jump_statementContext : public antlr4::ParserRuleContext {
  public:
    Jump_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONTINUE();
    antlr4::tree::TerminalNode *SEMICOLON();
    antlr4::tree::TerminalNode *BREAK();
    antlr4::tree::TerminalNode *RETURN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *DISCARD();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Jump_statementContext* jump_statement();

  class  External_declarationContext : public antlr4::ParserRuleContext {
  public:
    External_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_definitionContext *function_definition();
    DeclarationContext *declaration();
    antlr4::tree::TerminalNode *SEMICOLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  External_declarationContext* external_declaration();

  class  Function_definitionContext : public antlr4::ParserRuleContext {
  public:
    Function_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Function_prototypeContext *function_prototype();
    Compound_statement_no_new_scopeContext *compound_statement_no_new_scope();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_definitionContext* function_definition();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool postfix_expressionSempred(Postfix_expressionContext *_localctx, size_t predicateIndex);
  bool binary_expressionSempred(Binary_expressionContext *_localctx, size_t predicateIndex);
  bool expressionSempred(ExpressionContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

