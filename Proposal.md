Code generators
===============
(Draft rev 3)

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
      $for (auto member : $driver.members) {
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

`$define` and `$driver` directives wait a driver which is constexpr object.
In directives and in template driver variables you can use the constexpr object's methods and members.
Template driver parameters start with the dollar `$` sign.

Generating code parts is safe, because you can create only a typed id.

Some basic rules:
 * you have to use the `{ ... }` syntax after `$for`, `$if`, `$switch`, `$while` directives
 * you can create `meta::id_name` (in member, variable or parameter declaration context)
 * you can't create `meta::type_name` only compiler able to generate it. // TODO: what about class declaration?
 * `$define` define a named generator. Can be used later with the `$name` syntax, where name is the defined name.
 * `$use` can be attached to a template or can be scoped with `{ ... }`

Where the magic happens?
------------------------

1. From `User` to `ClassDecl`

  * Compiler sees that `$OperatorEqGenerator` is a defined generator with a driver where driver expect one parameter
  * Compiler generates an AST node struct for `User` with base type `decl`. Compiler can use its internal AST with a standandardized wrapper.
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

  * `template<astnode Node> $use<AssertDriver driver>` where astnode can be an expression but Driver got an AST node
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
  $driver.class_name
  {
    $for (auto member : $driver.members) {
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
    meta::id_name name; // you are create id_name
    meta::type_name type; // you can't create type_name only compiler able to generate it.
                          // you can get one from a compiler generated Decl class
                          // it has copy ctor
  };
  constexpr SoADriver(const ClassDecl& fromClassDecl, const ClassDecl& toClassDecl)
  {
    class_name = toClassDecl.getTypeName();
    for (auto& field : fromClassDecl.fields())
      members.emplace_back({field.getTypeName(),  field.getName() + "s", });
  }
  meta::type_name class_name;
  meta::vector<Member> members;
};

// usage
struct SoA_vector_of_S; // forward declaration
$SoAGenerator(S, SoA_vector_of_S);
```

### Replacing assert

My best solution is introduce a new keyword called "astnode"

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
// new keyword astnode, allowed only with $driver
template<astnode Node> $use<AssertDriver driver>
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
The function template parameter is "captured" before template instatization, and processed with EnumDriver.
EnumDriver's constructor is waiting an const EnumDecl* that is coming from meta::class<T>.

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
    $for (auto enumValueName : $driver.enumValueNames) {
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

We can replace the old macro system with this new one. For this reason $if and $swicth is introduced.
For normal code flow this conditional compilation as harmful (or helpful) as the normal macro style one.
This is just a new syntax. Some intended usage:

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

// function with a driver
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
  constexpr ConceptDriver(const ClassDecl* classDecl) // first check, T must be an class
    // for more check, see http://clang.llvm.org/doxygen/classclang_1_1CXXRecordDecl.html
    static_assert(classDecl->hasMoveConstructor(), "Move constructor is missing")
  }
};
```

Drivers
=======

Example

// TODO: more example

```C++
$use(NamespaceDriver driver)
{
  namespace $driver.name
  {
    $for (auto className : $driver.classNames)
    {
      class $className
      {
      };
    }
  }
}
```

A case study:
-------------

from this enum declaration:

```C++
enum class EVote
{
  Yes,
  RatherYes,
  Undecided,
  RatherNo,
  No
};
```

I want to generate this declaration:

```C++
template<>
void Json::readFrom(boost::optional<EVote>& obj, const std::string& data)
{
  folly::fbstring jsonVal = folly::parseJson(data).asString();
  llvm::StringRef decoded(jsonVal.c_str(), jsonVal.length());
  obj = llvm::StringSwitch<EVote>(decoded)
    .Case("EVote::Yes", EVote::Yes)
    .Case("EVote::RatherYes", EVote::RatherYes)
    .Case("EVote::Undecided", EVote::Undecided)
    .Case("EVote::RatherNo", EVote::RatherNo)
    .Case("EVote::No", EVote::No);
}
```

With this external code generator code snipshet. This code snippet comes from reflector-mini (but not from that version that I posted, because other parts of it is not open sourced)

```C++
const EnumDecl* enumDecl;
std::string enumName = enumDecl->getNameAsString();
// -------------------------------------------------------------------------------------------------------------------
void processEnumDefsForJsonReadFromHeader()
{
  ssForJsonCc <<
    "\n"
    "template<>\n"
    "void Json::readFrom(boost::optional<" << enumName << ">& obj, const std::string& data)\n"
    "{\n"
    "  folly::fbstring jsonVal = folly::parseJson(data).asString();\n"
    "  llvm::StringRef decoded(jsonVal.c_str(), jsonVal.length());\n"
    "  obj = llvm::StringSwitch<" << enumName << ">(decoded)";
}
// -------------------------------------------------------------------------------------------------------------------
void processEnumDefsForJsonReadFromBody()
{
  for (auto it = enumDecl->enumerator_begin(); it != enumDecl->enumerator_end(); it++)
  {
    ssForJsonCc <<
      "\n"
      "    .Case(\"" << scope << (*it)->getName().data() << "\", " << scope << (*it)->getName().data() << ")";
  }
  ssForJsonCc <<
    ";\n";
}
// -------------------------------------------------------------------------------------------------------------------
void processEnumDefsForJsonReadFromTail()
{
  ssForJsonCc <<
    "}\n";
}
```

Now, here come some completely different.
AngulerJS Html template system in 1 minute
More tutorials on:
http://docs.angularjs.org/tutorial/step_02

```HTML
<!doctype html>
<html ng-app>
  <head>
    <script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.0.8/angular.min.js"></script>
    <script src="todo.js"></script>
    <link rel="stylesheet" href="todo.css">
  </head>
  <body>
    <h2>Todo</h2>
    <div ng-controller="TodoCtrl">
      <span>{{remaining()}} of {{todos.length}} remaining</span>
      [ <a href="" ng-click="archive()">archive</a> ]
      <ul class="unstyled">
        <li ng-repeat="todo in todos">
        <input type="checkbox" ng-model="todo.done">
        <span class="done-{{todo.done}}">{{todo.text}}</span>
        </li>
      </ul>
      <form ng-submit="addTodo()">
        <input type="text" ng-model="todoText" size="30"
        placeholder="add new todo here">
        <input class="btn-primary" type="submit" value="add">
      </form>
    </div>
  </body>
</html>
```

```JavaScript
function TodoCtrl($scope) { // used in <div ng-controller="TodoCtrl">
  $scope.todos = [ // used in {{todos.length}} and ng-repeat="todo in todos"
  {text:'learn angular', done:true},
  {text:'build an angular app', done:false}];

  $scope.addTodo = function() { // used in ng-submit="addTodo()"
    $scope.todos.push({text:$scope.todoText, done:false});
    $scope.todoText = '';
  };

  $scope.remaining = function() { // used in {{remaining()}}
    var count = 0;
    angular.forEach($scope.todos, function(todo) {
      count += todo.done ? 0 : 1;
    });
    return count;
  };

  $scope.archive = function() { // used in ng-click="archive()"
    var oldTodos = $scope.todos;
    $scope.todos = [];
    angular.forEach(oldTodos, function(todo) {
      if (!todo.done) $scope.todos.push(todo);
    });
  };
}
```

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
