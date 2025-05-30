#include <catch2/catch_test_macros.hpp>
#include "../leaf/Inc/leaf-filters.h"
#include "../leaf/leaf.h"
#include "../leaf/Inc/leaf-math.h"

static float myrand() {return (float)rand()/RAND_MAX;}

TEST_CASE("Tests for `tAllpass` filer", "[tAllpass]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    tAllpass* filter;
    tAllpass_init(&filter, 50, 100, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tAllpass_free(&filter));
}

TEST_CASE("Tests for `tAllpassSO` filer", "[tAllpassSO]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tAllpassSO* filter;
    tAllpassSO_init(&filter, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tAllpassSO_free(&filter));
}

TEST_CASE("Tests for `tThiranAllpassSOCascade` filer", "[tThiranAllpassSOCascade]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tThiranAllpassSOCascade* filter;
    tThiranAllpassSOCascade_init(&filter, 3, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tThiranAllpassSOCascade_free(&filter));
}

TEST_CASE("Tests for `tOnePole` filer", "[tOnePole]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tOnePole* filter;
    tOnePole_init(&filter, 22050, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tOnePole_free(&filter));
}

TEST_CASE("Tests for `tCookOnePole` filer", "[tCookOnePole]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tCookOnePole* filter;
    tCookOnePole_init(&filter, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tCookOnePole_free(&filter));
}

TEST_CASE("Tests for `tTwoPole` filer", "[tTwoPole]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tTwoPole* filter;
    tTwoPole_init(&filter, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tTwoPole_free(&filter));
}

TEST_CASE("Tests for `tOneZero` filer", "[tOneZero]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tOneZero* filter;
    tOneZero_init(&filter, 1, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tOneZero_free(&filter));
}

TEST_CASE("Tests for `tTwoZero` filer", "[tTwoZero]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tTwoZero* filter;
    tTwoZero_init(&filter, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tTwoZero_free(&filter));
}

TEST_CASE("Tests for `tPoleZero` filer", "[tPoleZero]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tPoleZero* filter;
    tPoleZero_init(&filter, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tPoleZero_free(&filter));
}

TEST_CASE("Tests for `tBiQuad` filer", "[tBiQuad]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tBiQuad* filter;
    tBiQuad_init(&filter, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tBiQuad_free(&filter));
}

TEST_CASE("Tests for `tSVF` filer", "[tSVF]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tSVF* filter1;
    tSVF_init(&filter1, SVFTypeHighpass, 22050, 4, &leaf);

    REQUIRE(filter1 != nullptr);
    REQUIRE_NOTHROW(tSVF_free(&filter1));

tSVF* filter2;
    tSVF_init(&filter2, SVFTypeLowpass, 22050, 4, &leaf);

    REQUIRE(filter2 != nullptr);
    REQUIRE_NOTHROW(tSVF_free(&filter2));

tSVF* filter3;
    tSVF_init(&filter3, SVFTypeBandpass, 22050, 4, &leaf);

    REQUIRE(filter3 != nullptr);
    REQUIRE_NOTHROW(tSVF_free(&filter3));

tSVF* filter4;
    tSVF_init(&filter4, SVFTypeNotch, 22050, 4, &leaf);

    REQUIRE(filter4 != nullptr);
    REQUIRE_NOTHROW(tSVF_free(&filter4));

tSVF* filter5;
    tSVF_init(&filter5, SVFTypePeak, 22050, 4, &leaf);

    REQUIRE(filter5 != nullptr);
    REQUIRE_NOTHROW(tSVF_free(&filter5));

tSVF* filter6;
    tSVF_init(&filter6, SVFTypeLowShelf, 22050, 4, &leaf);

    REQUIRE(filter6 != nullptr);
    REQUIRE_NOTHROW(tSVF_free(&filter6));

tSVF* filter7;
    tSVF_init(&filter7, SVFTypeHighShelf, 22050, 4, &leaf);

    REQUIRE(filter7 != nullptr);
    REQUIRE_NOTHROW(tSVF_free(&filter7));
}

TEST_CASE("Tests for `tEfficientSVF` filer", "[tEfficientSVF]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tEfficientSVF* filter1;
    tEfficientSVF_init(&filter1, SVFTypeHighpass, 22050, 4, &leaf);

    REQUIRE(filter1 != nullptr);
    REQUIRE_NOTHROW(tEfficientSVF_free(&filter1));

tEfficientSVF* filter2;
    tEfficientSVF_init(&filter2, SVFTypeLowpass, 22050, 4, &leaf);

    REQUIRE(filter2 != nullptr);
    REQUIRE_NOTHROW(tEfficientSVF_free(&filter2));

tEfficientSVF* filter3;
    tEfficientSVF_init(&filter3, SVFTypeBandpass, 22050, 4, &leaf);

    REQUIRE(filter3 != nullptr);
    REQUIRE_NOTHROW(tEfficientSVF_free(&filter3));

tEfficientSVF* filter4;
    tEfficientSVF_init(&filter4, SVFTypeNotch, 22050, 4, &leaf);

    REQUIRE(filter4 != nullptr);
    REQUIRE_NOTHROW(tEfficientSVF_free(&filter4));

tEfficientSVF* filter5;
    tEfficientSVF_init(&filter5, SVFTypePeak, 22050, 4, &leaf);

    REQUIRE(filter5 != nullptr);
    REQUIRE_NOTHROW(tEfficientSVF_free(&filter5));

tEfficientSVF* filter6;
    tEfficientSVF_init(&filter6, SVFTypeLowShelf, 22050, 4, &leaf);

    REQUIRE(filter6 != nullptr);
    REQUIRE_NOTHROW(tEfficientSVF_free(&filter6));

tEfficientSVF* filter7;
    tEfficientSVF_init(&filter7, SVFTypeHighShelf, 22050, 4, &leaf);

    REQUIRE(filter7 != nullptr);
    REQUIRE_NOTHROW(tEfficientSVF_free(&filter7));
}

TEST_CASE("Tests for `tHighpass` filer", "[tHighpass]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tHighpass* filter;
    tHighpass_init(&filter, 22050, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tHighpass_free(&filter));
}

TEST_CASE("Tests for `tButterworth` filer", "[tButterworth]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tButterworth* filter;
    tButterworth_init(&filter, 3, 1, 1, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tButterworth_free(&filter));
}


TEST_CASE("Tests for `tFIR` filer", "[tFIR]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

    Lfloat coeffs[512];
    LEAF_generate_sine(coeffs, 512);

tFIR* filter;
    tFIR_init(&filter, coeffs, 1, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tFIR_free(&filter));
}

/******************************** FIX!!! **************************************/

//TEST_CASE("Tests for `tFIR` filer", "[tFIR]") {
//
//    LEAF leaf;
//    char leafMemory[65535];
//    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);
//
//    tMedianFilter filter;
//    tMedianFilter_init(&filter,10, &leaf);
//
//    REQUIRE(filter != nullptr);
//    REQUIRE_NOTHROW(tMedianFilter_free(&filter));
//}

TEST_CASE("Tests for `tVZFilter` filer", "[tVZFilter]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tVZFilter* filter1;
    tVZFilter_init(&filter1, Highpass, 22050, 4, &leaf);

    REQUIRE(filter1 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter1));

tVZFilter* filter2;
    tVZFilter_init(&filter2, Lowpass, 22050, 4, &leaf);

    REQUIRE(filter2 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter2));

tVZFilter* filter3;
    tVZFilter_init(&filter3, BandpassSkirt, 22050, 4, &leaf);

    REQUIRE(filter3 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter3));

tVZFilter* filter4;
    tVZFilter_init(&filter4, BandpassPeak, 22050, 4, &leaf);

    REQUIRE(filter4 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter4));

tVZFilter* filter5;
    tVZFilter_init(&filter5, BandReject, 22050, 4, &leaf);

    REQUIRE(filter5 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter5));

tVZFilter* filter6;
    tVZFilter_init(&filter6, Bell, 22050, 4, &leaf);

    REQUIRE(filter6 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter6));

tVZFilter* filter7;
    tVZFilter_init(&filter7, Lowshelf, 22050, 4, &leaf);

    REQUIRE(filter7 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter7));

tVZFilter* filter8;
    tVZFilter_init(&filter8, Highshelf, 22050, 4, &leaf);

    REQUIRE(filter8 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter8));

tVZFilter* filter9;
    tVZFilter_init(&filter9, Morph, 22050, 4, &leaf);

    REQUIRE(filter9 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter9));

tVZFilter* filter10;
    tVZFilter_init(&filter10, Bypass, 22050, 4, &leaf);

    REQUIRE(filter10 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter10));

tVZFilter* filter11;
    tVZFilter_init(&filter11, Allpass, 22050, 4, &leaf);

    REQUIRE(filter11 != nullptr);
    REQUIRE_NOTHROW(tVZFilter_free(&filter11));
}

TEST_CASE("Tests for `tVZFilterLS` filer", "[tVZFilterLS]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tVZFilterLS* filter;
    tVZFilterLS_init(&filter, 22050, 1, 1, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tVZFilterLS_free(&filter));
}

TEST_CASE("Tests for `tVZFilterHS` filer", "[tVZFilterHS]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tVZFilterHS* filter;
    tVZFilterHS_init(&filter, 22050, 1, 1, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tVZFilterHS_free(&filter));
}

TEST_CASE("Tests for `tVZFilterBell` filer", "[tVZFilterBell]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tVZFilterBell* filter;
    tVZFilterBell_init(&filter, 22050, 1, 1, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tVZFilterBell_free(&filter));
}

TEST_CASE("Tests for `tVZFilterBR` filer", "[tVZFilterBR]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tVZFilterBR* filter;
    tVZFilterBR_init(&filter, 22050, 1, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tVZFilterBR_free(&filter));
}

TEST_CASE("Tests for `tDiodeFilter` filer", "[tDiodeFilter]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tDiodeFilter* filter;
    tDiodeFilter_init(&filter, 22050, 1, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tDiodeFilter_free(&filter));
}

TEST_CASE("Tests for `tLadderFilter` filer", "[tLadderFilter]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tLadderFilter* filter;
    tLadderFilter_init(&filter, 22050, 1, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tLadderFilter_free(&filter));
}

TEST_CASE("Tests for `tTiltFilter` filer", "[tTiltFilter]") {

    LEAF leaf;
    char leafMemory[65535];
    LEAF_init(&leaf, 44100.f, leafMemory, 65535, &myrand);

tTiltFilter* filter;
    tTiltFilter_init(&filter, 22050, &leaf);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tTiltFilter_free(&filter));
}