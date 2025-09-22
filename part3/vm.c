#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#if !defined(_MSC_VER)
#define THREADED_DISPATCH 1
#endif

VM vm;

static Value clockNative(int argCount, Value *args) {
  return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

void resetStack() {
  vm.stackTop = vm.stack;
  vm.frameCount = 0;
  vm.openUpvalues = NULL;
}

__attribute__((cold)) static void runtimeError(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  for (int i = vm.frameCount - 1; i >= 0; i--) {
    CallFrame *frame = &vm.frames[i];
    ObjFunction *function = frame->closure->function;
    size_t instruction = frame->ip - function->chunk.code - 1;
    fprintf(stderr, "[line %d] in ", function->chunk.lines[instruction]);
    if (function->name == NULL) {
      fprintf(stderr, "script\n");
    } else {
      fprintf(stderr, "%s()\n", function->name->chars);
    }
  }
  resetStack();
}

static void defineNative(const char *name, NativeFn function) {
  push_fn(OBJ_VAL(copyString(name, (int)strlen(name))));
  push_fn(OBJ_VAL(newNative(function)));
  tableSet(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
  pop_fn();
  pop_fn();
}

static Value peek_fn(int distance) { return vm.stackTop[-1 - distance]; }

static void defineMethod(ObjString *name) {
  Value method = peek_fn(0);
  ObjClass *klass = AS_CLASS(peek_fn(1));
  tableSet(&klass->methods, name, method);
  pop_fn();
}

static bool call(ObjClosure *closure, int argCount) {
  if (argCount != closure->function->arity) {
    runtimeError("Expected %d arguments but got %d", closure->function->arity,
                 argCount);
    return false;
  }

  if (vm.frameCount == FRAMES_MAX) {
    runtimeError("Stack Overflow");
    return false;
  }

  CallFrame *frame = &vm.frames[vm.frameCount++];
  frame->closure = closure;
  frame->ip = closure->function->chunk.code;

  frame->slots = vm.stackTop - argCount - 1;
  return true;
}

static bool callValue(Value callee, int argCount) {
  if (IS_OBJ(callee)) {
    switch (OBJ_TYPE(callee)) {
      case OBJ_BOUND_METHOD: {
        ObjBoundMethod *bound = AS_BOUND_METHOD(callee);
        vm.stackTop[-argCount - 1] = bound->receiver;
        return call(bound->method, argCount);
      }
      case OBJ_CLASS: {
        ObjClass *klass = AS_CLASS(callee);
        Value initializer;
        vm.stackTop[-argCount - 1] = OBJ_VAL(newInstance(klass));
        if (tableGet(&klass->methods, vm.initString, &initializer)) {
          return call(AS_CLOSURE(initializer), argCount);
        } else if (argCount != 0) {
          runtimeError("Expected 0 arguments but got %d", argCount);
          return false;
        }
        return true;
      }
      case OBJ_CLOSURE:
        return call(AS_CLOSURE(callee), argCount);
      case OBJ_NATIVE: {
        NativeFn native = AS_NATIVE(callee);
        Value result = native(argCount, vm.stackTop - argCount);
        vm.stackTop -= argCount + 1;
        push_fn(result);
        return true;
      }

      default:
        // non callable object type
        break;
    }
  }

  runtimeError("Can only call functions and classes");
  return false;
}

static bool invokeFromClass(ObjClass *klass, ObjString *name, int argCount) {
  Value method;
  if (!tableGet(&klass->methods, name, &method)) {
    runtimeError("Undefined Property '%s'", name->chars);
    return false;
  }

  return call(AS_CLOSURE(method), argCount);
}

static bool invoke(ObjString *name, int argCount) {
  Value receiver = peek_fn(argCount);

  if (!IS_INSTANCE(receiver)) {
    runtimeError("Only instances have methods");
    return false;
  }

  ObjInstance *instance = AS_INSTANCE(receiver);

  Value value;
  if (tableGet(&instance->fields, name, &value)) {
    vm.stackTop[-argCount - 1] = value;
    return callValue(value, argCount);
  }

  return invokeFromClass(instance->klass, name, argCount);
}

static ObjUpvalue *capturedUpvalue(Value *local) {
  ObjUpvalue *prevUpvalue = NULL;
  ObjUpvalue *upvalue = vm.openUpvalues;

  while (upvalue != NULL && upvalue->location > local) {
    prevUpvalue = upvalue;
    upvalue = upvalue->next;
  }

  if (upvalue != NULL && upvalue->location == local) {
    return upvalue;
  }

  ObjUpvalue *createdUpvalue = newUpvalue(local);

  if (prevUpvalue == NULL) {
    vm.openUpvalues = createdUpvalue;
  } else {
    prevUpvalue->next = createdUpvalue;
  }

  return createdUpvalue;
}

static void closeUpvalues(Value *last) {
  while (vm.openUpvalues != NULL && vm.openUpvalues->location >= last) {
    ObjUpvalue *upvalue = vm.openUpvalues;
    upvalue->closed = *upvalue->location;
    upvalue->location = &upvalue->closed;
    vm.openUpvalues = upvalue->next;
  }
}

static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
  ObjString *b = AS_STRING(peek_fn(0));
  ObjString *a = AS_STRING(peek_fn(1));

  int length = a->length + b->length;
  push_fn(OBJ_VAL(a));
  push_fn(OBJ_VAL(b));

  char *chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';
  ObjString *result = takeString(chars, length);
  pop_fn();
  pop_fn();
  pop_fn();
  pop_fn();
  push_fn(OBJ_VAL(result));
}

// explicitly to support var a = instance.method; a();
static bool bindMethod(ObjClass *klass, ObjString *name) {
  Value method;
  if (!tableGet(&klass->methods, name, &method)) {
    runtimeError("Undefined property '%s'", name->chars);
    return false;
  }

  ObjBoundMethod *bound = newBoundMethod(peek_fn(0), AS_CLOSURE(method));
  pop_fn();
  push_fn(OBJ_VAL(bound));
  return true;
}

void initVM() {
  resetStack();
  vm.objects = NULL;
  vm.byteAllocated = 0;
  vm.nextGC = 1024 * 1024;

  vm.grayCount = 0;
  vm.grayCapacity = 0;
  vm.grayStack = NULL;

  initTable(&vm.globals);
  initTable(&vm.strings);

  // avoid getting GC'ed before getting allocated
  vm.initString = NULL;
  vm.initString = copyString("init", 4);

  defineNative("clock", clockNative);

#ifdef DEBUG_TRACE_EXECUTION
  __value_self_test();
#endif
}

void freeVM() {
#ifdef DEBUG_LOG_GC
  fprintf(stderr, "freeVM: free globals\n");
#endif
  freeTable(&vm.globals);
#ifdef DEBUG_LOG_GC
  fprintf(stderr, "freeVM: free strings\n");
#endif
  freeTable(&vm.strings);
#ifdef DEBUG_LOG_GC
  fprintf(stderr, "freeVM: free objects\n");
#endif
  vm.initString = NULL;
  freeObjects();
}

static const char *valueTypeName(Value v) {
  if (IS_BOOL(v))
    return "bool";
  if (IS_NIL(v))
    return "nil";
  if (IS_NUMBER(v))
    return "number";
  if (IS_OBJ(v)) {
    switch (OBJ_TYPE(v)) {
      case OBJ_STRING:
        return "string";
      case OBJ_FUNCTION:
        return "function";
      case OBJ_NATIVE:
        return "native";
      case OBJ_CLOSURE:
        return "closure";
      case OBJ_UPVALUE:
        return "upvalue";
      case OBJ_CLASS:
        return "class";
      case OBJ_INSTANCE:
        return "instance";
      case OBJ_BOUND_METHOD:
        return "bound method";
    }
  }
  return "unknown";
}

static void debugTag(const char *label, Value v, const char *name) {
  const char *t = IS_OBJ(v)      ? "OBJ"
                  : IS_NUMBER(v) ? "NUM"
                  : IS_BOOL(v)   ? "BOOL"
                  : IS_NIL(v)    ? "NIL"
                                 : "?";
  fprintf(stderr, "%s %s: tag=%s\n", label, name, t);
}

static inline __attribute__((always_inline)) uint8_t readByte(uint8_t **pip) {
  return *(*pip)++;
}

static inline __attribute__((always_inline)) uint16_t readShort(uint8_t **pip) {
  uint8_t *p = *pip;
  uint16_t s = (uint16_t)((p[0] << 8) | p[1]);
  *pip = p + 2;
  return s;
}

static InterpretResult run() {
  uint8_t *ip;
  register Value *stackTop = vm.stackTop;
  CallFrame *frame = &vm.frames[vm.frameCount - 1];
  ip = frame->ip;
#define WITH_VM_STACK(expr)                                                    \
  do {                                                                         \
    vm.stackTop = stackTop;                                                    \
    expr;                                                                      \
    stackTop = vm.stackTop;                                                    \
  } while (0)
#define PUSH(v) (*stackTop++ = (v))
#define POP() (*--stackTop)
#define PEEK(n) (stackTop[-1 - (n)])
#define SYNC_OUT() (frame->ip = ip, vm.stackTop = stackTop)
#define SYNC_IN()                                                              \
  do {                                                                         \
    frame = &vm.frames[vm.frameCount - 1];                                     \
    ip = frame->ip;                                                            \
    stackTop = vm.stackTop;                                                    \
  } while (0)
#define SYNC_STACK_OUT() (vm.stackTop = stackTop)
#define READ_BYTE() readByte(&ip)
#define READ_SHORT() readShort(&ip)
#define READ_CONSTANT()                                                        \
  (frame->closure->function->chunk.constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define BINARY_OP(valueType, op)                                               \
  do {                                                                         \
    if (!IS_NUMBER(PEEK(0)) || !IS_NUMBER(PEEK(1))) {                          \
      runtimeError("Operands must be numbers");                                \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double b = AS_NUMBER(POP());                                               \
    double a = AS_NUMBER(POP());                                               \
    PUSH(valueType(a op b));                                                   \
  } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value *slot = vm.stack; slot < stackTop; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(&frame->closure->function->chunk,
                           (int)(ip - frame->closure->function->chunk.code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        PUSH(constant);
        break;
      }
      case OP_NIL: {
        PUSH(NIL_VAL);
        break;
      }
      case OP_TRUE: {
        PUSH(BOOL_VAL(true));
        break;
      }
      case OP_FALSE: {
        PUSH(BOOL_VAL(false));
        break;
      }
      case OP_POP: {
        POP();
        break;
      }
      case OP_GET_LOCAL: {
        uint8_t slot = READ_BYTE();
        PUSH(frame->slots[slot]);
        break;
      }
      case OP_GET_GLOBAL: {
        ObjString *name = READ_STRING();
        Value value;
        if (!tableGet(&vm.globals, name, &value)) {
          runtimeError("Undefined variable '%s'", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
#ifdef DEBUG_PRINT_CODE
        debugTag("get_global", value, name->chars);
#endif
        PUSH(value);
        break;
      }
      case OP_DEFINE_GLOBAL: {
        ObjString *name = READ_STRING();
        SYNC_STACK_OUT();
        tableSet(&vm.globals, name, PEEK(0));
#ifdef DEBUG_PRINT_CODE
        debugTag("define_global", PEEK(0), name->chars);
#endif
        POP();
        break;
      }
      case OP_SET_LOCAL: {
        uint8_t slot = READ_BYTE();
        frame->slots[slot] = PEEK(0);
        break;
      }
      case OP_SET_GLOBAL: {
        ObjString *name = READ_STRING();
        SYNC_STACK_OUT();
        if (tableSet(&vm.globals, name, PEEK(0))) {
          tableDelete(&vm.globals, name);
          runtimeError("Undefined variable '%s'", name->chars);
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_GET_UPVALUE: {
        uint8_t slot = READ_BYTE();
        PUSH(*frame->closure->upvalues[slot]->location);
        break;
      }
      case OP_SET_UPVALUE: {
        uint8_t slot = READ_BYTE();
        *frame->closure->upvalues[slot]->location = PEEK(0);
        break;
      }
      case OP_GET_PROPERTY: {
        if (!IS_INSTANCE(PEEK(0))) {
          runtimeError("Only instances have properties");
          return INTERPRET_RUNTIME_ERROR;
        }
        ObjInstance *instance = AS_INSTANCE(PEEK(0));
        ObjString *name = READ_STRING();

        Value value;
        if (tableGet(&instance->fields, name, &value)) {
          POP(); // remove the instance
          PUSH(value);
          break;
        }

        WITH_VM_STACK(if (!bindMethod(instance->klass, name)) {
          return INTERPRET_RUNTIME_ERROR;
        });
        break;
      }
      case OP_SET_PROPERTY: {
        if (!IS_INSTANCE(PEEK(1))) {
          runtimeError("Only instances have fields");
          return INTERPRET_RUNTIME_ERROR;
        }

        ObjInstance *instance = AS_INSTANCE(PEEK(1));
        SYNC_STACK_OUT();
        tableSet(&instance->fields, READ_STRING(), PEEK(0));

        // print x = 5 -- should print 5
        Value value = POP();
        POP();
        PUSH(value);
        break;
      }
      case OP_GET_SUPER: {
        ObjString *name = READ_STRING();
        ObjClass *superclass = AS_CLASS(POP());
        if (!bindMethod(superclass, name)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_EQUAL: {
        PUSH(BOOL_VAL(valuesEqual(POP(), POP())));
        break;
      }
      case OP_GREATER: {
        BINARY_OP(BOOL_VAL, >);
        break;
      }
      case OP_LESS: {
        BINARY_OP(BOOL_VAL, <);
        break;
      }
      case OP_ADD: {
        if (IS_STRING(PEEK(0)) && IS_STRING(PEEK(1))) {
          WITH_VM_STACK(concatenate());
        } else if (IS_NUMBER(PEEK(0)) && IS_NUMBER(PEEK(1))) {
          double b = AS_NUMBER(POP());
          double a = AS_NUMBER(POP());
          PUSH(NUMBER_VAL(a + b));
        } else {
          runtimeError("Operands must be two numbers or two string");
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_SUBTRACT: {
        BINARY_OP(NUMBER_VAL, -);
        break;
      }
      case OP_MULTIPLY: {
        BINARY_OP(NUMBER_VAL, *);
        break;
      }
      case OP_DIVIDE: {
        BINARY_OP(NUMBER_VAL, /);
        break;
      }
      case OP_NOT: {
        PUSH(BOOL_VAL(isFalsey(POP())));
        break;
      }
      case OP_NEGATE: {
        if (!IS_NUMBER(PEEK(0))) {
          runtimeError("Operand must be a number");
          return INTERPRET_RUNTIME_ERROR;
        }
        PUSH(NUMBER_VAL(-AS_NUMBER(POP())));
        break;
      }
      case OP_PRINT: {
        printValue(POP());
        printf("\n");
        break;
      }
      case OP_JUMP: {
        uint16_t offset = READ_SHORT();
        ip += offset;
        break;
      }
      case OP_JUMP_IF_FALSE: {
        uint16_t offset = READ_SHORT();
        if (isFalsey(PEEK(0))) {
          ip += offset;
        }
        break;
      }
      case OP_LOOP: {
        uint16_t offset = READ_SHORT();
        ip -= offset;
        break;
      }
      case OP_CALL: {
        int argCount = READ_BYTE();
        SYNC_OUT();
        if (!callValue(PEEK(argCount), argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        SYNC_IN();
        break;
      }
      case OP_SUPER_INVOKE: {
        ObjString *method = READ_STRING();
        int argCount = READ_BYTE();
        ObjClass *superclass = AS_CLASS(POP());
        SYNC_OUT();
        if (!invokeFromClass(superclass, method, argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        SYNC_IN();
        break;
      }
      case OP_INVOKE: {
        ObjString *method = READ_STRING();
        int argCount = READ_BYTE();
        SYNC_OUT();
        if (!invoke(method, argCount)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        SYNC_IN();
        break;
      }
      case OP_CLOSURE: {
        ObjFunction *function = AS_FUNCTION(READ_CONSTANT());
        vm.stackTop = stackTop;
        ObjClosure *closure = newClosure(function);
        PUSH(OBJ_VAL(closure));
        vm.stackTop = stackTop;
        for (int i = 0; i < closure->upvalueCount; i++) {
          uint8_t isLocal = READ_BYTE();
          uint8_t index = READ_BYTE();
          if (isLocal) {
            closure->upvalues[i] = capturedUpvalue(frame->slots + index);
          } else {
            closure->upvalues[i] = frame->closure->upvalues[index];
          }
        }
        break;
      }
      case OP_CLOSE_UPVALUE: {
        closeUpvalues(stackTop - 1);
        POP();
        break;
      }
      case OP_CLASS: {
        vm.stackTop = stackTop;
        PUSH(OBJ_VAL(newClass(READ_STRING())));
        vm.stackTop = stackTop;
        break;
      }
      case OP_INHERIT: {
        Value superclass = PEEK(1);
        if (!IS_CLASS(superclass)) {
          runtimeError("Superclass must be a class");
          return INTERPRET_RUNTIME_ERROR;
        }
        ObjClass *subclass = AS_CLASS(PEEK(0));
        tableAddAll(&AS_CLASS(superclass)->methods, &subclass->methods);
        POP(); // subclass
        break;
      }
      case OP_METHOD: {
        ObjString *name = READ_STRING();
        WITH_VM_STACK(defineMethod(name));
        break;
      }
      case OP_RETURN: {
        Value result = POP();
        closeUpvalues(frame->slots);

        vm.frameCount--;
        if (vm.frameCount == 0) {
          POP();
          return INTERPRET_OK;
        }

        // Restore caller
        stackTop = frame->slots;
        PUSH(result);

        // Reload locals from caller
        frame = &vm.frames[vm.frameCount - 1];
        ip = frame->ip;
        break;
      }
    }
  }

#undef WITH_VM_STACK
#undef PUSH
#undef POP
#undef PEEK
#undef SYNC_OUT
#undef SYNC_IN
#undef SYNC_STACK_OUT
#undef READ_BYTE
#undef READ_SHORT
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP
}

InterpretResult interpret(const char *source) {
  ObjFunction *function = compile(source);
  if (function == NULL) {
    return INTERPRET_COPMILE_ERROR;
  }

  // push(OBJ_VAL(function));
  ObjClosure *closure = newClosure(function);
  push_fn(OBJ_VAL(closure));
  call(closure, 0);
  // callValue(OBJ_VAL(function), 0);

  return run();
}

void push_fn(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}

Value pop_fn() {
  vm.stackTop--;
  return *vm.stackTop;
}
