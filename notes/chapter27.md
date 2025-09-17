# Chapter 27 - Classes and Instances

- since java is higher level, it took us two chapters to implement
- for C, it will take us three
- this chapter, we will cover classes, instances, and fields
- classes serve two main purposes in a language
    1. how you create new instances
    2. They contain a method
- instance has a pointer to the class that it is an instance of
  - this is critical for methods
  - we will mainly focus on how instances store their state for now
  - we need to dynamically add values at runtime so a set seems promising
- the vm has types all the same under the hood
  - two classes or two types are no different in the VMs eyes
- simple but nice: If the value being called the vm produces an instance putting back on stack
- we will use dot notation for getters and setters
