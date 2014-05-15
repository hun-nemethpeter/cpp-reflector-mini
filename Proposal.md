Non-type template parameters for reflection
===========================================
(Draft 1)



The C++ language has a grammar which define language objects e.g.: classes, functions, enums, namespaces ...
If a language object is named we can use it later in the source code. If we can use it we can inspect it also.
Compile time reflection is a way to inspect these named language objects. Theoretically we can inspect every
attributes of these objects. One possible way of compile time reflection is to ask the compiler to create a
descriptor-object (an IPR node) for a named language object. The IPR means Internal Program Representation.
The descriptor-object is an abstract AST node.


Language object -> IPR node transition
--------------------------------------

The first main step for reflection is to make inspectable every named language-object.
This paper examines the way where every language-object has a corresponding IPR node at compile time.

In C++ language the most common declaration for a template is `template<typename T> class Foo {};` where T is represent a type name.
`template<int N> class foo {};` also a valid for, in this case the template will be instatized with an integer. E.g.
`Foo<1> foo;` and this structure called as non-type template parameter. One of the key idea of this paper is introducing
new non-type template parameters. These special non-type template parameters will convert a C++ language element to
a descriptor object.

 `template<std::ipr::Class iprT> class Foo {};`

Here the new `std::ipr::Class` parameter a normal class, but it has a special capability. It can accept a class type name.

  `Foo<std::string> foo;`

During template instatiation the `iprT` parameter acts as a normal value parameter and it is filled
with informaion of `std::string`.

```C++
enum sample_enum { item1, item2 };

template<ipr::Class iprT>
void print_class_name()
{
  std::cout << iprT.name() << std::endl;
}
template<ipr::Enum iprT>
void print_enum_size()
{
  std::cout << iprT.members().size() << std::endl;
}

usage:
int foo()
{
  print_class_name<std::string>();
  print_enum_size<sample_enum>();
}

output:
::std::string
2
```

Note, member call on this value template paramater (`iprT.name()`) happens during template instantiation and not in runtime.
Every method of an IPR node is a constexpr function. And an IPR parameter itself is a constexpr object.

Standardized IPR nodes
----------------------

TODO: modelled after Pivot https://parasol.tamu.edu/pivot/

Some used method:

* ipr::Class
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_class.html
  * `const Sequence<Decl>& members() const`

* ipr::Decl
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_decl.html
  * `virtual const Type& type() const = 0`
  * `virtual const Name& name() const = 0`

* ipr::Type
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_type.html
  * `virtual const Name& name() const = 0`

* ipr::Enum
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_enum.html
  * `virtual const Sequence<Enumerator>& members() const = 0`
  
* ipr::Enumerator
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_enumerator.html
  * `virtual const Name& name() const = 0`
  * `virtual const Expr& initializer() const = 0`
  * `virtual int position() const = 0`
  
* ipr::Name

* ipr::Identifier : ipr::Name
  https://parasol.tamu.edu/pivot/doc/ipr/structipr_1_1_identifier.html
  * `typedef const String& Arg_type`
  * `Arg_type string() const`


Links
-----

The Pivot is a framework
https://parasol.tamu.edu/pivot/

You can comment it on (isocpp.org/forums -> SG7 â€“ Reflection)
https://groups.google.com/a/isocpp.org/forum/#!forum/reflection

These code snippet based on this repo:
https://github.com/hun-nemethpeter/cpp-reflector-mini

Call for Compile-Time Reflection Proposals:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3814.html
