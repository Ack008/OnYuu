
// Generated from GLSLParser.g4 by ANTLR 4.10

#pragma once


#include "antlr4-runtime.h"
#include "GLSLParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by GLSLParser.
 */
class  GLSLParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by GLSLParser.
   */
    virtual std::any visitTranslation_unit(GLSLParser::Translation_unitContext *context) = 0;

    virtual std::any visitVariable_identifier(GLSLParser::Variable_identifierContext *context) = 0;

    virtual std::any visitPrimary_expression(GLSLParser::Primary_expressionContext *context) = 0;

    virtual std::any visitPostfix_expression(GLSLParser::Postfix_expressionContext *context) = 0;

    virtual std::any visitField_selection(GLSLParser::Field_selectionContext *context) = 0;

    virtual std::any visitInteger_expression(GLSLParser::Integer_expressionContext *context) = 0;

    virtual std::any visitFunction_call(GLSLParser::Function_callContext *context) = 0;

    virtual std::any visitFunction_identifier(GLSLParser::Function_identifierContext *context) = 0;

    virtual std::any visitFunction_call_parameters(GLSLParser::Function_call_parametersContext *context) = 0;

    virtual std::any visitUnary_expression(GLSLParser::Unary_expressionContext *context) = 0;

    virtual std::any visitUnary_operator(GLSLParser::Unary_operatorContext *context) = 0;

    virtual std::any visitAssignment_expression(GLSLParser::Assignment_expressionContext *context) = 0;

    virtual std::any visitAssignment_operator(GLSLParser::Assignment_operatorContext *context) = 0;

    virtual std::any visitBinary_expression(GLSLParser::Binary_expressionContext *context) = 0;

    virtual std::any visitExpression(GLSLParser::ExpressionContext *context) = 0;

    virtual std::any visitConstant_expression(GLSLParser::Constant_expressionContext *context) = 0;

    virtual std::any visitDeclaration(GLSLParser::DeclarationContext *context) = 0;

    virtual std::any visitIdentifier_list(GLSLParser::Identifier_listContext *context) = 0;

    virtual std::any visitFunction_prototype(GLSLParser::Function_prototypeContext *context) = 0;

    virtual std::any visitFunction_parameters(GLSLParser::Function_parametersContext *context) = 0;

    virtual std::any visitParameter_declarator(GLSLParser::Parameter_declaratorContext *context) = 0;

    virtual std::any visitParameter_declaration(GLSLParser::Parameter_declarationContext *context) = 0;

    virtual std::any visitParameter_type_specifier(GLSLParser::Parameter_type_specifierContext *context) = 0;

    virtual std::any visitInit_declarator_list(GLSLParser::Init_declarator_listContext *context) = 0;

    virtual std::any visitSingle_declaration(GLSLParser::Single_declarationContext *context) = 0;

    virtual std::any visitTypeless_declaration(GLSLParser::Typeless_declarationContext *context) = 0;

    virtual std::any visitFully_specified_type(GLSLParser::Fully_specified_typeContext *context) = 0;

    virtual std::any visitInvariant_qualifier(GLSLParser::Invariant_qualifierContext *context) = 0;

    virtual std::any visitInterpolation_qualifier(GLSLParser::Interpolation_qualifierContext *context) = 0;

    virtual std::any visitLayout_qualifier(GLSLParser::Layout_qualifierContext *context) = 0;

    virtual std::any visitLayout_qualifier_id_list(GLSLParser::Layout_qualifier_id_listContext *context) = 0;

    virtual std::any visitLayout_qualifier_id(GLSLParser::Layout_qualifier_idContext *context) = 0;

    virtual std::any visitPrecise_qualifier(GLSLParser::Precise_qualifierContext *context) = 0;

    virtual std::any visitType_qualifier(GLSLParser::Type_qualifierContext *context) = 0;

    virtual std::any visitSingle_type_qualifier(GLSLParser::Single_type_qualifierContext *context) = 0;

    virtual std::any visitStorage_qualifier(GLSLParser::Storage_qualifierContext *context) = 0;

    virtual std::any visitType_name_list(GLSLParser::Type_name_listContext *context) = 0;

    virtual std::any visitType_name(GLSLParser::Type_nameContext *context) = 0;

    virtual std::any visitType_specifier(GLSLParser::Type_specifierContext *context) = 0;

    virtual std::any visitArray_specifier(GLSLParser::Array_specifierContext *context) = 0;

    virtual std::any visitDimension(GLSLParser::DimensionContext *context) = 0;

    virtual std::any visitType_specifier_nonarray(GLSLParser::Type_specifier_nonarrayContext *context) = 0;

    virtual std::any visitPrecision_qualifier(GLSLParser::Precision_qualifierContext *context) = 0;

    virtual std::any visitStruct_specifier(GLSLParser::Struct_specifierContext *context) = 0;

    virtual std::any visitStruct_declaration_list(GLSLParser::Struct_declaration_listContext *context) = 0;

    virtual std::any visitStruct_declaration(GLSLParser::Struct_declarationContext *context) = 0;

    virtual std::any visitStruct_declarator_list(GLSLParser::Struct_declarator_listContext *context) = 0;

    virtual std::any visitStruct_declarator(GLSLParser::Struct_declaratorContext *context) = 0;

    virtual std::any visitInitializer(GLSLParser::InitializerContext *context) = 0;

    virtual std::any visitInitializer_list(GLSLParser::Initializer_listContext *context) = 0;

    virtual std::any visitDeclaration_statement(GLSLParser::Declaration_statementContext *context) = 0;

    virtual std::any visitStatement(GLSLParser::StatementContext *context) = 0;

    virtual std::any visitSimple_statement(GLSLParser::Simple_statementContext *context) = 0;

    virtual std::any visitCompound_statement(GLSLParser::Compound_statementContext *context) = 0;

    virtual std::any visitStatement_no_new_scope(GLSLParser::Statement_no_new_scopeContext *context) = 0;

    virtual std::any visitCompound_statement_no_new_scope(GLSLParser::Compound_statement_no_new_scopeContext *context) = 0;

    virtual std::any visitStatement_list(GLSLParser::Statement_listContext *context) = 0;

    virtual std::any visitExpression_statement(GLSLParser::Expression_statementContext *context) = 0;

    virtual std::any visitSelection_statement(GLSLParser::Selection_statementContext *context) = 0;

    virtual std::any visitSelection_rest_statement(GLSLParser::Selection_rest_statementContext *context) = 0;

    virtual std::any visitCondition(GLSLParser::ConditionContext *context) = 0;

    virtual std::any visitSwitch_statement(GLSLParser::Switch_statementContext *context) = 0;

    virtual std::any visitCase_label(GLSLParser::Case_labelContext *context) = 0;

    virtual std::any visitIteration_statement(GLSLParser::Iteration_statementContext *context) = 0;

    virtual std::any visitFor_init_statement(GLSLParser::For_init_statementContext *context) = 0;

    virtual std::any visitFor_rest_statement(GLSLParser::For_rest_statementContext *context) = 0;

    virtual std::any visitJump_statement(GLSLParser::Jump_statementContext *context) = 0;

    virtual std::any visitExternal_declaration(GLSLParser::External_declarationContext *context) = 0;

    virtual std::any visitFunction_definition(GLSLParser::Function_definitionContext *context) = 0;


};

