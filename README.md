cpp-reflector-mini
==================

This tool generate minimal reflection info about classes and enums based on clang 3.2

It requires clang 3.2, waf (included), python
it works with Ubuntu 13.04 stock packages: llvm llvm-dev clang clang-dev

Compilation
===========
waf/waf configure
waf

Usage
=====
cd test
./test.sh

will process Test.hh and prints:

```
Classes:
========
class: Test
0,int,m_field1,
1,float,m_field2,
2,double,m_field3,
3,char,m_field4,
4,_Bool,m_field5

class: Foo
0,class Test,m_test,
1,int,m_a,
2,std::string,member_b,
3,std::vector<std::string>,member_c

Enums:
======
enum: ERating
ERating::Excellent, 0
ERating::VeryGood, 1
ERating::Average, 2
ERating::Poor, 3
ERating::Terrible, 4
```
