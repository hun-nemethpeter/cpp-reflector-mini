#include <ast.h>

namespace std { namespace ast {

const ast_sum sum::empty;

constexpr ast_linkage linkage::c;
constexpr ast_linkage linkage::cpp;

constexpr ast_identifier keywords::id_void;

constexpr ast_identifier keywords::id_bool;

constexpr ast_identifier keywords::id_char;
constexpr ast_identifier keywords::id_schar;
constexpr ast_identifier keywords::id_uchar;

constexpr ast_identifier keywords::id_wchar_t;

constexpr ast_identifier keywords::id_short;
constexpr ast_identifier keywords::id_ushort;

constexpr ast_identifier keywords::id_int;
constexpr ast_identifier keywords::id_uint;

constexpr ast_identifier keywords::id_long;
constexpr ast_identifier keywords::id_ulong;

constexpr ast_identifier keywords::id_long_long;
constexpr ast_identifier keywords::id_ulong_long;

constexpr ast_identifier keywords::id_float;
constexpr ast_identifier keywords::id_double;
constexpr ast_identifier keywords::id_long_double;

constexpr ast_as_type builtin_types::type_void;

constexpr ast_as_type builtin_types::type_bool;

constexpr ast_as_type builtin_types::type_char;
constexpr ast_as_type builtin_types::type_schar;
constexpr ast_as_type builtin_types::type_uchar;

constexpr ast_as_type builtin_types::type_wchar_t;

constexpr ast_as_type builtin_types::type_short;
constexpr ast_as_type builtin_types::type_ushort;

constexpr ast_as_type builtin_types::type_int;
constexpr ast_as_type builtin_types::type_uint;

constexpr ast_as_type builtin_types::type_long;
constexpr ast_as_type builtin_types::type_ulong;

constexpr ast_as_type builtin_types::type_long_long;
constexpr ast_as_type builtin_types::type_ulong_long;

constexpr ast_as_type builtin_types::type_float;
constexpr ast_as_type builtin_types::type_double;
constexpr ast_as_type builtin_types::type_long_double;

}}
