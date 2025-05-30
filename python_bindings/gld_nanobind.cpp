#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include "gld_api.h"  // Keeping in same folder temporarily

namespace nb = nanobind;

NB_MODULE(gridlabd_python, m) {
    nb::enum_<GLDErrorCode>(m, "GLDErrorCode")
        .value("SUCCESS", GLDErrorCode::SUCCESS)
        .value("FILE_NOT_FOUND", GLDErrorCode::FILE_NOT_FOUND);

    nb::class_<GridLabD>(m, "GridLabD")
        .def(nb::init<>())
        .def("set_config_file", &GridLabD::set_config_file)
        .def("load_glm", &GridLabD::load_glm);
}