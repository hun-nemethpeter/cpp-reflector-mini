Code checkers & generators
==========================
(Draft rev 5)


Native Json & HTML, built-in tutorial, easy meta programming, reflection proposal to C++

Why?
----

C++ is a great programming language, but it falters when we try to use it for cloning and
generating code parts. Programmable code generators let you create very complex clones of a given code part
(class, enums, templates, statements, etc.) with familiar syntax.
The result is extraordinarily expressive, readable, and quick to develop.

Motivating example
------------------

```C++
// origin
class User
{
  std::string name;
  Date birthDate;
  double weight;

  bool operator==(const User& rhs) const; // declaring
};

// driver
class EqualityDriver
{
  constexpr EqualityDriver(const ClassDecl& classDecl)
  {
    class_name = classDecl.getTypeName();
    for (auto& field : classDecl.fields()) {
      if (field.getName() == "weight") // you can filter out members
        continue;
      members.emplace_back(field.getName());
    }
  }
  meta::vector<meta::id_name> members;
  meta::type_name class_name;
};

// pattern
$define OperatorEqGenerator(EqualityDriver driver)
{
  bool $driver.class_name::operator==(const $driver.class_name& rhs) const
  {
    return true
      $for (member : driver.members) {
        && $member == rhs.$member
      }
    ;
  }
}

// usage
$OperatorEqGenerator(User); // define an operator== for User
```

How?
----

The idea comes from the AngularJS templating system which is a proven to work and efficient solution for HTML templating.
Drivers are constexpr objects. Manipulating code parts are directed with directives.

Directives are: `$define`, `$use`, `$for`, `$if`, `$switch`

`$define` and `$use` directives expect a driver which is a constexpr object.
In directives and in template driver variables you can use the constexpr object's methods and members.
Generating code parts is safe, because you can create only a typed id.

Some basic rules:
 * you have to use the `{ ... }` syntax after `$for`, `$if`, `$switch` directives
 * you can create `meta::id_name` (in member, variable or parameter declaration context)
 * you can create `meta::type_name` and this can be used where a new type is introduced (e.g. after `class` keyword)
 * `$define` defines a pattern. Can be used later with the `$name` syntax, where name is the defined name.
 * if `$name` expects a grammar, then `(` and `)` can be optional
 * `$define` is namespace friendly (macro #define is not)
 * `$use` can be attached to a template or can be scoped with `{ ... }`

Where does the magic happen?
----------------------------

1. Getting compiler generated AST node
  * An AST node is a wrapped version of the internal one, where the interface is standardized
  * The compiler sees that `$OperatorEqGenerator` is a defined generator with a driver where the driver expects one parameter
  * The compiler generates an AST node struct for `User` with base type `decl`.
  * The compiler tries to cast `decl` to `ClassDecl`. If it fails, the compiler tells that the driver expects `ClassDecl`.
2. Inject generated tokens
  * `meta::id_name` can be constructed from string
  * with the dollar `$` syntax it can be pasted (e.g. `$member`) as a normal id in a generator template
  * `meta::id_name` can be pasted as a string literal
3. Parameter passing in templates
  * for one parameter `template<typename T> $use(Driver driver)` Driver got the AST nodized T
  * for more parameter `template<class T, class U> $use(Driver driver)` Driver constructor expects the same number of parameters as template has.
  * manual parameter passing `template<class T, class U> $use(Driver driver(U))` here only `U` is used.
4. New keyword `astnode` in template parameter
  * `template<astnode Node> $use(AssertDriver driver)` where astnode can be an expression, but Driver gets an AST node
  * `astnode` template parameter must be used with a driver
  * the grammar of astnode is defined in the driver's constructor parameters, it can be complex grammar

Standardized AST nodes
----------------------

TODO: modelled after clang http://clang.llvm.org/doxygen/classclang_1_1Decl.html

Clang Decl API uses `camelCase` C++ STL uses `underscores_type`. Only a minimal subset of ClangAPI is needed.

Other use cases
------------------

### Struct-of-Arrays vector

```C++
// origin
struct S {
    int a;
    int b;
    int c;
};

// driver
class SoADriver
{
public:
  struct Member
  {
    meta::id_name name;
    meta::type_name type;
  };
  constexpr SoADriver(const ClassDecl& fromClassDecl, const char* new_class_name)
    : new_class_name(new_class_name)
  {
    for (auto& field : fromClassDecl.fields())
      members.emplace_back({field.getTypeName(),  field.getName() + "s", });
  }
  meta::type_name new_class_name;
  meta::vector<Member> members;
};

// pattern
$define SoAGenerator(SoADriver driver)
{
  class $driver.new_class_name
  {
    $for (member : driver.members) {
      std::vector<$member.type> $member.name;
    }
  };
}

// usage
$SoAGenerator(S, "SoA_vector_of_S");
```

### Replacing assert

My best solution is to introduce a new keyword called `astnode`

```C++
// origin
int main()
{
  int a = 1;
  int b = 2;
  assert(a == b);

  return 0;
}

// driver
class AssertDriver
{
public:
  constexpr ArrayDriver(const ExpressionDecl& decl) // if astnode is not an expression we got compilation error
     expessionDecl(decl)
  {
    // check decl is boolean expr
  }
  const expessionDecl& decl;
};

// template with attached driver
// new keyword astnode, allowed only with $use
template<astnode Node> $use(AssertDriver driver)
void assert(Node)
{
  // this will run between two sequence points
  // after Node is evaluated
  // `get_result()` is a const ref to the result
  if (!$driver.decl.get_result()) {
    std::cout << "failed assert: " << $driver.decl.to_string() << std::endl;
    std::cout << $driver.decl.source.get_start_pos() << std::endl;
  }
}

// usage
int main()
{
  int a = 1;
  int b = 2;
  assert(a == b);
}
```

### Enumerating enums

This code snippet converts a string to an enum. The converter implementation is a function template.
The function template parameter is "captured" before template instatization, and processed with `EnumDriver`.
`EnumDriver`'s constructor is waiting an `const EnumDecl&` that is the AST node of `T`.

```C++
// origin
enum class EVote
{
  Yes,
  RatherYes,
  Undecided,
  RatherNo,
  No
};

// driver
class EnumDriver
{
  public:
    constexpr EnumDriver(const EnumDecl& enumDecl)
    {
      for (auto& enumerator : enumDecl.enumerators())
        enumValueNames.push_back(enumerator.getName());
    }

    // meta::vector is a constexpr vector
    meta::vector<meta::id_name> enumValueNames;
};

// template with attached driver
template<typename T> $use(EnumDriver driver)
void Json::readFrom(T& obj, const std::string& data)
{
  obj = llvm::StringSwitch<T>(data)
    $for (enumValueName : driver.enumValueNames) {
      .Case($enumValueName.asStr(), $enumValueName) }
  ;
}

int main()
{
  Evote vote;
  Json::readFrom(vote, "EVote::Undecided");

  return 0;
}
```

### Deprecating C style macro

We can replace the old macro system with this new one. For this reason `$if` and `$swicth` is introduced.
For normal code flow, this conditional compilation would be as harmful (or helpful) as the normal macro style one.
This is just a new syntax.

Pro:
 * namespace friendly
 * object oriented design
 * simple: only `$if` & `$else` instead of `#ifdef`, `#ifndef`, `#elif`, `#endif`, `#if defined`

Con:
 * cannot be used for header guard
 * `{` and `}` does not introduce scope but it seems so from the syntax

Some intended usage:

Instead of
```C++
#define DEBUG 1

void printBackTrace() {
#if DEBUG
  // do something
#else
  // do nothing
#endif
}
```

we can write

```C++
enum class Configuration
{
  Debug,
  Release
};
enum class Platform
{
  Win32,
  Win64,
  Linux
};
struct ConfigurationDriver
{
  constexpr Configuration configuration = Configuration::Debug;
  constexpr Platform platform = Platform::Linux;
  constexpr ConfigurationDriver() {}
};

// start using ConfigurationDriver
$use(ConfigurationDriver driver)
{

// function with a driver
void printBackTrace()
{
  $if (driver.configuration == Configuration::Debug)
  {
    realPrintBackTrace();
  }
  $else
  {
    // do nothing
  }
}

// function with a driver
void foo()
{
  $switch (driver.platform)
  {
    $case Platform::Win32 // fallthrough
    $case Platform::Win64
    {
      WinApiXyz();
    }
    $case Platform::Linux
    {
      GlibcCallXyz();
    }
  }
}

} // $use
```

### Deprecating template metaprogramming

Working with code generators and checkers is much easier, faster and safer than TMP.
My proposal is to issue a deprecate warning when template instantiation depth reaches a low number e.g. `16`.
This limit may be removed with a  compiler switch e.g. `-ftemplate-depth-20000`
With native DSL support we can invent a new DSL language for functional style metaprogramming and for concepts checking. 

### Code checking (also concepts check)

If we use the `$use` without an instance name it means that the driver is doing only checks

#### For concepts check
```C++
// attached checker for a class template
template<typename T> $use(ConceptsChecker)
class Foo
{
}

struct ConceptsChecker {
  constexpr ConceptsChecker(const ClassDecl& classDecl) // first check, T must be an class
  {
    // for more check, see http://clang.llvm.org/doxygen/classclang_1_1CXXRecordDecl.html
    static_assert(classDecl.hasMoveConstructor(), "Move constructor is missing");
  }
};
```

#### For compile time call site parameter check
```C++
struct FormatChecker {
  // http://clang.llvm.org/doxygen/classclang_1_1Expr.html
  constexpr FormatChecker(const meta::expr& expr)
  {
    if (expr.isa<meta::string_literal>()) 
    {
       // do format check for format string
    }
  }
};

// attached checker for a normal function
void printDate(const char* formatStr) $use(FormatChecker)
{
}
```

#### For compile time general call site check, ideal for ex. tutorials and coding style checkers.
```C++
// attached checker after a normal function
struct TutorialChecker {
  constexpr TutorialChecker(const meta::compound_stmt& stmt)
  {
     // check for C function and show std::cout example
  }
};

int main()
{
  printf("My first program!");
  return 0;
} $use (TutorialChecker, CodingStyleChecker)

```

### Domain-specific language support

With the help of this we can achieve native HTML, Json, XML, Regex, SQL, whatever support. It would be pretty awesome.
If we can solve with C++ modules to use JIT compiler on constexpr drivers it does not slow down the compilation too much.
The grammar of astnode is defined in the driver's constructor parameters, it can be complex grammar.
We should indicate, that a struct is a grammar struct, with inherited meta::grammar.
The grammar struct should not contain any method (including ctor, dtor, ..)

```C++
// full grammar is here: http://www.json.org/
struct JsonParamGrammarItem : meta::grammar
{
  meta::id_name key;
  meta::op_name colon = ':';
  meta::expr value;
};

struct JsonParamGrammarTail : meta::grammar
{
  meta::id_name comma = ',';
  JsonParamGrammarItem item;
};

struct JsonParamGrammar : meta::grammar
{
  meta::symbol open_brace = '{';
  JsonParamGrammarItem paramFirst;
  meta::vector<JsonParamGrammarTail> paramMore; // this can be used for varargs ...
  meta::symbol close_brace = '}';
};

class JsonParamDriver
{
   constexpr JsonParamDriver(JsonParamGrammar grammar);
};

/*
grammar rules
or - meta::or
and - member in struct
optional - meta::optional
any - meta::vector

so JsonParamGrammarItem is
meta::id_name & ':' & meta::expr
*/

// usage
class SomeWidget
{
  template<astnode Node>
  SomeWidget(Node) $use(JsonParamDriver driver)
  {
    ...
    // We should process with an other driver that associate member names with param names.
  }

  SomeWindow window;
  SomeLabel label;
};

SomeWidget widget({
                     window: "Hello world",
                     label:  "Foo"
                  });
```

TODO
----

I selected the first one.

Which is better, so how hard for the compiler?
 `$driver.class_name` is one token
 * `bool $driver.class_name::operator==`
 * `bool $driver.class_name$::operator==`
 * `bool $(driver.class_name)::operator==`

How can I name this paper?
 * Code checkers & generators
 * Compile time reflection
 * Template drivers
 * AngularC++
 
What is the name of the `$define` thing?
 * pattern
 * macro-ng
 * new macro
 * generator template
 * named generator

Standardize Clang Api names from http://clang.llvm.org/doxygen/classclang_1_1NamedDecl.html
 * `meta::class_name` from http://clang.llvm.org/doxygen/classclang_1_1CXXRecordDecl.html
 * `meta::template_name` from http://clang.llvm.org/doxygen/classclang_1_1TemplateDecl.html
 * `meta::namespace_name` from http://clang.llvm.org/doxygen/classclang_1_1NamespaceDecl.html
 * `meta::function_name` from http://clang.llvm.org/doxygen/classclang_1_1FunctionDecl.html
 * `meta::value_name` from http://clang.llvm.org/doxygen/classclang_1_1DeclStmt.html
 * `meta::operator_name` from http://clang.llvm.org/doxygen/OperatorKinds_8h_source.html
 * ...

Restrict pattern to a subgrammar item
 * `$define MyFooAttribute : meta::attribute_decl { [[ foo ]] }`
 * `$define MyFooAttribute() : meta::attribute_decl { [[ foo ]] }`
 * `$define MyFooAttribute $use(SomeGrammarStruct) { [[ foo ]] }`
 * `$define MyFooAttribute() $use(SomeGrammarStruct) { [[ foo ]] }`
 
Links
-----
You can comment it on (isocpp.org/forums -> SG7 – Reflection)
https://groups.google.com/a/isocpp.org/forum/#!forum/reflection

The idea of this proposal comes from the AngularJS HTML template framework:
http://angularjs.org/

prebuilt directives ngRepeat directive in module ng:
http://docs.angularjs.org/api/ng.directive:ngRepeat

These code snippet based on this repo:
https://github.com/hun-nemethpeter/cpp-reflector-mini

Call for Compile-Time Reflection Proposals:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3814.html
