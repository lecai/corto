/* Fast_Parser.h
 *
 * This file contains generated code. Do not modify!
 */

#ifndef Fast_Parser_H
#define Fast_Parser_H

#include "cortex.h"
#include "Fast__type.h"

#include "Fast__api.h"

#include "Fast__meta.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ::cortex::Fast::Parser::addStatement(Fast::Node statement) */
void Fast_Parser_addStatement(Fast_Parser _this, Fast_Node statement);

/* ::cortex::Fast::Parser::argumentToString(Fast::Variable type,lang::string id,lang::bool reference) */
cx_string Fast_Parser_argumentToString(Fast_Parser _this, Fast_Variable type, cx_string id, cx_bool reference);

/* ::cortex::Fast::Parser::binaryExpr(Fast::Expression lvalues,Fast::Expression rvalues,lang::operatorKind operator) */
Fast_Expression Fast_Parser_binaryExpr(Fast_Parser _this, Fast_Expression lvalues, Fast_Expression rvalues, cx_operatorKind operator);

/* ::cortex::Fast::Parser::bind(Fast::Variable function,Fast::Block block) */
cx_int16 Fast_Parser_bind(Fast_Parser _this, Fast_Variable function, Fast_Block block);

/* ::cortex::Fast::Parser::bindOneliner(Fast::Variable function,Fast::Block block,Fast::Expression expr) */
cx_int16 Fast_Parser_bindOneliner(Fast_Parser _this, Fast_Variable function, Fast_Block block, Fast_Expression expr);

/* ::cortex::Fast::Parser::blockPop() */
void Fast_Parser_blockPop(Fast_Parser _this);

/* ::cortex::Fast::Parser::blockPush(lang::bool presetBlock) */
Fast_Block Fast_Parser_blockPush(Fast_Parser _this, cx_bool presetBlock);

/* ::cortex::Fast::Parser::callExpr(Fast::Expression function,Fast::Expression arguments) */
Fast_Expression Fast_Parser_callExpr(Fast_Parser _this, Fast_Expression function, Fast_Expression arguments);

/* ::cortex::Fast::Parser::castExpr(Fast::Expression lvalue,Fast::Expression rvalue) */
Fast_Expression Fast_Parser_castExpr(Fast_Parser _this, Fast_Expression lvalue, Fast_Expression rvalue);

/* ::cortex::Fast::Parser::collect(lang::object o) */
cx_void Fast_Parser_collect(Fast_Parser _this, cx_object o);

/* ::cortex::Fast::Parser::collectHeap(word addr) */
cx_void Fast_Parser_collectHeap(Fast_Parser _this, cx_word addr);

/* callback ::cortex::lang::class::construct(lang::object object) -> ::cortex::Fast::Parser::construct(Parser object) */
cx_int16 Fast_Parser_construct(Fast_Parser object);

/* ::cortex::Fast::Parser::declaration(Variable type,lang::string id,lang::bool isReference) */
Fast_Variable Fast_Parser_declaration(Fast_Parser _this, Fast_Variable type, cx_string id, cx_bool isReference);

/* ::cortex::Fast::Parser::declareFunction(Variable returnType,lang::string id,lang::type kind,bool returnsReference) */
Fast_Variable Fast_Parser_declareFunction(Fast_Parser _this, Fast_Variable returnType, cx_string id, cx_type kind, cx_bool returnsReference);

/* ::cortex::Fast::Parser::declareFunctionParams(Variable function) */
Fast_Block Fast_Parser_declareFunctionParams(Fast_Parser _this, Fast_Variable function);

/* ::cortex::Fast::Parser::define() */
cx_int16 Fast_Parser_define(Fast_Parser _this);

/* ::cortex::Fast::Parser::defineScope() */
cx_int16 Fast_Parser_defineScope(Fast_Parser _this);

/* ::cortex::Fast::Parser::defineVariable(Variable object) */
cx_int16 Fast_Parser_defineVariable(Fast_Parser _this, Fast_Variable object);

/* callback ::cortex::lang::class::destruct(lang::object object) -> ::cortex::Fast::Parser::destruct(Parser object) */
void Fast_Parser_destruct(Fast_Parser object);

/* ::cortex::Fast::Parser::elementExpr(Fast::Expression lvalue,Fast::Expression rvalue) */
Fast_Expression Fast_Parser_elementExpr(Fast_Parser _this, Fast_Expression lvalue, Fast_Expression rvalue);

/* ::cortex::Fast::Parser::foreach(lang::string loopId,Fast::Expression collection) */
cx_int16 Fast_Parser_foreach(Fast_Parser _this, cx_string loopId, Fast_Expression collection);

/* ::cortex::Fast::Parser::getComplexType() */
cx_type Fast_Parser_getComplexType(Fast_Parser _this);

/* ::cortex::Fast::Parser::getLvalue(lang::bool assignment) */
Fast_Expression Fast_Parser_getLvalue(Fast_Parser _this, cx_bool assignment);

/* ::cortex::Fast::Parser::getLvalueType(lang::bool assignment) */
cx_type Fast_Parser_getLvalueType(Fast_Parser _this, cx_bool assignment);

/* ::cortex::Fast::Parser::ifStatement(Fast::Expression condition,Fast::Block trueBranch,Fast::If falseBranch) */
Fast_Node Fast_Parser_ifStatement(Fast_Parser _this, Fast_Expression condition, Fast_Block trueBranch, Fast_If falseBranch);

/* ::cortex::Fast::Parser::initDeclareStaged(Fast::Expression expr) */
void Fast_Parser_initDeclareStaged(Fast_Parser _this, Fast_Expression expr);

/* ::cortex::Fast::Parser::initKeyValuePop() */
cx_int16 Fast_Parser_initKeyValuePop(Fast_Parser _this);

/* ::cortex::Fast::Parser::initKeyValuePush() */
cx_int16 Fast_Parser_initKeyValuePush(Fast_Parser _this);

/* ::cortex::Fast::Parser::initKeyValueSet(Fast::Expression expr) */
cx_int16 Fast_Parser_initKeyValueSet(Fast_Parser _this, Fast_Expression expr);

/* ::cortex::Fast::Parser::initMember(lang::string member) */
cx_int16 Fast_Parser_initMember(Fast_Parser _this, cx_string member);

/* ::cortex::Fast::Parser::initPop() */
cx_int16 Fast_Parser_initPop(Fast_Parser _this);

/* ::cortex::Fast::Parser::initPush() */
cx_int16 Fast_Parser_initPush(Fast_Parser _this);

/* ::cortex::Fast::Parser::initPushExpression() */
Fast_Expression Fast_Parser_initPushExpression(Fast_Parser _this);

/* ::cortex::Fast::Parser::initPushIdentifier(Expression type) */
Fast_Expression Fast_Parser_initPushIdentifier(Fast_Parser _this, Fast_Expression type);

/* ::cortex::Fast::Parser::initPushStatic() */
cx_int16 Fast_Parser_initPushStatic(Fast_Parser _this);

/* ::cortex::Fast::Parser::initStage(lang::string id,bool found) */
void Fast_Parser_initStage(Fast_Parser _this, cx_string id, cx_bool found);

/* ::cortex::Fast::Parser::initValue(Expression expr) */
cx_int16 Fast_Parser_initValue(Fast_Parser _this, Fast_Expression expr);

/* ::cortex::Fast::Parser::isAbortSet() */
cx_bool Fast_Parser_isAbortSet(Fast_Parser _this);

/* ::cortex::Fast::Parser::isErrSet() */
cx_bool Fast_Parser_isErrSet(Fast_Parser _this);

/* ::cortex::Fast::Parser::lookup(lang::string id,lang::object source) */
Fast_Expression Fast_Parser_lookup(Fast_Parser _this, cx_string id, cx_object source);

/* ::cortex::Fast::Parser::memberExpr(Fast::Expression lvalue,Fast::Expression rvalue) */
Fast_Expression Fast_Parser_memberExpr(Fast_Parser _this, Fast_Expression lvalue, Fast_Expression rvalue);

/* ::cortex::Fast::Parser::observerDeclaration(lang::string id,Fast::Expression object,lang::eventMask mask,Fast::Object dispatcher) */
Fast_Variable Fast_Parser_observerDeclaration(Fast_Parser _this, cx_string id, Fast_Expression object, cx_eventMask mask, Fast_Object dispatcher);

/* ::cortex::Fast::Parser::observerPop() */
void Fast_Parser_observerPop(Fast_Parser _this);

/* ::cortex::Fast::Parser::observerPush() */
void Fast_Parser_observerPush(Fast_Parser _this);

/* ::cortex::Fast::Parser::parse() */
cx_uint32 Fast_Parser_parse(Fast_Parser _this);

/* ::cortex::Fast::Parser::parseExpression(lang::string expr,Fast::Block block,Fast::Variable scope,uint32 line,uint32 column) */
Fast_Expression Fast_Parser_parseExpression(Fast_Parser _this, cx_string expr, Fast_Block block, Fast_Variable scope, cx_uint32 line, cx_uint32 column);

/* ::cortex::Fast::Parser::popComplexType() */
void Fast_Parser_popComplexType(Fast_Parser _this);

/* ::cortex::Fast::Parser::popLvalue() */
void Fast_Parser_popLvalue(Fast_Parser _this);

/* ::cortex::Fast::Parser::popScope(Fast::Variable previous) */
void Fast_Parser_popScope(Fast_Parser _this, Fast_Variable previous);

/* ::cortex::Fast::Parser::postfixExpr(Fast::Expression lvalue,lang::operatorKind operator) */
Fast_Expression Fast_Parser_postfixExpr(Fast_Parser _this, Fast_Expression lvalue, cx_operatorKind operator);

/* ::cortex::Fast::Parser::pushComplexType(Fast::Expression lvalue) */
void Fast_Parser_pushComplexType(Fast_Parser _this, Fast_Expression lvalue);

/* ::cortex::Fast::Parser::pushLvalue(Fast::Expression lvalue,lang::bool isAssignment) */
void Fast_Parser_pushLvalue(Fast_Parser _this, Fast_Expression lvalue, cx_bool isAssignment);

/* ::cortex::Fast::Parser::pushReturnAsLvalue(lang::function function) */
void Fast_Parser_pushReturnAsLvalue(Fast_Parser _this, cx_function function);

/* ::cortex::Fast::Parser::pushScope() */
Fast_Variable Fast_Parser_pushScope(Fast_Parser _this);

/* ::cortex::Fast::Parser::reset() */
void Fast_Parser_reset(Fast_Parser _this);

/* ::cortex::Fast::Parser::ternaryExpr(Fast::Expression cond,Fast::Expression iftrue,Fast::Expression iffalse) */
Fast_Expression Fast_Parser_ternaryExpr(Fast_Parser _this, Fast_Expression cond, Fast_Expression iftrue, Fast_Expression iffalse);

/* ::cortex::Fast::Parser::unaryExpr(Fast::Expression lvalue,lang::operatorKind operator) */
Fast_Expression Fast_Parser_unaryExpr(Fast_Parser _this, Fast_Expression lvalue, cx_operatorKind operator);

/* ::cortex::Fast::Parser::updateStatement(Fast::Expression expr,Fast::Block block) */
Fast_Node Fast_Parser_updateStatement(Fast_Parser _this, Fast_Expression expr, Fast_Block block);

/* ::cortex::Fast::Parser::waitExpr(list{Fast::Expression} exprList,Fast::Expression timeout) */
Fast_Expression Fast_Parser_waitExpr(Fast_Parser _this, Fast_Expression_list exprList, Fast_Expression timeout);

/* ::cortex::Fast::Parser::whileStatement(Fast::Expression condition,Fast::Block trueBranch,lang::bool isUntil) */
Fast_Node Fast_Parser_whileStatement(Fast_Parser _this, Fast_Expression condition, Fast_Block trueBranch, cx_bool isUntil);

#ifdef __cplusplus
}
#endif
#endif

