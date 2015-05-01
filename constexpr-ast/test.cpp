// first working version
// no runtime info was generated
// repo: https://github.com/hun-nemethpeter/clang in branch typid_ast
// direct link to patch: https://github.com/llvm-mirror/clang/compare/master...hun-nemethpeter:typid_ast

#include "ast.h"

struct Test
{
  bool member1;
  char member2;
};

int main()
{
  static_assert(typeid<Test>.name() == "Test");
  static_assert(typeid<Test>.members().size() == 2);
  static_assert(typeid<Test>.members()[0].name() == "member1");
  static_assert(typeid<Test>.members()[0].type().name() == "bool");
  static_assert(typeid<Test>.members()[1].name() == "member2");
  static_assert(typeid<Test>.members()[1].type().name() == "char");
  static_assert(typeid<Test>.bases().size() == 0);
  static_assert(typeid<Test2>.name() == "Test2");

  return 0;
}
