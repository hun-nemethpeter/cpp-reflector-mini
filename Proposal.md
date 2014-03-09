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

// OperatorEqGenerator will be a dependent name
template<ipr::Class T>
auto OperatorEqGenerator
{
  bool T::operator==(const T& rhs) const
  {
    return true
      static for (field : T.fields())
      {
        && auto<field.getName()> == rhs.auto<field.getName()>
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

Language object -> IPR node transition
--------------------------------------

The first main step for reflection is to make inspectable every named language-object.
This paper examines the way where every language-object has a corresponding IPR node at compile time.

Getting an IPR node of a named language object is done through extendending the template declaration syntax.
Instead of typename/class keyword `ipr::GrammarElelemt` can use.

 `temaplate<ipr::Class T>`

This IPR node optionally can be forwarded to a constexpr object with the `->` syntax.

 `temaplate<ipr::Class T> -> (SomeDriver driver)`

 here `driver` will be a dependent name that can be used during template instantiation.
 `SomeDriver` must be a constexpr constructor with a corresponding `ipr::...` argument:

 ```C++
 class SomeDriver
 {
   constexpr SomeDriver(const ipr::Class& classDecl)
   ...
 };
 ```

 Template declaration syntax is extended with auto template. This creates a standalone dependent names in a namespaced scope:
```C++
template <ipr::Class T>
auto MacroName
{
  class Foo {};
  int bar;
  void auto<"test" + T.getName()>();
}

// usage
auto<MacroName<Foo>>; 

// expanded to
class Foo;
int bar;
void testFoo();
```

you can attach a driver to an auto template
```C++
template <ipr::Class T>
auto MacroName -> (SomeDriver driver)
{
  void foo() { std::cout << driver.foo() << std::endl; }
}
```

auto template can be restricted to a grammar object, this will be a "type-template"
```C++
template <>
ipr::Type MacroName
{
  int
}
```

This way can leads to a better C macro

IPR node -> language object transition
--------------------------------------

There are two way of pasting a dependent name
 - the `auto<...>` syntax
 - the `typename<...>` syntax
 
 `typename<...>` syntax is for improve the code readability where variable is created


You can acces the ipr:: Node methods directly
```C++
template<ipr::Class T>
class Foo : public typename<T.getName() + "Test">
{
  typename<T.getName() + "Test"> memberName;
};
```
 
Or you can use a driver if there is a complex transforming
```C++
template<ipr::Class T> -> (SomeDriver driver)
class Foo : public typename<driver.foo()>
{
  typename<driver.bar()> auto<driver.getName()>;
};
```

Repeating with static for
-------------------------

A helper `static for` grammar object is created for repeating grammar parts. It accepts constexprs containers and
works like a range base for.
The syntax is simple `static for (item : container) { }`

`{ }` doesn't introduce scope.

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

// SoAGenerator will be a dependent name
template<ipr::Class T>
auto SoAGenerator
{
  class typename<"SoA_vector_of_" + T.getName()>
  {
    static for (field : T.fields())
    {
      std::vector<typename<field.getName()>> auto<field.getName()>;
    }
  };
}

// usage
auto<SoAGenerator<S>>;
```

### Replacing assert

```C++
template<ipr::Expression Node>
void assert(Node)
{
  // get_result() is a const ref to the result
  if (!auto<driver.decl.get_result()>)
  {
    std::cout << "failed assert: " << auto<Node.decl.stringify()> << std::endl;
    std::cout << auto<Node.decl.source.get_start_pos()> << std::endl;
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

template <ipr::Enum T>
void Json::writeTo(const T& obj, std::ostream& os)
{
  switch (obj)
  {
    static for (enumerator : T.enumerators())
    {
       case auto<enumerator.getName()>:
         os << auto<enumerator.getName().stringify()>;
         return;
    }
  }
}

template<ipr::Enum T>
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
struct ConceptsChecker
{
  constexpr ConceptsChecker(const ipr::Class& classDecl)
  {
    static_assert(classDecl.hasMoveConstructor(), "Move constructor is missing");
  }
};

// attached checker for a class template
template<ipr::Class T> -> (ConceptsChecker)
class Foo
{
}
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
