#ifndef ROVER_EVALUATION_HPP
#define ROVER_EVALUATION_HPP
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <vector>

namespace Rover::Details {

  struct TypeErasingPtr {
    template<typename PtrFwd, std::enable_if_t<!std::is_same_v<std::decay_t<
      PtrFwd>, TypeErasingPtr>>* = nullptr>
    TypeErasingPtr(PtrFwd&& ptr)
      : m_ptr(std::forward<PtrFwd>(ptr)),
        m_type(typeid(*std::declval<std::decay_t<PtrFwd>>())) {}
    inline bool operator ==(const TypeErasingPtr& other) const {
      return m_ptr == other.m_ptr && m_type == other.m_type;
    }
    const void* m_ptr;
    std::type_index m_type;
  };
}

namespace Rover {

  /** Encapsulates the state needed to evaluate a generator. */
  class Evaluator {
    public:

      //! Returns the argument evaluated by a generator.
      /*!
        \param generator The generator to evaluate.
        \return The argument generated.
      */
      template<typename Generator>
      typename Generator::Type evaluate(Generator& generator);

    private:
      struct BaseEntry {
        virtual ~BaseEntry() = default;
      };
      template<typename T>
      struct Entry : BaseEntry {
        using Type = T;
        Type m_value;

        template<typename U>
        Entry(U&& value);
      };
      struct Evaluation {
        Details::TypeErasingPtr m_generator;
        std::unique_ptr<BaseEntry> m_entry;

        template<typename Generator>
        Evaluation(Generator& generator, std::unique_ptr<BaseEntry> entry);
      };
      std::vector<Evaluation> m_evaluations;
  };

  template<typename T>
  template<typename U>
  Evaluator::Entry<T>::Entry(U&& value)
    : m_value(std::forward<U>(value)) {}

  template<typename Generator>
  Evaluator::Evaluation::Evaluation(Generator& generator,
      std::unique_ptr<BaseEntry> entry)
    : m_generator(&generator),
      m_entry(std::move(entry)) {}

  template<typename Generator>
  typename Generator::Type Evaluator::evaluate(Generator& generator) {
    using Type = typename Generator::Type;
    auto type_erasing_ptr = Details::TypeErasingPtr(&generator);
    auto i = std::find_if(m_evaluations.begin(), m_evaluations.end(),
      [&type_erasing_ptr](const auto& e) {
        return type_erasing_ptr == e.m_generator;
      });
    if(i == m_evaluations.end()) {
      auto entry = std::make_unique<Entry<Type>>(generator.generate(*this));
      m_evaluations.emplace_back(type_erasing_ptr, std::move(entry));
      i = m_evaluations.end() - 1;
    }
    return static_cast<Entry<Type>&>(*i->m_entry).m_value;
  }
}

#endif
