# Chapter 23 - Jumping Back and Forth

- we can finally add control flow to our VM
- for jlox, we use java's control flow
- for a VM, we need some sort of byte code representation to handle this
- flow has to do with the path in which our vm runs
- in jlox, the locus of attnetion was implicit based on which ast nodes we are running
- in clox, it is more explicit since we track with our ip
  - seems like all we need to do is mutate our ip
- if (a) print(b);
  - if a is true, we run the next instruction, but if false, we skip over it
  - else statement adds some complexity but anyway
- at the end of the day, the compiler lowers down to raw jumps
  - this is actually goto statements
- add compiler support for if statemnt
- keep the condition value on the top of stack
- emit a new instruction `OP_JUMP_IF_FALSE`
  - has an operaound to decide how much to jump
- but we dont know exactly how much do jump do we
- backpatching
  - submit a jump with a placeholder offset
  - next, we compile how far we go
  - then we recalculate get our jump location

```{c}
static int emitJump(uint8_t instruction) {
  emitByte(instruction);
  emitByte(0xff);
  emitByte(0xff);
  return currentChunk()->count - 2;
}
```

- 16 bit offset lets us jump over up to 65,536 bytes of code
- we then need to calculate offset to figure out where we should've really jumped to
- to support else, we could just add a statement if we see else
- problem is, the if case would then go into the else
- so that needs a second jump for that
- Logical operators
  - and and or are just another binary operators like +/-
  - but since they short circuit they actual behave more like control flow
  - little if/else variants if you think about it
  - OR is slightly more complicated short circuit
    - left: `OP_JUMP_IF_FALSE -> OP_POP or OP_JUMP -> CONTINUE`
  - implementing ORs and ANDs like this add more overhead since we need more instructions
  - but this is a nice excercise of building things from smaller pieces
