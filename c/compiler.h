#ifndef clox_compiler_h
#define clox_compiler_h
#include "vm.h"
#include "scanner.h"
#include "common.h"
#include "object.h"

typedef void (*ParseFn)(bool canAssign);

typedef struct
{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

typedef enum
{
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     //
    PREC_UNARY,      //
    PREC_CALL,       // . ()
    PREC_PRIMARY
} Precedence;

typedef enum
{
    TYPE_FUNCTION,
    TYPE_SCRIPT,
    TYPE_METHOD,
    TYPE_INITIALIZER
} FunctionType;
typedef struct
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

typedef struct
{
    Token name;
    int depth;
    bool isCaptured;
} Local;
typedef struct{
    uint8_t index;
    bool isLocal;
}Upvalue;
typedef struct Compiler
{   
    struct Compiler* enclosing;
    ObjFunction *function;
    FunctionType type;
    Local locals[UINT8_COUNT];
    int localCount;
    int scopeDepth;
    Upvalue upvalues[UINT8_COUNT];
} Compiler;

typedef struct ClassCompiler{
    struct ClassCompiler* enclosing;
    bool hasSuperclass;

}ClassCompiler;

ObjFunction* compile(const char *source);
void markCompilerRoots();
#endif