This proposal based on the AngularJS Mtml template framework.
http://angularjs.org/
I posted earlier my mini reflector tool, these code snippet comes from there.


So a case study:

from this enum declaration:

This proposal based on the AngularJS Mtml template framework.
http://angularjs.org/
I posted earlier my mini reflector tool, these code snippet comes from there.


So a case study:

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

No, here come some completely different.
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

prebuilt directives
ngRepeat
directive in module ng
http://docs.angularjs.org/api/ng.directive:ngRepeat

So based on above AngularC++ looks like this:

```C++
// tamplate part
template<> [[std::controller="EnumController(meta::class<EVote>) ctrl"]]
void Json::readFrom(boost::optional<$ctrl.enumName$>& obj, const std::string& data)
{
  folly::fbstring jsonVal = folly::parseJson(data).asString();
  llvm::StringRef decoded(jsonVal.c_str(), jsonVal.length());
  obj = llvm::StringSwitch<$ctrl.enumName$>(decoded)
    [[std::repeat="enumValueName:enumValueNames"]]
    .Case($ctrl.enumValueName$, {{enumValueName}})
  ;
}

// controller part
class EnumController
{
  const EnumDecl* enumDecl;
  public:
    // used in [[std::controller="EnumController(meta::class<EVote>) ctrl"]]
    constexpr EnumController(const EnumDecl* enumDecl)
      : enumDecl(enumDecl)
    {
      std::string enumName = enumDecl->getNameAsString();
      for (auto it = enumDecl->enumerator_begin(); it != enumDecl->enumerator_end(); it++)
        enumNames.push_back((*it)->getName().data());
    }
    // used in [[std::repeat="enumValueName:ctrl.enumValueNames"]]
    meta::vector<meta::id_string> enumValueNames;
    // used in $ctrl.enumName$
    meta::id_string enumName;
};
```

