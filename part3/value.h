#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum {
  VAL_BOOL,
  VAL_NIL,
  VAL_NUMBER,
  VAL_OBJ,
} ValueType;

#define IS_BOOL(v) ((v).type == VAL_BOOL)
#define IS_NIL(v) ((v).type == VAL_NIL)
#define IS_NUMBER(v) ((v).type == VAL_NUMBER)
#define IS_OBJ(v) ((v).type == VAL_OBJ)
#define IS_STRING(v) isObjType(v, OBJ_STRING)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value) ((value).as.obj)
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

#define BOOL_VAL(b) ((Value){VAL_BOOL, {.boolean = b}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(n) ((Value){VAL_NUMBER, {.number = n}})
#define OBJ_VAL(objPtr) ((Value){VAL_OBJ, {.obj = (Obj *)objPtr}})

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
    Obj *obj;
  } as;
} Value;

typedef struct {
  int capacity;
  int count;
  Value *values;
} ValueArray;

bool valuesEqual(Value a, Value b);
void initValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);
void freeValueArray(ValueArray *array);
void printValue(Value value);

#endif
