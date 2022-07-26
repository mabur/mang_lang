OVERVIEW
* Compile to C and macros.
* Generic programming.
* Array programming.
* Start by writing sample programs and library in C, including macros,
  and look for patterns that I want to improve.
* Try variable length arrays.

MISSING FEATURES IN C
* Simple syntax.
* Type inference.
* Scope-based memory management.
* Generic functions.
  - map
  - filter
* Coyping/cloning och nested structures and containers.

MEMORY MANAGEMENT
* Variable length arrays - Seems to be for arrays used inside functions but not in structs.
  - Automatically deallocates when out of scope.
  - Can we have it so that structs/classes/user-defined-types are only used in high-level-language
    and removed when compiling to C?
  - Can we do that with macros inside C? So that macros are used for spreading types.
* Scope-based memory management.
* Free memory when variable goes out of scope.
* How to handle references to same data?
  - Ok as long as they are in the same function-scope.
  - Problem if passed out from function.
  - Only allow assignment= for new definitions.
  - Do not allow assignment= for out parameters?

SEMANTICS
Mutable reference by default?

DATA STRUCTURES
Arrays, SOA, AOS, dataframes.
Have dataframe as only datastructure?
