#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>   
#include "gldapi.h"

namespace nb = nanobind;

NB_MODULE(_gridlabd_impl, m) {
    m.doc() = "GridLAB-D nanobind bindings (minimal)";

    m.def("hello", []() { return "Hello from GridLAB-D bindings!"; });

    // Bind enums
    nb::enum_<GLDErrorCode>(m, "GLDErrorCode")
        .value("SUCCESS", GLDErrorCode::GLD_SUCCESS)
        .value("FILE_NOT_FOUND", GLDErrorCode::GLD_FILE_NOT_FOUND)
        .value("INVALID_FORMAT", GLDErrorCode::GLD_INVALID_FORMAT)
        .value("OPERATION_FAILED", GLDErrorCode::GLD_OPERATION_FAILED)
        .value("OBJECT_NOT_FOUND", GLDErrorCode::GLD_OBJECT_NOT_FOUND)
        .value("TIME_STEP_ERROR", GLDErrorCode::GLD_TIME_STEP_ERROR);

    nb::enum_<GLDApplicationType>(m, "GLDApplicationType")
        .value("UNKNOWN", GLDApplicationType::GLD_APPLICATION_TYPE_UNKNOWN)
        .value("GRIDLABD", GLDApplicationType::GLD_APPLICATION_TYPE_GRIDLABD)
        .value("OTHER", GLDApplicationType::GLD_APPLICATION_TYPE_OTHER);

    nb::enum_<GLDCheckPointMode>(m, "GLDCheckPointMode")
        .value("NONE", GLDCheckPointMode::GLD_CHECKPOINT_MODE_NONE)
        .value("SAVE", GLDCheckPointMode::GLD_CHECKPOINT_MODE_SAVE)
        .value("LOAD", GLDCheckPointMode::GLD_CHECKPOINT_MODE_LOAD);

    // Bind the GridLabD class
    nb::class_<GridLabD>(m, "GridLabD")
        .def(nb::init<>())
        .def("set_config_file", &GridLabD::set_config_file)
        .def("load_glm", &GridLabD::load_glm)
        .def("save_checkpoint", &GridLabD::save_checkpoint,
             nb::arg("save_path"),
             nb::arg("mode") = GLDCheckPointMode::GLD_CHECKPOINT_MODE_SAVE)
        .def("load_checkpoint", &GridLabD::load_checkpoint)
        .def("set_application_mode", &GridLabD::set_application_mode)
        .def("set_time_step", &GridLabD::set_time_step)
        .def("set_time", &GridLabD::set_time)
        .def("get_time", [](GridLabD &self) {
            std::string current;
            GLDErrorCode code = self.get_time(current);
            return nb::make_tuple(code, current);
        })
        .def("run", [](GridLabD &self, double start_time, double end_time) {
            double sim_time = 0.0;
            GLDErrorCode code = self.run(start_time, end_time, sim_time);
            return nb::make_tuple(code, sim_time);
        }, nb::arg("start_time"), nb::arg("end_time"))
        .def("step", [](GridLabD &self, double current_time) {
            // Convenience wrapper: accept current_time, return new time
            double sim_time = current_time;
            GLDErrorCode code = self.step(sim_time);
            return nb::make_tuple(code, sim_time);
        }, nb::arg("current_time") = 0.0)
        .def("reset_step", [](GridLabD &self) {
            double current_time = 0.0;
            GLDErrorCode code = self.reset_step(current_time);
            return nb::make_tuple(code, current_time);
        });

    // TODO: Add GLDData-based methods once we decide on a Python<->C++ mapping
    // e.g., dict[str, object] <-> std::map<std::string, std::any>
}