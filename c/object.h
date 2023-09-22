#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"
#include "chunk.h"

#define AS_FUNCTION(value) ((ObjFunction*) AS_OBJ(value))
#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_STRING(value) isObjType(value,OBJ_STRING)
#define AS_STRING(value) (((ObjString*)AS_OBJ(value)))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)
#define IS_FUNCTION(value) isObjType(value,OBJ_FUNCTION);
#define IS_NATIVE(value) isObjType(value,OBJ_NATIVE);
#define AS_NATIVE(value) \
    (((ObjNative*)AS_OBJ(value))->function)
typedef enum{
    OBJ_STRING,
    OBJ_FUNCTION,
    OBJ_NATIVE
}ObjType;


struct Obj{
    ObjType type;
    struct Obj* next;
};
typedef struct 
{
    Obj obj;
    int arity;
    Chunk chunk;
    ObjString* name;
    
}ObjFunction;

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
ObjNative* newNative(NativeFn function);
void printObject(Value value);
#endif