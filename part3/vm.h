#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "common.h"
#include "object.h"
#include "table.h"
#include <stdint.h>

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct {
  ObjClosure *closure;
  uint8_t *ip;
  Value *slots;
} CallFrame;

typedef struct {
  CallFrame frames[FRAMES_MAX];
  int frameCount;

  Value stack[STACK_MAX];
  Value *stackTop;
  Table globals;
  Table strings;
  ObjString *initString;
  ObjUpvalue *openUpvalues;

  size_t byteAllocated;
  size_t nextGC;

  Obj *objects;
  int grayCount;
  int grayCapacity;
  Obj **grayStack;
} VM;

// Compile-time sanity:
_Static_assert(sizeof(Value) == 8, "NaN-boxing requires 64-bit Value");
_Static_assert(sizeof(double) == 8, "NaN-boxing requires 64-bit double");

// Quick runtime self-test (call once at startup in DEBUG):
static inline void __value_self_test(void) {
  Value vN = NUMBER_VAL(42.0);
  Value vT = TRUE_VAL, vF = FALSE_VAL, vZ = NIL_VAL;
  Obj dummy;
  Value vO = OBJ_VAL(&dummy);

  assert(IS_NUMBER(vN) && !IS_BOOL(vN) && !IS_OBJ(vN));
  assert(IS_BOOL(vT) && AS_BOOL(vT) == true);
  assert(IS_BOOL(vF) && AS_BOOL(vF) == false);
  assert(IS_NIL(vZ));
  assert(IS_OBJ(vO) && AS_OBJ(vO) == &dummy);
}

typedef enum {
  INTERPRET_OK,
  INTERPRET_COPMILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char *source);
void push_fn(Value value);
Value pop_fn();

#endif
