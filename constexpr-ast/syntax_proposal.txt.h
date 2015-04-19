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
  <for> (member : T.class.members())
    // syntax 1 - normal declaration syntax, but type come from member.type()
    //            id come from member.name()
    ''member.type()'' ''member.name()'';

    // syntax 2
    ''member.name()'': member.type();

    // syntax 3 - type first then name
    ''member.type()'': member.name();

    // syntax 4 - xml like syntax
    <auto type="member.type()" name="member.name()"/>

    // syntax 4 - cast to a decltype
    (decltype)member.type(): member.name();
  </for>
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

// ========== if<> ============
template<typename T>
struct TestClass()
{
  // A Preliminary Proposal for a Static if
  // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3322.pdf
  //
  // static if declaration
  // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3329.pdf
  //
  // “Static If” Considered
  // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3613.pdf
  //
  // 1. statif if was two keyword
  //
  // static
  //
  // /*
  //   blah blah blah
  // */
  //
  // if (sizeof(T)==8) {
  //   gun(32);
  // else
  //   gun();
  // }
  //
  // 2. static if braces looks like a scope
  //
  // 3. If we do not provide a static_for, hackers will simply make one out of static_if or fake equivalents.
  //
  //
  // now:
  // - <if> and </if> is one keyword, no space allowed
  // - <else> also different
  // - <if> doesn't use braces
  // - <for> is provided
  // - new, but familiar syntax

  <if> (member.size() == 0)
    char dummy[4];
  <else>
    <if> (...)
    </if>
  </if>
}
