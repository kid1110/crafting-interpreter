#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "table.h"
#include "value.h"
#include "object.h"
#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX*UINT8_COUNT)

typedef struct
{
    ObjClosure* closure;
    uint8_t* ip;
    Value* slots;
}CallFrame;


typedef struct 
{
    CallFrame frames[FRAMES_MAX];
    int frameCount;
    Value stack[STACK_MAX];
    Value* stackTop;
    Table strings;
    Obj* objects;
    Table globals;
    ObjUpvalue* openUpvalues;
    int grayCount;
    int grayCapacity;
    Obj** grayStack;
    size_t bytesAllocated;
    size_t nextGC;
    ObjString* initString;
}VM;





typedef enum{
    INTERPRET_OK,
    INTERPRET_COMILE_ERROR,
    INTERPRET_RUNTIME_ERROR,

}InterpretResult;

extern VM vm;

void initVM();
void freeVM();
void push(Value value);
Value pop();
InterpretResult interpret(const char* source);

#endif