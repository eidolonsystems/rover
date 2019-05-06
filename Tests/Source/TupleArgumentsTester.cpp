#include <catch.hpp>
#include "Rover/Sample.hpp"

using namespace Rover;

namespace {
  template<int>
  class IntWrapper {
    public:
      IntWrapper()
        : m_value(-1) {}

      explicit IntWrapper(int value)
        : m_value(value) {}

      explicit operator int() const {
        return m_value;
      }

      template<int I>
      IntWrapper<I> operator +(IntWrapper<I> other) {
        return IntWrapper<I>(m_value + other.m_value);
      }

    private:
      int m_value;
  };

  template<typename R, typename X, typename Y>
  void sum(R&, X, Y) {}

  template<typename R>
  void sum(R& r, R x, R y) {
    r = x + y;
  }
}

TEST_CASE("test_tuple_arguments_size", "[TupleArguments]") {
  SECTION("Empty tuple.") {
    auto sample = Sample<int>{ 1, {} };
    static_assert(arguments_size(sample.m_arguments) == 0);
    REQUIRE(arguments_size(sample.m_arguments) == 0);
  }
  SECTION("Non-empty tuple.") {
    auto sample = Sample<int, double, char>{ 1, { 2., 'a' } };
    static_assert(arguments_size(sample.m_arguments) == 2);
    REQUIRE(arguments_size(sample.m_arguments) == 2);
  }
}

TEST_CASE("test_tuple_arguments_visit", "[TupleArguments]") {
  SECTION("Empty tuple.") {
    auto i = 0;
    auto sample = Sample<int>{ 1, {} };
    visit_arguments([&](const auto& value, auto index) {
      ++i;
    }, sample.m_arguments);
    REQUIRE(i == 0);
  }
  SECTION("Download to vector.") {
    auto v = std::vector<int>(3);
    auto sample = Sample<int, int, int, int>{ 1, { 5, 3, 2 } };
    visit_arguments([&](const auto& value, auto index) {
      v[index] = value;
    }, sample.m_arguments);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == 5);
    REQUIRE(v[1] == 3);
    REQUIRE(v[2] == 2);
  }
  SECTION("Download to vector incompatible types.") {
    auto v = std::vector<int>(3);
    auto sample = Sample<int, IntWrapper<0>, IntWrapper<1>, IntWrapper<2>>{ 
      1, { IntWrapper<0>(5), IntWrapper<1>(3), IntWrapper<2>(2) } };
    visit_arguments([&](const auto& value, auto index) {
      v[index] = static_cast<int>(value);
    }, sample.m_arguments);
    REQUIRE(v.size() == 3);
    REQUIRE(v[0] == 5);
    REQUIRE(v[1] == 3);
    REQUIRE(v[2] == 2);
  }
  SECTION("Sum two samples up.") {
    auto s1 = Sample<int, int, int, int>{ 1, { 5, 3, 2 } };
    auto s2 = Sample<int, int, int, int>{ 10, { 11, 12, 6 } };
    auto s3 = Sample<int, int, int, int>();
    visit_arguments([&](auto& result, auto k) {
      visit_arguments([&](const auto& lhs, auto i) {
        if(k == i) {
          visit_arguments([&](const auto& rhs, auto j) {
            if(i == j) {
              result = lhs + rhs;
            }
          }, s2.m_arguments);
        }
      }, s1.m_arguments);
    }, s3.m_arguments);
    REQUIRE(std::get<0>(s3.m_arguments) == 16);
    REQUIRE(std::get<1>(s3.m_arguments) == 15);
    REQUIRE(std::get<2>(s3.m_arguments) == 8);
  }
  SECTION("Sum two samples up incompatible types.") {
    auto s1 = Sample<int, IntWrapper<0>, IntWrapper<1>, IntWrapper<2>>{
      1, { IntWrapper<0>(5), IntWrapper<1>(3), IntWrapper<2>(2) } };
    auto s2 = Sample<int, IntWrapper<0>, IntWrapper<1>, IntWrapper<2>>{
      10, { IntWrapper<0>(11), IntWrapper<1>(12), IntWrapper<2>(6) } };
    auto s3 = Sample<int, IntWrapper<0>, IntWrapper<1>, IntWrapper<2>>();
    visit_arguments([&](auto& result, auto k) {
      visit_arguments([&](const auto& lhs, auto i) {
        if(k == i) {
          visit_arguments([&](const auto& rhs, auto j) {
            if(i == j) {
              sum(result, lhs, rhs);
            }
          }, s2.m_arguments);
        }
      }, s1.m_arguments);
    }, s3.m_arguments);
    REQUIRE(static_cast<int>(std::get<0>(s3.m_arguments)) == 16);
    REQUIRE(static_cast<int>(std::get<1>(s3.m_arguments)) == 15);
    REQUIRE(static_cast<int>(std::get<2>(s3.m_arguments)) == 8);
  }
}
