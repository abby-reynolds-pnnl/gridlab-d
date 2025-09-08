#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include "gldapi.h"

#include <any>
#include <typeinfo>
#include <stdexcept>

namespace nb = nanobind;

// Convert Python dict[str, object] -> GLDData (map<string, any>)
// Supported value types: str, bool, int, float
static GLDData dict_to_glddata(nb::dict d) {
    GLDData out;
    for (auto item : d) {
        std::string key = nb::cast<std::string>(item.first);
        nb::handle v = item.second;
        if (nb::isinstance<nb::str>(v)) {
            out[key] = nb::cast<std::string>(v);
        } else if (nb::isinstance<nb::bool_>(v)) {
            out[key] = nb::cast<bool>(v);
        } else if (nb::isinstance<nb::int_>(v)) {
            // Store ints as 64-bit
            out[key] = static_cast<long long>(nb::cast<long long>(v));
        } else if (nb::isinstance<nb::float_>(v)) {
            out[key] = nb::cast<double>(v);
        } else {
            std::string msg = "Unsupported value type in GLDData for key '" + key + "'";
            throw nb::type_error(msg.c_str());
        }
    }
    return out;
}

// Convert GLDData -> Python dict[str, object]
static nb::dict glddata_to_dict(const GLDData& data) {
    nb::dict out;
    for (const auto& kv : data) {
        const std::string& key = kv.first;
        const std::any& val = kv.second;
        if (val.type() == typeid(std::string)) {
            out[key.c_str()] = nb::cast(std::any_cast<const std::string&>(val));
        } else if (val.type() == typeid(bool)) {
            out[key.c_str()] = nb::cast(std::any_cast<bool>(val));
        } else if (val.type() == typeid(long long)) {
            out[key.c_str()] = nb::cast(std::any_cast<long long>(val));
        } else if (val.type() == typeid(double)) {
            out[key.c_str()] = nb::cast(std::any_cast<double>(val));
        } else {
            out[key.c_str()] = nb::none(); // unknown types -> None
        }
    }
    return out;
}

NB_MODULE(_gridlabd_impl, m) {
    m.doc() = "GridLAB-D nanobind bindings";

    // Smoke test
    m.def("hello", []() { return "Hello from GridLAB-D bindings!"; });

    // Enums
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

    // Class
    nb::class_<GridLabD>(m, "GridLabD")
        .def(nb::init<>())

        // Simple methods
        .def("set_config_file", &GridLabD::set_config_file)
        .def("load_glm", &GridLabD::load_glm)
        .def("save_checkpoint", &GridLabD::save_checkpoint,
             nb::arg("save_path"),
             nb::arg("mode") = GLDCheckPointMode::GLD_CHECKPOINT_MODE_SAVE)
        .def("load_checkpoint", &GridLabD::load_checkpoint)
        .def("set_application_mode", &GridLabD::set_application_mode)
        .def("set_time_step", &GridLabD::set_time_step)
        .def("set_time", &GridLabD::set_time)

        // Methods with out-params -> return tuple
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
            double sim_time = current_time;
            GLDErrorCode code = self.step(sim_time);
            return nb::make_tuple(code, sim_time);
        }, nb::arg("current_time") = 0.0)
        .def("reset_step", [](GridLabD &self) {
            double current_time = 0.0;
            GLDErrorCode code = self.reset_step(current_time);
            return nb::make_tuple(code, current_time);
        })

        // GLDData roundtrip
        .def("get_glm_data", [](GridLabD &self, const std::string& query) {
            GLDData result;
            GLDErrorCode code = self.get_glm_data(query, result);
            return nb::make_tuple(code, glddata_to_dict(result));
        })
        .def("set_glm_data", [](GridLabD &self, nb::dict data) {
            GLDData in = dict_to_glddata(data);
            return self.set_glm_data(in);
        })
        .def("add_object", [](GridLabD &self, nb::dict obj) {
            GLDData in = dict_to_glddata(obj);
            return self.add_object(in);
        })
        .def("edit_object", [](GridLabD &self, const std::string& name, nb::dict updated) {
            GLDData in = dict_to_glddata(updated);
            return self.edit_object(name, in);
        })
        .def("delete_object", &GridLabD::delete_object);
}