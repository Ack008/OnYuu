
// Generated from GLSLParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"
#include "GLSLParserListener.h"


/**
 * This class provides an empty implementation of GLSLParserListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  GLSLParserBaseListener : public GLSLParserListener {
public:

  virtual void enterTranslation_unit(GLSLParser::Translation_unitContext * /*ctx*/) override { }
  virtual void exitTranslation_unit(GLSLParser::Translation_unitContext * /*ctx*/) override { }

  virtual void enterVariable_identifier(GLSLParser::Variable_identifierContext * /*ctx*/) override { }
  virtual void exitVariable_identifier(GLSLParser::Variable_identifierContext * /*ctx*/) override { }

  virtual void enterPrimary_expression(GLSLParser::Primary_expressionContext * /*ctx*/) override { }
  virtual void exitPrimary_expression(GLSLParser::Primary_expressionContext * /*ctx*/) override { }

  virtual void enterPostfix_expression(GLSLParser::Postfix_expressionContext * /*ctx*/) override { }
  virtual void exitPostfix_expression(GLSLParser::Postfix_expressionContext * /*ctx*/) override { }

  virtual void enterField_selection(GLSLParser::Field_selectionContext * /*ctx*/) override { }
  virtual void exitField_selection(GLSLParser::Field_selectionContext * /*ctx*/) override { }

  virtual void enterInteger_expression(GLSLParser::Integer_expressionContext * /*ctx*/) override { }
  virtual void exitInteger_expression(GLSLParser::Integer_expressionContext * /*ctx*/) override { }

  virtual void enterFunction_call(GLSLParser::Function_callContext * /*ctx*/) override { }
  virtual void exitFunction_call(GLSLParser::Function_callContext * /*ctx*/) override { }

  virtual void enterFunction_identifier(GLSLParser::Function_identifierContext * /*ctx*/) override { }
  virtual void exitFunction_identifier(GLSLParser::Function_identifierContext * /*ctx*/) override { }

  virtual void enterFunction_call_parameters(GLSLParser::Function_call_parametersContext * /*ctx*/) override { }
  virtual void exitFunction_call_parameters(GLSLParser::Function_call_parametersContext * /*ctx*/) override { }

  virtual void enterUnary_expression(GLSLParser::Unary_expressionContext * /*ctx*/) override { }
  virtual void exitUnary_expression(GLSLParser::Unary_expressionContext * /*ctx*/) override { }

  virtual void enterUnary_operator(GLSLParser::Unary_operatorContext * /*ctx*/) override { }
  virtual void exitUnary_operator(GLSLParser::Unary_operatorContext * /*ctx*/) override { }

  virtual void enterAssignment_expression(GLSLParser::Assignment_expressionContext * /*ctx*/) override { }
  virtual void exitAssignment_expression(GLSLParser::Assignment_expressionContext * /*ctx*/) override { }

  virtual void enterAssignment_operator(GLSLParser::Assignment_operatorContext * /*ctx*/) override { }
  virtual void exitAssignment_operator(GLSLParser::Assignment_operatorContext * /*ctx*/) override { }

  virtual void enterBinary_expression(GLSLParser::Binary_expressionContext * /*ctx*/) override { }
  virtual void exitBinary_expression(GLSLParser::Binary_expressionContext * /*ctx*/) override { }

  virtual void enterExpression(GLSLParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(GLSLParser::ExpressionContext * /*ctx*/) override { }

  virtual void enterConstant_expression(GLSLParser::Constant_expressionContext * /*ctx*/) override { }
  virtual void exitConstant_expression(GLSLParser::Constant_expressionContext * /*ctx*/) override { }

  virtual void enterDeclaration(GLSLParser::DeclarationContext * /*ctx*/) override { }
  virtual void exitDeclaration(GLSLParser::DeclarationContext * /*ctx*/) override { }

  virtual void enterIdentifier_list(GLSLParser::Identifier_listContext * /*ctx*/) override { }
  virtual void exitIdentifier_list(GLSLParser::Identifier_listContext * /*ctx*/) override { }

  virtual void enterFunction_prototype(GLSLParser::Function_prototypeContext * /*ctx*/) override { }
  virtual void exitFunction_prototype(GLSLParser::Function_prototypeContext * /*ctx*/) override { }

  virtual void enterFunction_parameters(GLSLParser::Function_parametersContext * /*ctx*/) override { }
  virtual void exitFunction_parameters(GLSLParser::Function_parametersContext * /*ctx*/) override { }

  virtual void enterParameter_declarator(GLSLParser::Parameter_declaratorContext * /*ctx*/) override { }
  virtual void exitParameter_declarator(GLSLParser::Parameter_declaratorContext * /*ctx*/) override { }

  virtual void enterParameter_declaration(GLSLParser::Parameter_declarationContext * /*ctx*/) override { }
  virtual void exitParameter_declaration(GLSLParser::Parameter_declarationContext * /*ctx*/) override { }

  virtual void enterParameter_type_specifier(GLSLParser::Parameter_type_specifierContext * /*ctx*/) override { }
  virtual void exitParameter_type_specifier(GLSLParser::Parameter_type_specifierContext * /*ctx*/) override { }

  virtual void enterInit_declarator_list(GLSLParser::Init_declarator_listContext * /*ctx*/) override { }
  virtual void exitInit_declarator_list(GLSLParser::Init_declarator_listContext * /*ctx*/) override { }

  virtual void enterSingle_declaration(GLSLParser::Single_declarationContext * /*ctx*/) override { }
  virtual void exitSingle_declaration(GLSLParser::Single_declarationContext * /*ctx*/) override { }

  virtual void enterTypeless_declaration(GLSLParser::Typeless_declarationContext * /*ctx*/) override { }
  virtual void exitTypeless_declaration(GLSLParser::Typeless_declarationContext * /*ctx*/) override { }

  virtual void enterFully_specified_type(GLSLParser::Fully_specified_typeContext * /*ctx*/) override { }
  virtual void exitFully_specified_type(GLSLParser::Fully_specified_typeContext * /*ctx*/) override { }

  virtual void enterInvariant_qualifier(GLSLParser::Invariant_qualifierContext * /*ctx*/) override { }
  virtual void exitInvariant_qualifier(GLSLParser::Invariant_qualifierContext * /*ctx*/) override { }

  virtual void enterInterpolation_qualifier(GLSLParser::Interpolation_qualifierContext * /*ctx*/) override { }
  virtual void exitInterpolation_qualifier(GLSLParser::Interpolation_qualifierContext * /*ctx*/) override { }

  virtual void enterLayout_qualifier(GLSLParser::Layout_qualifierContext * /*ctx*/) override { }
  virtual void exitLayout_qualifier(GLSLParser::Layout_qualifierContext * /*ctx*/) override { }

  virtual void enterLayout_qualifier_id_list(GLSLParser::Layout_qualifier_id_listContext * /*ctx*/) override { }
  virtual void exitLayout_qualifier_id_list(GLSLParser::Layout_qualifier_id_listContext * /*ctx*/) override { }

  virtual void enterLayout_qualifier_id(GLSLParser::Layout_qualifier_idContext * /*ctx*/) override { }
  virtual void exitLayout_qualifier_id(GLSLParser::Layout_qualifier_idContext * /*ctx*/) override { }

  virtual void enterPrecise_qualifier(GLSLParser::Precise_qualifierContext * /*ctx*/) override { }
  virtual void exitPrecise_qualifier(GLSLParser::Precise_qualifierContext * /*ctx*/) override { }

  virtual void enterType_qualifier(GLSLParser::Type_qualifierContext * /*ctx*/) override { }
  virtual void exitType_qualifier(GLSLParser::Type_qualifierContext * /*ctx*/) override { }

  virtual void enterSingle_type_qualifier(GLSLParser::Single_type_qualifierContext * /*ctx*/) override { }
  virtual void exitSingle_type_qualifier(GLSLParser::Single_type_qualifierContext * /*ctx*/) override { }

  virtual void enterStorage_qualifier(GLSLParser::Storage_qualifierContext * /*ctx*/) override { }
  virtual void exitStorage_qualifier(GLSLParser::Storage_qualifierContext * /*ctx*/) override { }

  virtual void enterType_name_list(GLSLParser::Type_name_listContext * /*ctx*/) override { }
  virtual void exitType_name_list(GLSLParser::Type_name_listContext * /*ctx*/) override { }

  virtual void enterType_name(GLSLParser::Type_nameContext * /*ctx*/) override { }
  virtual void exitType_name(GLSLParser::Type_nameContext * /*ctx*/) override { }

  virtual void enterType_specifier(GLSLParser::Type_specifierContext * /*ctx*/) override { }
  virtual void exitType_specifier(GLSLParser::Type_specifierContext * /*ctx*/) override { }

  virtual void enterArray_specifier(GLSLParser::Array_specifierContext * /*ctx*/) override { }
  virtual void exitArray_specifier(GLSLParser::Array_specifierContext * /*ctx*/) override { }

  virtual void enterDimension(GLSLParser::DimensionContext * /*ctx*/) override { }
  virtual void exitDimension(GLSLParser::DimensionContext * /*ctx*/) override { }

  virtual void enterType_specifier_nonarray(GLSLParser::Type_specifier_nonarrayContext * /*ctx*/) override { }
  virtual void exitType_specifier_nonarray(GLSLParser::Type_specifier_nonarrayContext * /*ctx*/) override { }

  virtual void enterPrecision_qualifier(GLSLParser::Precision_qualifierContext * /*ctx*/) override { }
  virtual void exitPrecision_qualifier(GLSLParser::Precision_qualifierContext * /*ctx*/) override { }

  virtual void enterStruct_specifier(GLSLParser::Struct_specifierContext * /*ctx*/) override { }
  virtual void exitStruct_specifier(GLSLParser::Struct_specifierContext * /*ctx*/) override { }

  virtual void enterStruct_declaration_list(GLSLParser::Struct_declaration_listContext * /*ctx*/) override { }
  virtual void exitStruct_declaration_list(GLSLParser::Struct_declaration_listContext * /*ctx*/) override { }

  virtual void enterStruct_declaration(GLSLParser::Struct_declarationContext * /*ctx*/) override { }
  virtual void exitStruct_declaration(GLSLParser::Struct_declarationContext * /*ctx*/) override { }

  virtual void enterStruct_declarator_list(GLSLParser::Struct_declarator_listContext * /*ctx*/) override { }
  virtual void exitStruct_declarator_list(GLSLParser::Struct_declarator_listContext * /*ctx*/) override { }

  virtual void enterStruct_declarator(GLSLParser::Struct_declaratorContext * /*ctx*/) override { }
  virtual void exitStruct_declarator(GLSLParser::Struct_declaratorContext * /*ctx*/) override { }

  virtual void enterInitializer(GLSLParser::InitializerContext * /*ctx*/) override { }
  virtual void exitInitializer(GLSLParser::InitializerContext * /*ctx*/) override { }

  virtual void enterInitializer_list(GLSLParser::Initializer_listContext * /*ctx*/) override { }
  virtual void exitInitializer_list(GLSLParser::Initializer_listContext * /*ctx*/) override { }

  virtual void enterDeclaration_statement(GLSLParser::Declaration_statementContext * /*ctx*/) override { }
  virtual void exitDeclaration_statement(GLSLParser::Declaration_statementContext * /*ctx*/) override { }

  virtual void enterStatement(GLSLParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(GLSLParser::StatementContext * /*ctx*/) override { }

  virtual void enterSimple_statement(GLSLParser::Simple_statementContext * /*ctx*/) override { }
  virtual void exitSimple_statement(GLSLParser::Simple_statementContext * /*ctx*/) override { }

  virtual void enterCompound_statement(GLSLParser::Compound_statementContext * /*ctx*/) override { }
  virtual void exitCompound_statement(GLSLParser::Compound_statementContext * /*ctx*/) override { }

  virtual void enterStatement_no_new_scope(GLSLParser::Statement_no_new_scopeContext * /*ctx*/) override { }
  virtual void exitStatement_no_new_scope(GLSLParser::Statement_no_new_scopeContext * /*ctx*/) override { }

  virtual void enterCompound_statement_no_new_scope(GLSLParser::Compound_statement_no_new_scopeContext * /*ctx*/) override { }
  virtual void exitCompound_statement_no_new_scope(GLSLParser::Compound_statement_no_new_scopeContext * /*ctx*/) override { }

  virtual void enterStatement_list(GLSLParser::Statement_listContext * /*ctx*/) override { }
  virtual void exitStatement_list(GLSLParser::Statement_listContext * /*ctx*/) override { }

  virtual void enterExpression_statement(GLSLParser::Expression_statementContext * /*ctx*/) override { }
  virtual void exitExpression_statement(GLSLParser::Expression_statementContext * /*ctx*/) override { }

  virtual void enterSelection_statement(GLSLParser::Selection_statementContext * /*ctx*/) override { }
  virtual void exitSelection_statement(GLSLParser::Selection_statementContext * /*ctx*/) override { }

  virtual void enterSelection_rest_statement(GLSLParser::Selection_rest_statementContext * /*ctx*/) override { }
  virtual void exitSelection_rest_statement(GLSLParser::Selection_rest_statementContext * /*ctx*/) override { }

  virtual void enterCondition(GLSLParser::ConditionContext * /*ctx*/) override { }
  virtual void exitCondition(GLSLParser::ConditionContext * /*ctx*/) override { }

  virtual void enterSwitch_statement(GLSLParser::Switch_statementContext * /*ctx*/) override { }
  virtual void exitSwitch_statement(GLSLParser::Switch_statementContext * /*ctx*/) override { }

  virtual void enterCase_label(GLSLParser::Case_labelContext * /*ctx*/) override { }
  virtual void exitCase_label(GLSLParser::Case_labelContext * /*ctx*/) override { }

  virtual void enterIteration_statement(GLSLParser::Iteration_statementContext * /*ctx*/) override { }
  virtual void exitIteration_statement(GLSLParser::Iteration_statementContext * /*ctx*/) override { }

  virtual void enterFor_init_statement(GLSLParser::For_init_statementContext * /*ctx*/) override { }
  virtual void exitFor_init_statement(GLSLParser::For_init_statementContext * /*ctx*/) override { }

  virtual void enterFor_rest_statement(GLSLParser::For_rest_statementContext * /*ctx*/) override { }
  virtual void exitFor_rest_statement(GLSLParser::For_rest_statementContext * /*ctx*/) override { }

  virtual void enterJump_statement(GLSLParser::Jump_statementContext * /*ctx*/) override { }
  virtual void exitJump_statement(GLSLParser::Jump_statementContext * /*ctx*/) override { }

  virtual void enterExternal_declaration(GLSLParser::External_declarationContext * /*ctx*/) override { }
  virtual void exitExternal_declaration(GLSLParser::External_declarationContext * /*ctx*/) override { }

  virtual void enterFunction_definition(GLSLParser::Function_definitionContext * /*ctx*/) override { }
  virtual void exitFunction_definition(GLSLParser::Function_definitionContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

