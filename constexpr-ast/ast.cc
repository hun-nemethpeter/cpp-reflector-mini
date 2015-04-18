#include <ast.h>

namespace std { namespace ast {

const ast_linkage linkage::c("C");
const ast_linkage linkage::cpp("C++");

const ast_sum sum::empty;

const ast_identifier keywords::id_void("void");
const ast_identifier keywords::id_bool("bool");
const ast_identifier keywords::id_char("char");
const ast_identifier keywords::id_schar("signed char");
const ast_identifier keywords::id_uchar("unsigned char");
const ast_identifier keywords::id_wchar_t("wchar_t");
const ast_identifier keywords::id_short("short");
const ast_identifier keywords::id_ushort("unsigned short");
const ast_identifier keywords::id_int("int ");
const ast_identifier keywords::id_uint("unsigned int");
const ast_identifier keywords::id_long("long");
const ast_identifier keywords::id_ulong("unsigned long");
const ast_identifier keywords::id_long_long("long long");
const ast_identifier keywords::id_ulong_long("unsigned long long");
const ast_identifier keywords::id_float("float");
const ast_identifier keywords::id_double("double");
const ast_identifier keywords::id_long_double("long double");

const ast_as_type builtin_types::type_void(keywords::id_void);
const ast_as_type builtin_types::type_bool(keywords::id_bool);
const ast_as_type builtin_types::type_char(keywords::id_char);
const ast_as_type builtin_types::type_schar(keywords::id_schar);
const ast_as_type builtin_types::type_uchar(keywords::id_uchar);
const ast_as_type builtin_types::type_wchar_t(keywords::id_wchar_t);
const ast_as_type builtin_types::type_short(keywords::id_schar);
const ast_as_type builtin_types::type_ushort(keywords::id_uchar);
const ast_as_type builtin_types::type_int(keywords::id_int);
const ast_as_type builtin_types::type_uint(keywords::id_uint);
const ast_as_type builtin_types::type_long(keywords::id_long);
const ast_as_type builtin_types::type_ulong(keywords::id_ulong);
const ast_as_type builtin_types::type_long_long(keywords::id_long_long);
const ast_as_type builtin_types::type_ulong_long(keywords::id_ulong);
const ast_as_type builtin_types::type_float(keywords::id_float);
const ast_as_type builtin_types::type_double(keywords::id_double);
const ast_as_type builtin_types::type_long_double(keywords::id_long_double);

}}
