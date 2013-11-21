Code generators and drivers (CGD)
=================================
(Draft rev 2)

Why?
----

C++ is a great programming language, but it falters when we try to use it for cloning and
generating code parts. Template drivers lets you create very complicate clones of a given code part
(class, enums, templates, statements) with familiar syntax.
The result is extraordinarily expressive, readable, and quick to develop.

How?
----

The idea come from the AngularJS templating system which is a proven to work an efficient solution for HTML templating.
Drivers are constexpr objects. Manipulating code parts are directed with directives.

Directives are: `meta::driver`, `meta::define`, `meta::driver`, `$for`, `$if`, `$switch`, `$while`

`meta::define` and `meta::driver` directives wait a driver which is constexpr object.
In directives and in template driver variables you can use the constexpr object's methods and members.
Template driver parameters start with the dollar `$` sign.

Generating code parts is safe, because you can't create new type only just using an existing one it in CGD.

Some basic rules:
 * you have to use the `{ ... }` syntax after `$for`, `$if`, `$switch`, `$while` directives

 * you can create `meta::id_name` (in member, variable or parameter declaration context)

 * you can't create `meta::type_name` only compiler able to generate it. // TODO: what about class declaration?

 * `meta::define` define a named CGD. Can be used later with the `$name` syntax, where name is the defined name.

 * `meta::driver` attach a driver to a template will be called during template instantiation.

Targeted use cases
------------------

### Generating equality operators

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
[[meta::define(OperatorEqGenerator, "EqualityDriver driver")]]
bool $driver.class_name::operator==(const $driver.class_name& rhs) const
{
    return true
      $for (auto member : $driver.members) {
        && $member == rhs.$member
      }
    ;
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
struct [[meta::define(SoAGenerator, "SoADriver driver")]]
$driver.class_name {
    $for (auto member : $driver.members) {
      std::vector<$member.type> $member.name;
    }
};

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

// declaration
[[meta::driver("AssertDriver driver(Node))"]]
template<astnode Node> // new keyword astnode, allowed only in a meta::driver
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

// driver template
[[meta::driver("EnumDriver driver(meta::class<T>))"]]
template<typename T>
// EnumDriver is a constexpr class, that gets a compiler generated AST node
// in constructor parameter, through meta::class<T> in a type safe manner.
// (ex. enum declaration generates const EnumDecl*,
// and a class declaration generates const CXXRecordDecl*).
// driver instance name will be "driver", this is a normal C++ syntax
// members and methods of this object can be accessed with the following syntax:
// $driver.member or $driver.method(param1, param2, ...)
void Json::readFrom($driver.enumName& obj, const std::string& data)
{
  obj = llvm::StringSwitch<$driver.enumName>(data)
    // controlling directive meta::for, with the syntax of range base for
    // enumValueName will be a local variable of a CGD
    // directive scope here is the method call
    $for (auto enumValueName : $driver.enumValueNames) {
      .Case($enumValueName.asStr(), $enumValueName) }
  ;
}

// driver
class EnumDriver
{
  const EnumDecl& enumDecl;
  public:
    constexpr EnumDriver(const EnumDecl& enumDecl)
      : enumDecl(enumDecl)
    {
      std::string enumName = enumDecl.getNameAsString();
      for (auto& enumerator : enumDecl.enumerators())
        enumValueNames.push_back(enumerator.getName());
    }
    // used in $for (auto enumValueName : $driver.enumValueNames)
    // meta::vector is a constexpr vector
    meta::vector<meta::id_name> enumValueNames;
    // used in $driver.enumName
    // meta::id_name is a constexpr string that contains only valid C++ identifier
    // it has an asStr() that gives back a stringified string, so between "" signes
    meta::id_name enumName;
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

[[meta::driver("ConfigurationDriver driver()"]] // not: default constructor is used
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

[[meta::driver("ConfigurationDriver driver()"]] // not: default constructor is used
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
```

### Concept checking

If we use the meta::driver without an instance name it means that the driver is doing only checks

```C++
template<typename T> [[meta::driver("ConceptDriver(T)"]]
class Foo
{
}

struct ConceptDriver {
  constexpr ConfigurationDriver(const ClassDecl* classDecl) // first check, T must be an class
    // for more check, see http://clang.llvm.org/doxygen/classclang_1_1CXXRecordDecl.html
    static_assert(classDecl->hasMoveConstructor(), "Move constructor is missing")
  }
};
```

Drivers
=======

Driver attached to:

```C++
// will need a NamespaceDriver(const namespaceDecl* ) declaration
namespace [[meta::driver("NamespaceDriver  driver")]] foo {}


// will need a ClassDriver(const classDecl*)
class [[meta::driver("ClassDriver driver")]] foo
{
  void method(bool param);
  int member;
};

// will need a MethodDriver(const methodDecl*)
[[meta::driver("MethodDriver driver")]]
class foo::method(bool param)
{
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
