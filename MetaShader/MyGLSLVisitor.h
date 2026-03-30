#include "antlr4-runtime.h"
#include "GLSLLexer.h"
#include "GLSLParserBaseVisitor.h"
#include "GLSLParserVisitor.h"
#include "GLSLParser.h"
#include "shaderInfo.h"
#include "SemanticVisitor.h"
using namespace antlr4;

class MyGLSLVisitor : public GLSLParserBaseVisitor {
    ShaderInfo info;
    void structDeclaration(GLSLParser::DeclarationContext* ctx) {
        if (!ctx) return;

        auto collectFields = [&](GLSLParser::Struct_declaration_listContext* listCtx,
                                 StructInfo& outInfo) {
            if (!listCtx) return;

            for (auto* decl : listCtx->struct_declaration()) {
                if (!decl || !decl->type_specifier() || !decl->struct_declarator_list()) {
                    continue;
                }

                const std::string fieldType = decl->type_specifier()->getText();
                for (auto* var : decl->struct_declarator_list()->struct_declarator()) {
                    if (!var || !var->IDENTIFIER()) continue;

                    int arraySize = -1;
                    if (auto* arr = var->array_specifier()) {
                        if (!arr->dimension().empty()) {
                            auto* ce = arr->dimension(0)->constant_expression();
                            if (ce) {
                                try {
                                    arraySize = std::stoi(ce->getText());
                                } catch (...) {
                                    arraySize = -1;
                                }
                            }
                        } else {
                            arraySize = 0;
                        }
                    }

                    outInfo.fields.push_back({fieldType, var->IDENTIFIER()->getText(), "", arraySize});
                }
            }
        };

        GLSLParser::Struct_specifierContext* structCtx = nullptr;
        if (auto* idl = ctx->init_declarator_list()) {
            if (auto* sd = idl->single_declaration()) {
                if (auto* fst = sd->fully_specified_type()) {
                    if (auto* ts = fst->type_specifier()) {
                        if (auto* tsn = ts->type_specifier_nonarray()) {
                            structCtx = tsn->struct_specifier();
                        }
                    }
                }
            }
        }

        if (structCtx) {
            const std::string structName = structCtx->IDENTIFIER() ? structCtx->IDENTIFIER()->getText() : "";
            if (structName.empty()) return;

            StructInfo structInfo;
            collectFields(structCtx->struct_declaration_list(), structInfo);
            info.structs[structName] = std::move(structInfo);
            return;
        }

        if (!ctx->struct_declaration_list()) return;
        if (ctx->IDENTIFIER().empty()) return;

        const std::string blockOrStructName = ctx->IDENTIFIER(0)->getText();
        if (blockOrStructName.empty()) return;

        StructInfo structInfo;
        collectFields(ctx->struct_declaration_list(), structInfo);
        info.structs[blockOrStructName] = std::move(structInfo);
    }   

    
    // Intercetta dichiarazioni globali
    std::any visitDeclaration(GLSLParser::DeclarationContext* ctx) override {
        structDeclaration(ctx);
        variablesDeclaration(ctx);
        return visitChildren(ctx);
    }

    std::any visitFunction_definition(GLSLParser::Function_definitionContext* ctx) override {
        std::cout << "Visitando funzione: " << ctx->function_prototype()->IDENTIFIER()->getText() << std::endl;
        auto* proto = ctx->function_prototype();
        FunctionInfo fn;
        fn.returnType = proto->fully_specified_type()->getText();
        fn.name       = proto->IDENTIFIER()->getText();

        if (auto* params = proto->function_parameters()) {
            for (auto* pd : params->parameter_declaration()) {
                ParamInfo p;
                if (pd->parameter_declarator()) {
                    p.type = pd->parameter_declarator()->type_specifier()->getText();
                    p.name = pd->parameter_declarator()->IDENTIFIER()->getText();
                } else {
                    p.type = pd->parameter_type_specifier()->getText();
                }
                if (pd->type_qualifier()) {
                    p.qualifier = pd->type_qualifier()->getText(); // "in","out","inout"
                }
                fn.params.push_back(p);
            }
        }
        std::shared_ptr<BlockStmt> body = std::make_shared<BlockStmt>(); // Placeholder, da costruire visitando il function body
        body->stmts = {}; // da riempire visitando il function body
        if(auto statementList = ctx->compound_statement_no_new_scope()->statement_list()) {
            for (auto* stmt : statementList->statement()) {
                body->stmts.push_back(dispatchStatement(stmt));
            }
        }
        fn.body = body;
        info.functions.push_back(std::move(fn));
        return {}; 
    }

    StatementPtr dispatchStatement(GLSLParser::StatementContext* ctx) 
    {
        if(ctx->simple_statement()) {
            return dispatchSimpleStatement(ctx->simple_statement());
        }
        else if(auto stmt = ctx->compound_statement()) {
            return dispatchCompoundStatement(stmt);
        }
        
        return std::make_shared<BlockStmt>(); 
    }

    StatementPtr dispatchSimpleStatement(GLSLParser::Simple_statementContext* ctx) 
    {
        if(ctx->expression_statement()) {
            std::shared_ptr<ExprStmt> stmt = std::make_shared<ExprStmt>();
            if (auto* exprCtx = ctx->expression_statement()->expression()) {
                stmt->expr = dispatchExpression(exprCtx);
            }
            return stmt;
        }
        else if(auto stmt = ctx->selection_statement()) {
            std::shared_ptr<IfStmt> ifStmt = std::make_shared<IfStmt>();
            ifStmt->condition = dispatchExpression(stmt->expression());
            ifStmt->thenBranch = dispatchStatement(stmt->selection_rest_statement()->statement(0));
            if(stmt->selection_rest_statement()->statement().size() > 1) {
                ifStmt->elseBranch = dispatchStatement(stmt->selection_rest_statement()->statement(1));
            }
            return ifStmt;
        }else if(auto stmt = ctx->switch_statement()) {
            return dispatchSwitchStatement(stmt);
        }
        else if (auto* caseLabel = ctx->case_label()) {
            return dispatchCaseLabel(caseLabel);
        }
        else if(auto stmt = ctx->iteration_statement()) {
            return dispatchIterationStatement(stmt);
        }
        else if(auto stmt = ctx->jump_statement()) {
            std::shared_ptr<JumpStmt> jumpStmt = std::make_shared<JumpStmt>();
            if (stmt->CONTINUE()) {
                jumpStmt->kind = JumpStmt::Kind::Continue;
            } else if (stmt->BREAK()) {
                jumpStmt->kind = JumpStmt::Kind::Break;
            } else if (stmt->RETURN()) {
                jumpStmt->kind = JumpStmt::Kind::Return;
                if (stmt->expression()) {
                    jumpStmt->value = dispatchExpression(stmt->expression());
                }
            } else if (stmt->DISCARD()) {
                jumpStmt->kind = JumpStmt::Kind::Discard;
            }
            return jumpStmt;
        }
        else if( auto stmt = ctx->declaration_statement()) {
            return dispatchDeclarationStatement(stmt);
        }
        else {
            std::cout << "Tipo di semplice statement non gestito: " << ctx->getText() << std::endl;
        }
        
        return std::make_shared<BlockStmt>();

    }
    
    StatementPtr dispatchStatementNoNewScope(GLSLParser::Statement_no_new_scopeContext* ctx) {
        if (!ctx) return std::make_shared<BlockStmt>();
        if (auto* simple = ctx->simple_statement()) {
            return dispatchSimpleStatement(simple);
        }
        if (auto* compound = ctx->compound_statement_no_new_scope()) {
            std::vector<StatementPtr> stmts;
            if (auto* list = compound->statement_list()) {
                for (auto* s : list->statement()) {
                    stmts.push_back(dispatchStatement(s));
                }
            }
            std::shared_ptr<BlockStmt> block = std::make_shared<BlockStmt>();
            block->stmts = std::move(stmts);
            return block;
        }
        return std::make_shared<BlockStmt>();
    }

    StatementPtr dispatchSwitchStatement(GLSLParser::Switch_statementContext* ctx) {
        if (!ctx) return std::make_shared<BlockStmt>();

        std::shared_ptr<SwitchStmt> switchStmt = std::make_shared<SwitchStmt>();
        switchStmt->expr = dispatchExpression(ctx->expression());

        std::shared_ptr<BlockStmt> switchBody = std::make_shared<BlockStmt>();
        if (auto* statementList = ctx->statement_list()) {
            for (auto* stmt : statementList->statement()) {
                switchBody->stmts.push_back(dispatchStatement(stmt));
            }
        }

        switchStmt->body = switchBody;
        return switchStmt;
    }

    StatementPtr dispatchCaseLabel(GLSLParser::Case_labelContext* ctx) {
        if (!ctx) return std::make_shared<BlockStmt>();

        std::shared_ptr<CaseLabel> caseStmt = std::make_shared<CaseLabel>();
        if (ctx->DEFAULT()) {
            caseStmt->label = "default:";
        } else {
            std::string exprText = ctx->expression() ? ctx->expression()->getText() : "";
            caseStmt->label = "case " + exprText + ":";
        }

        caseStmt->stmt = std::make_shared<BlockStmt>();
        return caseStmt;
    }

    StatementPtr dispatchDeclarationStatement(GLSLParser::Declaration_statementContext* stmtCtx) {
        if (!stmtCtx) return std::make_shared<BlockStmt>();
        auto* decl = stmtCtx->declaration();
        if (!decl) return std::make_shared<BlockStmt>();

        auto* idl = decl->init_declarator_list();
        if (!idl) return std::make_shared<BlockStmt>();

        std::shared_ptr<BlockStmt> block = std::make_shared<BlockStmt>();
        auto* sd = idl->single_declaration();
        if (!(sd && sd->fully_specified_type())) return block;

        const std::string baseType = sd->fully_specified_type()->type_specifier()->getText();

        auto appendVarDecl = [&](GLSLParser::Typeless_declarationContext* td) {
            if (!td || !td->IDENTIFIER()) return;
            std::shared_ptr<VarDeclStmt> varStmt = std::make_shared<VarDeclStmt>();
            varStmt->var.type = baseType;
            varStmt->var.name = td->IDENTIFIER()->getText();
            if (auto* init = td->initializer()) {
                varStmt->var.initializer = init->getText();
                if (auto* assign = init->assignment_expression()) {
                    varStmt->initializerExpr = dispatchAssigmentExpression(assign);
                }
            }
            if (auto* arr = td->array_specifier()) {
                if (!arr->dimension().empty()) {
                    auto* ce = arr->dimension(0)->constant_expression();
                    if (ce) {
                        try {
                            varStmt->var.arraySize = std::stoi(ce->getText());
                        } catch (...) {
                            varStmt->var.arraySize = -1;
                        }
                    }
                } else {
                    varStmt->var.arraySize = 0;  // Array senza dimensione (es. int arr[];)
                }
            }
            block->stmts.push_back(varStmt);
        };

        appendVarDecl(sd->typeless_declaration());
        for (auto* td : idl->typeless_declaration()) {
            appendVarDecl(td);
        }

        if (block->stmts.size() == 1) {
            return block->stmts.front();
        }
        return block;
    }

    // condition : expression | fully_specified_type IDENTIFIER EQUAL initializer
    // Se è la forma con dichiarazione, per ora usiamo il valore dell'initializer
    // come espressione di condizione (il simbolo dichiarato richiederebbe un nodo AST dedicato).
    ExpressionPtr dispatchCondition(GLSLParser::ConditionContext* ctx) {
        if (!ctx) return nullptr;
        if (auto* exprCtx = ctx->expression()) {
            return dispatchExpression(exprCtx);
        }
        if (auto* init = ctx->initializer()) {
            if (auto* assign = init->assignment_expression()) {
                return dispatchAssigmentExpression(assign);
            }
        }
        std::shared_ptr<VariableExpr> v = std::make_shared<VariableExpr>();
        v->name = ctx->getText();
        return v;
    }



    StatementPtr dispatchIterationStatement(GLSLParser::Iteration_statementContext* ctx) {
        if (!ctx) return std::make_shared<BlockStmt>();

        // WHILE '(' condition ')' statement_no_new_scope
        if (ctx->WHILE() && !ctx->FOR()) {
            std::shared_ptr<WhileStmt> ws = std::make_shared<WhileStmt>();
            ws->condition = dispatchCondition(ctx->condition());
            ws->body = dispatchStatementNoNewScope(ctx->statement_no_new_scope());
            return ws;
        }

        // DO statement WHILE '(' expression ')' ';'
        if (ctx->DO()) {

            std::shared_ptr<WhileStmt> ws = std::make_shared<WhileStmt>();
            ws->condition = dispatchExpression(ctx->expression());
            ws->body = dispatchStatement(ctx->statement());
            return ws;
        }

        // FOR '(' for_init_statement for_rest_statement ')' statement_no_new_scope
        if (ctx->FOR()) {
            std::shared_ptr<ForStmt> fs = std::make_shared<ForStmt>();

            // init: expression_statement | declaration_statement
            if (auto* initCtx = ctx->for_init_statement()) {
                if (auto* exprStmtCtx = initCtx->expression_statement()) {
                    std::shared_ptr<ExprStmt> initStmt = std::make_shared<ExprStmt>();
                    if (auto* exprCtx = exprStmtCtx->expression()) {
                        initStmt->expr = dispatchExpression(exprCtx);
                    }
                    fs->init = initStmt;
                } else if (initCtx->declaration_statement()) {
                    fs->init = dispatchDeclarationStatement(initCtx->declaration_statement());
                }
            }

            // rest: condition? ';' expression?
            if (auto* restCtx = ctx->for_rest_statement()) {
                if (auto* condCtx = restCtx->condition()) {
                    fs->condition = dispatchCondition(condCtx);
                }
                if (auto* iterCtx = restCtx->expression()) {
                    fs->iteration = dispatchExpression(iterCtx);
                }
            }

            fs->body = dispatchStatementNoNewScope(ctx->statement_no_new_scope());
            return fs;
        }

        return std::make_shared<BlockStmt>();
    }

    std::string getAssignmentOperatorText(GLSLParser::Assignment_operatorContext* ctx) {
        if (!ctx) return "=";
        if (ctx->EQUAL()) return "=";
        if (ctx->MUL_ASSIGN()) return "*=";
        if (ctx->DIV_ASSIGN()) return "/=";
        if (ctx->MOD_ASSIGN()) return "%=";
        if (ctx->ADD_ASSIGN()) return "+=";
        if (ctx->SUB_ASSIGN()) return "-=";
        if (ctx->LEFT_ASSIGN()) return "<<=";
        if (ctx->RIGHT_ASSIGN()) return ">>=";
        if (ctx->AND_ASSIGN()) return "&=";
        if (ctx->XOR_ASSIGN()) return "^=";
        if (ctx->OR_ASSIGN()) return "|=";
        return "=";
    }

    std::string getBinaryOperatorText(GLSLParser::Binary_expressionContext* ctx) {
        if (ctx->STAR()) return "*";
        if (ctx->SLASH()) return "/";
        if (ctx->PERCENT()) return "%";
        if (ctx->PLUS()) return "+";
        if (ctx->DASH()) return "-";
        if (ctx->LEFT_OP()) return "<<";
        if (ctx->RIGHT_OP()) return ">>";
        if (ctx->LEFT_ANGLE()) return "<";
        if (ctx->RIGHT_ANGLE()) return ">";
        if (ctx->LE_OP()) return "<=";
        if (ctx->GE_OP()) return ">=";
        if (ctx->EQ_OP()) return "==";
        if (ctx->NE_OP()) return "!=";
        if (ctx->AMPERSAND()) return "&";
        if (ctx->CARET()) return "^";
        if (ctx->VERTICAL_BAR()) return "|";
        if (ctx->AND_OP()) return "&&";
        if (ctx->XOR_OP()) return "^^";
        if (ctx->OR_OP()) return "||";
        return "";
    }

    ExpressionPtr dispatchPrimaryExpression(GLSLParser::Primary_expressionContext* ctx) {
        if (!ctx) return std::make_shared<VariableExpr>();

        if (auto* var = ctx->variable_identifier()) {
            std::shared_ptr<VariableExpr> expr = std::make_shared<VariableExpr>();
            expr->name = var->getText();
            return expr;
        }
        if (ctx->TRUE() || ctx->FALSE()) {
            std::shared_ptr<BoolLiteralExpr> expr = std::make_shared<BoolLiteralExpr>();
            expr->value = ctx->TRUE() != nullptr;
            return expr;
        }
        if (ctx->INTCONSTANT() || ctx->UINTCONSTANT()) {
            std::shared_ptr<IntLiteralExpr> expr = std::make_shared<IntLiteralExpr>();
            const std::string literal = ctx->INTCONSTANT() ? ctx->INTCONSTANT()->getText() : ctx->UINTCONSTANT()->getText();
            try {
                expr->value = std::stoll(literal, nullptr, 0);
            } catch (...) {
                expr->value = 0;
            }
            expr->isUnsigned = ctx->UINTCONSTANT() != nullptr;
            return expr;
        }
        if (ctx->FLOATCONSTANT() || ctx->DOUBLECONSTANT()) {
            std::shared_ptr<FloatLiteralExpr> expr = std::make_shared<FloatLiteralExpr>();
            const std::string literal = ctx->FLOATCONSTANT() ? ctx->FLOATCONSTANT()->getText() : ctx->DOUBLECONSTANT()->getText();
            try {
                expr->value = std::stod(literal);
            } catch (...) {
                expr->value = 0.0;
            }
            expr->isDouble = ctx->DOUBLECONSTANT() != nullptr;
            return expr;
        }
        if (auto* subExpr = ctx->expression()) {
            return dispatchExpression(subExpr);
        }

        std::shared_ptr<VariableExpr> fallback = std::make_shared<VariableExpr>();
        fallback->name = ctx->getText();
        return fallback;
    }

    std::vector<ExpressionPtr> dispatchFunctionArgs(GLSLParser::Function_call_parametersContext* params) {
        std::vector<ExpressionPtr> args;
        for (auto* arg : params->assignment_expression()) {
            args.push_back(dispatchAssigmentExpression(arg));
        }
        return args;
    }

    ExpressionPtr dispatchPostfixExpression(GLSLParser::Postfix_expressionContext* ctx) {
        //std::cout << "Dispatching postfix expression: " << ctx->getText() << std::endl;
        if (!ctx) return std::make_shared<VariableExpr>();

        if (auto* primary = ctx->primary_expression()) {
            return dispatchPrimaryExpression(primary);
        }

        if (ctx->type_specifier() && ctx->LEFT_PAREN()) {
            std::shared_ptr<CallExpr> call = std::make_shared<CallExpr>();
            call->callee = ctx->type_specifier()->getText();
            call->isConstructor = true;
            call->args = dispatchFunctionArgs(ctx->function_call_parameters());
            return call;
        }

        if (ctx->postfix_expression() && ctx->LEFT_PAREN()) {
            std::shared_ptr<CallExpr> call = std::make_shared<CallExpr>();
            ExpressionPtr calleeExpr = dispatchPostfixExpression(ctx->postfix_expression());
            call->args = dispatchFunctionArgs(ctx->function_call_parameters());
            if (auto* var = dynamic_cast<VariableExpr*>(calleeExpr.get())) {
                call->callee = var->name;
            } else if (auto* member = dynamic_cast<MemberExpr*>(calleeExpr.get())) {
                call->callee = member->field;
                call->calleeExpr = member->object;
            } else {
                call->calleeExpr = calleeExpr;
            }
            return call;
        }

        if (ctx->postfix_expression() && ctx->LEFT_BRACKET()) {
            std::shared_ptr<IndexExpr> indexExpr = std::make_shared<IndexExpr>();
            indexExpr->object = dispatchPostfixExpression(ctx->postfix_expression());
            if (auto* idxExpr = ctx->integer_expression() ? ctx->integer_expression()->expression() : nullptr) {
                indexExpr->index = dispatchExpression(idxExpr);
            }
            return indexExpr;
        }

        if (ctx->postfix_expression() && ctx->DOT()) {
            std::shared_ptr<MemberExpr> memberExpr = std::make_shared<MemberExpr>();
            memberExpr->object = dispatchPostfixExpression(ctx->postfix_expression());
            if (auto* field = ctx->field_selection()) {
                memberExpr->field = field->getText();
            }
            return memberExpr;
        }

        if (ctx->postfix_expression() && (ctx->INC_OP() || ctx->DEC_OP())) {
            std::shared_ptr<PostfixExpr> postfixExpr = std::make_shared<PostfixExpr>();
            postfixExpr->operand = dispatchPostfixExpression(ctx->postfix_expression());
            postfixExpr->op = ctx->INC_OP() ? "++" : "--";
            return postfixExpr;
        }

        std::shared_ptr<VariableExpr> fallback = std::make_shared<VariableExpr>();
        fallback->name = ctx->getText();
        return fallback;
    }

    ExpressionPtr dispatchUnaryExpression(GLSLParser::Unary_expressionContext* ctx) {
        if (!ctx) return std::make_shared<VariableExpr>();

        if (auto* postfix = ctx->postfix_expression()) {
            return dispatchPostfixExpression(postfix);
        }

        if (auto* operandCtx = ctx->unary_expression()) {
            std::shared_ptr<UnaryExpr> expr = std::make_shared<UnaryExpr>();
            if (ctx->INC_OP()) expr->op = "++";
            else if (ctx->DEC_OP()) expr->op = "--";
            else if (auto* op = ctx->unary_operator()) expr->op = op->getText();
            expr->operand = dispatchUnaryExpression(operandCtx);
            return expr;
        }

        std::shared_ptr<VariableExpr> fallback = std::make_shared<VariableExpr>();
        fallback->name = ctx->getText();
        return fallback;
    }

    ExpressionPtr dispatchBinaryExpression(GLSLParser::Binary_expressionContext* ctx) {
        if (!ctx) return std::make_shared<VariableExpr>();

        if (auto* unary = ctx->unary_expression()) {
            return dispatchUnaryExpression(unary);
        }

        if (ctx->binary_expression().size() == 2) {
            std::shared_ptr<BinaryExpr> expr = std::make_shared<BinaryExpr>();
            expr->left = dispatchBinaryExpression(ctx->binary_expression(0));
            expr->right = dispatchBinaryExpression(ctx->binary_expression(1));
            expr->op = getBinaryOperatorText(ctx);
            return expr;
        }

        std::shared_ptr<VariableExpr> fallback = std::make_shared<VariableExpr>();
        fallback->name = ctx->getText();
        return fallback;
    }

    ExpressionPtr dispatchConstantExpression(GLSLParser::Constant_expressionContext* ctx) {
        if (!ctx) return std::make_shared<VariableExpr>();

        if (ctx->QUESTION()) {
            std::shared_ptr<TernaryExpr> expr = std::make_shared<TernaryExpr>();
            expr->condition = dispatchBinaryExpression(ctx->binary_expression());
            expr->trueExpr = dispatchExpression(ctx->expression());
            expr->falseExpr = dispatchAssigmentExpression(ctx->assignment_expression());
            return expr;
        }

        return dispatchBinaryExpression(ctx->binary_expression());
    }
    
    ExpressionPtr dispatchAssigmentExpression(GLSLParser::Assignment_expressionContext* ctx) 
    {
        if (!ctx) {
            return std::make_shared<VariableExpr>();
        }

        if (auto* constExpr = ctx->constant_expression()) {
            return dispatchConstantExpression(constExpr);
        }

        if (ctx->unary_expression() && ctx->assignment_operator() && ctx->assignment_expression()) {
            std::shared_ptr<AssignExpr> expr = std::make_shared<AssignExpr>();
            expr->left = dispatchUnaryExpression(ctx->unary_expression());
            expr->op = getAssignmentOperatorText(ctx->assignment_operator());
            expr->right = dispatchAssigmentExpression(ctx->assignment_expression());
            return expr;
        }

        std::shared_ptr<VariableExpr> fallback = std::make_shared<VariableExpr>();
        fallback->name = ctx->getText();
        return fallback;
    }

    ExpressionPtr dispatchExpression(GLSLParser::ExpressionContext* ctx) 
    {
        if (!ctx) {
            return std::make_shared<VariableExpr>();
        }

        if(auto subExpr = ctx->expression()) {
            std::shared_ptr<CommaExpr> commaExpr = std::make_shared<CommaExpr>();
            commaExpr->left = dispatchExpression(subExpr);
            commaExpr->right = dispatchAssigmentExpression(ctx->assignment_expression());
            return commaExpr;
        }
        if(auto subExpr = ctx->assignment_expression()) {
            return dispatchAssigmentExpression(subExpr);
        }
        std::shared_ptr<VariableExpr> fallback = std::make_shared<VariableExpr>();
        fallback->name = ctx->getText();
        return fallback;
    }


    StatementPtr dispatchSelectionStatement(GLSLParser::Selection_statementContext* ctx) {
        if (!ctx) return std::make_shared<BlockStmt>();

        std::shared_ptr<IfStmt> ifStmt = std::make_shared<IfStmt>();
        ifStmt->condition = dispatchExpression(ctx->expression());
        ifStmt->thenBranch = dispatchStatement(ctx->selection_rest_statement()->statement(0));
        if (ctx->selection_rest_statement()->ELSE()) {
            ifStmt->elseBranch = dispatchStatement(ctx->selection_rest_statement()->statement(1));
        }
        return ifStmt;
    }
    StatementPtr dispatchCompoundStatementNoNewScope(GLSLParser::Compound_statement_no_new_scopeContext* ctx) {
        std::vector<StatementPtr> statements;
        if (auto* statementList = ctx->statement_list()) {
            for (auto* stmt : statementList->statement()) {
                statements.push_back(dispatchStatement(stmt));
            }
        }
        std::shared_ptr<BlockStmt> block = std::make_shared<BlockStmt>();
        block->stmts = std::move(statements);
        return block;
    }

    StatementPtr dispatchCompoundStatement(GLSLParser::Compound_statementContext* ctx) 
    {
        std::vector<StatementPtr> statements;
        if (auto* statementList = ctx->statement_list()) {
            for (auto* stmt : statementList->statement()) {
                statements.push_back(dispatchStatement(stmt));
            }
        }
        std::shared_ptr<BlockStmt> block = std::make_shared<BlockStmt>();
        block->stmts = std::move(statements);
        return block;
    }

    uint32_t evaluateArraySize(GLSLParser::Typeless_declarationContext *td) {
        if (td->array_specifier()) {
            if (!td->array_specifier()->dimension().empty()) {
                auto* ce = td->array_specifier()->dimension(0)->constant_expression();
                if (ce) {
                    try {
                        return std::stoul(ce->getText());
                    } catch (...) {
                        return -1;
                    }
                }
            } else {
                return 0;  // Array senza dimensione (es. int arr[];)
            }
        }
        return -1;
    }

    void variablesDeclaration(GLSLParser::DeclarationContext *ctx)
    {
        if (auto *idl = ctx->init_declarator_list())
        {
            auto *sd = idl->single_declaration();
            auto *fst = sd->fully_specified_type();
            bool isIn = false, isOut = false;
            bool isUniform = false;
            if (fst->type_qualifier())
            {
                for (auto *sq : fst->type_qualifier()->single_type_qualifier())
                {
                    if (sq->storage_qualifier() && sq->storage_qualifier()->UNIFORM())
                    {
                        isUniform = true;
                    }
                    if (sq->storage_qualifier() && sq->storage_qualifier()->IN())
                        isIn = true;
                    if (sq->storage_qualifier() && sq->storage_qualifier()->OUT())
                        isOut = true;
                }
                if (isUniform)
                {
                    std::string typeName = fst->type_specifier()->getText();
                    bool isSampler = typeName.find("sampler") != std::string::npos || typeName.find("Sampler") != std::string::npos;

                    // Prima variabile (nella single_declaration)
                    if (sd->typeless_declaration())
                    {
                        auto* firstDecl = sd->typeless_declaration();
                        int arraySize = evaluateArraySize(firstDecl);
                        info.uniforms.push_back({typeName,
                            firstDecl->IDENTIFIER()->getText(),
                            isSampler,
                            arraySize,
                            firstDecl->initializer() ? firstDecl->initializer()->getText() : ""});
                    }
                    // Eventuali ulteriori nella lista (COMMA typeless_declaration)
                    for (auto *td : idl->typeless_declaration())
                    {
                        int arraySize = evaluateArraySize(td);
                        info.uniforms.push_back({typeName,
                            td->IDENTIFIER()->getText(),
                            isSampler,
                            arraySize,
                            td->initializer() ? td->initializer()->getText() : ""});
                    }
                }
                if (isIn || isOut)
                {
                    collectInOut(ctx, fst->type_qualifier());
                }
                //std::cout << "Dichiarazione globale senza qualificatore di storage: " << ctx->getText() << std::endl;
            }
            if (!isUniform && !isIn && !isOut)
            {
                // Variabile globale senza qualificatore di storage (es. "float g_time;")
                std::string typeName = fst->type_specifier()->getText();

                auto makeGlobalVarDecl = [&](GLSLParser::Typeless_declarationContext* td) -> VariableDeclaration {
                    VariableDeclaration declaration;
                    declaration.type = typeName;
                    declaration.name = td->IDENTIFIER()->getText();
                    declaration.arraySize = evaluateArraySize(td);

                    if (auto* init = td->initializer()) {
                        declaration.initializer = init->getText();
                        if (auto* assign = init->assignment_expression()) {
                            declaration.initializerExpr = dispatchAssigmentExpression(assign);
                        }
                    }

                    return declaration;
                };
               
                if (auto* ftsd = sd->typeless_declaration())
                {
                    VariableDeclaration declaration = makeGlobalVarDecl(ftsd);
                    std::cout << "Dichiarazione globale: " << typeName << " " << declaration.name
                              << (declaration.arraySize != -1 ? "[" + std::to_string(declaration.arraySize) + "]" : "")
                              << (!declaration.initializer.empty() ? " = " + declaration.initializer : "")
                              << std::endl;
                    info.globalVariables.push_back(std::move(declaration));
                }
                for (auto *td : idl->typeless_declaration())
                {
                    VariableDeclaration declaration = makeGlobalVarDecl(td);
                    std::cout << "Dichiarazione globale: " << typeName << " " << declaration.name
                              << (declaration.arraySize != -1 ? "[" + std::to_string(declaration.arraySize) + "]" : "")
                              << (!declaration.initializer.empty() ? " = " + declaration.initializer : "")
                              << std::endl;
                    info.globalVariables.push_back(std::move(declaration));
                }
            }
        };
    }

private:
    void collectInOut(GLSLParser::DeclarationContext* ctx,
                      GLSLParser::Type_qualifierContext* tq) {
        bool isIn = false, isOut = false;
        int location = -1;
        for (auto* stq : tq->single_type_qualifier()) {
            if (stq->storage_qualifier()) {
                if (stq->storage_qualifier()->IN())  isIn  = true;
                if (stq->storage_qualifier()->OUT()) isOut = true;
            }
            if (stq->layout_qualifier()) {
                for (auto* lid : stq->layout_qualifier()->layout_qualifier_id_list()->layout_qualifier_id()) {
                    if (lid->IDENTIFIER() && lid->IDENTIFIER()->getText() == "location" && lid->constant_expression())
                        location = std::stoi(lid->constant_expression()->getText());
                }
            }
        }
        auto* sd = ctx->init_declarator_list()->single_declaration();
        std::string typeName = sd->fully_specified_type()->type_specifier()->getText();
        if (isIn) {
            info.inputs.push_back({typeName, sd->typeless_declaration()->IDENTIFIER()->getText(), location});
        }
        if (isOut) {
            info.outputs.push_back({typeName, sd->typeless_declaration()->IDENTIFIER()->getText(), location});
        }

        // ... aggiungi a info.inputs / info.outputs
    }

public:
    const ShaderInfo& getShaderInfo() const { return info; }

    void printAllShaderInfo() {
        std::cout << "Uniforms:\n";
        for (const auto& u : info.uniforms) {
            std::cout << "  " << u.type << " " << u.name << " [isSampler=" << u.isSampler << ", arraySize=" << u.arraySize << "]";
            if (!u.initializer.empty()) std::cout << " = " << u.initializer;
            std::cout << "\n";
        }
        std::cout << "Inputs:\n";
        for (const auto& i : info.inputs) {
            std::cout << "  " << i.type << " " << i.name << " [location=" << i.location << "]\n";
        }
        std::cout << "Outputs:\n";
        for (const auto& o : info.outputs) {
            std::cout << "  " << o.type << " " << o.name << " [location=" << o.location << "]\n";
        }
        std::cout << "Global Variables:\n";
        for (const auto& v : info.globalVariables) {
            std::cout << "  " << v.type << " " << v.name << " [arraySize=" << v.arraySize << "]\n";
        }
        for (const auto& s : info.structs) {
            std::cout << "Struct: " << s.first << "\n";
            for (const auto& f : s.second.fields) {
                std::cout << "  " << f.type << " " << f.name;
                if (f.arraySize != -1) std::cout << "[" << f.arraySize << "]";
                std::cout << "\n";
            }
        }


        std::cout << "Functions:\n";
        for (const auto& f : info.functions) {
            std::cout << "  " << f.returnType << " " << f.name << "(";
            for (size_t j = 0; j < f.params.size(); ++j) {
                const auto& p = f.params[j];
                if (!p.qualifier.empty()) std::cout << p.qualifier << " ";
                std::cout << p.type << " " << p.name;
                if (j < f.params.size() - 1) std::cout << ", ";
            }
            std::cout << ")\n";
            std::cout << " \n" << f.body->toString() << "\n";
        }
        
    }

    
};