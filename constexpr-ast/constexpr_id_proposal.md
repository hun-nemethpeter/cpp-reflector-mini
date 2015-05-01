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


## II. The constexpr-id-expression

  An important part of my previous proposal was the cabability of pasting constexpr result to an id-expression. That was the $ sign. The $ sign was hated so much so I replaced now with the hash sign (#) + xml element syntax ( #< constexpr > ). The hash sign (#) comes from the preprocessor. Now the hash + less then sign is a compiler error so we can redefine it. #<element> will stand between the preprocessor and template. It won't be a preprocessor directive but evalueted during template parsing.

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

Here are some use case for using the constexpr + `#< ... >:`

1. Variable declaration
  
  `int #<generatIdTest()>;`
  
  is the same as
  
  `int test;` // declare a variable with name testVariable;
  
2. Function declaration
  
  `int #<generatIdTest()> (int a, int b);`
  
  is the same as
  
  `int test(int a, int b);`
  
3. Namespace declaration
  
  `namespace #<generatIdTest()> { ... }`
  
  is the same as
  
  `namespace test { ... }`
  
4. Access specifier
  
  class A
  {
    #<getPublicSpecifier()>:
  };
  
  is the same as

  class A
  {
    public:
  };


## III. Statement and declaration repeater

For a usable code generation the constexpr-id is not enough. Repeating a templated-code is necessary.

My proposed syntax is:
  #<for> (item : items())
     declarations or
     statements
  #</for>

## IV. Generalize the statement and declaration manipulation

If we allow for we should allow other control structures as well. Intended use case is replacing the textual pre-processor.

My proposed syntax is:

1. if..else
  #<if> (member.size() == 0)
    char dummy[4];
  #<else>
    #<if> (...)
    #</if>
  #</if>

2. switch.. case
  #<switch> (typeid<int>.size_in_bits())
    #<case> (4)
      int32();
    #</case>
    #<case> (8)
      int64();
    #</case>
    #<case> (16)
      int128();
    #</case>
    #<default>
      static_assert(false, "Not supported int size");
    #</default>
  #</switch>

3. define meta function

#<define> copyClassMembers(const std::ast::ast_class& srcClass)
  #<for> (member : srcClass.members())
    ''member.type()'' ''member.name()'';
  #</for>
  #<if> (members.empty())
     char padding[8];
  #</if>
#</define>

4. call meta function

#<call>copyClassMembers(typeid<TestClass>)#</call>

5. try lookup a symbol

#<lookup>symbolname#</lookup>
if symbolname is exist then the result is true otherwise false.

constexpr bool alloca_exist = #<lookup>void* alloca(size_t)#</lookup>;
#<if> (!alloca_exist)
void* alloca(size_t size)
{
  return 0;
}
#</if>

