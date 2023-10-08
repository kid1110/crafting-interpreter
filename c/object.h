#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"
#include "chunk.h"

#define AS_FUNCTION(value) ((ObjFunction*) AS_OBJ(value))
#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_STRING(value) isObjType(value,OBJ_STRING)
#define AS_STRING(value) (((ObjString*)AS_OBJ(value)))
#define AS_CLOSURE(value) ((ObjClosure*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)
#define IS_CLOSURE(value) isObjType(value,OBJ_CLOSURE)
#define IS_FUNCTION(value) isObjType(value,OBJ_FUNCTION);
#define IS_NATIVE(value) isObjType(value,OBJ_NATIVE);
#define AS_NATIVE(value) \
    (((ObjNative*)AS_OBJ(value))->function)
typedef enum{
    OBJ_STRING,
    OBJ_FUNCTION,
    OBJ_NATIVE,
    OBJ_CLOSURE,
    OBJ_UPVALUE
}ObjType;




struct Obj{
    ObjType type;
    bool isMarked;
    struct Obj* next;
};
typedef struct ObjUpvalue{
    Obj obj;
    Value* location;
    Value closed;
    struct ObjUpvalue* next;
}ObjUpvalue;
typedef struct 
{
    Obj obj;
    int arity;
    Chunk chunk;
    int upvalueCount;
    ObjString* name;
    
}ObjFunction;

typedef struct 
{
    Obj obj;
    ObjFunction* function;
    ObjUpvalue** upvalues;
    int upvalueCount;
}ObjClosure;

typedef Value (*NativeFn)(int argCount,Value* args);
typedef struct 
{
    Obj obj;
    NativeFn function;

}ObjNative;


struct  ObjString
{
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
};
ObjFunction* newFunction();
static inline bool isObjType(Value value,ObjType type){
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

ObjString* takeString(char* chars,int length);
ObjString* copyString(const char* chars,int length);
ObjClosure* newClosure(ObjFunction* function);
ObjNative* newNative(NativeFn function);
ObjUpvalue* newUpvalue(Value* slot);
void printObject(Value value);
#endif