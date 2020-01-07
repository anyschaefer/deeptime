//
// Created by mho on 1/7/20.
//

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <random>
#include <chrono>

namespace py = pybind11;
using namespace pybind11::literals;

template<typename dtype>
using np_array = py::array_t<dtype, py::array::c_style>;

template<typename dtype>
np_array<int> trajectory(std::size_t N, int start, const np_array<dtype> &P, long seed) {
    py::gil_scoped_release gil;

    auto nStates = P.shape(0);

    np_array<int> result ({N});
    int* data = result.mutable_data(0);
    data[0] = start;
    if (seed == -1) {
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    std::default_random_engine generator (seed);

    std::discrete_distribution<> ddist;

    const dtype* pPtr = P.data();

    for(std::size_t t = 1; t < N; ++t) {
        auto prevState = data[t-1];
        ddist.param(decltype(ddist)::param_type(pPtr + prevState * nStates, pPtr + (prevState+1) * nStates));
        data[t] = ddist(generator);
    }
    return result;
}

PYBIND11_MODULE(_markovprocess_generation_bindings, m) {
    m.def("trajectory", &trajectory<float>, "N"_a, "start"_a, "P"_a, "seed"_a = -1);
    m.def("trajectory", &trajectory<double>, "N"_a, "start"_a, "P"_a, "seed"_a = -1);
}
