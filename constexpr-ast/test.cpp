// first working version
// no runtime info was generated
// repo: https://github.com/hun-nemethpeter/clang in branch typid_ast
// direct link to patch: https://github.com/llvm-mirror/clang/compare/master...hun-nemethpeter:typid_ast

#include "ast.h"

struct Test
{
  bool               member1;
  char               member2;
  signed char        member3;
  unsigned char      member4;
  wchar_t            member5;
  short              member6;
  unsigned short     member7;
  int                member8;
  unsigned int       member9;
  long               member10;
  unsigned long      member11;
  long long          member12;
  unsigned long long member13;
  float              member14;
  double             member15;
  long double        member16;
};

struct Test2
{
  int a;
};

int main()
{
  static_assert(typeid<Test>.name() == "Test", "");
  static_assert(typeid<Test>.members().size() == 16, "");
  static_assert(typeid<Test>.members()[0].name() == "member1", "");
  static_assert(typeid<Test>.members()[0].type().name() == "bool", "");
  static_assert(typeid<Test>.members()[1].name() == "member2", "");
  static_assert(typeid<Test>.members()[1].type().name() == "char", "");
  static_assert(typeid<Test>.bases().size() == 0, "");
  static_assert(typeid<Test2>.name() == "Test2", "");

  return 0;
}
