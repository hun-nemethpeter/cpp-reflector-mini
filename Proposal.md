Code generators
===============
(Draft rev 4)

You can comment it on (isocpp.org/forums -> SG7 – Reflection)
https://groups.google.com/a/isocpp.org/forum/#!forum/reflection

Why?
----

C++ is a great programming language, but it falters when we try to use it for cloning and
generating code parts. Programmable code generators lets you create very complicate clones of a given code part
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

// generator template
// can be used later with $OperatorEqGenerator syntax
$define OperatorEqGenerator(EqualityDriver driver)
{
  bool $driver.class_name::operator==(const $driver.class_name& rhs) const
  {
    return true
      $for (auto member : driver.members) {
        && $member == rhs.$member
      }
    ;
  }
}

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

// usage
$OperatorEqGenerator(User); // define an operator== for User
```

How?
----

The idea come from the AngularJS templating system which is a proven to work an efficient solution for HTML templating.
Drivers are constexpr objects. Manipulating code parts are directed with directives.

Directives are: `$define`, `$use`, `$for`, `$if`, `$switch`, `$while`

`$define` and `$use` directives wait a driver which is constexpr object.
In directives and in template driver variables you can use the constexpr object's methods and members.
Template driver parameters start with the dollar `$` sign.

Generating code parts is safe, because you can create only a typed id.

Some basic rules:
 * you have to use the `{ ... }` syntax after `$for`, `$if`, `$switch`, `$while` directives
 * you can create `meta::id_name` (in member, variable or parameter declaration context)
 * you can't create `meta::type_name` only compiler able to generate it.
 * `$define` define a named generator. Can be used later with the `$name` syntax, where name is the defined name.
 * `$use` can be attached to a template or can be scoped with `{ ... }`

Where the magic happens?
------------------------

1. Getting compiler generated AST node
  * AST node is a wrapped version of the internal one where the interface is standardized
  * Compiler sees that `$OperatorEqGenerator` is a defined generator with a driver where driver expect one parameter
  * Compiler generates an AST node struct for `User` with base type `decl`.
  * Compiler tries to cast `decl` to `ClassDecl`. If it failed compiler tells that driver expects `ClassDecl`.
2. Inject generated tokens
  * `meta::id_name` can be constructed from string
  * with the dollar `$` syntax it can be pasted (ex. $member) as a normal id in a generator template
  * `meta::id_name` can be pasted as a string literal
3. Parameter passing in templates
  * for one parameter `template<typename T> $use(Driver driver)` Driver got the AST nodized T
  * for more parameter `template<class T, class U> $use(Driver driver)` Driver constructor expect the same number of parameters as template has.
  * manual parameter passing `template<class T, class U> $use(Driver driver(U))` here only `U` is used.
4. New keyword `astnode` in template parameter
  * `template<astnode Node> $use(AssertDriver driver)` where astnode can be an expression but Driver got an AST node
  * `astnode` template parameter must be used with a driver

Standardized AST nodes
----------------------

TODO: modelled after clang http://clang.llvm.org/doxygen/classclang_1_1Decl.html

Clang Decl API use `camelCase` C++ STL use `underscores_type`. Only a minimal subset of ClangAPI is needed.

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

// generator template
// can be used later with $SoAGenerator syntax
$define SoAGenerator(SoADriver driver)
{
  class $driver.new_class_name
  {
    $for (auto member : driver.members) {
      std::vector<$member.type> $member.name;
    }
  };
}

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

// usage
$SoAGenerator(S, "SoA_vector_of_S");
```

### Replacing assert

My best solution is introduce a new keyword called `astnode`

```C++
// origin
int main()
{
  int a = 1;
  int b = 2;
  assert(a == b);

  return 0;
}

// template with attached driver
// new keyword astnode, allowed only with $use
template<astnode Node> $use(AssertDriver driver)
void assert(Node)
{
  if (!$driver.decl.get_result()) { // evaluated runtime inplace, but the result will be here
    std::cout << "failed assert: " << $driver.decl.to_string() << std::endl;
    std::cout << $driver.decl.source.get_start_pos() << std::endl;
  }
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

// template with attached driver
template<typename T> $use(EnumDriver driver)
void Json::readFrom(T& obj, const std::string& data)
{
  obj = llvm::StringSwitch<T>(data)
    $for (auto enumValueName : driver.enumValueNames) {
      .Case($enumValueName.asStr(), $enumValueName) }
  ;
}

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

int main()
{
  Evote vote;
  Json::readFrom(vote, "EVote::Undecided");

  return 0;
}
```

### Replacing normal macro

We can replace the old macro system with this new one. For this reason `$if` and `$swicth` is introduced.
For normal code flow this conditional compilation as harmful (or helpful) as the normal macro style one.
This is just a new syntax.

Pro:
 - namespace friendly
 - object oriented design

Con:
 - can not used for header guard (because it is namespace friendly)
 - `{` and `}` does not introduce scope but it looks like from the syntax

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
  const Configuration configuration;
  const Platform platform;
  constexpr ConfigurationDriver()
    : configuration (Configuration::Debug)
    , platform(Platform::Linux)
  { }
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

### Concept checking

If we use the $use without an instance name it means that the driver is doing only checks

```C++
template<typename T> $use(ConceptDriver)
class Foo
{
}

struct ConceptDriver {
  constexpr ConceptDriver(const ClassDecl& classDecl) // first check, T must be an class
    // for more check, see http://clang.llvm.org/doxygen/classclang_1_1CXXRecordDecl.html
    static_assert(classDecl.hasMoveConstructor(), "Move constructor is missing")
  }
};
```

Domain-specific language support
--------------------------------

With the help of this we can achive native HTML, Json, XML, Regex, SQL, whatever support. It would be pretty awesome.
If we can solve with C++ modules to use JIT compiler on constexpr drivers it does not slow down the compilation too much.
Grammar of astnode is defined in the driver's constructor parameters, it can be complex grammar.
We should sign, that a struct is a grammar struct, with inheriting meta::grammar.
Grammar struct should not contain any method (including ctor, dtor, ..)

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
   JsonParamDriver(JsonParamGrammar grammar);
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

Which is better
 * `$use(AssertDriver driver)`
 * `$use<AssertDriver driver>`

Which is better
 * `$OperatorEqGenerator(User); // new syntax`
 * `OperatorEqGenerator<User>;  // template syntax`
 * `OperatorEqGenerator(User);  // macro syntax`
 
Which is better, so how hard for the compiler?
 `$driver.class_name` is one token
 * `bool $driver.class_name::operator==`
 * `bool $driver.class_name$::operator==`
 * `bool $(driver.class_name)::operator==`

"you can't create meta::type_name only compiler able to generate it. // TODO: what about class declaration?"
So why not? What about creating:
 * `meta::class_name`
 * `meta::template_name`
 * `meta::namespace_name`
 * `meta::function_name`
 * `meta::value_name`
 * `meta::operator_name`

Links
-----

The idea of this proposal comes from the AngularJS HTML template framework:
http://angularjs.org/

prebuilt directives ngRepeat directive in module ng:
http://docs.angularjs.org/api/ng.directive:ngRepeat

These code snippet based on this repo:
https://github.com/hun-nemethpeter/cpp-reflector-mini

Call for Compile-Time Reflection Proposals:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3814.html
