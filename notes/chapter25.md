# Chatper 25 - Closures

- right now a function has no way to reference a variable outside of its scope
  - except for global variables that is
- before we thought of variables as having stack semantics
  - variables are discared in the reverse order they are created
- closure doesn't totally follow
- jlox solves this by allocating memory for all local variables -- envs
- throw it all on the heap but we are using the stack
- new approach
  - for locals without closure, keep on stack
  - when local is captured in closure, move them onto heap so they can live longer
  - this design comes from the Lua VM
  - not much code and pretty fast
  - suited for single pass compiler
- Closure objects
  - before we treated functions like strings, or other literals
  - the issue is that closure can't have this
  - consider a function which returns a function
    - if called twice, it must hold some representation inside
    - otherwise they would need same output
- our current ObjFunction type represents a "raw" compile-time state
  - we are going to wrap the ObjFunction in a new ObjClosure struct
  - access to bare function + runtime state
    - a little wasteful but let's do this for all functions
  - we only free the ObjClosure itself since the closure doesn't own the function
  - we can't free ObFunction until all obejcts referencing are gone
  - including surrounding functions who's constant table refer to it
  - we'll use a gc to track this
- from a user perspective, the difference between ObjFunction and ObjClosure is hidden detail
- most of work will be at runtime
- a CallFrame will now hold a ObjClosure instead of an ObjFunction
- oddly enough, we need to push the raw function onto the stack too to avoid gc problems
- UpValues
  - our read/write to local variables are limited to a single function's stack window
  - locals form surrounding functions are outside the inner function's window
  - imagine an instruction which takes a relative stack slot offset before the cur fun's window
    - but variables can outlive functions
  - what about have any local var always on the heap
    - having a single pass compiler makes this hard
  - thanks to luadev team, we use a level of indirection called upvalues
    - an up value refers to a local variable in an eclosed function
    - every closure maintains an array of upvalues
    - the up value points back into the stack where the variable it captures lives
    - when the closure needs to access a closed-over variable, it goes through the upvalue
    - when the we creates a function, the vm creates the array of upvalues and wires them

```{clox}
var a = 3;
fun f() {
  print a;
}
```

- ObjClosure will contains the upvalue array which will reference the location of a (3)
- compiling up values
  - as usual, we want to do as much work compile time
  - currently, we walk local -> global searching for a variable in scope
  - we don't consider the local scopes of enclosing functions
- one small note, we dont want to generate multiple up values if we reference same closure var
  - so we check it already exists before creating new one
  - we will keep track of isLocal so that we can thread upvalues through nested funs
- one last thing we need, we need to hoist variables off the stack
  - since they dont have stack sematincs if we return a function for example
  - which holds closure
- closure captures variables not values
  - you can think of them as capturing the place the value lives
- closing upvalues
  - following lua
  - open upvalue = local variable still on stack
  - closed upvalue = local variable moved to the heap
  - `ObjUpvalue` referes to a value on the heap
  - we can move back to stack when variable goes out of scope
- general functionality goal, we only want ppl to pay for features if they use them
- variables that dont use closure live and die on stack like before
- right now, if we run `OP_CLOSE_UPVALUE` to move stack -> heap but it's shared
  - we get an orphaned value
- to fix, when vm needs an upvalue that captures a local slot, we will search for existing
- if found, we will reuse that
- but finding the memory isn't easy rn
  - give the vm its own list of all open upvalues: point to variables still on stack
  - in practice, searching this list isnt too bad
  - number of variables on stack tends to be small
  - also function declarations that create closure are rarely on performance crtiical exec path
  - we can order by the slot index
    - allows us to short circuit
  - linked list it is
  - three ways to exit
    1. local slot we stopped at is one we are looking for
    2. we ran out of upvalues to search
    3. we found an upvalue whose local slot is belo the one we are looking for
- a large fraction of "optimization" is about
  - adding special case code that detects certain patterns
  - provides a custom =-build faster path for the code that fits the pattern
