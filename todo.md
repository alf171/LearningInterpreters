# a couple todos which might be interesting at the EOP

- TODOS:
    1. 23/ is there something more efficent than linear scan through local variables
    2. 23/ add support for final keyword
    3. ?/ flexible array members -- can make ObString store char[] instead of char*
    4. 21/ support variable reassignment results in duplicate constants. fix this
    5. 23/ support switch statements
      - very similar to if or for
    6. 23/ support continue
      - jump to the top of the nearest enclosing loop skipping rest of body
    7. [DONE] 24/ we spend most of our time touching ip
      - can we keep it in a native cpu register
    8. 24/ add an arity check to native functions
    9. 25/ Change clox to not need to wrap every `ObjFunction` in `ObjClosure`
      - check performance on both closure and non closure usecase
    10. 25/ define a poor mans object using just closures
    11. 26/ is there a way to concat Obj header struct
      - it has `type`, `isMarked`, and `next` right now
    12. 26/ is there a more efficent approach then clearing `isMarked` after each gc
    13. 26/ try another gc: `Cheney's algo`, `Lisp 2 mark-compact algo`
    14. 27/ how do languages handle settings fields to nil // wouldn't work rn
    15. 28/ is there a faster way to implement the hash table for the init() method O(1)
    16. 28/ the whole thing where we don't know if we have a member or method cost perf
      - is there a better way to do this? was this a good tradeoff
