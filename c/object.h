#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"
#include "chunk.h"
#include "table.h"


#define IS_BOUND_METHOD(value) isObjType(value,OBJ_BOUND_METHOD)
#define AS_BOUND_METHOD(value) ((ObjBoundMethod*)AS_OBJ(value))
#define IS_INSTANCE(value) isObjType(value,OBJ_INSTANCE)
#define AS_INSTANCE(value) ((ObjInstance*)AS_OBJ(value))
#define IS_CLASS(value) ((ObjClass*)AS_OBJ(value))
#define AS_CLASS(value) ((ObjClass*)AS_OBJ(value))
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
    OBJ_UPVALUE,
    OBJ_CLASS,
    OBJ_INSTANCE,
    OBJ_BOUND_METHOD
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

typedef struct {
    Obj obj;
    ObjString* name;
    Table methods;
}ObjClass;

typedef struct 
{
    Obj obj;
    ObjClass* klass;
    Table fields;
}ObjInstance;

typedef struct 
{
    Obj obj;
    Value receiver;
    ObjClosure* method;
}ObjBoundMethod;


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
ObjClass* newClass(ObjString* name);
ObjString* takeString(char* chars,int length);
ObjString* copyString(const char* chars,int length);
ObjClosure* newClosure(ObjFunction* function);
ObjNative* newNative(NativeFn function);
ObjUpvalue* newUpvalue(Value* slot);
ObjInstance* newInstance(ObjClass* klass);
ObjBoundMethod* newBoundMethod(Value receiver,ObjClosure* method);

void printObject(Value value);
#endif