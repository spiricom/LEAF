#include <catch2/catch_test_macros.hpp>
#include "../Inc/leaf-oscillators.h"
#include "../leaf.h"
#include "../Inc/leaf-math.h"

static float myrand() {return (float)rand()/RAND_MAX;}

TEST_CASE("Tests for `tCycle` object", "[tCycle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tCycle osc;
    tCycle_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tCycle_free(&osc));
}

TEST_CASE("Tests for `tTriangle` object", "[tTriangle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tTriangle osc;
    tTriangle_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tTriangle_free(&osc));
}

TEST_CASE("Tests for `tSquare` object", "[tSquare]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tSquare osc;
    tSquare_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSquare_free(&osc));
}

TEST_CASE("Tests for `tSawtooth` object", "[tSawtooth]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tSawtooth osc;
    tSawtooth_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSawtooth_free(&osc));
}

TEST_CASE("Tests for `tPBSineTriangle` object", "[tPBSineTriangle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPBSineTriangle osc;
    tPBSineTriangle_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBSineTriangle_free(&osc));
}

TEST_CASE("Tests for `tPBTriangle` object", "[tPBTriangle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPBTriangle osc;
    tPBTriangle_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBTriangle_free(&osc));
}

TEST_CASE("Tests for `tPBPulse` object", "[tPBPulse]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPBPulse osc;
    tPBPulse_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBPulse_free(&osc));
}

TEST_CASE("Tests for `tPBSaw` object", "[tPBSaw]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tPBSaw osc;
    tPBSaw_init(&osc, &leaf);

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
    tPhasor_init(&osc, &leaf);

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
    tNeuron_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tNeuron_free(&osc));
}

TEST_CASE("Tests for `tMBPulse` object", "[tMBPulse]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tMBPulse osc;
    tMBPulse_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBPulse_free(&osc));
}

TEST_CASE("Tests for `tMBTriangle` object", "[tMBTriangle]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tMBTriangle osc;
    tMBTriangle_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBTriangle_free(&osc));
}

TEST_CASE("Tests for `tMBSineTri` object", "[tMBSineTri]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tMBSineTri osc;
    tMBSineTri_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBSineTri_free(&osc));
}

TEST_CASE("Tests for `tMBSaw` object", "[tMBSaw]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tMBSaw osc;
    tMBSaw_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBSaw_free(&osc));
}

TEST_CASE("Tests for `tMBSawPulse` object", "[tMBSawPulse]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tMBSawPulse osc;
    tMBSawPulse_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBSawPulse_free(&osc));
}

TEST_CASE("Tests for `tTable` object", "[tTable]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tTable osc;
    Lfloat num[512];

    LEAF_generate_sine(num, 512);
    tTable_init(&osc, num, 512, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tTable_free(&osc));
}

TEST_CASE("Tests for `tWaveTable` object", "[tWaveTable]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    Lfloat num[512];
    LEAF_generate_sine(num, 512);

    tWaveTable osc;
    tWaveTable_init(&osc, num, 512, 22050, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tWaveTable_free(&osc));
}

TEST_CASE("Tests for `tWaveOsc` object", "[tWaveOsc]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tWaveTable waveTable;
    Lfloat num[512];
    LEAF_generate_sine(num, 512);

    tWaveTable_init(&waveTable, num, 512, 22050, &leaf);

    tWaveOsc osc;
    tWaveOsc_init(&osc, &waveTable, waveTable->numTables, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tWaveOsc_free(&osc));
}

TEST_CASE("Tests for `tWaveTableS` object", "[tWaveTableS]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    Lfloat num[512];
    LEAF_generate_sine(num, 512);

    tWaveTableS osc;
    tWaveTableS_init(&osc, num, 512, 22050, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tWaveTableS_free(&osc));
}

TEST_CASE("Tests for `tWaveOscS` object", "[tWaveOscS]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tWaveTableS waveTable;
    Lfloat num[512];
    LEAF_generate_sine(num, 512);

    tWaveTableS_init(&waveTable, num, 512, 22050, &leaf);

    tWaveOscS osc;
    tWaveOscS_init(&osc, &waveTable, waveTable->numTables, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tWaveOscS_free(&osc));
}

TEST_CASE("Tests for `tIntPhasor` object", "[tIntPhasor]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tIntPhasor osc;
    tIntPhasor_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tIntPhasor_free(&osc));
}

TEST_CASE("Tests for `tSquareLFO` object", "[tSquareLFO]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tSquareLFO osc;
    tSquareLFO_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSquareLFO_free(&osc));
}

TEST_CASE("Tests for `tSawSquareLFO` object", "[tSawSquareLFO]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tSawSquareLFO osc;
    tSawSquareLFO_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSawSquareLFO_free(&osc));
}

TEST_CASE("Tests for `tTriLFO` object", "[tTriLFO]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tTriLFO osc;
    tTriLFO_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tTriLFO_free(&osc));
}

TEST_CASE("Tests for `tSineTriLFO` object", "[tSineTriLFO]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tSineTriLFO osc;
    tSineTriLFO_init(&osc, &leaf);

    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSineTriLFO_free(&osc));
}

// TEST_CASE("Tests for `tDampedOscillator` object", "[tDampedOscillator]") {
//
//    LEAF leaf;
//    char leafMemory[65535];
//    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);
//
//    tDampedOscillator osc;
//    tDampedOscillator_init(&osc, &leaf);
//
//    REQUIRE(osc != nullptr);
//    REQUIRE_NOTHROW(tDampedOscillator_free(&osc));
//}
