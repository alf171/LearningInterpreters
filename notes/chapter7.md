
## Chapter 7 Evaluating Expressions

- Currently our compiler supports expressions
- we will go from expression -> value
  - for each expression type,
  - what kind of values do we produce?
  - how do we organize those chunks of code
- Representing values
  - a lox value can be any lox type
  - we are going to use java.lang.Object in order to handle this for us
  | Lox Type | Java representation |
  | -------------- | --------------- |
  | Any Lox value | Object |
  | nil | null |
  | Boolean | Boolean |
  | number  | Double |
  | string  | String |
  
  - we can use instanceof to find out at runtime which type Object is
  - we'll need to do more for functions, classes and instances but for primatives, we are set
- Evaluating Expression
  - we are going to reuse visitor pattern
