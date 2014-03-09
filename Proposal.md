Code checkers & generators
==========================
(Draft rev 2)


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
  constexpr EqualityDriver(const ipr::Class& classDecl)
  {
    class_name = classDecl.getTypeName();
    for (auto& field : classDecl.fields()) {
      if (field.getName() == "weight") // you can filter out members
        continue;
      members.emplace_back(field.getName());
    }
  }
  ipr::vector<ipr::id_name> members;
  ipr::type_name class_name;
};

// OperatorEqGenerator will be a dependent name
template<ipr::Class T> -> (EqualityDriver driver) OperatorEqGenerator
{
  bool auto<driver.class_name>::operator==(const auto<driver.class_name>& rhs) const
  {
    return true
      static for (member : driver.members)
      {
        && auto<member> == rhs.auto<member>
      }
    ;
  }
}

// usage
auto<OperatorEqGenerator<User>>;
```

How?
----

The C++ language has a grammar which define language objects e.g.: classes, functions, enums, namespaces ...
If a language object is named we can use it later in the source code. If we can use it we can inspect it also.
Compile time reflection is a way to inspect these named language objects. Theoretically we can inspect every
attributes of these objects. One possible way of compile time reflection is to ask the compiler to create a
descriptor-object (an IPR node) for a named language object and pass it to a constexpr object.
I call this descriptor-object as IPR node. The IPR means Internal Program Representation.
The descriptor-object is very similar to an AST node but it is not a real one.

Language objects name -> IPR transition
---------------------------------------

The first main step for reflection is to make inspectable every named language-object.
This paper examines the way where every language-object has a corresponding IPR node at compile time.

This paper introduce two new way for creating a dependent name

1. Using dependent name in a template:
`temaplate<typename T> -> (SomeDriver driver)`


 here `driver` will be a dependent name that can be used during template instantiation.
 The template parameter `T` will be forwarded to SomeDriver class's constructor parameter as an IPR node
 `SomeDriver` must be a constexpr constructor with an `ipr::...` argument:

 ```C++
 class SomeDriver
 {
   constexpr SomeDriver(const ipr::Class& classDecl)
   ...
 };
 ```

 This looks like a magic first, but if this transition is in this form
 `temaplate<typename T> -> (SomeDriver driver(ToIPRNode<T>))`
 the syntax become redundant, so the `(ToIPRNode<T>)` is just a syntax noise here.

2. Creating standalone dependent names in a namespaced scope:
 `template <auto T> -> (SomeDriver driver) DependentName`

 By default it can be used in a namespace scope to declare new function/class/variable. It can be restricted to
 a grammar part with the following syntax
 `template <auto T> -> (SomeDriver driver) DependentName : ipr::grammar_part`
 This way leads to a better C macro

IPR node -> language object transition
--------------------------------------

Using dependent name is uniformed with the `auto<...>` syntax.

It can be used in a template
```C++
template<typename T> -> (SomeDriver driver)
class Foo : public auto<driver.foo()> { ... };
```

It can be used in a normal function/classs
```C++
template<auto> MyTypeName : ipr::type_name
{
  int
}

class Foo
{
  auto<MyTypeName> member;
};

```


Standardized IPR nodes
----------------------

TODO: modelled after Pivot https://parasol.tamu.edu/pivot/


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
    ipr::id_name name;
    ipr::type_name type;
  };
  constexpr SoADriver(const ipr::Class& fromClassDecl, const char* new_class_name)
    : new_class_name(new_class_name)
  {
    for (auto& field : fromClassDecl.fields())
      members.emplace_back({field.getTypeName(),  field.getName() + "s", });
  }
  ipr::type_name new_class_name;
  ipr::vector<Member> members;
};

// SoAGenerator will be a dependent name
template<ipr::Class T, const char*> SoAGenerator -> (SoADriver driver)
{
  class auto<driver.new_class_name>
  {
    static for (member : driver.members)
    {
      std::vector<auto<member.type>> auto<member.name>;
    }
  };
}

// usage
auto<SoAGenerator<S, "SoA_vector_of_S">>;
```

### Replacing assert

Using auto keyword instead of typename/class in a template declaration means it can be a grammar element, e.g. a expression.

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
  constexpr ArrayDriver(const ipr::ExpressionDecl& decl) // if Node is not an expression we got compilation error
     expessionDecl(decl)
  {
    // check decl is boolean expr
  }
  const expessionDecl& decl;
};

// template with attached driver
template<ipr::Expression Node> -> (AssertDriver driver)
void assert(Node)
{
  // get_result() is a const ref to the result
  if (!auto<driver.decl.get_result()>)
  {
    std::cout << "failed assert: " << auto<driver.decl.stringify()> << std::endl;
    std::cout << auto<driver.decl.source.get_start_pos()> << std::endl;
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
`EnumDriver`'s constructor is waiting an `const ipr::Enum&` that is the IPR node of `T`.

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
template<ipr::Enum T> -> (EnumDriver driver)
void Json::readFrom(T& obj, const std::string& data)
{
  obj = llvm::StringSwitch<T>(data)
    static for (enumerator : T.enumerators())
    {
      .Case(auto<enumerator.getName().stringify()>, auto<enumerator.getName()>)
    }
  ;
}

int main()
{
  Evote vote;
  Json::readFrom(vote, "EVote::Undecided");

  return 0;
}
```

### Code checking (also concepts check)

If we use the driver without an instance name it means that the driver is doing only checks

#### For concepts check
```C++
// attached checker for a class template
template<ipr::Class T> -> (ConceptsChecker)
class Foo
{
}

struct ConceptsChecker {
  constexpr ConceptsChecker(const ipr::Class& classDecl) // first check, T must be an class
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
  constexpr FormatChecker(const ipr::Expression& expr)
  {
    if (expr.isa<ipr::string_literal>()) 
    {
       // do format check for format string
    }
  }
};

// attached checker for a normal function
void printDate(const char* formatStr) -> (FormatChecker)
{
}
```

#### For compile time general call site check, ideal for ex. tutorials and coding style checkers.
```C++
// attached checker after a normal function
struct TutorialChecker
{
  constexpr TutorialChecker(const ipr::CompoundStmt& stmt)
  {
     // check for C function and show std::cout example
  }
};

int main()
{
  printf("My first program!");
  return 0;
} -> (TutorialChecker, CodingStyleChecker)

```

### Domain-specific language support

With the help of this we can achieve native HTML, Json, XML, Regex, SQL, whatever support. It would be pretty awesome.
If we can solve with C++ modules to use JIT compiler on constexpr drivers it does not slow down the compilation too much.
The grammar of `Node` is defined in the driver's constructor parameters, it can be complex grammar.
We should indicate, that a struct is a grammar struct, with inherited ipr::grammar.
The grammar struct should not contain any method (including ctor, dtor, ..)

```C++
// full grammar is here: http://www.json.org/
struct JsonParamGrammarItem : ipr::grammar
{
  ipr::id_name key;
  ipr::op_name colon = ':';
  ipr::expr value;
};

struct JsonParamGrammarTail : ipr::grammar
{
  ipr::id_name comma = ',';
  JsonParamGrammarItem item;
};

struct JsonParamGrammar : ipr::grammar
{
  ipr::symbol open_brace = '{';
  JsonParamGrammarItem paramFirst;
  ipr::vector<JsonParamGrammarTail> paramMore; // this can be used for varargs ...
  ipr::symbol close_brace = '}';
};

class JsonParamDriver
{
   constexpr JsonParamDriver(JsonParamGrammar grammar);
};

/*
grammar rules
or - ipr::or
and - member in struct
optional - ipr::optional
any - ipr::vector

so JsonParamGrammarItem is
ipr::id_name & ':' & ipr::expr
*/

// usage
class SomeWidget
{
  template<auto Node>
  SomeWidget(Node) -> (JsonParamDriver driver)
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

Links
-----

The Pivot is a framework
https://parasol.tamu.edu/pivot/

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
