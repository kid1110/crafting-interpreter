#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef enum
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBR,

} ValueType;

typedef struct
{
    ValueType type;
    union
    {
        bool boolean;
        double number;
    } as;
} Value;

typedef struct
{
    int capacity;
    int count;
    Value *values;
} ValueArray;






#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBR)
#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL      (Value){VAL_BOOL,{.number = 0}}
#define NUMBER_VAL(value)     (Value){VAL_NUMBR,{.number = value}}

void initValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value Value);
void freeValueArray(ValueArray *array);
void printValue(Value value);
bool valuesEqual(Value a,Value b);
#endif