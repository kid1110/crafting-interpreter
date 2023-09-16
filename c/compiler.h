#ifndef clox_compiler_h
#define clox_compiler_h
#include "vm.h"
#include "scanner.h"

typedef void (*ParseFn)();

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
typedef struct
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;



bool compile(const char* source,Chunk* chunk);
#endif