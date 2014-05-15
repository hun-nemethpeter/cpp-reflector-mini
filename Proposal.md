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

// 1. instead of typename T => ipr::Class
template<ipr::Class iprT, typename T = $(iprT.name())>
auto OperatorEqGenerator
{
  bool T::operator==(const T& rhs) const
  {
      bool ret = true;
      for <member : iprT.members()> // 2. for template
      {
        if <member.category == ipr::field_cat> // 3. if template
        {
           ret = ret && $(member.name()) == rhs.$(member.name()); // token pasting with $
        }
      }

      return ret;
  }
}

// usage
OperatorEqGenerator<User>;

// expands to
bool User::operator==(const User& rhs) const
{
  bool ret = true;
  ret = ret && name == rhs.name;
  ret = ret && birthDate == rhs.birthDate;
  ret = ret && weight == rhs.weight;

  return ret;
}
```


How?
----

The C++ language has a grammar which define language objects e.g.: classes, functions, enums, namespaces ...
If a language object is named we can use it later in the source code. If we can use it we can inspect it also.
Compile time reflection is a way to inspect these named language objects. Theoretically we can inspect every
attributes of these objects. One possible way of compile time reflection is to ask the compiler to create a
descriptor-object (an IPR node) for a named language object. The IPR means Internal Program Representation.
The descriptor-object is an abstract AST node.

Language object -> IPR node transition (reading)
------------------------------------------------

The first main step for reflection is to make inspectable every named language-object.
This paper examines the way where every language-object has a corresponding IPR node at compile time.

In C++ language the most common declaration for a template is `template<typename T> class Foo {};` where T is represent a type name.
`template<int N> class foo {};` also a valid for, int this case the template will be instatized with an integer. E.g.
`Foo<1> foo;` and this structure called as non-type template parameter. One of the key idea of this paper is introducing
new non-type template parameters. These special non-type template parameters will convert a C++ language element to
a descriptor object.

 `template<std::ipr::Class iprT> class Foo {};`

Here the new `std::ipr::Class` parameter a normal class, but it has a special capability. It can accept a class type name.

  `Foo<std::string> foo;`

During template instatiation the `iprT` parameter acts as a normal value parameter. That parameter is created at compile time filled
with informaion of `std::string`.

```C++
template<ipr::Class iprT>
class Descriptor
{
  static print()
  { std::cout << T.name() << std::endl; }
};

usage:
int foo()
{
  Descriptor<std::string>::printName();
}
```

This IPR node optionally can be forwarded to a constexpr object with the `->` syntax.

 `template<ipr::Class T> -> (SomeDriver driver)`

 here `driver` will be a dependent name that can be used during template instantiation.
 `SomeDriver` must be a constexpr constructor with a corresponding `ipr::...` argument:

 ` class SomeDriver { constexpr SomeDriver(const ipr::Class& classDecl)  ... }; `

 Template declaration syntax is extended with auto template. This creates a standalone dependent names in a namespaced scope:
```C++
template<ipr::Class T>
auto MacroName
{
  class Foo {};
  int bar;
  void $("test" + T.name())();
}

// usage
MacroName<Foo>;

// expanded to
class Foo;
int bar;
void testFoo();
```

you can attach a driver to an auto template
```C++
template<ipr::Class T>
auto MacroName <-> (SomeDriver driver)
{
  void foo() { std::cout << driver.foo() << std::endl; }
}
```

auto template can be restricted to a grammar object
```C++
// this will be a "type-template"
template<>
ipr::Type MacroName
{
  int
}

// this is a named-template with attribute
template<ipr::Class T>
ipr::Attribute MacroName
{
  [[ $(T.name() + "some_attribute") ]]
}
...

```

This way can leads to a better C macro

IPR node -> language object transition (writing)
------------------------------------------------

There are two way of pasting a dependent name
 - the `$(...)` syntax
 - the `typename<...>` syntax
 
 `typename<...>` syntax is for improve the code readability where variable is created
 
 they can accept a string literal but it must be an grammatically correct identifier
```C++
typename<"aaa"> // OK
typename<"aa1"> // OK
typename<"aa_"> // OK
typename<""> // illformed
typename<" "> // illformed
typename<"1aa"> // illformed
typename<"aa("> // illformed
typename<"#aa"> // illformed

template<ipr::Class T>
struct Foo
{
  typename<T.name()> member1; // OK from ipr::Name
  typename<T.type()> member2; // OK from ipr::Type
  typename<"aaa" + T.name()> member3; // OK from ipr::Name, Note1
  typename<s"aaa" + T.name()> member4; // OK from ipr::Name, Note2
  typename<T.name() + "aaa"> member5; // OK from ipr::Name
};

// Note1: we need a built-in operator+(const char*, const ipr::Name&)
// Note2: we need a built-in operator+(const std::string&, const ipr::Name&)

```

You can acces the ipr:: Node methods directly
```C++
template<ipr::Class T>
class Foo : public typename<T.name() + "Test">
{
  typename<T.name() + "Test"> memberName;
};
```
 
Or you can use a driver if there is a complex transforming
```C++
template<ipr::Class T> <-> (SomeDriver driver)
class Foo : public typename<driver.foo()>
{
  typename<driver.bar()> $(driver.name());
};
```

Repeating with "for-template"
-----------------------------

A helper `for<..:..>` for template grammar object is created for repeating grammar parts. It accepts constexprs containers and works like a range base for.
The syntax is simple `for <item : container> { }`

`{ }` doesn't introduce scope.

Only full statements can be repeated.

Selecting with "if-template"
----------------------------

// TODO

Separate out template logic with drivers
----------------------------------------

// TODO

Standardized IPR nodes
----------------------

TODO: modelled after Pivot https://parasol.tamu.edu/pivot/

Some used method:

* ipr::Class
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_class.html
  * `const Sequence<Decl>& members() const`

* ipr::Decl
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_decl.html
  * `virtual const Type& type() const = 0`
  * `virtual const Name& name() const = 0`

* ipr::Type
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_type.html
  * `virtual const Name& name() const = 0`

* ipr::Enum
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_enum.html
  * `virtual const Sequence<Enumerator>& members() const = 0`
  
* ipr::Enumerator
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_enumerator.html
  * `virtual const Name& name() const = 0`
  * `virtual const Expr& initializer() const = 0`
  * `virtual int position() const = 0`
  
* ipr::Name

* ipr::Identifier : ipr::Name
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_identifier.html
  * `typedef const String& Arg_type`
  * `Arg_type string() const`

Other use cases
------------------

### Struct-of-Arrays vector

```C++
// origin
struct S
{
    int a;
    int b;
    int c;
};

// SoAGenerator will be a dependent name
template<ipr::Class T>
auto SoAGenerator
{
  struct typename<"SoA_vector_of_" + T.name()>
  {
    for <member : T.members()>
    {
      std::vector<typename<member.type()>> $(member.name());
    }
  };
}

// usage
SoAGenerator<S>;

// expands to
struct SoA_vector_of_S
{
    std::vector<int> a;
    std::vector<int> b;
    std::vector<int> c;
};
```

### Replacing assert

```C++
template<ipr::Expression Node>
void assert()
{
  // get_result() is a const ref to the result
  if (!$(Node.result()))
  {
    std::cout << "failed assert: '" << Node.stringify() << "' in file: ";
    std::cout << Node.source().file_name() << " at line:";
    std::cout << Node.source().pos_line() << std::endl;
  }
}

// usage
int main()
{
  int a = 1;
  int b = 2;
  assert<a == b>();
}
```

### Enumerating enums

This code snippet converts a string to an enum. The converter implementation is a function template.

https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_enum.html
ipr::Enum has `virtual const Sequence<Enumerator>& members() const = 0`


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

template<ipr::Enum T>
void Json::writeTo(const T& obj, std::ostream& os)
{
  switch (obj)
  {
    for <enumerator : T.members()>
    {
       case $(enumerator):
         os << $(enumerator.name().stringify());
         return;
    }
  }
}

int main()
{
  Evote vote == EVote::Yes;

  // prints "EVote::Yes" to std::cout
  Json::writeTo(vote, std::cout);

  return 0;
}
```

### Code checking (also concepts check)

If we use the driver without an instance name it means that the driver is doing only checks, but in this case
there is no reason for a class, only a constexpr function is enough.

#### For concepts check
```C++
constexpr void ConceptsChecker(const ipr::Class& classDecl)
{
  static_assert(classDecl.hasMoveConstructor());
}

// attached checker for a class template
template<ipr::Class T> <-> (ConceptsChecker)
class Foo
{
}
```

#### For compile time call site parameter check
```C++
constexpr void FormatChecker(const ipr::FunctionDecl& decl)
{
  if (expr.isa<ipr::Literal>()) 
  {
     // do format check for format string
  }
}

// attached checker for a normal function
printDate(const char* formatStr) -> void <-> (FormatChecker)
{
}
```

#### For compile time general call site check, ideal for ex. tutorials and coding style checkers.
```C++
// attached checker after a normal function
constexpr TutorialChecker(const ipr::CompoundStmt& stmt)
{
 // check for C function and show std::cout example
}

int main()
{
  printf("My first program!");
  return 0;
} <-> (TutorialChecker, CodingStyleChecker)

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
  ipr::Name key;
  ipr::Token colon = ':';
  ipr::Expr value;
};

struct JsonParamGrammarTail : ipr::grammar
{
  ipr::Token comma = ',';
  JsonParamGrammarItem item;
};

struct JsonParamGrammarBody : ipr::grammar
{
  JsonParamGrammarItem paramsFirst;
  ipr::vector<JsonParamGrammarTail> paramMore; // this can be used for varargs ...
}

struct JsonParamGrammar : ipr::grammar
{
  ipr::Token open_brace = '{';
  std::optional<JsonParamGrammarBody> body;
  ipr::Token close_brace = '}';
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
  template<JsonParamGrammar jsonData>
  SomeWidget()
  {
    if (jsonData.body)
    {
      std::cout << jsonData.body.key.name() << ": " << jsonData.body.value.result().stringify() << std::endl;
      for (auto& it : jsonData.body.paramMore)
        std::cout << it.item.key.name()  << ": " << it.item.value.result().stringify() << std::endl;
    }
    ...
  }

  SomeWindow window;
  SomeLabel label;
};

SomeWidget<{
              window: "Hello world",
              label:  "Foo"
            }>();
```

Extending the language
----------------------

```
[temp]
template-declaration:
    template < template-parameter-list > < - > (  class-name template-driver-name ) declaration 
    template < template-parameter-list > auto identifier { declaration-seq? }

template-driver-name:
    identifier

[temp.explicit]
explicit-instantiation:
    explicit-ipr-instantiation

explicit-ipr-instantiation:
    for < for-template-iter-name : for-range-initializer > { declaration-seq? }
    if < constant-expression > { declaration-seq? }

for-template-iter-name:
    identifier

[temp.param]
template-parameter:
    ipr-parameter

ipr-parameter:
    ipr-type-name ipr-parameter-name
    
ipr-parameter-name:
    identifier

ipr-type-name:
    ipr::Class
    ipr::Enum
    ipr::Namespace
    ipr::Template
    ipr::Union
    ipr::Expr

[stmt.stmt]
statement:
    explicit-ipr-instantiation

[expr.prim.general]
primary-expression:
    pasting-expression

pasting-expression:
    typename < ipr-parameter-name >
    $ ( ipr-parameter-name )
```

Links
-----

The Pivot is a framework
https://parasol.tamu.edu/pivot/

You can comment it on (isocpp.org/forums -> SG7 â€“ Reflection)
https://groups.google.com/a/isocpp.org/forum/#!forum/reflection

The idea of this proposal comes from the AngularJS HTML template framework:
http://angularjs.org/

prebuilt directives ngRepeat directive in module ng:
http://docs.angularjs.org/api/ng.directive:ngRepeat

These code snippet based on this repo:
https://github.com/hun-nemethpeter/cpp-reflector-mini

Call for Compile-Time Reflection Proposals:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3814.html
