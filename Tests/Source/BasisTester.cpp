#include <catch2/catch.hpp>
#include "Rover/Basis.hpp"
#include "Rover/ListTrial.hpp"
#include "Rover/Sample.hpp"

using namespace Rover;

namespace {
  template<typename B, typename T>
  void check_result(const B& basis, T result) {
    REQUIRE(basis.restore_result(basis.apply({ result, { 1. } }).m_result) ==
      result);
  }

  template<typename B>
  void check_result_positive(const B& basis) {
    check_result(basis, 0.5);
    check_result(basis, 1.);
    check_result(basis, 2.);
  }

  template<typename B>
  void check_result_negative(const B& basis) {
    check_result(basis, -0.5);
    check_result(basis, -1.);
    check_result(basis, -2.);
  }

  template<typename B>
  void check_result_singular(const B& basis) {
    check_result(basis, 0.);
  }

  template<typename B>
  void check_result(const B& basis) {
    check_result_positive(basis);
    check_result_negative(basis);
    check_result_singular(basis);
  }
}

TEST_CASE("test_basis_positive_elements", "[Basis]") {
  SECTION("Two samples.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 2., { 4. } });
    trial.insert({ 4., { 8. } });
    for(auto i = 0; i < 100; ++i) {
      auto basis = Basis<Sample<double, double>, double>(trial);
      auto sample = basis.apply({ 1., { 1. } });
      REQUIRE((sample.m_result == Approx(0.5) || sample.m_result ==
        Approx(0.25)));
      REQUIRE(sample.m_arguments.size() == 1);
      REQUIRE((sample.m_arguments[0] == Approx(0.25) ||
        sample.m_arguments[0] == Approx(0.125)));
      auto arguments = basis.apply(Sample<double, double>::Arguments{ 1. });
      REQUIRE(sample.m_arguments == arguments);
      check_result_positive(basis);
    }
  }
  SECTION("Three samples.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 1., { 1. } });
    trial.insert({ 2., { 4. } });
    trial.insert({ 4., { 8. } });
    for(auto i = 0; i < 100; ++i) {
      auto basis = Basis<Sample<double, double>, double>(trial);
      auto sample = basis.apply({ 1., { 1. } });
      REQUIRE((sample.m_result == Approx(0.5) || sample.m_result ==
        Approx(0.25) || sample.m_result == Approx(1.)));
      REQUIRE(sample.m_arguments.size() == 1);
      REQUIRE((sample.m_arguments[0] == Approx(0.25) ||
        sample.m_arguments[0] == Approx(0.125) ||
        sample.m_arguments[0] == Approx(1.)));
      auto arguments = basis.apply(Sample<double, double>::Arguments{ 1. });
      REQUIRE(sample.m_arguments == arguments);
      check_result_positive(basis);
    }
  }
}

TEST_CASE("test_basis_singular_elements", "[Basis]") {
  SECTION("Apply to singlular.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 2., { 4. } });
    trial.insert({ 4., { 8. } });
    for(auto i = 0; i < 100; ++i) {
      auto basis = Basis<Sample<double, double>, double>(trial);
      auto sample = basis.apply({ 0., { 0. } });
      REQUIRE(sample.m_result == Approx(0.));
      REQUIRE(sample.m_arguments.size() == 1);
      REQUIRE(sample.m_arguments[0] == Approx(0.));
      auto arguments = basis.apply(Sample<double, double>::Arguments{ 0. });
      REQUIRE(sample.m_arguments == arguments);
      check_result_singular(basis);
    }
  }
  SECTION("Avoid singular basis.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 2., { 4. } });
    trial.insert({ 0., { 0. } });
    for(auto i = 0; i < 100; ++i) {
      auto basis = Basis<Sample<double, double>, double>(trial);
      auto sample = basis.apply({ 1., { 1. } });
      REQUIRE(sample.m_result == Approx(0.5));
      REQUIRE(sample.m_arguments.size() == 1);
      REQUIRE(sample.m_arguments[0] == Approx(0.25));
      auto arguments = basis.apply(Sample<double, double>::Arguments{ 1. });
      REQUIRE(sample.m_arguments == arguments);
      check_result_positive(basis);
    }
  }
}

TEST_CASE("test_basis_negative_elements", "[Basis]") {
  SECTION("Apply to negative.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 2., { 4. } });
    trial.insert({ 4., { 8. } });
    for(auto i = 0; i < 100; ++i) {
      auto basis = Basis<Sample<double, double>, double>(trial);
      auto sample = basis.apply({ -1., { -1. } });
      REQUIRE((sample.m_result == Approx(-0.5) || sample.m_result ==
        Approx(-0.25)));
      REQUIRE(sample.m_arguments.size() == 1);
      REQUIRE((sample.m_arguments[0] == Approx(-0.25) ||
        sample.m_arguments[0] == Approx(-0.125)));
      auto arguments = basis.apply(Sample<double, double>::Arguments{ -1. });
      REQUIRE(sample.m_arguments == arguments);
      check_result_negative(basis);
    }
  }
  SECTION("Negative elements only.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ -2., { -4. } });
    trial.insert({ -4., { -8. } });
    for(auto i = 0; i < 100; ++i) {
      auto basis = Basis<Sample<double, double>, double>(trial);
      auto positive_sample = basis.apply({ 1., { 1. } });
      REQUIRE((positive_sample.m_result == Approx(0.5) ||
        positive_sample.m_result == Approx(0.25)));
      REQUIRE(positive_sample.m_arguments.size() == 1);
      REQUIRE((positive_sample.m_arguments[0] == Approx(0.25) ||
        positive_sample.m_arguments[0] == Approx(0.125)));
      auto positive_arguments = basis.apply(
        Sample<double, double>::Arguments{ 1. });
      REQUIRE(positive_sample.m_arguments == positive_arguments);
      auto negative_sample = basis.apply({ -1., { -1. } });
      REQUIRE((negative_sample.m_result == Approx(-0.5) ||
        negative_sample.m_result == Approx(-0.25)));
      REQUIRE(negative_sample.m_arguments.size() == 1);
      REQUIRE((negative_sample.m_arguments[0] == Approx(-0.25) ||
        negative_sample.m_arguments[0] == Approx(-0.125)));
      auto negative_arguments = basis.apply(
        Sample<double, double>::Arguments{ -1. });
      REQUIRE(negative_sample.m_arguments == negative_arguments);
      check_result(basis);
    }
  }
  SECTION("Positive, negative, and singular elements.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 0., { 0. } });
    trial.insert({ 2., { 4. } });
    trial.insert({ -4., { -8. } });
    for(auto i = 0; i < 100; ++i) {
      auto basis = Basis<Sample<double, double>, double>(trial);
      auto positive_sample = basis.apply({ 1., { 1. } });
      REQUIRE((positive_sample.m_result == Approx(0.5) ||
        positive_sample.m_result == Approx(0.25)));
      REQUIRE(positive_sample.m_arguments.size() == 1);
      REQUIRE((positive_sample.m_arguments[0] == Approx(0.25) ||
        positive_sample.m_arguments[0] == Approx(0.125)));
      auto positive_arguments = basis.apply(
        Sample<double, double>::Arguments{ 1. });
      REQUIRE(positive_sample.m_arguments == positive_arguments);
      auto negative_sample = basis.apply({ -1., { -1. } });
      REQUIRE((negative_sample.m_result == Approx(-0.5) ||
        negative_sample.m_result == Approx(-0.25)));
      REQUIRE(negative_sample.m_arguments.size() == 1);
      REQUIRE((negative_sample.m_arguments[0] == Approx(-0.25) ||
        negative_sample.m_arguments[0] == Approx(-0.125)));
      auto negative_arguments = basis.apply(
        Sample<double, double>::Arguments{ -1. });
      REQUIRE(negative_sample.m_arguments == negative_arguments);
      check_result(basis);
    }
  }
}

TEST_CASE("test_basis_categorical", "[Basis]") {
  SECTION("One categorical only.") {
    auto trial = ListTrial<Sample<double, std::string>>();
    trial.insert({ 2., { "abc" } });
    trial.insert({ 4., { "bcd" } });
    for(auto i = 0; i < 100; ++i) {
      auto basis = Basis<Sample<double, std::string>, double>(trial);
      {
        auto sample = basis.apply({ 1., { "abc" } });
        REQUIRE(sample.m_arguments.size() == 1);
        REQUIRE((sample.m_arguments[0] == 1. || sample.m_arguments[0]) == 0.);
        auto arguments = basis.apply(Sample<double, std::string>::Arguments{
          "abc" });
        REQUIRE(sample.m_arguments == arguments);
      }
      {
        auto sample = basis.apply({ 1., { "bcd" } });
        REQUIRE(sample.m_arguments.size() == 1);
        REQUIRE((sample.m_arguments[0] == 1. || sample.m_arguments[1] == 0.));
        auto arguments = basis.apply(Sample<double, std::string>::Arguments{
          "bcd" });
        REQUIRE(sample.m_arguments == arguments);
      }
      {
        auto sample = basis.apply({ 1., { "cde" } });
        REQUIRE(sample.m_arguments.size() == 1);
        REQUIRE(sample.m_arguments[0] == Approx(0.5));
        auto arguments = basis.apply(Sample<double, std::string>::Arguments{
          "cde" });
        REQUIRE(sample.m_arguments == arguments);
      }
    }
  }
  SECTION("Mixed.") {
    using Sample = Rover::Sample<double, std::string, double, std::string>;
    auto trial = ListTrial<Sample>();
    trial.insert({ 2., { "abc", 1., "abc" } });
    trial.insert({ 4., { "bcd", 2., "qwe" } });
    trial.insert({ 4., { "abc", 1., "zxc" } });
    trial.insert({ 2., { "bcd", 2., "qwe" } });
    for(auto i = 0; i < 100; ++i) {
      auto basis = Basis<Sample, double>(trial);
      {
        auto sample = basis.apply({ 1., { "abc", 1., "abc" } });
        REQUIRE(sample.m_arguments.size() == 4);
        REQUIRE((sample.m_arguments[0] == 1. || sample.m_arguments[0] == 0.));
        REQUIRE((sample.m_arguments[1] == Approx(1.) ||
          sample.m_arguments[1] == Approx(0.5)));
        REQUIRE(((sample.m_arguments[2] == 1. && sample.m_arguments[3] ==
          0.) || (sample.m_arguments[2] == 0. && sample.m_arguments[3] ==
          1.) || (sample.m_arguments[2] == 0. && sample.m_arguments[3] ==
          0.)));
      }
      {
        auto sample = basis.apply({ 1., { "bcd", 1., "zxc" } });
        REQUIRE(sample.m_arguments.size() == 4);
        REQUIRE((sample.m_arguments[0] == 1. || sample.m_arguments[0] == 0.));
        REQUIRE((sample.m_arguments[1] == Approx(1.) ||
          sample.m_arguments[1] == Approx(0.5)));
        REQUIRE(((sample.m_arguments[2] == 1. && sample.m_arguments[3] ==
          0.) || (sample.m_arguments[2] == 0. && sample.m_arguments[3] ==
          1.) || (sample.m_arguments[2] == 0. && sample.m_arguments[3] ==
          0.)));
      }
      {
        auto sample = basis.apply({ 1., { "qwe", 1., "qwe" } });
        REQUIRE(sample.m_arguments.size() == 4);
        REQUIRE(sample.m_arguments[0] == Approx(0.5));
        REQUIRE((sample.m_arguments[1] == Approx(1.) ||
          sample.m_arguments[1] == Approx(0.5)));
        REQUIRE(((sample.m_arguments[2] == 1. && sample.m_arguments[3] ==
          0.) || (sample.m_arguments[2] == 0. && sample.m_arguments[3] ==
          1.) || (sample.m_arguments[2] == 0. && sample.m_arguments[3] ==
          0.)));
      }
      {
        auto sample = basis.apply({ 1., { "abc", 1., "asd" } });
        REQUIRE(sample.m_arguments.size() == 4);
        REQUIRE((sample.m_arguments[0] == 1. || sample.m_arguments[0] == 0.));
        REQUIRE((sample.m_arguments[1] == Approx(1.) ||
          sample.m_arguments[1] == Approx(0.5)));
        REQUIRE(sample.m_arguments[2] == Approx(1. / 3));
        REQUIRE(sample.m_arguments[3] == Approx(1. / 3));
      }
    }
  }
}
