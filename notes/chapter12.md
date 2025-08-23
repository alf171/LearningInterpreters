# Chapter 12 - Classes

- Three broad paths to OOP
  - classes
    - we'll use this classic approach for Lox
  - prototypes
    - JS + Lua have increased popularity
  - multimethods
- classic OOP classes explained
    1. expose a constructor to create and initialize new instances of the class
    2. Provide a way to store and access fields on instances
    3. Define a set of methods (shared by instance + operate on instance data)
- pretty minimal def
- declaration -> classDecl | funDecl | varDecl | statement ;
- classDecl -> "class" IDENTIFIER "{" function* "}"
- function -> IDENTIFIER "(" parameteres? ")" block;
- parameters -> IDENTIFIER "(" "," IDENTIFIER )* ;
- to allow for accessing properties, we are going to expand our def of call
  - same precendence as the parentheses in a function call expression
  - call -> primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
- fields versus properties
  - fields are named bits of state stored directly in an instance
  - properties are the named things that a get expression may return
  - every field is a property
  - not every property is a field
- now that we have fetching or calling properties, we need to implement setting
- assignment -> ( call "." )? IDENTIFIER "=" assignment | logical_or;
- well how can we support chains a.b.c.d = e
  - turns out (a.b.c) => getter prev.d = e => setter
- do we want to suport var m = object.method; m(argument)
- what about other way?
  - var box = Box();
  - box.function = notMethod;
  - box.function("argument")
- we'll say yes to both b/c we are treating functions like values (first-class fns)
- example why its important
  - breakfast(omlette.filledWith(cheese), sausage)
  - var eggs = omlette.filledWith(cheese); breakfast(eggs, sausage);

What about

```{code}
Class Person {
  sayName() {
    print this.name;
  }

var jane = Person();
jane.name = "Jane";

var bill = Person();
bill.name = "Bill";

bill.sayName = jane.sayName;
print bill.sayName
}
```

- lua/js would print bill
- python, C#, others methods have "bindA this to the original instance"
- python calls this "bound methods"
- constructors are rather messy
    1. runtime allocates the memory for a fresh instance (beneath what user code can access)
    2. user-provided chunk of code is called which initializes the unformed object
- 2 is what we think of when we consider what a constructor does
- we've already done part1 in lox. We will next work on 2
- we will make init() always return this. implementation will be simpler later on
- what if we said, we want to get rid LoxClass and only have LoxInstance
- How would we share common properties since LoxClass stores that
  - LoxInstance delegate directly to another LoxInstance to reuise its fields and methods
  - this is sort of like inheritance
  - results in simpler runtime with only one internal construct
  - instead of canonical and protypical objects that others refine
  - that's where the name "prototypes" comes from for this paradigm
- power = breadth * ease / complexity
  - breath = range of things you can express
  - ease is how little effort it takes to make the language do what you want
  - complexity is how big the language is (runtime, core libraries, tools, ecosystem, etc)
