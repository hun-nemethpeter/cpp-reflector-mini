#include <str_const.h>
#include <ast.h>

using namespace std::ast;

void test_str_const()
{
  constexpr str_const test("Hello!!");
  static_assert(test.size() == 7, "");
  static_assert(test[6] == '!', "");
  static_assert(test[7] == 0, "");
}

void test_ast_class()
{
  constexpr static ast_class test_ast_class;
  constexpr sequence<ast_class> classes_empty(nullptr, 0);
  constexpr sequence<ast_class> classes(&test_ast_class, 1);
  static_assert(classes.size() == 1, "");
  static_assert(classes.size() == 1, "");
}

void test_ast_building()
{
  static const ast_identifier id_test("test");
  static const ast_identifier id_param1("param1");
  static const ast_identifier id_param2("param2");

  //  examples
  //  int test;
  const ast_var test_var1(&builtin_types::type_int, &id_test);

  //  const int test;
  static const ast_qualified const_int(ast_type::Const, &builtin_types::type_int);
  const ast_var test_var2(&const_int, &id_test);

  //  const int* test;
  static const ast_pointer pointer_to_const_int(&const_int);
  const ast_var test_var3(&pointer_to_const_int, &id_test);

  //  int& test;
  static const ast_reference int_reference(&builtin_types::type_int);
  const ast_var test_var4(&int_reference, &id_test);

  //  double test[2];
  static const ast_string str_2("2");
  static const ast_literal literal_2(&builtin_types::type_int, &str_2);
  static const ast_array double_array_2(&builtin_types::type_double, &literal_2);
  const ast_var test_var5(&double_array_2, &id_test);

  //  int** test;
  static const ast_pointer pointer_to_int(&builtin_types::type_int);
  static const ast_pointer pointer_to_pointer_to_int(&pointer_to_int);
  const ast_var test_var6(&pointer_to_pointer_to_int, &id_test);

  // abstract function declaration
  // int (*)();
  // usage1:
  // auto fooFn1_Cpp11() -> int (*)();
  //                        ^^^^^^^^^ // only this
  // usgae2:
  // int (*fooFn1_Cpp89())(); // almost impossible to read this declaration for an avarage programmer
  //
  // usage3:
  // typedef int (*fooFn_typedef)();
  // fooFn_typedef foo_fn();
  static const ast_function abstract_int_funtion_param0(nullptr, &builtin_types::type_int);


  //  int (*)(char, double);
  auto fooFn2() -> int (*)(char, double);
  static const sequence<const ast_type*> param_seq = { &builtin_types::type_char, &builtin_types::type_double };
  static const ast_product param_list_2(&param_seq);
  static const ast_function abstract_int_funtion_param_2(&param_list_2, &builtin_types::type_int);

  //  int test();
  static const ast_fundecl int_funtion_param0(&id_test, &abstract_int_funtion_param0, nullptr);

  //  int test(char param1, double param2);
  static const ast_parameter test_param_1(&builtin_types::type_char, &id_param1);
  static const ast_parameter test_param_2(&builtin_types::type_double, &id_param2);
  static const ast_parameter_list test_param_list_2 = { &test_param_1, &test_param_2 };
  static const ast_mapping mappings_param_2(&test_param_list_2);
  static const ast_fundecl int_funtion_param2(&id_test, &abstract_int_funtion_param_2, &mappings_param_2);

  //  class test;
  //  TODO
}

int main()
{
  test_str_const();
  test_ast_class();
  test_ast_building();

  return 0;
}
