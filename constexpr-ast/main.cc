#include <str_const.h>
#include <ast.h>
void gets();
#include <iostream>

using namespace std::ast;

void test_str_const()
{
  constexpr str_const test("Hello!!");
  static_assert(test.size() == 7, "");
  static_assert(test[6] == '!', "");
  static_assert(test[7] == 0, "");
}

void test_ast_building()
{
  static constexpr ast_identifier id_test("test");
  static constexpr ast_identifier id_param1("param1");
  static constexpr ast_identifier id_param2("param2");
  static constexpr ast_identifier id_member1("member1");
  static constexpr ast_identifier id_member2("member2");

  //  examples
  //  int test;
  constexpr ast_var test_var1(builtin_types::type_int, id_test);
  static_assert(&builtin_types::type_int.name() != nullptr, "");
  static_assert(test_var1.name() == "test", "");
  static_assert(test_var1.type().name() == "int", "");

  //  const int test;
  static constexpr ast_qualified const_int(ast_type::Const, builtin_types::type_int);

  constexpr ast_var test_var2(const_int, id_test);
  static_assert(const_int.qualifiers() == ast_type::Const, "");
  static_assert(test_var2.type().kind() == kind_qualified, "");
  static_assert(get_as<ast_qualified>(test_var2.type()).qualifiers() == ast_type::Const, "");
  static_assert(test_var2.name() == "test", "");
  static_assert(test_var2.type().name() == "int", "");

  //  const int* test;
  static constexpr ast_pointer pointer_to_const_int(const_int);
  constexpr ast_var test_var3(pointer_to_const_int, id_test);
  static_assert(test_var3.name() == "test", "");

  //  int& test;
  static constexpr ast_reference int_reference(builtin_types::type_int);
  constexpr ast_var test_var4(int_reference, id_test);
  static_assert(test_var4.name() == "test", "");

  //  double test[2];
  static constexpr ast_string str_2("2");
  static constexpr ast_literal literal_2(builtin_types::type_int, str_2);
  static constexpr ast_array double_array_2(builtin_types::type_double, literal_2);
  constexpr ast_var test_var5(double_array_2, id_test);
  static_assert(test_var5.name() == "test", "");

  //  int** test;
  static constexpr ast_pointer pointer_to_int(builtin_types::type_int);
  static constexpr ast_pointer pointer_to_pointer_to_int(pointer_to_int);
  constexpr ast_var test_var6(pointer_to_pointer_to_int, id_test);
  static_assert(test_var6.name() == "test", "");

  // abstract function declaration
  // int (*)();
  static constexpr ast_product param_list_0;
  static constexpr ast_function abstract_int_funtion_param_0(param_list_0, builtin_types::type_int);
  static_assert(abstract_int_funtion_param_0.target().name() == "int", "");
  static_assert(abstract_int_funtion_param_0.source().elements().size() == 0, "");
  static_assert(abstract_int_funtion_param_0.throws().elements().size() == 0, "");
  static_assert(abstract_int_funtion_param_0.lang_linkage().language() == "C++", "");


  // abstract function declaration with 2 params
  // int (*)(char, double);
  static constexpr const ast_type* param_seq[] = { &builtin_types::type_char, &builtin_types::type_double };
  static constexpr ast_product param_list_2({ param_seq, 2});

  static constexpr ast_function abstract_int_funtion_param_2(param_list_2, builtin_types::type_int);
  static_assert(abstract_int_funtion_param_2.target().name() == "int", "");
  static_assert(abstract_int_funtion_param_2.source().elements().size() == 2, "");
  static_assert(abstract_int_funtion_param_2.source().elements()[0].name() == "char", "");
  static_assert(abstract_int_funtion_param_2.source().elements()[1].name() == "double", "");
  static_assert(abstract_int_funtion_param_2.throws().elements().size() == 0, "");
  static_assert(abstract_int_funtion_param_2.lang_linkage().language() == "C++", "");

  //  int test();
  //  static constexpr ast_parameter_list test_param_list_0({});
  static constexpr ast_mapping mappings_empty;

  static constexpr ast_fundecl int_funtion_param_0(id_test, abstract_int_funtion_param_0, mappings_empty);
  static_assert(int_funtion_param_0.parameters().size() == 0, "");
  static_assert(int_funtion_param_0.name() == "test", "");

  //  int test(char param1, double param2);
  static constexpr ast_parameter test_param_1(builtin_types::type_char, id_param1);
  static constexpr ast_parameter test_param_2(builtin_types::type_double, id_param2);
  static constexpr const ast_parameter* ast_parameters[] = { &test_param_1, &test_param_2 };
  static constexpr ast_parameter_list test_param_list_2({ast_parameters, 2});
  static constexpr ast_mapping mappings_param_2(test_param_list_2);

  static constexpr ast_fundecl int_funtion_param2(id_test, abstract_int_funtion_param_2, mappings_param_2);
  static_assert(int_funtion_param2.name() == "test", "");
  //static_assert(int_funtion_param2.type().name() == "int", ""); // TODO
  static_assert(int_funtion_param2.parameters().size() == 2, "");
  static_assert(int_funtion_param2.parameters()[0].name() == "param1", "");
  static_assert(int_funtion_param2.parameters()[0].type().name() == "char", "");
  static_assert(int_funtion_param2.parameters()[1].name() == "param2", "");
  static_assert(int_funtion_param2.parameters()[1].type().name() == "double", "");

  //  class test;
  static constexpr ast_class test_class_empty(id_test, {}, {});
  static_assert(test_class_empty.name() == "test", "");
  static_assert(test_class_empty.members().size() == 0, "");
  static_assert(test_class_empty.bases().size() == 0, "");

  //  class test
  //  {
  //    char member1;
  //    double member2;
  //  };
  static constexpr ast_var test_member1(builtin_types::type_char, id_member1);
  static constexpr ast_var test_member2(builtin_types::type_double, id_member2);
  static constexpr const ast_decl* test_class_members[] = { &test_member1, &test_member2 };

  static constexpr ast_class test_class(id_test, { test_class_members, 2 }, {});
  static_assert(test_class.name() == "test", "");
  static_assert(test_class.members().size() == 2, "");
  static_assert(test_class.members()[0].name() == "member1", "");
  static_assert(test_class.members()[0].type().name() == "char", "");
  static_assert(test_class.members()[1].name() == "member2", "");
  static_assert(test_class.members()[1].type().name() == "double", "");
  static_assert(test_class.bases().size() == 0, "");
}

int main()
{
  test_str_const();
  test_ast_building();

  return 0;
}
