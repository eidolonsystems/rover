#ifndef ROVER_BOX_HPP
#define ROVER_BOX_HPP
#include <memory>
#include <type_traits>
#include "Rover/Evaluator.hpp"
#include "Rover/Noncopyable.hpp"
#include "Rover/Pointer.hpp"

namespace Rover {

  //! Provides type-erasure over generators evaluating to a type T.
  /*!
    \tparam T The type of value to generate.
  */
  template<typename T>
  class Box final : private Noncopyable {
    public:

      //! The type of value to generate.
      using Type = T;

      //! Packs a generator or generator pointer into a Box
      template<typename GeneratorFwd, std::enable_if_t<!std::is_convertible_v<
        std::decay_t<GeneratorFwd>, Box>>* = nullptr>
      explicit Box(GeneratorFwd&& gen);

      Type generate(Evaluator& evaluator);

    private:
      class WrapperBase {
        public:
          virtual ~WrapperBase() = default;
          virtual Type generate(Evaluator& evaluator) = 0;
      };

      template<typename>
      class ValueWrapper;

      template<typename>
      class PointerWrapper;

      std::unique_ptr<WrapperBase> m_generator;
  };

  template<typename GeneratorFwd>
  Box(GeneratorFwd&&) -> Box<generator_type_t<std::decay_t<GeneratorFwd>>>;

  template<typename T>
  template<typename GeneratorFwd, std::enable_if_t<!std::is_convertible_v<
    std::decay_t<GeneratorFwd>, Box<T>>>*>
  Box<T>::Box(GeneratorFwd&& gen)
    : m_generator([&gen]() mutable {
        if constexpr(is_object_pointer_v<std::decay_t<GeneratorFwd>>) {
          return std::make_unique<PointerWrapper<std::decay_t<GeneratorFwd>>>(
            std::forward<GeneratorFwd>(gen));
        } else {
          return std::make_unique<ValueWrapper<std::decay_t<GeneratorFwd>>>(
            std::forward<GeneratorFwd>(gen));
        }
      }()) {}

  template<typename T>
  T Box<T>::generate(Evaluator& evaluator) {
    return m_generator->generate(evaluator);
  }

  template<typename T>
  template<typename Generator>
  class Box<T>::ValueWrapper final : public WrapperBase {
    public:

      template<typename GeneratorFwd, std::enable_if_t<!std::is_convertible_v<
        std::decay_t<GeneratorFwd>, ValueWrapper>>* = nullptr>
      ValueWrapper(GeneratorFwd&& gen)
        : m_generator(std::forward<GeneratorFwd>(gen)) {}

      T generate(Evaluator& evaluator) override {
        return evaluator.evaluate(m_generator);
      }

    private:
      Generator m_generator;
  };

  template<typename T>
  template<typename Generator>
  class Box<T>::PointerWrapper final : public WrapperBase {
    public:

      template<typename GeneratorFwd, std::enable_if_t<!std::is_convertible_v<
        std::decay_t<GeneratorFwd>, PointerWrapper>>* = nullptr>
      PointerWrapper(GeneratorFwd&& gen)
        : m_generator(std::forward<GeneratorFwd>(gen)) {}

      T generate(Evaluator& evaluator) override {
        return evaluator.evaluate(*m_generator);
      }

    private:
      Generator m_generator;
  };
}

#endif
