#include <cstddef>
#include <initializer_list>

#define TODO 0

namespace std
{
namespace ast
{
  class ast_base_type;
  class ast_classic;
  class ast_decl;
  class ast_enumerator;
  class ast_expr;
  class ast_linkage;
  class ast_node;
  class ast_parameter;
  class ast_parameter_list;
  class ast_product;
  class ast_scope;
  class ast_sum;
  class ast_string;
  class ast_type;
  class ast_typedecl;

  enum kind_t
  {
    kind_address,
    kind_alias,
    kind_and,
    kind_annotation,
    kind_array,
    kind_array_delete,
    kind_array_ref,
    kind_arrow,
    kind_arrow_star,
    kind_as_type,
    kind_asm,
    kind_assign,
    kind_base_type,
    kind_bitand_assign,
    kind_bitand,
    kind_bitfield,
    kind_bitor_assign,
    kind_bitor,
    kind_bitxor_assign,
    kind_bitxor,
    kind_block,
    kind_break,
    kind_call,
    kind_cast,
    kind_class,
    kind_comma,
    kind_comment,
    kind_complement,
    kind_conditional,
    kind_const_cast,
    kind_continue,
    kind_conversion,
    kind_ctor_body,
    kind_ctor_name,
    kind_datum,
    kind_decltype,
    kind_delete,
    kind_deref,
    kind_div_assign,
    kind_div,
    kind_do,
    kind_dot,
    kind_dot_star,
    kind_dtor_name,
    kind_dynamic_cast,
    kind_enum,
    kind_enumerator,
    kind_equal,
    kind_expr_list,
    kind_expr_sizeof,
    kind_expr_stmt,
    kind_expr_typeid,
    kind_field,
    kind_for,
    kind_for_in,
    kind_function,
    kind_fundecl,
    kind_goto,
    kind_greater,
    kind_greater_equal,
    kind_handler,
    kind_id_expr,
    kind_identifier,
    kind_if_then,
    kind_if_then_else,
    kind_label,
    kind_labeled_stmt,
    kind_last_code,
    kind_less,
    kind_less_equal,
    kind_linkage,
    kind_literal,
    kind_lshift_assign,
    kind_lshift,
    kind_mapping,
    kind_member_init,
    kind_minus_assign,
    kind_minus,
    kind_modulo_assign,
    kind_modulo,
    kind_mul_assign,
    kind_mul,
    kind_named_map,
    kind_namespace,
    kind_new,
    kind_not,
    kind_not_equal,
    kind_operator,
    kind_or,
    kind_overload,
    kind_parameter,
    kind_paren_expr,
    kind_phantom,
    kind_plus_assign,
    kind_plus,
    kind_pointer,
    kind_post_decrement,
    kind_post_increment,
    kind_pre_decrement,
    kind_pre_increment,
    kind_product,
    kind_ptr_to_member,
    kind_qualified,
    kind_reference,
    kind_region,
    kind_reinterpret_cast,
    kind_return,
    kind_rname,
    kind_rshift_assign,
    kind_rshift,
    kind_rvalue_reference,
    kind_scope,
    kind_scope_ref,
    kind_static_cast,
    kind_string,
    kind_sum,
    kind_switch,
    kind_template,
    kind_template_id,
    kind_throw,
    kind_type_id,
    kind_type_sizeof,
    kind_type_typeid,
    kind_typedecl,
    kind_unary_minus,
    kind_unary_plus,
    kind_union,
    kind_unit,
    kind_var,
    kind_while
  };

  template<typename T>
  class sequence {
    public:
      class iterator;

      typedef T value_type;
      typedef const T& reference;
      typedef const T* pointer;

      constexpr sequence() : data(0), size_(0) {}
      constexpr sequence(const T* data, std::size_t size) : data(data), size_(size) {}
      constexpr sequence(const std::initializer_list<T>& values) : data(values.begin()), size_(values.size()) {}

      constexpr const T& operator[](std::size_t index) const
      { return data[index]; }

      constexpr std::size_t size() const
      { return size_; }

      constexpr iterator begin() const
      { return iterator(this, 0); }

      constexpr iterator end() const
      { return iterator(this, -1); } // TODO

      constexpr const T& get(std::size_t index) const
      { return data[index]; }

    private:
      const T* const data;
      const std::size_t size_;
  };

  template<class T>
  class sequence<T>::iterator
  {
    public:
      constexpr iterator() : seq(0), index(0) {}

      constexpr iterator(const sequence<T>* s, int i) : seq(s), index(i) { }

      constexpr const T& operator*() const
      { return seq->get(index); }

      const T* operator->() const
      { return &seq->get(index); }

      constexpr iterator& operator++()
      {
        ++index;
        return *this;
      }

      constexpr iterator& operator--()
      {
        --index;
        return *this;
      }

      constexpr iterator operator++(int)
      {
        iterator tmp = *this;
        ++index;
        return tmp;
      }

      constexpr iterator operator--(int)
      {
        iterator tmp = *this;
        --index;
        return tmp;
      }

      constexpr bool operator==(iterator other) const
      { return seq == other.seq && index == other.index; }

      constexpr bool operator!=(iterator other) const
      { return !(*this == other); }

    private:
      const sequence<T>* seq;
      int index;
  };

  class ast_node
  {
    public:
      constexpr ast_node(kind_t kind_) : kind_(kind_) {}
      kind_t kind();

    private:
      const kind_t kind_;
  };

  class ast_string : public ast_node
  {
    public:
      constexpr ast_string(const char* data_) : ast_node(kind_string), size_(0), data_(data_) {}
      typedef const char* iterator;
      constexpr int size() const
      { return size_; }
      constexpr iterator begin() const
      { return data_; }
      constexpr iterator end() const
      { return begin() + size_; }
      constexpr bool compare(const char* s2) const
      {
        int n = size_;
        const char* s1 = data_;
        for (; n; --n, ++s1, ++s2)
        {
          if (*s1 != *s2)
            return false;
        }
        return true;
      }

    protected:
      const int size_;
      const char* data_;
  };

  class ast_linkage : public ast_node
  {
    public:
      constexpr ast_linkage(const ast_string& language) : ast_node(kind_linkage), language_(language)
      { }

      const ast_string& language() const
      { return language_; }

    protected:
      const ast_string language_;
  };

  struct linkage
  {
    static const ast_linkage c;
    static const ast_linkage cpp;
  };

  class ast_expr : public ast_node
  {
    public:
      const ast_type& type() const
      { return *type_; }

    protected:
      constexpr ast_expr(kind_t kind)
        : ast_node(kind)
        , type_(nullptr)
    { }

      const ast_type* type_;
  };

  //--- Scope --
  /// A "declaration" is a type specification for a name. A "Scope" is
  /// a "sequence" of declarations, that additionally supports "lookup"
  /// by "Name".  A name may have more than one declarations in a given
  /// scope; such a name is said "overloaded".  The result of
  /// looking up a name in a scope is a set of all declarations, called
  /// an "overload set", for that name in that scope.  An "overload set"
  /// is a "sequence" of declarations, that additionaly supports
  /// lookup by "Type".
  class ast_scope : public ast_expr
  {
    public:
      typedef sequence<const ast_decl*>::iterator iterator;

      constexpr ast_scope() : ast_expr(kind_scope)
      {
      }

      constexpr ast_scope(const std::initializer_list<const ast_decl*> members)
        : ast_expr(kind_scope), members_(members)
      {
      }

      /// The sequence of declarations this scope contain.
      constexpr const sequence<const ast_decl*>& members() const
      { return members_; }

      /// The region that determine this scope.
      /// virtual const Region& region() const = 0;

#if TODO
      /// Look-up by name returns the overload-set of all declarations,
      /// for the subscripting name, contained in this scope.
      constexpr const ast_overload& operator[](const ast_name&) const
      { return TODO; }
#endif

      /// How may declarations are there in this Scope.
      constexpr int size() const
      { return members().size(); }

      iterator begin() const
      { return members().begin(); }

      iterator end() const
      { return members().end(); }

      constexpr const ast_decl& operator[](int i) const
      { return *members()[i]; }

    protected:
      const sequence<const ast_decl*> members_;
  };


                               //--- Region --
  /// A Region node represents a region of program text.  It is mostly
  /// useful for capturing the notion of scope (in Standard C++ sense).
  /// In IPR, we're using a generalized notion of Scope (a sequence of
  /// declarations).  The  notion of Region helps making precise when
  /// some implicit actions like cleanup-ups happen, or nesting of scopes.
  /// The sequence of declarations appearing in a Region makes up the
  /// Scope of that region.
  class ast_region : public ast_node
  {
    public:
      constexpr ast_region(const std::initializer_list<const ast_decl*> decls)
        : ast_node(kind_region)
        , enclosing_(nullptr)
        , bindings_(decls)
        , owner_(nullptr)
      { }
      constexpr ast_region(const ast_region* enclosing)
        : ast_node(kind_region)
        , enclosing_(enclosing)
        , owner_(nullptr)
      { }

#if TODO
      typedef std::pair<Unit_location, Unit_location> Location_span;
      virtual const Location_span& span() const = 0;
#endif

      constexpr const ast_region& enclosing() const
      { return *enclosing_; }

      constexpr const ast_scope& bindings() const
      { return bindings_; }

      constexpr const ast_expr& owner() const
      { return *owner_; }

    protected:
      const ast_region* enclosing_;
      const ast_scope bindings_;
      const ast_expr* owner_;
   };

                               //--- Classic --
  /// Classic expressions are those constructed with operators
  /// directly expressible in standard C++.  Most of those operators
  /// can be overloaded and given user-defined implementation. For
  /// instantce,  cout << 2, involves a user-defined operator.  The IPR
  /// representation is not a function call but, something like
  ///
  ///                        << ---> implementation
  ///                       /  \.
  ///                    cout   2
  ///
  /// where we record the fact that the left-shift operator has a
  /// user-defined menaning.  In IPR, we take the general apporach that
  /// all operators can be given user-defined meanings.  Consequently,
  /// we define this class to represent the idea that an expression
  /// has a user-supplied meaning.
  class ast_classic : public ast_expr
  {
    public:
      /// This is true if and only if, this is an overloaded operator
      /// with user-defined declaration.
      constexpr bool has_impl_decl() const
      { return impl_decl_ != nullptr; }

      /// The declaration for the corresponding operator function.
      constexpr const ast_decl& impl_decl() const
      { return *impl_decl_; }

  protected:
     constexpr ast_classic(kind_t kind) : ast_expr(kind), impl_decl_(nullptr)
     { }

     const ast_decl* impl_decl_;
  };

  class ast_name : public ast_expr
  {
    public:
      const ast_string& data() const
      { return data_; }

      constexpr bool operator==(const char* str) const
      { return data_.compare(str); }

    protected:
      constexpr ast_name(kind_t kind, const ast_string& data) : ast_expr(kind), data_(data)
      { }

      const ast_string data_;
  };

  //--- Identifier --
  /// An identifier is a sequence of alphanumeric characters starting
  /// with either a letter or an underbar ('_').
  class ast_identifier : public ast_name
  {
    public:
      /// The character sequence of this identifier
      const ast_string& string() const { return data(); }

      constexpr ast_identifier(const char* id) : ast_name(kind_identifier, id)
      { }
  };

  struct keywords
  {
    static const ast_identifier id_void;

    static const ast_identifier id_bool;

    static const ast_identifier id_char;
    static const ast_identifier id_schar;
    static const ast_identifier id_uchar;

    static const ast_identifier id_wchar_t;

    static const ast_identifier id_short;
    static const ast_identifier id_ushort;

    static const ast_identifier id_int;
    static const ast_identifier id_uint;

    static const ast_identifier id_long;
    static const ast_identifier id_ulong;

    static const ast_identifier id_long_long;
    static const ast_identifier id_ulong_long;

    static const ast_identifier id_float;
    static const ast_identifier id_double;
    static const ast_identifier id_long_double;
  };

                               //--- Overload --
  /// An overload-set is an expression whose value is the set of all
  /// declarations for a name in a given scope.  An overload-set supports
  /// look-up by type.  The result of such lookup is the sequence of
  /// declarations of that name with that type.
  class ast_overload : public sequence<const ast_decl*>,
                       public ast_expr
  {
    public:
      constexpr ast_overload() : ast_expr(kind_overload) {}

      constexpr const sequence<const ast_decl*> operator[](const ast_type&) const
      {
        sequence<const ast_decl*> ret;
        // TODO
        return ret;
      }
  };


                               //--- General types --
  /// A type is a collection of constraints and operations that preserve
  /// some invariants.  Since a Type is also an Expression, it has a type.
  /// A type of a type is what we call a "concept".  A type in IPR has a
  /// much broader significance than Standard C++ types (henceforth called
  /// "classic type").  In particular, in IPR, namespace is a type.
  /// Simirlary, an overload-set has a type.
  class ast_type : public ast_expr
  {
    public:
      /// cv-qualifiers are actually type operators.  Much of these operators
      /// currently make sense only for classic type -- although it might
      /// be interesting to explore the notion of pointer to overload-set
      /// or reference to such an expression.
      enum qualifier_t
      {
        None             = 0,
        Const            = 1 << 0,
        Volatile         = 1 << 1,
        Restrict         = 1 << 2        ///< not standard C++
      };

      /// A type can be decomposed into two canonical components:
      ///    - (1) its cv-qualifiers;
      ///    - (2) the cv-unqualified part, also called the "main variant".
      ///      virtual qualifier_t qualifiers() const = 0;
      ///      virtual const ast_type& main_variant() const = 0;
      /// All types have have names.
      constexpr const ast_name& name() const
      { return *name_; }

    protected:
      constexpr ast_type(kind_t kind, const ast_name* name = nullptr) : ast_expr(kind), name_(name)
      { }

      const ast_name* name_;
  };

  /// These overloads for ast_type::qualifier_t are necessary because there
  /// is no implicit conversion from integers to enums in C++.
  inline ast_type::qualifier_t
  operator|(ast_type::qualifier_t a, ast_type::qualifier_t b)
  {
     return ast_type::qualifier_t(int(a) | int(b));
  }

  inline ast_type::qualifier_t&
  operator|=(ast_type::qualifier_t& a, ast_type::qualifier_t b)
  {
     return a = ast_type::qualifier_t(int(a) | int(b));
  }

  inline ast_type::qualifier_t
  operator&(ast_type::qualifier_t a, ast_type::qualifier_t b)
  {
     return ast_type::qualifier_t(int(a) & int(b));
  }

  inline ast_type::qualifier_t&
  operator&=(ast_type::qualifier_t& a, ast_type::qualifier_t b)
  {
     return a = ast_type::qualifier_t(int(a) & int(b));
  }

  inline ast_type::qualifier_t
  operator^(ast_type::qualifier_t a, ast_type::qualifier_t b)
  {
     return ast_type::qualifier_t(int(a) ^ int(b));
  }

  inline ast_type::qualifier_t&
  operator^=(ast_type::qualifier_t& a, ast_type::qualifier_t b)
  {
     return a = ast_type::qualifier_t(int(a) ^ int(b));
  }
                               //--- Type_id --
  /// This node is used for elaborated expressions that designate types.
  /// For example, "const T*" is a Type_id , so is "int (T&)".
  class ast_type_id : public ast_name
  {
    public:
      constexpr ast_type_id(const ast_type& type, const char* id)
        : ast_name(kind_type_id, id), type_(type)
      { }

      const ast_type& type_expr() const
      { return type_; }

    protected:
      const ast_type& type_;
  };

                               //--- Array --
  /// An array-type describes object expressions that designate C-style
  /// homogenous object containers that meet the random-access
  /// requirements.  When an array-type is declared with unspecified
  /// bound, "bound()" returns a null-expression.
  /// An alternate design choice would have been to have a predicate
  /// "has_unknown_bound()", which when true would make "bound()" throw
  /// an exception if accessed.
  class ast_array : public ast_type
  {
    public:
      constexpr ast_array(const ast_type& element_type, const ast_expr& bound)
        : ast_type(kind_array), element_type_(element_type), bound_(bound)
      { }

      constexpr const ast_type& element_type() const
      { return element_type_; }

      constexpr const ast_expr& bound() const
      { return bound_; }

    protected:
      const ast_type& element_type_;
      const ast_expr& bound_;
  };

                               //--- As_type --
  /// This node represents the use of a general expression as
  /// a type.  Such situation arises in cases where a declaration
  /// node can be used to designate a type, as in
  /// \code
  ///    struct S;
  ///    typedef int count;
  ///    typename T::size_type s = 90;
  ///    template<typename T, T t> ...
  /// \endcode
  class ast_as_type : public ast_type
  {
    public:
      constexpr ast_as_type(const ast_expr& expr) : ast_type(kind_as_type), expr_(expr)
      { }

      constexpr const ast_expr& expr() const
      { return expr_; }

    protected:
      const ast_expr& expr_;
  };

  struct builtin_types
  {
    static const ast_as_type type_void;

    static const ast_as_type type_bool;

    static const ast_as_type type_char;
    static const ast_as_type type_schar;
    static const ast_as_type type_uchar;

    static const ast_as_type type_wchar_t;

    static const ast_as_type type_short;
    static const ast_as_type type_ushort;

    static const ast_as_type type_int;
    static const ast_as_type type_uint;

    static const ast_as_type type_long;
    static const ast_as_type type_ulong;

    static const ast_as_type type_long_long;
    static const ast_as_type type_ulong_long;

    static const ast_as_type type_float;
    static const ast_as_type type_double;
    static const ast_as_type type_long_double;
  };


                               //--- Decltype --
  /// This node represents query for the "generalized declared type"
  /// of an expression.  Likely C++0x extension.
  class ast_decltype : public ast_type
  {
    public:
      constexpr ast_decltype(const ast_expr& expr) : ast_type(kind_decltype), expr_(expr)
      { }

      constexpr const ast_expr& expr() const
      { return expr_; }

    protected:
      const ast_expr& expr_;
  };

  //--- Sum --
  /// A Sum type represents a distinct union of types.  This is currently
  /// used only for exception specification in Function type.
  class ast_sum : public ast_type
  {
    public:
      constexpr ast_sum(const std::initializer_list<const ast_type*> elements) : ast_type(kind_sum), elements_(elements) { }

      constexpr const sequence<const ast_type*>& elements() const
      { return elements_; }

      constexpr int size() const
      { return elements().size(); }

      constexpr const ast_type& operator[](int i) const
      { return *elements()[i]; }

    private:
      const sequence<const ast_type*> elements_;
  };

  struct sum
  {
    static const ast_sum empty;
  };


                               //--- Function --
  /// This node class represents a ast_type that describes an expression
  /// that refers to a function.  In full generality, a template
  /// is a also a function (it describes ast_type-valued functions);
  /// however, we've made a special node for template.  ISO C++ specifies
  /// that function types have language linkages and two function types
  /// with different language linkages are different.
  class ast_function : public ast_type
  {
    public:
      constexpr ast_function(const ast_product& source,
                             const ast_type& target,
                             const ast_sum& throws = sum::empty,
                             const ast_linkage& lang_linkage = linkage::cpp)
        : ast_type(kind_function), source_(source), target_(target), throws_(throws), lang_linkage_(lang_linkage)
      {
      }
      /// Parameter-type-list of a function of this type.  In full
      /// generality, this also describes template signature.
      constexpr const ast_product& source() const
      { return source_; }

      /// return-type
      constexpr const ast_type& target() const
      { return target_; }

      /// list of exception types a function of this type may throw
      constexpr const ast_sum& throws() const
      { return throws_; }

      /// The language linkage for this function type.  For most function
      /// types, it is C++ -- for it is the default.
      constexpr const ast_linkage& lang_linkage() const
      { return lang_linkage_; }

    private:
      const ast_product& source_;
      const ast_type& target_;
      const ast_sum& throws_;
      const ast_linkage& lang_linkage_;
  };

                               //--- Pointer --
  /// A pointer-type is type that describes an Address node.
  class ast_pointer : public ast_type
  {
    public:
      constexpr ast_pointer(const ast_type& points_to) : ast_type(kind_pointer), points_to_(points_to)
      { }
      /// The type of the entity whose address an object of this
      /// type may hold.
      constexpr const ast_type& points_to() const
      { return points_to_; }

    private:
      const ast_type& points_to_;
  };

                               //--- Product --
  /// A Product represents a Cartesian product of Types.  Pragmatically,
  /// it may be viewed as a Sequence of Types.  It is a ast_type.
  class ast_product : public ast_type
  {
    public:
      constexpr ast_product() : ast_type(kind_product) {}
      constexpr ast_product(const std::initializer_list<const ast_type*> elements) : ast_type(kind_product), elements_(elements)
      { }

      constexpr const sequence<const ast_type*>& elements() const
      { return elements_; }

      constexpr int size() const
      { return elements().size(); }

      constexpr const ast_type& operator[](int i) const
      { return *elements()[i]; }

    private:
      const sequence<const ast_type*> elements_;
  };

                               //--- Ptr_to_member --
  /// This is for pointer-to-member type, e.g. int A::* or void (A::*)().
  /// A pointer to member really is not a pointer type, it is much closer
  /// a pair of a type and offset that usual pointer types.
  class ast_ptr_to_member : public ast_type
  {
    public:
      constexpr ast_ptr_to_member(const ast_type& containing_type, const ast_type& member_type)
        : ast_type(kind_ptr_to_member), containing_type_(containing_type), member_type_(member_type)
      { }
      constexpr const ast_type& containing_type() const
      { return containing_type_; }

      constexpr const ast_type& member_type() const
      { return member_type_; }

    private:
      const ast_type& containing_type_;
      const ast_type& member_type_;
  };

                               //--- Qualified --
  /// A cv-qualified type.  Representing a cv-qualified type with all
  /// available information attainable in at most one indirection is
  /// very tricky. Consequently, we currently represents a cv-qualified
  /// type with a separate node as a binary operator.  Notice that we
  /// maintain the invariant
  ///     Qualified(cv2, Qualified(cv1, T)) = Qualified(cv1 | cv2, T)
  /// In particular, the Qualified::main_variant is never a Qualified node.
  /// We also maintain the invariant that Qualified::qualifiers is never
  /// ast_type::None, consequently it is an error to attempt to create such a node.
  class ast_qualified : public ast_type
  {
    public:
      constexpr ast_qualified(ast_type::qualifier_t qualifier, const ast_type& main_variant)
        : ast_type(kind_qualified), qualifier_(qualifier), main_variant_(main_variant)
      { }

      constexpr const ast_type::qualifier_t qualifiers() const
      { return qualifier_; }

      constexpr const ast_type& main_variant() const
      { return main_variant_; }

    private:
      const ast_type::qualifier_t qualifier_;
      const ast_type& main_variant_;
  };

                               //--- Reference --
  /// A reference-type describes an expression that acts like an alias
  /// for a object or function.  However, unlike a pointer-type, it is
  /// not an object-type.
  class ast_reference : public ast_type
  {
    public:
      constexpr ast_reference(const ast_type& refers_to) : ast_type(kind_reference), refers_to_(refers_to)
      { }

      /// The type of the object or function an expression of this
      /// type refers to.
      constexpr const ast_type& refers_to() const
      { return refers_to_; }

    private:
      const ast_type& refers_to_;
  };

                               //--- Rvalue_reference --
  /// An rvalue-reference-type to support move semantics in C++0x.
  class ast_rvalue_reference : public ast_type
  {
    public:
      constexpr ast_rvalue_reference(const ast_type& refers_to) : ast_type(kind_rvalue_reference), refers_to_(refers_to)
      { }

      /// The type of the object or function an expression of this
      /// type refers to.
      constexpr const ast_type& refers_to() const
      { return refers_to_; }

    private:
      const ast_type& refers_to_;
   };

                               //--- Template --
  /// This represents the type of a template declaration.  In the near future,
  /// when "concepts" are integrated, it will become a Ternary node where the
  /// third operand will represent the "where-clause".
  class ast_template : public ast_type
  {
    public:
      constexpr ast_template(const ast_product& source, const ast_type& target)
        : ast_type(kind_template), source_(source), target_(target)
      { }

      /// The constraints or types of the template-parameters.
      constexpr const ast_product& source() const
      { return source_; }

      /// The type of the instantiation result.
      constexpr const ast_type& target() const
      { return target_; }

    private:
      const ast_product& source_;
      const ast_type& target_;
  };

                              //--- Udt --
  /// Base class for user-defined types Nodes -- factor our common properties.
  class ast_udt : public ast_type
  {
    public:
      /// The region delimited by the definition of this Udt.
      constexpr const ast_region& region() const
      { return region_; }

      constexpr const ast_scope& scope() const
      { return region().bindings(); }

    protected:
      /// It is an error to create a node of this type.
      constexpr ast_udt(kind_t kind, const ast_region& region) : ast_type(kind), region_(region) ///< Used by derived classes.
      { }

      const ast_region& region_;
  };

                               //--- Namespace --
  /// The type of a Standard C++ namespace.  A namespace is primarily
  /// interesting because it is a sequence of declarations that can be
  /// given a name.
  class ast_namespace : public ast_udt
  {
    public:
      typedef ast_decl member_t;      ///< -- type of members of this type.

      constexpr ast_namespace(const std::initializer_list<const ast_decl*> members)
        : ast_udt(kind_namespace, region_), region_(members)
      { }

      constexpr const sequence<const ast_decl*>& members() const
      { return scope().members(); }

    private:
      const ast_region region_;
  };

                               //--- Class --
  class ast_class : public ast_udt
  {
    public:
      typedef ast_decl member_t;      ///< -- type of members of this type.

      constexpr ast_class(const ast_name& name,
                          const std::initializer_list<const ast_decl*> members,
                          const std::initializer_list<const ast_base_type*> bases)
        : ast_udt(kind_class, region_)
        , bases_(bases)
        , region_(members)
      {
        name_ = &name;
      }
      constexpr const sequence<const ast_decl*>& members() const
      { return scope().members(); }

      constexpr const sequence<const ast_base_type*>& bases() const
      { return bases_; }

    private:
      const sequence<const ast_base_type*> bases_;
      const ast_region region_;
  };

                               //--- Union --
  class ast_union : public ast_udt
  {
    public:
     typedef ast_decl member_t;      ///< -- type of members of this type.

     constexpr ast_union(const std::initializer_list<const ast_decl*> members)
        : ast_udt(kind_union, region_), region_(members)
      { }

     constexpr const sequence<const ast_decl*>& members() const
     { return scope().members(); }

    private:
      const ast_region region_;
  };

                               //--- Enum --
  /// An enumration is an object-type whose members are named constants
  /// the definitions of which as part of the definition of the enumeration
  /// itself.  By historical accident, enumerators are not "properly scoped".
  class ast_enum : public ast_udt
  {
    public:
      typedef ast_enumerator member_t;      ///< -- type of members of this type.

      constexpr ast_enum(const std::initializer_list<const ast_enumerator*> members)
        : ast_udt(kind_enum, region_), members_(members)
      { }

      constexpr const sequence<const ast_enumerator*>& members() const
      { return members_; }

    private:
      const sequence<const ast_enumerator*> members_;
  };


                               //--- Phantom --
  /// This nodes represents the "bound" of an array type with
  /// unknown bound, as in "unsigned char charset[];"
  /// We do not unify Phantom expressions, as two arrays with
  /// unknown bounds may not designate the same type.
  class ast_phantom : public ast_expr
  {
    public:
      constexpr ast_phantom() : ast_expr(kind_phantom) {}
  };

                               //--- Literal --
  /// An IPR literal is just like a standard C++ literal.
  class ast_literal : public ast_classic
  {
    public:
      constexpr ast_literal(const ast_type& type, const ast_string& string)
        : ast_classic(kind_literal), type_(type), string_(string)
      { }
      /// See comments for the cast operators regarding type().

      constexpr const ast_type& type() const
      { return type_; }

      /// The texttual representation of this literal as it appears
      /// in the program text.
      constexpr const ast_string& string() const
      { return string_; }

    private:
      const ast_type& type_;
      const ast_string& string_;
   };

  //--- Parameter_list --
  /// \todo Parameter_list should have its category ste properly.
  class ast_parameter_list : public ast_region,
                             public sequence<const ast_parameter*>
  {
    public:
      constexpr ast_parameter_list()
        : ast_region({})
      { }

      constexpr ast_parameter_list(const std::initializer_list<const ast_parameter*> params)
        : ast_region({}), sequence<const ast_parameter*>(params)
      { }
  };


                               //--- Mapping --
  /// This node represents a parameterized expression.
  /// Its type is a Function in case of parameterized classic expression,
  /// and Template otherwise.
  class ast_mapping : public ast_expr
  {
    public:
      constexpr ast_mapping()
        : ast_expr(kind_mapping)
        , depth_(0)
        , result_(0)
      {}
      constexpr ast_mapping(const ast_parameter_list& param_list)
        : ast_expr(kind_mapping)
        , depth_(0)
        , param_list_(param_list)
        , result_(0)
      {}

      constexpr const ast_parameter_list& params() const
      { return param_list_; }

//      virtual const ast_type& result_type() const = 0;
      constexpr const ast_expr& result() const
      { return *result_; }

      /// Mappings may have nested mappings (e.g. member templates of
      /// templates)
      /// The depth of a template declaration is its nesting nevel.
      constexpr int depth() const
      { return depth_; }

    protected:
      int depth_;
      const ast_parameter_list param_list_;
      const ast_expr* result_;
  };

  class ast_stmt : public ast_expr
  {
    /// The location of this statement in its unit.
    //  virtual const Unit_location& unit_location() const = 0;
    //  virtual const Source_location& source_location() const = 0;

    protected:
      constexpr ast_stmt(kind_t kind) : ast_expr(kind)
      { }
  };

  class ast_decl : public ast_stmt
  {
    public:
      enum specifier_t {
        None = 0,
        Auto       = 1 << 0,
        Register   = 1 << 1,
        Static     = 1 << 2,
        Extern     = 1 << 3,
        Mutable    = 1 << 4,
        StorageClass  = Auto | Register | Static | Extern | Mutable,

        Inline     = 1 << 5,
        Virtual    = 1 << 6,   ///< also used as storage class specifier
        ///< for virtual base subobjects
        Explicit   = 1 << 7,
        Pure       = 1 << 8,
        FunctionSpecifier = Inline | Virtual | Explicit | Pure,

        Friend     = 1 << 9,
        Typedef    = 1 << 10,

        Public     = 1 << 11,
        Protected  = 1 << 12,
        Private    = 1 << 13,
        AccessProtection = Public | Protected | Private,

        Export     = 1 << 14,  ///< for exported template declarations.
        Constexpr  = 1 << 15   ///< C++ 0x
      };

      constexpr const specifier_t specifiers() const
      { return specifiers_; }

      constexpr const ast_linkage& lang_linkage() const
      { return *lang_linkage_; }

      constexpr const ast_name& name() const
      { return *name_; }

      /// The region where the declaration really belongs to.  This region is
      /// the same for all declarations.
      constexpr const ast_region& home_region() const
      { return *home_region_; }

      /// The region where this declaration appears -- purely lexical.
      /// For many declarations, this is the same as the home region.
      /// Exceptions are invisible friends, member functions defined
      /// outside of their enclosing class or namespaces.
      constexpr const ast_region& lexical_region() const
      { return *lexical_region_; }

      constexpr bool has_initializer() const
      { return initializer_ != nullptr; }

      constexpr const ast_expr& initializer() const
      { return *initializer_; }

#if TODO
      virtual const ast_named_map& generating_map() const = 0;
      virtual const sequence<ast_substitution>& substitutions() const = 0;

      virtual int position() const = 0;

      /// This is the first seen declaration for name() in a given
      /// translation unit.  The master declaration is therefore the
      /// first element of the declaration-set.
      virtual const ast_decl& master() const = 0;

      virtual const sequence<ast_decl>& decl_set() const = 0;
#endif

    protected:
      constexpr ast_decl(kind_t kind)
        : ast_stmt(kind)
        , name_(nullptr)
        , specifiers_(None)
        , lang_linkage_(nullptr)
        , home_region_(nullptr)
        , lexical_region_(nullptr)
        , initializer_(nullptr)
      {}

      const ast_name* name_;
      specifier_t specifiers_;
      const ast_linkage* lang_linkage_;
      const ast_region* home_region_;
      const ast_region* lexical_region_;
      const ast_expr* initializer_;
  };

  inline ast_decl::specifier_t
  operator|(ast_decl::specifier_t a, ast_decl::specifier_t b)
  { return ast_decl::specifier_t(int(a) | int(b)); }

  inline ast_decl::specifier_t&
  operator|=(ast_decl::specifier_t& a, ast_decl::specifier_t b)
  { return a = ast_decl::specifier_t(int(a) | int(b)); }

  inline ast_decl::specifier_t
  operator&(ast_decl::specifier_t a, ast_decl::specifier_t b)
  { return ast_decl::specifier_t(int(a) & int(b)); }

  inline ast_decl::specifier_t&
  operator&=(ast_decl::specifier_t& a, ast_decl::specifier_t b)
  { return a = ast_decl::specifier_t(int(a) & int(b)); }

  inline ast_decl::specifier_t
  operator^(ast_decl::specifier_t a, ast_decl::specifier_t b)
  { return ast_decl::specifier_t(int(a) ^ int(b)); }

  inline ast_decl::specifier_t&
  operator^=(ast_decl::specifier_t& a, ast_decl::specifier_t b)
  { return a = ast_decl::specifier_t(int(a) ^ int(b)); }

                               //--- Named_map --
  /// This represents a parameterized declaration.  If its
  /// template-parameter list is empty, that means that it is
  /// either an explicit specialization -- if result() has a
  /// non-empty substitutions() -- or a really non-template
  /// declaration  -- if result().substitutions() is empty.
  /// The list of specializations of this template (either
  /// partial or explicit) is given by specializations().
  class ast_named_map : public ast_decl
  {
    public:
      constexpr ast_named_map() : ast_decl(kind_named_map) {}

      virtual const ast_named_map& primary_named_map() const = 0;
      virtual const sequence<ast_decl>& specializations() const = 0;
      virtual const ast_mapping& mapping() const = 0;

      constexpr const ast_parameter_list& params() const
      { return mapping().params(); }

      constexpr const ast_expr& result() const
      { return mapping().result(); }

      virtual const ast_named_map& definition() const = 0;
  };

  /// This represents a classic enumerator.
  class ast_enumerator : public ast_decl
  {
    public:
      constexpr ast_enumerator(const ast_enum& membership_)
        : ast_decl(kind_enumerator), membership_(membership_)
      { }

      constexpr const ast_type& type() const
      { return membership(); }

      constexpr const ast_enum& membership() const
      { return membership_; }

    protected:
      const ast_enum& membership_;
  };

                               //--- Base_type --
  /// Each base-specifier in a base-clause morally declares an unnamed
  /// subobject.  We represent that subobject declaration by a Base_type.
  /// For consistency with other non-static members, the name of that
  /// subobject is pretended to be the same as that of the base-class
  /// so that when it appears in a member-initializer list, it can
  /// conveniently be thought of as initialization of that subobject.
  class ast_base_type : public ast_decl
  {
    public:
      constexpr ast_base_type() : ast_decl(kind_base_type) { }

      /// A base-object is, by definition, unnamed.  However, it
      /// is convenient to refer to it by the name of the corresponding
      /// type -- in C++ tradition.
      constexpr const ast_name& name() const
      { return type().name(); }
  };

                               //--- Parameter --
  /// A parameter is uniquely characterized by its position in
  /// a parameter list.
  class ast_parameter : public ast_decl
  {
    public:
      constexpr ast_parameter(const ast_type& type, const ast_name& name)
        : ast_decl(kind_parameter), membership_(nullptr)
      {
        type_ = &type;
        name_ = &name;
      }

      constexpr const ast_expr& default_value() const
      { return initializer(); }

      constexpr const ast_parameter_list& membership() const
      { return *membership_; }

    protected:
      const ast_parameter_list* membership_;
  };

                               //--- Fundecl --
  /// This node represents a function declaration. Notice that the
  /// exception specification is actually made part of the function
  /// type.  For convenience, it is represented here too.
  class ast_fundecl : public ast_decl
  {
    public:
      constexpr ast_fundecl(const ast_name& name, const ast_type& type, const ast_mapping& mapping)
         : ast_decl(kind_fundecl)
         , membership_(nullptr)
         , definition_(nullptr)
         , mapping_(mapping)
      {
        name_ = &name;
        type_ = &type;
      }

      constexpr const ast_parameter_list& parameters() const
      { return mapping().params(); }

      constexpr const ast_udt& membership() const
      { return *membership_; }

      constexpr const ast_fundecl& definition() const
      { return *definition_; }

      constexpr const ast_mapping& mapping() const
      { return mapping_; }

    protected:
      const ast_udt* membership_;
      const ast_fundecl* definition_;
      const ast_mapping& mapping_;
   };

                               //--- Var --
  /// This represents a variable declaration.  It is also used to
  /// represent a static data member.
  class ast_var : public ast_decl
  {
    public:
      constexpr ast_var(const ast_type& type,
          const ast_name& name,
          ast_decl::specifier_t specifier = ast_decl::None,
          const ast_linkage& linkage = linkage::cpp) : ast_decl(kind_var)
      {
        type_ = &type;
        name_ = &name;
        specifiers_ = specifier;
        lang_linkage_ = &linkage;
      }
  };

                                //--- Field --
  /// This node represents a nonstatic data member.
  class ast_field : public ast_decl
  {
    public:
      constexpr ast_field() : ast_decl(kind_field), membership_(nullptr) {}

      constexpr const ast_udt& membership() const
      { return *membership_; }

    protected:
      const ast_udt* membership_;
  };

                                //--- Bitfield --
  /// A bit-field data member.
  class ast_bitfield : public ast_decl
  {
    public:
      constexpr ast_bitfield() : ast_decl(kind_bitfield), precision_(nullptr), membership_(nullptr) {}

      constexpr const ast_expr& precision() const
      { return *precision_; }

      constexpr const ast_udt& membership() const
      { return *membership_; }

    protected:
      const ast_expr* precision_;
      const ast_udt* membership_;
  };

                                //--- Typedecl --
  class ast_typedecl : public ast_decl
  {
    public:
      constexpr ast_typedecl() : ast_decl(kind_typedecl), membership_(nullptr), definition_(nullptr) {}

      constexpr const ast_udt& membership() const
      { return *membership_; }

      constexpr const ast_typedecl& definition() const
      { return *definition_; }

    protected:
      const ast_udt* membership_;
      const ast_typedecl* definition_;
   };

}}
