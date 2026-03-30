
// Generated from GLSLParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"
#include "GLSLParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by GLSLParser.
 */
class  GLSLParserListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterTranslation_unit(GLSLParser::Translation_unitContext *ctx) = 0;
  virtual void exitTranslation_unit(GLSLParser::Translation_unitContext *ctx) = 0;

  virtual void enterVariable_identifier(GLSLParser::Variable_identifierContext *ctx) = 0;
  virtual void exitVariable_identifier(GLSLParser::Variable_identifierContext *ctx) = 0;

  virtual void enterPrimary_expression(GLSLParser::Primary_expressionContext *ctx) = 0;
  virtual void exitPrimary_expression(GLSLParser::Primary_expressionContext *ctx) = 0;

  virtual void enterPostfix_expression(GLSLParser::Postfix_expressionContext *ctx) = 0;
  virtual void exitPostfix_expression(GLSLParser::Postfix_expressionContext *ctx) = 0;

  virtual void enterField_selection(GLSLParser::Field_selectionContext *ctx) = 0;
  virtual void exitField_selection(GLSLParser::Field_selectionContext *ctx) = 0;

  virtual void enterInteger_expression(GLSLParser::Integer_expressionContext *ctx) = 0;
  virtual void exitInteger_expression(GLSLParser::Integer_expressionContext *ctx) = 0;

  virtual void enterFunction_call(GLSLParser::Function_callContext *ctx) = 0;
  virtual void exitFunction_call(GLSLParser::Function_callContext *ctx) = 0;

  virtual void enterFunction_identifier(GLSLParser::Function_identifierContext *ctx) = 0;
  virtual void exitFunction_identifier(GLSLParser::Function_identifierContext *ctx) = 0;

  virtual void enterFunction_call_parameters(GLSLParser::Function_call_parametersContext *ctx) = 0;
  virtual void exitFunction_call_parameters(GLSLParser::Function_call_parametersContext *ctx) = 0;

  virtual void enterUnary_expression(GLSLParser::Unary_expressionContext *ctx) = 0;
  virtual void exitUnary_expression(GLSLParser::Unary_expressionContext *ctx) = 0;

  virtual void enterUnary_operator(GLSLParser::Unary_operatorContext *ctx) = 0;
  virtual void exitUnary_operator(GLSLParser::Unary_operatorContext *ctx) = 0;

  virtual void enterAssignment_expression(GLSLParser::Assignment_expressionContext *ctx) = 0;
  virtual void exitAssignment_expression(GLSLParser::Assignment_expressionContext *ctx) = 0;

  virtual void enterAssignment_operator(GLSLParser::Assignment_operatorContext *ctx) = 0;
  virtual void exitAssignment_operator(GLSLParser::Assignment_operatorContext *ctx) = 0;

  virtual void enterBinary_expression(GLSLParser::Binary_expressionContext *ctx) = 0;
  virtual void exitBinary_expression(GLSLParser::Binary_expressionContext *ctx) = 0;

  virtual void enterExpression(GLSLParser::ExpressionContext *ctx) = 0;
  virtual void exitExpression(GLSLParser::ExpressionContext *ctx) = 0;

  virtual void enterConstant_expression(GLSLParser::Constant_expressionContext *ctx) = 0;
  virtual void exitConstant_expression(GLSLParser::Constant_expressionContext *ctx) = 0;

  virtual void enterDeclaration(GLSLParser::DeclarationContext *ctx) = 0;
  virtual void exitDeclaration(GLSLParser::DeclarationContext *ctx) = 0;

  virtual void enterIdentifier_list(GLSLParser::Identifier_listContext *ctx) = 0;
  virtual void exitIdentifier_list(GLSLParser::Identifier_listContext *ctx) = 0;

  virtual void enterFunction_prototype(GLSLParser::Function_prototypeContext *ctx) = 0;
  virtual void exitFunction_prototype(GLSLParser::Function_prototypeContext *ctx) = 0;

  virtual void enterFunction_parameters(GLSLParser::Function_parametersContext *ctx) = 0;
  virtual void exitFunction_parameters(GLSLParser::Function_parametersContext *ctx) = 0;

  virtual void enterParameter_declarator(GLSLParser::Parameter_declaratorContext *ctx) = 0;
  virtual void exitParameter_declarator(GLSLParser::Parameter_declaratorContext *ctx) = 0;

  virtual void enterParameter_declaration(GLSLParser::Parameter_declarationContext *ctx) = 0;
  virtual void exitParameter_declaration(GLSLParser::Parameter_declarationContext *ctx) = 0;

  virtual void enterParameter_type_specifier(GLSLParser::Parameter_type_specifierContext *ctx) = 0;
  virtual void exitParameter_type_specifier(GLSLParser::Parameter_type_specifierContext *ctx) = 0;

  virtual void enterInit_declarator_list(GLSLParser::Init_declarator_listContext *ctx) = 0;
  virtual void exitInit_declarator_list(GLSLParser::Init_declarator_listContext *ctx) = 0;

  virtual void enterSingle_declaration(GLSLParser::Single_declarationContext *ctx) = 0;
  virtual void exitSingle_declaration(GLSLParser::Single_declarationContext *ctx) = 0;

  virtual void enterTypeless_declaration(GLSLParser::Typeless_declarationContext *ctx) = 0;
  virtual void exitTypeless_declaration(GLSLParser::Typeless_declarationContext *ctx) = 0;

  virtual void enterFully_specified_type(GLSLParser::Fully_specified_typeContext *ctx) = 0;
  virtual void exitFully_specified_type(GLSLParser::Fully_specified_typeContext *ctx) = 0;

  virtual void enterInvariant_qualifier(GLSLParser::Invariant_qualifierContext *ctx) = 0;
  virtual void exitInvariant_qualifier(GLSLParser::Invariant_qualifierContext *ctx) = 0;

  virtual void enterInterpolation_qualifier(GLSLParser::Interpolation_qualifierContext *ctx) = 0;
  virtual void exitInterpolation_qualifier(GLSLParser::Interpolation_qualifierContext *ctx) = 0;

  virtual void enterLayout_qualifier(GLSLParser::Layout_qualifierContext *ctx) = 0;
  virtual void exitLayout_qualifier(GLSLParser::Layout_qualifierContext *ctx) = 0;

  virtual void enterLayout_qualifier_id_list(GLSLParser::Layout_qualifier_id_listContext *ctx) = 0;
  virtual void exitLayout_qualifier_id_list(GLSLParser::Layout_qualifier_id_listContext *ctx) = 0;

  virtual void enterLayout_qualifier_id(GLSLParser::Layout_qualifier_idContext *ctx) = 0;
  virtual void exitLayout_qualifier_id(GLSLParser::Layout_qualifier_idContext *ctx) = 0;

  virtual void enterPrecise_qualifier(GLSLParser::Precise_qualifierContext *ctx) = 0;
  virtual void exitPrecise_qualifier(GLSLParser::Precise_qualifierContext *ctx) = 0;

  virtual void enterType_qualifier(GLSLParser::Type_qualifierContext *ctx) = 0;
  virtual void exitType_qualifier(GLSLParser::Type_qualifierContext *ctx) = 0;

  virtual void enterSingle_type_qualifier(GLSLParser::Single_type_qualifierContext *ctx) = 0;
  virtual void exitSingle_type_qualifier(GLSLParser::Single_type_qualifierContext *ctx) = 0;

  virtual void enterStorage_qualifier(GLSLParser::Storage_qualifierContext *ctx) = 0;
  virtual void exitStorage_qualifier(GLSLParser::Storage_qualifierContext *ctx) = 0;

  virtual void enterType_name_list(GLSLParser::Type_name_listContext *ctx) = 0;
  virtual void exitType_name_list(GLSLParser::Type_name_listContext *ctx) = 0;

  virtual void enterType_name(GLSLParser::Type_nameContext *ctx) = 0;
  virtual void exitType_name(GLSLParser::Type_nameContext *ctx) = 0;

  virtual void enterType_specifier(GLSLParser::Type_specifierContext *ctx) = 0;
  virtual void exitType_specifier(GLSLParser::Type_specifierContext *ctx) = 0;

  virtual void enterArray_specifier(GLSLParser::Array_specifierContext *ctx) = 0;
  virtual void exitArray_specifier(GLSLParser::Array_specifierContext *ctx) = 0;

  virtual void enterDimension(GLSLParser::DimensionContext *ctx) = 0;
  virtual void exitDimension(GLSLParser::DimensionContext *ctx) = 0;

  virtual void enterType_specifier_nonarray(GLSLParser::Type_specifier_nonarrayContext *ctx) = 0;
  virtual void exitType_specifier_nonarray(GLSLParser::Type_specifier_nonarrayContext *ctx) = 0;

  virtual void enterPrecision_qualifier(GLSLParser::Precision_qualifierContext *ctx) = 0;
  virtual void exitPrecision_qualifier(GLSLParser::Precision_qualifierContext *ctx) = 0;

  virtual void enterStruct_specifier(GLSLParser::Struct_specifierContext *ctx) = 0;
  virtual void exitStruct_specifier(GLSLParser::Struct_specifierContext *ctx) = 0;

  virtual void enterStruct_declaration_list(GLSLParser::Struct_declaration_listContext *ctx) = 0;
  virtual void exitStruct_declaration_list(GLSLParser::Struct_declaration_listContext *ctx) = 0;

  virtual void enterStruct_declaration(GLSLParser::Struct_declarationContext *ctx) = 0;
  virtual void exitStruct_declaration(GLSLParser::Struct_declarationContext *ctx) = 0;

  virtual void enterStruct_declarator_list(GLSLParser::Struct_declarator_listContext *ctx) = 0;
  virtual void exitStruct_declarator_list(GLSLParser::Struct_declarator_listContext *ctx) = 0;

  virtual void enterStruct_declarator(GLSLParser::Struct_declaratorContext *ctx) = 0;
  virtual void exitStruct_declarator(GLSLParser::Struct_declaratorContext *ctx) = 0;

  virtual void enterInitializer(GLSLParser::InitializerContext *ctx) = 0;
  virtual void exitInitializer(GLSLParser::InitializerContext *ctx) = 0;

  virtual void enterInitializer_list(GLSLParser::Initializer_listContext *ctx) = 0;
  virtual void exitInitializer_list(GLSLParser::Initializer_listContext *ctx) = 0;

  virtual void enterDeclaration_statement(GLSLParser::Declaration_statementContext *ctx) = 0;
  virtual void exitDeclaration_statement(GLSLParser::Declaration_statementContext *ctx) = 0;

  virtual void enterStatement(GLSLParser::StatementContext *ctx) = 0;
  virtual void exitStatement(GLSLParser::StatementContext *ctx) = 0;

  virtual void enterSimple_statement(GLSLParser::Simple_statementContext *ctx) = 0;
  virtual void exitSimple_statement(GLSLParser::Simple_statementContext *ctx) = 0;

  virtual void enterCompound_statement(GLSLParser::Compound_statementContext *ctx) = 0;
  virtual void exitCompound_statement(GLSLParser::Compound_statementContext *ctx) = 0;

  virtual void enterStatement_no_new_scope(GLSLParser::Statement_no_new_scopeContext *ctx) = 0;
  virtual void exitStatement_no_new_scope(GLSLParser::Statement_no_new_scopeContext *ctx) = 0;

  virtual void enterCompound_statement_no_new_scope(GLSLParser::Compound_statement_no_new_scopeContext *ctx) = 0;
  virtual void exitCompound_statement_no_new_scope(GLSLParser::Compound_statement_no_new_scopeContext *ctx) = 0;

  virtual void enterStatement_list(GLSLParser::Statement_listContext *ctx) = 0;
  virtual void exitStatement_list(GLSLParser::Statement_listContext *ctx) = 0;

  virtual void enterExpression_statement(GLSLParser::Expression_statementContext *ctx) = 0;
  virtual void exitExpression_statement(GLSLParser::Expression_statementContext *ctx) = 0;

  virtual void enterSelection_statement(GLSLParser::Selection_statementContext *ctx) = 0;
  virtual void exitSelection_statement(GLSLParser::Selection_statementContext *ctx) = 0;

  virtual void enterSelection_rest_statement(GLSLParser::Selection_rest_statementContext *ctx) = 0;
  virtual void exitSelection_rest_statement(GLSLParser::Selection_rest_statementContext *ctx) = 0;

  virtual void enterCondition(GLSLParser::ConditionContext *ctx) = 0;
  virtual void exitCondition(GLSLParser::ConditionContext *ctx) = 0;

  virtual void enterSwitch_statement(GLSLParser::Switch_statementContext *ctx) = 0;
  virtual void exitSwitch_statement(GLSLParser::Switch_statementContext *ctx) = 0;

  virtual void enterCase_label(GLSLParser::Case_labelContext *ctx) = 0;
  virtual void exitCase_label(GLSLParser::Case_labelContext *ctx) = 0;

  virtual void enterIteration_statement(GLSLParser::Iteration_statementContext *ctx) = 0;
  virtual void exitIteration_statement(GLSLParser::Iteration_statementContext *ctx) = 0;

  virtual void enterFor_init_statement(GLSLParser::For_init_statementContext *ctx) = 0;
  virtual void exitFor_init_statement(GLSLParser::For_init_statementContext *ctx) = 0;

  virtual void enterFor_rest_statement(GLSLParser::For_rest_statementContext *ctx) = 0;
  virtual void exitFor_rest_statement(GLSLParser::For_rest_statementContext *ctx) = 0;

  virtual void enterJump_statement(GLSLParser::Jump_statementContext *ctx) = 0;
  virtual void exitJump_statement(GLSLParser::Jump_statementContext *ctx) = 0;

  virtual void enterExternal_declaration(GLSLParser::External_declarationContext *ctx) = 0;
  virtual void exitExternal_declaration(GLSLParser::External_declarationContext *ctx) = 0;

  virtual void enterFunction_definition(GLSLParser::Function_definitionContext *ctx) = 0;
  virtual void exitFunction_definition(GLSLParser::Function_definitionContext *ctx) = 0;


};

