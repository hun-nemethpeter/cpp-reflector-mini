#include <cstddef>

class str_const
{
  public:
    template<std::size_t N>
    constexpr str_const(const char(&data)[N]) : data(data), length(N - 1) {}

    constexpr char operator[](std::size_t index) const
    { return data[index]; }

    constexpr std::size_t size() const
    { return length; }

  private:
    const char* const data;
    const std::size_t length;
};

