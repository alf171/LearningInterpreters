# Chapter 17 - Compiler Expressions

- three goals this chapter
    1. provides the final segment of our VMs execution pipeline
      - once in place, we can plumb the user's source code from scan -> execution
    2. second we get raw instructions from our compiler
      - it isn't native instructions for for chip, but it's way closer to metal
    3. we get to use "top-down operator precedence parsing"
      - handle prefix, postfix, infix, mixfix operators
- pratt's parsing algorithm is recursive so can be hard to wrap head around
- single pass compilation
  - compiler has two jobs
  1. parses user source to understand what it means
  2. produces low level instructions with the same semantics
  - many languages/compilers split these into their own passes
  - in clox, we'll do the more traditional of just having one pass
- let's try to get
  - number literals, parentheses for grouping, unary negation, and math (+,-,/,*)
- we evaluate the operand first which its value on the stack
- pop the value, negate it and push it back
- consider -a.b
  - currently we apply unary negate to a
  - we fixed this in jlox by using recursive descent parser
  - we'll write a method to decide precedence
- pratt parser sort of goes like this
  - first, parsePrecedence(), we look up a prefix parser for the current Token
  - first token will always be a prefix expression by def
  - may turn out to be nested as an operand inside one or more infix expression
  - after parsing, the prefix expression is done
  - now we look for an infix parser for the next token
  - if we find one, it means the prefix expression might be an operand for it
  - but only if the call to parsePrecedence has a precedence that is low enough \
  to permit the infix operator
  - if the next token is too low precedence, or isn't an infix operator, then we are done
  - we've parsed as much as we can
  - we we consume operator and hand of control to the infix we found
  - we keep looping like that crunching through infix operators
