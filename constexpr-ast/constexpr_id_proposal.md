## I. Statement and declaration capture

For a compile time reflection we need to query the language construts. One of the easiest way is to create constexpr object from declarations.

proposed syntax is `typeid<T>`

with the following usage:

```C++
static constexpr std::ast::ast_type reflectedType(typeid<const int*>);
static constexpr std::ast::ast_var reflectedVariable(typeid<testVariable>);
static constexpr std::ast::ast_fundecl reflectedFn(typeid<testFn>);
static constexpr std::ast::ast_class reflectedClass(typeid<TestClass>);
static constexpr std::ast::ast_namespace reflectedNamespace(typeid<std>);
```

A proof of concept implementation of this proposal is written. The following example is compiling:

```C++
#include "ast.h"

struct Test
{
  bool member1;
  char member2;
};

int main()
{
  static_assert(typeid<Test>.name() == "Test");
  static_assert(typeid<Test>.members().size() == 2);
  static_assert(typeid<Test>.members()[0].name() == "member1");
  static_assert(typeid<Test>.members()[0].type().name() == "bool");
  static_assert(typeid<Test>.members()[1].name() == "member2");
  static_assert(typeid<Test>.members()[1].type().name() == "char");
  static_assert(typeid<Test>.bases().size() == 0);

  return 0;
}
```
Implementation [is here](https://github.com/hun-nemethpeter/clang/tree/typid_ast)
, [direct link to the diff](https://github.com/llvm-mirror/clang/compare/master...hun-nemethpeter:typid_ast) .
The implementation is quite limited now.

## II. The constexpr-id-expression

  An important part of my previous proposal was the cabability of pasting constexpr result to an id-expression. That was the $ sign. I replaced now with the hash sign (`#`) + `$` sign ( `#$ ( constexpr )` ). The hash sign (`#`) comes from the preprocessor. Now the hash + dollar sign is a compiler error so we can redefine it. `#$( constexpr )` will stand between the preprocessor and template. It won't be a preprocessor directive but evalueted during template parsing.

  My idea is simple. The result of an constexpr must be one of the following

  - `const char*` type or `std::ast::ast_name`
  - `enum std::ast::ast_decl::specifier_t` with the following values: `Public`, `Protected`, `Private`.
  - `enum std::ast::ast_type::qualifier_t` with the following values: `Const`, `Volatile`, `None`.

The constexpr-id will help the compile time code generation. It can be used an drop-in replacement of an id.

Let's create some simple constexpr function

```C++
constexpr const char* generatIdTest()
{ return "test"; }

constexpr ast_decl::specifier_t getPublicSpecifier()
{ return ast_decl::Public; }
```

Here are some use case for using the constexpr + `#< ... >`:

1. Variable declaration
  
  `int #$(generatIdTest());`
  
  is the same as
  
  `int test;`
  
2. Function declaration
  
  `int #$(generatIdTest()) (int a, int b);`
  
  is the same as
  
  `int test(int a, int b);`
  
3. Namespace declaration
  
  `namespace #$(generatIdTest()) { ... }`
  
  is the same as
  
  `namespace test { ... }`
  
4. Access specifier

  ```C++
  class A
  {
    #$(getPublicSpecifier()):
  };
  ```
  
  is the same as

```C++
  class A
  {
    public:
  };
  ```


## III. The constexpr macro

For a usable code generation the constexpr-id is not enough. Repeating a templated-code is necessary. I propose to extend the constexpr functions/methods with an inline lambda block so a constexpr can emit lamda bodies during its run. Lambda bodies will not be executed but their bdies will be marge to the caller place.

Here is an example for generating a variable with constexpr macro. My proposed syntax is:
  ```C++
  constexpr void generateVariables(const ast_class& myClass) {
    for (const auto& member : myClass.members()) {
      inline class [] (const auto& member) {
        int #$(member.name());
    }
  }

  template<typename T>
  class Test
  {
    #!generateVariables(typeid<T>);
  };
  ```
  
In this example the assert macro is implemented with constexpr-macro:
  ```C++
  const bool is_asserts_enabled = true;

  constexpr void assert(const ast_expr& expr) {
    if (!is_asserts_enabled)
      return;
    inline block [] (const auto& expr) {
      if (#$(expr) == false)
        std::cerr << "Assert '" << #$(expr.to_string()) << "' failed!" << std::endl;
    }
  }
  
  // use as
  assert(typeid<a == 3>);
  
  // we need some syntax sugar for efficient use
  constexpr void assert(const ast_expr& expr = typeid) { ... }
  
  // use as normal assert macro now
  assert(a == 3);
  ```
