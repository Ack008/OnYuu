
// Generated from GLSLParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"
#include "GLSLParserVisitor.h"


/**
 * This class provides an empty implementation of GLSLParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  GLSLParserBaseVisitor : public GLSLParserVisitor {
public:

  virtual std::any visitTranslation_unit(GLSLParser::Translation_unitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariable_identifier(GLSLParser::Variable_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimary_expression(GLSLParser::Primary_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPostfix_expression(GLSLParser::Postfix_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitField_selection(GLSLParser::Field_selectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInteger_expression(GLSLParser::Integer_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_call(GLSLParser::Function_callContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_identifier(GLSLParser::Function_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_call_parameters(GLSLParser::Function_call_parametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnary_expression(GLSLParser::Unary_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnary_operator(GLSLParser::Unary_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignment_expression(GLSLParser::Assignment_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignment_operator(GLSLParser::Assignment_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBinary_expression(GLSLParser::Binary_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(GLSLParser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstant_expression(GLSLParser::Constant_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration(GLSLParser::DeclarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier_list(GLSLParser::Identifier_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_prototype(GLSLParser::Function_prototypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_parameters(GLSLParser::Function_parametersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParameter_declarator(GLSLParser::Parameter_declaratorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParameter_declaration(GLSLParser::Parameter_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParameter_type_specifier(GLSLParser::Parameter_type_specifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInit_declarator_list(GLSLParser::Init_declarator_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSingle_declaration(GLSLParser::Single_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTypeless_declaration(GLSLParser::Typeless_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFully_specified_type(GLSLParser::Fully_specified_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInvariant_qualifier(GLSLParser::Invariant_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterpolation_qualifier(GLSLParser::Interpolation_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLayout_qualifier(GLSLParser::Layout_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLayout_qualifier_id_list(GLSLParser::Layout_qualifier_id_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLayout_qualifier_id(GLSLParser::Layout_qualifier_idContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrecise_qualifier(GLSLParser::Precise_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_qualifier(GLSLParser::Type_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSingle_type_qualifier(GLSLParser::Single_type_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStorage_qualifier(GLSLParser::Storage_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_name_list(GLSLParser::Type_name_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_name(GLSLParser::Type_nameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_specifier(GLSLParser::Type_specifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArray_specifier(GLSLParser::Array_specifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDimension(GLSLParser::DimensionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_specifier_nonarray(GLSLParser::Type_specifier_nonarrayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrecision_qualifier(GLSLParser::Precision_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_specifier(GLSLParser::Struct_specifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_declaration_list(GLSLParser::Struct_declaration_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_declaration(GLSLParser::Struct_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_declarator_list(GLSLParser::Struct_declarator_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_declarator(GLSLParser::Struct_declaratorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInitializer(GLSLParser::InitializerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInitializer_list(GLSLParser::Initializer_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeclaration_statement(GLSLParser::Declaration_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(GLSLParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSimple_statement(GLSLParser::Simple_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCompound_statement(GLSLParser::Compound_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_no_new_scope(GLSLParser::Statement_no_new_scopeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCompound_statement_no_new_scope(GLSLParser::Compound_statement_no_new_scopeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_list(GLSLParser::Statement_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression_statement(GLSLParser::Expression_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelection_statement(GLSLParser::Selection_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelection_rest_statement(GLSLParser::Selection_rest_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCondition(GLSLParser::ConditionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSwitch_statement(GLSLParser::Switch_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCase_label(GLSLParser::Case_labelContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIteration_statement(GLSLParser::Iteration_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFor_init_statement(GLSLParser::For_init_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFor_rest_statement(GLSLParser::For_rest_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitJump_statement(GLSLParser::Jump_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExternal_declaration(GLSLParser::External_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_definition(GLSLParser::Function_definitionContext *ctx) override {
    return visitChildren(ctx);
  }


};

