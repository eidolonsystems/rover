#include "Evaluator.hpp"
#include "Box.hpp"
#include "Rover/Evaluator.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_evaluator(module& module) {
  class_<Evaluator>(module, "Evaluator")
    .def(init<>())
    .def("evaluate", &Evaluator::evaluate<Box<object>>);
}
