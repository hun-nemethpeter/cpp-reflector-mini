class Player
{
  public:
    int id;
    std::string name;
};

static_assert(Player.class.fields.size() == 2, "");

template<typename T>
void printClassName()
{
  std::cout << ''"T.class.name()]"'' << std::endl;
}

template<typename T>
class ClassNameAsMember
{
  int ''T.class.name()'';
};

// instatize:
// ClassNameAsMember<Player>;
//
// result:
// class ClassNameAsMember<Player>
// {
//    int Player;
// };

template<typename T>
class MirrorClass()
{
  for <member : T.class.members()>
  {
    // syntax 1
    ''member.type()'' ''member.name()'';

    // syntax 2
    ''member.name()'': member.type();
  }

  // A Preliminary Proposal for a Static if
  // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3322.pdf
  //
  // static if declaration
  // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3329.pdf
  //
  // “Static If” Considered
  // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3613.pdf
  if <member.size() == 0>
  {
    char dummy[4];
  }
}

Example
int a;
std::ast::ast_var ast = a.class;

// two form of pasting:
// declare something
''ast.name() + "1"'': ast.type();
result: int a1;

// stringify something
''"ast.name()"''
result: "a"

// generic expression call
std::ast::ast_identifier function_id = "test";
''function_id''(1, 2, 3);
result: test(1, 2, 3);

// generic template instantiation
std::ast::ast_identifier template_id = "test";
''template_id''<int>;
result: test<int>;

