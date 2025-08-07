#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/shared_ptr.h>
#include "gldapi.h"

namespace nb = nanobind;
using namespace nb::literals;
// If you want to expose enums as bitfield values, you can do that as well.

NB_MODULE(gridlabd_python, m) {
    nb::enum_<GLDErrorCode>(m, "GLDErrorCode")
        .value("GLD_SUCCESS", GLD_SUCCESS)
        .value("GLD_FILE_NOT_FOUND", GLD_FILE_NOT_FOUND)
        .value("GLD_INVALID_FORMAT", GLD_INVALID_FORMAT)
        .value("GLD_OPERATION_FAILED", GLD_OPERATION_FAILED)
        .value("GLD_OBJECT_NOT_FOUND", GLD_OBJECT_NOT_FOUND)
        .value("GLD_TIME_STEP_ERROR", GLD_TIME_STEP_ERROR)
        .export_values();

    nb::enum_<GLDApplicationType>(m, "GLDApplicationType")
        .value("GLD_APPLICATION_TYPE_UNKNOWN", GLD_APPLICATION_TYPE_UNKNOWN)
        .value("GLD_APPLICATION_TYPE_GRIDLABD", GLD_APPLICATION_TYPE_GRIDLABD)
        .value("GLD_APPLICATION_TYPE_OTHER", GLD_APPLICATION_TYPE_OTHER)
        .export_values();

    nb::enum_<GLDCheckPointMode>(m, "GLDCheckPointMode")
        .value("GLD_CHECKPOINT_MODE_NONE", GLD_CHECKPOINT_MODE_NONE)
        .value("GLD_CHECKPOINT_MODE_SAVE", GLD_CHECKPOINT_MODE_SAVE)
        .value("GLD_CHECKPOINT_MODE_LOAD", GLD_CHECKPOINT_MODE_LOAD)
        .export_values();

    // Bind the main class, but **do not bind methods using GLDData** (std::map<std::string, std::any>)
    nb::class_<GridLabD>(m, "GridLabD")
        .def(nb::init<>())
        .def("set_config_file", &GridLabD::set_config_file)
        .def("load_glm", &GridLabD::load_glm)
        .def("save_checkpoint", &GridLabD::save_checkpoint,
             nb::arg("save_path"), nb::arg("mode") = GLD_CHECKPOINT_MODE_SAVE)
        .def("load_checkpoint", &GridLabD::load_checkpoint)
        .def("delete_object", &GridLabD::delete_object)
        .def("run", [](GridLabD& self, double start, double end) {
            double sim_time = 0.0;
            auto ret = self.run(start, end, sim_time);
            return std::make_tuple(ret, sim_time);
        }, "start_time"_a, "end_time"_a)
        .def("step", [](GridLabD& self, double simulation_time) {
            auto time = simulation_time;
            auto ret = self.step(time);
            return std::make_tuple(ret, time);
        }, "simulation_time"_a)
        // Add more bindings as needed, omitting GLDData/Callback methods for now
        ;
}