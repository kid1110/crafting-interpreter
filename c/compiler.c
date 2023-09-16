#include <stdio.h>
#include "common.h"
#include <stdlib.h>
#include "compiler.h"
#include "scanner.h"
#include "debug.h"
#include "value.h"
#include "object.h"

Parser parser;
Chunk *compilingChunk;



static Chunk *currentChunk();
static void errorAtCurrent(const char *message);
static void error(const char *message);
static void errorAt(Token *token, const char *message);
static void advance();
static void consume(TokenType type, const char *message);
static void emitByte(uint8_t byte);
static void endCompiler();
static void number();
static void emitReturn();
static void emitConstant(Value value);
static void emitBytes(uint8_t byte1, uint8_t byte2);
static void grouping();
static void unary();
static void parsePrecedence(Precedence precedence);
static void expression();
static uint8_t makeConstant(Value value);
static ParseRule* getRule(TokenType type);
static void binary();
static void string();
bool compile(const char *source, Chunk *chunk);
static void literal();
static void statement();
static void declaration();
static bool check(TokenType type);
static bool match(TokenType type);
static void printStatement();
static uint8_t parseVariable(const char* errorMessage);
static void variable();
static void expressionStatement();
static uint8_t identifierConstant();
static void synchronize();
static void defineVariable(uint8_t global);
static void namedVariable(Token name);
static void varDeclaration();

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_NONE},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {variable, NULL, PREC_NONE},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static void literal(){
    switch(parser.previous.type){
        case TOKEN_FALSE: emitByte(OP_FALSE);break;
        case TOKEN_TRUE: emitByte(OP_TRUE);break;
        case TOKEN_NIL: emitByte(OP_NIL);break;
        default: return;
    }
}
static void declaration(){
    if(match(TOKEN_VAR)){
        varDeclaration();
    }else{
        statement();
    }
    if(parser.panicMode) synchronize();
}
static void varDeclaration(){
    uint8_t global = parseVariable("Expect variable name.");
    if(match(TOKEN_EQUAL)){
        expression();
    }else{
        emitByte(OP_NIL);
    }
    consume(TOKEN_SEMICOLON,"Expect ';' after variable declaration.");
    defineVariable(global);
}
static void variable(){
    namedVariable(parser.previous);
}
static void namedVariable(Token name){
    uint8_t arg = identifierConstant(&name);
    emitBytes(OP_GET_GLOBAL,arg);
}
static uint8_t parseVariable(const char* errorMessage){
    
    consume(TOKEN_IDENTIFIER,errorMessage);
    return identifierConstant(&parser.previous);
}
static void defineVariable(uint8_t global){
    emitBytes(OP_DEFINE_GLOBAL,global);
}
static uint8_t identifierConstant(Token* name){
    return makeConstant(OBJ_VAL(copyString(name->start,name->length)));
}
static void synchronize(){
    parser.panicMode =  false;
    while (parser.current.type != TOKEN_EOF)
    {
        if(parser.previous.type == TOKEN_SEMICOLON) return;
        switch(parser.current.type){
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
                return;
            default:
                ;
        }
        advance();
    }
    
}
static void statement(){
    if(match(TOKEN_PRINT)){
        printStatement();
    }else{
        
        expressionStatement();
    }
}
static void expressionStatement(){
    expression();
    consume(TOKEN_SEMICOLON,"Expect ';' after expression.");
    emitByte(OP_POP);
}
static bool match(TokenType type){
    if(!check(type)) return false;
    advance();
    return true;
}
static bool check(TokenType type){
    return parser.current.type == type;
}
static void printStatement(){
    expression();
    consume(TOKEN_SEMICOLON,"Expect ';' aftr value.");
    emitByte(OP_PRINT);
}
static Chunk *currentChunk()
{
    return compilingChunk;
}
static void errorAtCurrent(const char *message)
{
    errorAt(&parser.current, message);
}
static void error(const char *message)
{
    errorAt(&parser.previous, message);
}
static void errorAt(Token *token, const char *message)
{
    if (parser.panicMode)
        return;
    parser.panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);
    if (token->type == TOKEN_EOF)
    {
        fprintf(stderr, "at end");
    }
    else if (token->type == TOKEN_ERROR)
    {
    }
    else
    {
        fprintf(stderr, " at '%.*s", token->length, token->start);
    }
    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}
static void advance()
{
    parser.previous = parser.current;
    for (;;)
    {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR)
            break;
        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char *message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }
    errorAtCurrent(message);
}
static void emitByte(uint8_t byte)
{
    writeChunk(currentChunk(), byte, parser.previous.line);
}
static void endCompiler()
{
    emitReturn();
    #ifdef DEBUG_PRINT_CODE
        if(!parser.hadError){
            disassembleChunk(currentChunk(),"code");
        }
    #endif
}


static void number()
{
    double value = strtod(parser.previous.start, NULL);
    emitConstant(NUMBER_VAL(value));
}
static void string(){
    emitConstant(OBJ_VAL(copyString(parser.previous.start+1,parser.previous.length-2)));
}
static void emitReturn()
{
    emitByte(OP_RETURN);
}

static void emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static uint8_t makeConstant(Value value)
{   
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk");
        return 0;
    }
    
    return (uint8_t)constant;
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}
static void grouping()
{
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression");
}
static void unary()
{
    TokenType operatorType = parser.previous.type;
    parsePrecedence(PREC_UNARY);
    switch (operatorType)
    {
    case TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;
    case TOKEN_BANG:
        emitByte(OP_NOT);break;
    default:
        return;
    }
}
static void parsePrecedence(Precedence precedence)
{
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if(prefixRule == NULL){
        error("Expect expression.");
        return;
    }
    prefixRule();
    while(precedence <= getRule(parser.current.type)->precedence){
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }

}
static void expression()
{   
    // TODO expression();
    parsePrecedence(PREC_ASSIGNMENT);
}
static ParseRule* getRule(TokenType type){
    return &rules[type];
}
static void binary()
{
    TokenType operatorType = parser.previous.type;
    ParseRule *rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));
    switch (operatorType)
    {
    case TOKEN_PLUS:
        emitByte(OP_ADD);
        break;
    case TOKEN_MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TOKEN_STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TOKEN_SLASH:
        emitByte(OP_DIVIDE);
        break;
    case TOKEN_BANG_EQUAL:
        emitBytes(OP_EQUAL,OP_NOT);break;
    case TOKEN_EQUAL_EQUAL:
        emitByte(OP_EQUAL);break;
    case TOKEN_GREATER:
        emitByte(OP_GREATER);break;
    case TOKEN_GREATER_EQUAL:
        emitBytes(OP_LESS,OP_NOT);break;
    case TOKEN_LESS:
        emitByte(OP_LESS);break;
    case TOKEN_LESS_EQUAL:
        emitBytes(OP_GREATER,OP_NOT);break;
    default:
        return;
    }
}
bool compile(const char *source, Chunk *chunk)
{
    initScanner(source);
    compilingChunk = chunk;
    parser.hadError = false;
    parser.panicMode = false;
    advance();
    while (!match(TOKEN_EOF))
    {   
        declaration();
    }
    
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    return !parser.hadError;
}

