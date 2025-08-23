# Chapter 14 - Chunks of ByteCode

- jlox is pretty slow
- time to build our own virtual machine from scratch
- using nothing more than the C standard library
- also, turns out walking the AST isn't ideal
- next, we will build clox
  - before we relied on the jvm to do many things
- tree walking interpreter is fine for some kind of high level declarative language
- for general purpose imperative language, it wont fly
- fib(40) = 72 seconds, C is like 0.5 seconds
  - dynamically typed scripting language is never going to be as fast a statically type
  - with memory management but two orders of magnitude is too slow
- rethink our model
- Bytecode
  - why not walk the AST
    - very simple to implement
    - it's portable >> runs on any java platform
    - it's not memory efficent. Each piece of syntax becomes an AST node
    - 1 + 2 is
      - Expr.Binary
        - left: Expr.Literal -> double (1.0)
        - operator -> Token -> String ("+")
        - right: Expr.Literal -> double (2.0)
    - each pointer adds 32/64 bits of overhead to the object
    - also, objects are spread across the heap out
    - thus, we don't benefit from spatial locality
    - modern processor can process data quicker than they can load it
    - if the data is cached, we can fetch it up to ~100 times quicker
    - machine (CPU) does this with speculative decoding
    - pull adjacent things into RAM
  - why not compile to native code?
    - this is what the fastest languages do
    - native code is a dense series of operations encoded directly in binary
    - each instruction is between one and a few bytes long
    - most chips have old architectures with heaps of instructions accrued over the years
    - sophisticated register allocation, pipelining, and instruction scheduling
    - and also portability is out
      - spend some time mastering one for a new one to come along
    - you need to write a separate backend for each one
    - LLVM can act as this middle ground where if you create the IR, it can do rest
  - what is byte code?
    - bytecode is a bit in the middle of  tree-walk interpreter and native impl
    - remains portable, sacrifices some simplicity to get performance boost
    - bytecode resembles machine code
    - dense linear sequence of binary instructions
      - keeps overhead low and plays nice with cache
    - in many implementations, each instruction is only one byte long
    - problem is this architecture doesn't exist
    - solve this by writing an emulator << a virtual machine (VM)
    - adds overhead which is why byte code is slower than native
    - but it gives us portability
    - following: python, ruby, lua, ocaml, erlang, and others
- layout: Front End, Representation, Execution
- jlox: Parser -> Syntax Trees -> Interpreter
- clox: Compiler -> Bytecode -> Virtual Machine
- in our bytecode format,  each instruction has a one-bye operation code (opcode)
- that number controls what kind of instruction we are dealing with
- byte code is a series of instructions
- well store other data along with the code eventually
- since we don't know how big the array needs to be before we start compiling a chunk
  - the array must be dynamic
- dynamic arrays provide
    1. Cache-friendly, dense storage
    2. Constant-time indexed element lookup
    3. Constant-time appending to the end of the array
- we used these in jlox too by using Java's ArrayList class
- in c, we'll roll our own
  - we'll keep track of code, number in the array allocated and how many are used in count
  - when we add an element and we dont have capacity (i.e. count == capacity, we resize)
Steps
    1. Allocated a new array with more capacity
    2. Copy the existing elements from the old array to the new one
    3. Store the new capacity
    4. Delete the old array
    5. Update code to point to new array
    6. Store the element in the new array (since there is room)
    7. update the count
- C doesn't have constructs so we declare a function to initialize a new chunk
- okay so now we can run stuff by pushing things into memory
- but we cant tell if it's working
- for that we need a disassembler > assembler switch name to ADD MULT
- disassembler goes other way around
- for now, well only support double precision floating point number
  - abstract how values are represented in Clox `typedef double Value;`
  - For small fixed-sized values like ints, many instructions store \
    in the code stream right after the opcode
  - these are called immediate instructions
    - instructions are immediately after the opcode
- doesn't work well for strings or large variable sized things
- In native compilers, we store in a separate "constant data" region
- then we use address + offset pointing to where the value is stored in section
- jvm associates a constant pool with each compiled class
- for clox, each chunk will carry a list the values that appear as literals in program
  - keep things simple, we'll put everything in there even ints
- OP_CONST requires 2 bytes instead of 1 or return
  - 1 byte for opcode, 1 for constant index
  - the op code will define how many bytes we need to read
- we'll need to store lines so when a runtime error occurs, we can give source
- very inefficient approach in terms of memory
  - each chunk contains the number in the array corresponding to the bytecode
  - when runtime error occurs, we use it to lookup our line
