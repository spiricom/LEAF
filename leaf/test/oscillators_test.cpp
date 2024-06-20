#include <catch2/catch_test_macros.hpp>
#include "../Inc/leaf-oscillators.h"
#include "../leaf.h"

float myrand() {return (float)rand()/RAND_MAX;}

TEST_CASE("Tests for `tCycle` object", "[tCycle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tCycle osc;
    tCycle_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tCycle_free(&osc));
}

TEST_CASE("Tests for `tTriangle` object", "[tTriangle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tTriangle osc;
    tTriangle_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tTriangle_free(&osc));
}

TEST_CASE("Tests for `tSquare` object", "[tSquare]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tSquare osc;
    tSquare_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSquare_free(&osc));
}

TEST_CASE("Tests for `tSawtooth` object", "[tSawtooth]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tSawtooth osc;
    tSawtooth_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSawtooth_free(&osc));
}

TEST_CASE("Tests for `tPBSineTriangle` object", "[tPBSineTriangle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPBSineTriangle osc;
    tPBSineTriangle_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBSineTriangle_free(&osc));
}

TEST_CASE("Tests for `tPBTriangle` object", "[tPBTriangle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPBTriangle osc;
    tPBTriangle_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBTriangle_free(&osc));
}

TEST_CASE("Tests for `tPBPulse` object", "[tPBPulse]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPBPulse osc;
    tPBPulse_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBPulse_free(&osc));
}

TEST_CASE("Tests for `tPBSaw` object", "[tPBSaw]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPBSaw osc;
    tPBSaw_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBSaw_free(&osc));
}

TEST_CASE("Tests for `tPBSawSquare` object", "[tPBSawSquare]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPBSawSquare osc;
    tPBSawSquare_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBSawSquare_free(&osc));
}

TEST_CASE("Tests for `tSawOS` object", "[tSawOS]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tSawOS osc;
    tSawOS_init(&osc, 2, 2, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSawOS_free(&osc));
}

TEST_CASE("Tests for `tPhasor` object", "[tPhasor]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPhasor osc;
    tPhasor_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPhasor_free(&osc));
}

TEST_CASE("Tests for `tNoise` object", "[tNoise]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tNoise osc;
    tNoise_init(&osc, WhiteNoise, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tNoise_free(&osc));

    tNoise osc2;
    tNoise_init(&osc2, PinkNoise, &leaf);

    REQUIRE(osc2 != nullptr);
    REQUIRE_NOTHROW(tNoise_free(&osc2));
}

TEST_CASE("Tests for `tNeuron` object", "[tNeuron]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tNeuron osc;
    tNeuron_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tNeuron_free(&osc));
}

TEST_CASE("Tests for `tMBPulse` object", "[tMBPulse]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tMBPulse osc;
    tMBPulse_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBPulse_free(&osc));
}

TEST_CASE("Tests for `tMBTriangle` object", "[tMBTriangle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tMBTriangle osc;
    tMBTriangle_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBTriangle_free(&osc));
}

TEST_CASE("Tests for `tMBSineTri` object", "[tMBSineTri]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tMBSineTri osc;
    tMBSineTri_init(&osc,&leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBSineTri_free(&osc));
}
