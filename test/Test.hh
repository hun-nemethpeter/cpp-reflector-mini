#include <string>
#include <vector>

enum class ERating
{
  Excellent,
  VeryGood,
  Average,
  Poor,
  Terrible
};

class Test
{
  int m_field1;
  float m_field2;
  double m_field3;
  char m_field4;
  bool m_field5;
};

struct Foo 
{
  Test m_test;
  int m_a;
  std::string member_b;
  std::vector<std::string> member_c;
};

int main(int c, char** argv)
{
  return 0;
}
