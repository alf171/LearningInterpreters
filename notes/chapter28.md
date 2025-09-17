# Chapter 28 - Methods and Initializers

- next we are going to add support for methods
- classses will hold a table where key is `method name` and value is `ObjClosure`
- it is slightly more complicated than java
  - bc we dont want to walk entire AST like jlox
  - doing all the binding by the vm would be really crazy -- encoded in its ops
    - it would have to all be done with `OP_CLASS`
  - instead we are going to do it all at runtime by the compiler
  - the vm needs three things to define a method
    1. the method name
    2. the closure for the method body
    3. the class to bind the method to
- in lox and other dynamic langs instead of `instance.method(arg)`
  - you can do `var closure = instance.method` and `closure(arg)`
  - since users can do them idependently, we need to break them apart
  - problem is we lose access to the instance in the vm so we need that to be stateful
- the vm currently stores state through local variables and temps on the stack
  - globals in a hash table
  - variables in closures use upvalues
  - how can we track method's receivers in clox and a new runtime type
- bound methods
  - we are going to wrap a closure in a "bound method"
  - tracks the instance that the method was accessed from
  - evenetually we can call like a function trace through the object
- this keyword
  - we added support for bound method
  - bc we want to access internal state from receiver
  - we need to think about where the reciever should live in memory
  - let's store it in the compiler name
  - to do this, we need to decide what name to give to local slot zero
  - compiler needs to know whether it is compiling a function or method decleration
  - we add support for `TYPE_METHOD` to distinguish methods
  - if we just looked at where the current chunk is, we dont factor in callee
    - we sort of need know about the `nearest enclosing class`
    - this machinery will be useful for later also
  - the memory for the `ClassCompiler` lives directly on the C stack
    - benefit we get from recursive descent
    - at end of body, we pop the compiler off the stack
- why do OOP langauages always tie state and behavior together?
  - core idea: ensure that objects are always in a valid, meaningful state
  - when objects are initialized and even other can modify objects besides methods
  - these two things can make things go awry
- clox things
  - intializers work mostly like regular methods but some differences
    - runtime automatically invokes the initializer method
    - whenever an instance of a class is created
  - the caller of a construct always gets an instance back
    - regardless of what the initializer returns
    - i.e. you don't need to explicitly return `this`
  - in fact, an initializer can't return any value since it would never be seen anyway
- they way we are going to get the initializer to `return this` automatically is by
  - we are going to touch of frontend, specifically, the front will emite bytecode
    - instead of the implicit null
  - so we need to detect if we are running an init()
- right now method calls in clox are slow
  - we define a method invocation as a two step operation
  - one, we access methods and then two calling the result
  - our vm has to support them as seperate because user could call them in two parts
  - but always breaking them apart is costing performance
  - the runtime heap is allocating a new ObjBoundMethod, init the fields
  - then pulling right out
  - in addition, the GC later needs to spend time freeing the ephermeral bound methods
  - we can literally see with the next thing after the dot if there is a ()
- `OP_INVOKE` will take two operands
  1. the index of the proeprty name in the constant table
  2. the number of args passed to the method
  - this method combines the operands of `OP_GET_PROPERTY` and `OP_CALL`
- fundamental creed of performance is "thou shall not break correctness"
- we have a small bug with a field can hold a method
  - we can perform a hot swap for this when the vm is running
- the typical pattern for optimization is,
  1. we recognize a pattern
  2. add an optimized implementation of that pattern
  3. before the optimization kicks in, gaurd with some coditional logic to validate
- full time VM engineers spend much of their careers in this loop
- interesting idea
  - a dynamically typed language forces you to learn two langauges
    1. the runtime semantics
    2. the static type system
  - eventually you get the benefit but at the beginning, the first provides no value
- learning is about transferring the delta between what they already know and what they need to know
  - for each feature/difference you ad, ask yourself how much power it provides
  - you somewhat have a budget for the amount of these which you can add
