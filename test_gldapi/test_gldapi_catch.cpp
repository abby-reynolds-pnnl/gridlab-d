#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "gldapi.h"
#include <string>

GLDErrorCode dummy_callback(GridLabD* gld) {
    return GLD_SUCCESS;

}

TEST_CASE("Set config file", "[gldapi]") {
    GridLabD sim;
    REQUIRE((sim.set_config_file("config.cfg") == GLD_SUCCESS));
}

TEST_CASE("Load GLM", "[gldapi]") {
    GridLabD sim;
    REQUIRE((sim.load_glm("example.glm") == GLD_SUCCESS));
}

TEST_CASE("Add, Delete, Edit Object", "[gldapi]") {
    GridLabD sim;
    GLDData data = {
        {"name", std::any(std::string("object1"))},
        {"type", std::any(std::string("generator"))}
    };
    REQUIRE(sim.add_object(data) == GLD_SUCCESS);
    REQUIRE(sim.delete_object("object1") == GLD_SUCCESS);
    REQUIRE(sim.edit_object("object1", data) == GLD_SUCCESS);
}

TEST_CASE("Set/Get GLM Data", "[gldapi]") {
    GridLabD sim;
    GLDData data = {
        {"name", std::any(std::string("object1"))},
        {"type", std::any(std::string("generator"))}
    };
    REQUIRE(sim.set_glm_data(data) == GLD_SUCCESS);
    GLDData query_result;
    REQUIRE(sim.get_glm_data("SELECT * FROM all", query_result) == GLD_SUCCESS);
    REQUIRE(query_result.count("status") > 0);
}

TEST_CASE("Checkpoint & Callbacks", "[gldapi]") {
    GridLabD sim;
    REQUIRE(sim.save_checkpoint("state.chk", GLD_CHECKPOINT_MODE_SAVE) == GLD_SUCCESS);
    REQUIRE(sim.load_checkpoint("state.chk") == GLD_SUCCESS);
    REQUIRE(sim.set_prestep_callback(dummy_callback) == GLD_SUCCESS);
    REQUIRE(sim.set_poststep_callback(dummy_callback) == GLD_SUCCESS);
}

TEST_CASE("Time functions", "[gldapi]") {
    GridLabD sim;
    REQUIRE(sim.set_time("2025-06-18T10:00:00") == GLD_SUCCESS);
    std::string current_time;
    REQUIRE(sim.get_time(current_time) == GLD_SUCCESS);
}

TEST_CASE("Mode, step, and run", "[gldapi]") {
    GridLabD sim;
    REQUIRE(sim.set_application_mode(GLD_APPLICATION_TYPE_GRIDLABD) == GLD_SUCCESS);
    REQUIRE(sim.set_time_step(60.0) == GLD_SUCCESS);
    double sim_time = 0.0;
    REQUIRE(sim.run(0.0, 3600.0, sim_time) == GLD_SUCCESS);
    REQUIRE(sim.step(sim_time) == GLD_SUCCESS);
    REQUIRE(sim.reset_step(sim_time) == GLD_SUCCESS);
}