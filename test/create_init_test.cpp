//#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_test_macros.hpp>
#include "leaf.h"
#include "leaf-analysis.h"
#include "leaf-math.h"
#include "catch2/catch_approx.hpp"
#include "leaf-delay.h"
#include "leaf-filters.h"
#include "leaf-distortion.h"
#include "leaf-dynamics.h"
#include "leaf-envelopes.h"
#include "leaf-oscillators.h"
#include "leaf-effects.h"
#include "leaf-electrical.h"
#include "leaf-instruments.h"
#include "leaf-midi.h"
static float myrand() { return (float) rand() / RAND_MAX; }

#define LEAF_SETUP()                     \
    LEAF leaf;                           \
    char leafMemory[131070];              \
    LEAF_init(&leaf, 44100.f, leafMemory, 131070, &myrand)

/*==============================================================================
   tEnvelopeFollower
==============================================================================*/

TEST_CASE("Tests for `tEnvelopeFollower`", "[tEnvelopeFollower][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tEnvelopeFollower* follower;
    tEnvelopeFollower_create(&leaf.mempool, &follower);

    REQUIRE_NOTHROW(tEnvelopeFollower_init(&leaf, follower, 0.1f, 0.99f));
    REQUIRE(follower != nullptr);
    REQUIRE_NOTHROW(tEnvelopeFollower_free(&follower));
}

/*==============================================================================
   tZeroCrossingCounter
==============================================================================*/

TEST_CASE("Tests for `tZeroCrossingCounter`", "[tZeroCrossingCounter][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tZeroCrossingCounter* counter;
    tZeroCrossingCounter_create(&leaf.mempool, &counter);

    REQUIRE_NOTHROW(tZeroCrossingCounter_init(&leaf, counter, 1024));
    REQUIRE(counter != nullptr);
    REQUIRE_NOTHROW(tZeroCrossingCounter_free(&counter));
}

/*==============================================================================
   tPowerFollower
==============================================================================*/

TEST_CASE("Tests for `tPowerFollower`", "[tPowerFollower]") {
    LEAF_SETUP();

    tPowerFollower* follower;
    tPowerFollower_create(&leaf.mempool, &follower);

    REQUIRE_NOTHROW(tPowerFollower_init(&leaf, follower, 0.9f));
    REQUIRE(follower != nullptr);
    REQUIRE_NOTHROW(tPowerFollower_free(&follower));
}

/*==============================================================================
   tEnvPD
==============================================================================*/

TEST_CASE("Tests for `tEnvPD`", "[tEnvPD][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tEnvPD* env;
    tEnvPD_create(&leaf.mempool, &env);

    REQUIRE_NOTHROW(tEnvPD_init(&leaf, env, 1024, 256, 128));
    REQUIRE(env != nullptr);
    REQUIRE_NOTHROW(tEnvPD_free(&env));
}

/*==============================================================================
   tAttackDetection
==============================================================================*/

TEST_CASE("Tests for `tAttackDetection`", "[tAttackDetection][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tAttackDetection* detect;
    tAttackDetection_create(&leaf.mempool, &detect);

    REQUIRE_NOTHROW(tAttackDetection_init(&leaf, detect, 512, 10, 10));
    REQUIRE(detect != nullptr);
    REQUIRE_NOTHROW(tAttackDetection_free(&detect));
}

/*==============================================================================
   tSNAC
==============================================================================*/

TEST_CASE("Tests for `tSNAC`", "[tSNAC][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tSNAC* snac;
    tSNAC_create(&leaf.mempool, &snac);

    REQUIRE_NOTHROW(tSNAC_init(&leaf, snac, 1));
    REQUIRE(snac != nullptr);
    REQUIRE_NOTHROW(tSNAC_free(&snac));
}

/*==============================================================================
   tPeriodDetection
==============================================================================*/

TEST_CASE("Tests for `tPeriodDetection`", "[tPeriodDetection][leaf-analysis][create-init]") {
    LEAF_SETUP();

    float in[1024] = {0.0f};
    tPeriodDetection* pd;
    tPeriodDetection_create(&leaf.mempool, &pd);

    REQUIRE_NOTHROW(tPeriodDetection_init(&leaf, pd, in, 1024, 512));
    REQUIRE(pd != nullptr);
    REQUIRE_NOTHROW(tPeriodDetection_free(&pd));
}

/*==============================================================================
   tZeroCrossingInfo
==============================================================================*/

TEST_CASE("Tests for `tZeroCrossingInfo`", "[tZeroCrossingInfo][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tZeroCrossingInfo* info;
    tZeroCrossingInfo_create(&leaf.mempool, &info);

    REQUIRE_NOTHROW(tZeroCrossingInfo_init(&leaf, info));
    REQUIRE(info != nullptr);
    REQUIRE_NOTHROW(tZeroCrossingInfo_free(&info));
}

/*==============================================================================
   tZeroCrossingCollector
==============================================================================*/

TEST_CASE("Tests for `tZeroCrossingCollector`", "[tZeroCrossingCollector][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tZeroCrossingCollector* collector;
    tZeroCrossingCollector_create(&leaf.mempool, &collector);

    REQUIRE_NOTHROW(tZeroCrossingCollector_init(&leaf, collector, 64, 0.5f));
    REQUIRE(collector != nullptr);
    REQUIRE_NOTHROW(tZeroCrossingCollector_free(&collector));
}

/*==============================================================================
   tBitset
==============================================================================*/

TEST_CASE("Tests for `tBitset`", "[tBitset][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tBitset* bitset;
    tBitset_create(&leaf.mempool, &bitset);

    REQUIRE_NOTHROW(tBitset_init(&leaf, bitset, 128));
    REQUIRE(bitset != nullptr);
    REQUIRE_NOTHROW(tBitset_free(&bitset));
}

/*==============================================================================
   tBACF
==============================================================================*/

TEST_CASE("Tests for `tBACF`", "[tBACF][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tBitset* bitset;
    tBitset_create(&leaf.mempool, &bitset);
    tBitset_init(&leaf, bitset, 64);

    tBACF* bacf;
    tBACF_create(&leaf.mempool, &bacf);

    REQUIRE_NOTHROW(tBACF_init(&leaf, bacf, bitset));
    REQUIRE(bacf != nullptr);
    REQUIRE_NOTHROW(tBACF_free(&bacf));
    REQUIRE_NOTHROW(tBitset_free(&bitset));
}

/*==============================================================================
   tPeriodDetector
==============================================================================*/

TEST_CASE("Tests for `tPeriodDetector`", "[tPeriodDetector][leaf-analysis][create-init]") {
    LEAF_SETUP();

    tPeriodDetector* detector;
    tPeriodDetector_create(&leaf.mempool, &detector);

    REQUIRE_NOTHROW(tPeriodDetector_init(&leaf, detector, 400.f, 2000.f, 0.5f));
    REQUIRE(detector != nullptr);
    REQUIRE_NOTHROW(tPeriodDetector_free(&detector));
}


/*==============================================================================
   tDelay  (non-interpolating)
==============================================================================*/

TEST_CASE("Tests for `tDelay`", "[tDelay][leaf-delay][create-init]") {
    LEAF_SETUP();

    tDelay* d;
    tDelay_create(&leaf.mempool, &d);

    REQUIRE_NOTHROW(tDelay_init(&leaf, d, 64, 2048));
    REQUIRE(d != nullptr);

    REQUIRE_NOTHROW(tDelay_clear(d));
    REQUIRE_NOTHROW(tDelay_setDelay(d, 32));
    REQUIRE(tDelay_getDelay(d) == 32);

    REQUIRE_NOTHROW(tDelay_free(&d));
}


/*==============================================================================
   tLinearDelay
==============================================================================*/

TEST_CASE("Tests for `tLinearDelay`", "[tLinearDelay][leaf-delay][create-init]") {
    LEAF_SETUP();

    tLinearDelay* ld;
    tLinearDelay_create(&leaf.mempool, &ld);

    REQUIRE_NOTHROW(tLinearDelay_init(&leaf, ld, 48.f, 2048));
    REQUIRE(ld != nullptr);

    REQUIRE_NOTHROW(tLinearDelay_clear(ld));
    REQUIRE_NOTHROW(tLinearDelay_setDelay(ld, 24.f));
    REQUIRE(tLinearDelay_getDelay(ld) == Catch::Approx(24.f));

    REQUIRE_NOTHROW(tLinearDelay_free(&ld));
}


/*==============================================================================
   tHermiteDelay
==============================================================================*/

TEST_CASE("Tests for `tHermiteDelay`", "[tHermiteDelay][leaf-delay][create-init]") {
    LEAF_SETUP();

    tHermiteDelay* hd;
    tHermiteDelay_create(&leaf.mempool, &hd);

    REQUIRE_NOTHROW(tHermiteDelay_init(&leaf, hd, 40.f, 2048));
    REQUIRE(hd != nullptr);

    REQUIRE_NOTHROW(tHermiteDelay_clear(hd));
    REQUIRE_NOTHROW(tHermiteDelay_setDelay(hd, 20.f));
    REQUIRE(tHermiteDelay_getDelay(hd) == Catch::Approx(20.f));

    REQUIRE_NOTHROW(tHermiteDelay_free(&hd));
}


/*==============================================================================
   tLagrangeDelay
==============================================================================*/

TEST_CASE("Tests for `tLagrangeDelay`", "[tLagrangeDelay][leaf-delay][create-init]") {
    LEAF_SETUP();

    tLagrangeDelay* lg;
    tLagrangeDelay_create(&leaf.mempool, &lg);

    REQUIRE_NOTHROW(tLagrangeDelay_init(&leaf, lg, 30.f, 2048));
    REQUIRE(lg != nullptr);

    REQUIRE_NOTHROW(tLagrangeDelay_clear(lg));
    REQUIRE_NOTHROW(tLagrangeDelay_setDelay(lg, 15.f));
    REQUIRE(tLagrangeDelay_getDelay(lg) == Catch::Approx(15.f));

    REQUIRE_NOTHROW(tLagrangeDelay_free(&lg));
}


/*==============================================================================
   tAllpassDelay
==============================================================================*/

TEST_CASE("Tests for `tAllpassDelay`", "[tAllpassDelay][leaf-delay][create-init]") {
    LEAF_SETUP();

    tAllpassDelay* ap;
    tAllpassDelay_create(&leaf.mempool, &ap);

    REQUIRE_NOTHROW(tAllpassDelay_init(&leaf, ap, 20.f, 2048));
    REQUIRE(ap != nullptr);

    REQUIRE_NOTHROW(tAllpassDelay_clear(ap));
    REQUIRE_NOTHROW(tAllpassDelay_setDelay(ap, 10.f));
    REQUIRE(tAllpassDelay_getDelay(ap) == Catch::Approx(10.f));

    REQUIRE_NOTHROW(tAllpassDelay_free(&ap));
}


/*==============================================================================
   tTapeDelay
==============================================================================*/

TEST_CASE("Tests for `tTapeDelay`", "[tTapeDelay][leaf-delay][create-init]") {
    LEAF_SETUP();

    tTapeDelay* td;
    tTapeDelay_create(&leaf.mempool, &td);

    REQUIRE_NOTHROW(tTapeDelay_init(&leaf, td, 100.f, 4096));
    REQUIRE(td != nullptr);

    REQUIRE_NOTHROW(tTapeDelay_clear(td));
    REQUIRE_NOTHROW(tTapeDelay_setDelay(td, 50.f));
    REQUIRE(tTapeDelay_getDelay(td) == Catch::Approx(50.f));

    REQUIRE_NOTHROW(tTapeDelay_free(&td));
}


/*==============================================================================
   tRingBuffer
==============================================================================*/

TEST_CASE("Tests for `tRingBuffer`", "[tRingBuffer][leaf-delay][create-init]") {
    LEAF_SETUP();

    tRingBuffer* rb;
    tRingBuffer_create(&leaf.mempool, &rb);

    REQUIRE_NOTHROW(tRingBuffer_init(&leaf, rb, 128));
    REQUIRE(rb != nullptr);

    REQUIRE_NOTHROW(tRingBuffer_push(rb, 0.5f));
    REQUIRE(tRingBuffer_getNewest(rb) == Catch::Approx(0.5f));


    REQUIRE_NOTHROW(tRingBuffer_free(&rb));
}


/*==============================================================================
    tAllpass
==============================================================================*/

TEST_CASE("Tests for tAllpass create/init/free",
          "[leaf-filters][create-init][tAllpass]") {
    LEAF_SETUP();

    tAllpass* filter;
    tAllpass_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tAllpass_init(&leaf, filter, 10.0f, 1024));
    REQUIRE_NOTHROW(tAllpass_free(&filter));
}

/*==============================================================================
    tAllpassSO
==============================================================================*/

TEST_CASE("Tests for tAllpassSO create/init/free",
          "[leaf-filters][create-init][tAllpassSO]") {
    LEAF_SETUP();

    tAllpassSO* filter;
    tAllpassSO_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tAllpassSO_init(&leaf, filter));
    REQUIRE_NOTHROW(tAllpassSO_free(&filter));
}

/*==============================================================================
    tThiranAllpassSOCascade
==============================================================================*/

TEST_CASE("Tests for tThiranAllpassSOCascade create/init/free",
          "[leaf-filters][create-init][tThiranAllpassSOCascade]") {
    LEAF_SETUP();

    tThiranAllpassSOCascade* filter;
    tThiranAllpassSOCascade_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tThiranAllpassSOCascade_init(&leaf, filter, 4));
    REQUIRE_NOTHROW(tThiranAllpassSOCascade_free(&filter));
}

/*==============================================================================
    tOnePole
==============================================================================*/

TEST_CASE("Tests for tOnePole create/init/free",
          "[leaf-filters][create-init][tOnePole]") {
    LEAF_SETUP();

    tOnePole* filter;
    tOnePole_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tOnePole_init(&leaf, filter, 1000.0f));
    REQUIRE_NOTHROW(tOnePole_free(&filter));
}

/*==============================================================================
    tCookOnePole
==============================================================================*/

TEST_CASE("Tests for tCookOnePole create/init/free",
          "[leaf-filters][create-init][tCookOnePole]") {
    LEAF_SETUP();

    tCookOnePole* filter;
    tCookOnePole_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tCookOnePole_init(&leaf, filter));
    REQUIRE_NOTHROW(tCookOnePole_free(&filter));
}

/*==============================================================================
    tTwoPole
==============================================================================*/

TEST_CASE("Tests for tTwoPole create/init/free",
          "[leaf-filters][create-init][tTwoPole]") {
    LEAF_SETUP();

    tTwoPole* filter;
    tTwoPole_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tTwoPole_init(&leaf, filter));
    REQUIRE_NOTHROW(tTwoPole_free(&filter));
}

/*==============================================================================
    tOneZero
==============================================================================*/

TEST_CASE("Tests for tOneZero create/init/free",
          "[leaf-filters][create-init][tOneZero]") {
    LEAF_SETUP();

    tOneZero* filter;
    tOneZero_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tOneZero_init(&leaf, filter, 0.5f));
    REQUIRE_NOTHROW(tOneZero_free(&filter));
}

/*==============================================================================
    tTwoZero
==============================================================================*/

TEST_CASE("Tests for tTwoZero create/init/free",
          "[leaf-filters][create-init][tTwoZero]") {
    LEAF_SETUP();

    tTwoZero* filter;
    tTwoZero_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tTwoZero_init(&leaf, filter));
    REQUIRE_NOTHROW(tTwoZero_free(&filter));
}

/*==============================================================================
    tPoleZero
==============================================================================*/

TEST_CASE("Tests for tPoleZero create/init/free",
          "[leaf-filters][create-init][tPoleZero]") {
    LEAF_SETUP();

    tPoleZero* filter;
    tPoleZero_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tPoleZero_init(&leaf, filter));
    REQUIRE_NOTHROW(tPoleZero_free(&filter));
}

/*==============================================================================
    tBiQuad
==============================================================================*/

TEST_CASE("Tests for tBiQuad create/init/free",
          "[leaf-filters][create-init][tBiQuad]") {
    LEAF_SETUP();

    tBiQuad* filter;
    tBiQuad_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tBiQuad_init(&leaf, filter));
    REQUIRE_NOTHROW(tBiQuad_free(&filter));
}

/*==============================================================================
    tSVF
==============================================================================*/

TEST_CASE("Tests for tSVF create/init/free",
          "[leaf-filters][create-init][tSVF]") {
    LEAF_SETUP();

    tSVF* filter;
    tSVF_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tSVF_init(&leaf, filter, SVFTypeLowpass, 1000.0f, 0.707f));
    REQUIRE_NOTHROW(tSVF_free(&filter));
}

/*==============================================================================
    tSVF_LP
==============================================================================*/

TEST_CASE("Tests for tSVF_LP create/init/free",
          "[leaf-filters][create-init][tSVF_LP]") {
    LEAF_SETUP();

    tSVF_LP* filter;
    tSVF_LP_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tSVF_LP_init(&leaf, filter, 1000.0f, 0.707f));
    REQUIRE_NOTHROW(tSVF_LP_free(&filter));
}

/*==============================================================================
    tEfficientSVF
==============================================================================*/

TEST_CASE("Tests for tEfficientSVF create/init/free",
          "[leaf-filters][create-init][tEfficientSVF]") {
    LEAF_SETUP();

    tEfficientSVF* filter;
    tEfficientSVF_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tEfficientSVF_init(&leaf, filter, SVFTypeLowpass, 2048u, 0.707f));
    REQUIRE_NOTHROW(tEfficientSVF_free(&filter));
}

/*==============================================================================
    tHighpass
==============================================================================*/

TEST_CASE("Tests for tHighpass create/init/free",
          "[leaf-filters][create-init][tHighpass]") {
    LEAF_SETUP();

    tHighpass* filter;
    tHighpass_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tHighpass_init(&leaf, filter, 200.0f));
    REQUIRE_NOTHROW(tHighpass_free(&filter));
}

/*==============================================================================
    tButterworth
==============================================================================*/

TEST_CASE("Tests for tButterworth create/init/free",
          "[leaf-filters][create-init][tButterworth]") {
    LEAF_SETUP();

    tButterworth* filter;
    tButterworth_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tButterworth_init(&leaf, filter, 4, 200.0f, 5000.0f));
    REQUIRE_NOTHROW(tButterworth_free(&filter));
}

/*==============================================================================
    tFIR
==============================================================================*/

TEST_CASE("Tests for tFIR create/init/free",
          "[leaf-filters][create-init][tFIR]") {
    LEAF_SETUP();

    tFIR* filter;
    tFIR_create(&leaf.mempool, &filter);

    float coeffs[4] = { 0.25f, 0.25f, 0.25f, 0.25f };

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tFIR_init(&leaf, filter, coeffs, 4));
    REQUIRE_NOTHROW(tFIR_free(&filter));
}

/*==============================================================================
    tMedianFilter
==============================================================================*/

TEST_CASE("Tests for tMedianFilter create/init/free",
          "[leaf-filters][create-init][tMedianFilter]") {
    LEAF_SETUP();

    tMedianFilter* filter;
    tMedianFilter_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tMedianFilter_init(&leaf, filter, 7));
    REQUIRE_NOTHROW(tMedianFilter_free(&filter));
}

/*==============================================================================
    tVZFilter
==============================================================================*/

TEST_CASE("Tests for tVZFilter create/init/free",
          "[leaf-filters][create-init][tVZFilter]") {
    LEAF_SETUP();

    tVZFilter* filter;
    tVZFilter_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tVZFilter_init(&leaf, filter, Lowpass, 1000.0f, 0.707f));
    REQUIRE_NOTHROW(tVZFilter_free(&filter));
}

/*==============================================================================
    tVZFilterLS
==============================================================================*/

TEST_CASE("Tests for tVZFilterLS create/init/free",
          "[leaf-filters][create-init][tVZFilterLS]") {
    LEAF_SETUP();

    tVZFilterLS* filter;
    tVZFilterLS_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tVZFilterLS_init(&leaf, filter, 800.0f, 0.707f, 1.0f));
    REQUIRE_NOTHROW(tVZFilterLS_free(&filter));
}

/*==============================================================================
    tVZFilterHS
==============================================================================*/

TEST_CASE("Tests for tVZFilterHS create/init/free",
          "[leaf-filters][create-init][tVZFilterHS]") {
    LEAF_SETUP();

    tVZFilterHS* filter;
    tVZFilterHS_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tVZFilterHS_init(&leaf, filter, 3000.0f, 0.707f, 1.0f));
    REQUIRE_NOTHROW(tVZFilterHS_free(&filter));
}

/*==============================================================================
    tVZFilterBell
==============================================================================*/

TEST_CASE("Tests for tVZFilterBell create/init/free",
          "[leaf-filters][create-init][tVZFilterBell]") {
    LEAF_SETUP();

    tVZFilterBell* filter;
    tVZFilterBell_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tVZFilterBell_init(&leaf, filter, 1000.0f, 1.0f, 3.0f));
    REQUIRE_NOTHROW(tVZFilterBell_free(&filter));
}

/*==============================================================================
    tVZFilterBR
==============================================================================*/

TEST_CASE("Tests for tVZFilterBR create/init/free",
          "[leaf-filters][create-init][tVZFilterBR]") {
    LEAF_SETUP();

    tVZFilterBR* filter;
    tVZFilterBR_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tVZFilterBR_init(&leaf, filter, 1000.0f, 0.707f));
    REQUIRE_NOTHROW(tVZFilterBR_free(&filter));
}

/*==============================================================================
    tDiodeFilter
==============================================================================*/

TEST_CASE("Tests for tDiodeFilter create/init/free",
          "[leaf-filters][create-init][tDiodeFilter]") {
    LEAF_SETUP();

    tDiodeFilter* filter;
    tDiodeFilter_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tDiodeFilter_init(&leaf, filter, 800.0f, 0.7f));
    REQUIRE_NOTHROW(tDiodeFilter_free(&filter));
}

/*==============================================================================
    tLadderFilter
==============================================================================*/

TEST_CASE("Tests for tLadderFilter create/init/free",
          "[leaf-filters][create-init][tLadderFilter]") {
    LEAF_SETUP();

    tLadderFilter* filter;
    tLadderFilter_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tLadderFilter_init(&leaf, filter, 1000.0f, 0.7f));
    REQUIRE_NOTHROW(tLadderFilter_free(&filter));
}

/*==============================================================================
    tTiltFilter
==============================================================================*/

TEST_CASE("Tests for tTiltFilter create/init/free",
          "[leaf-filters][create-init][tTiltFilter]") {
    LEAF_SETUP();

    tTiltFilter* filter;
    tTiltFilter_create(&leaf.mempool, &filter);

    REQUIRE(filter != nullptr);
    REQUIRE_NOTHROW(tTiltFilter_init(&leaf, filter, 1000.0f));
    REQUIRE_NOTHROW(tTiltFilter_free(&filter));
}

TEST_CASE("Tests for tSampleReducer create/init/free",
          "[leaf-distortion][create-init][tSampleReducer]") {
    LEAF_SETUP();

    tSampleReducer* reducer;
    tSampleReducer_create(&leaf.mempool, &reducer);

    REQUIRE(reducer != nullptr);
    REQUIRE_NOTHROW(tSampleReducer_init(&leaf, reducer));
    REQUIRE_NOTHROW(tSampleReducer_free(&reducer));
}

TEST_CASE("Tests for tOversampler create/init/free",
          "[leaf-distortion][create-init][tOversampler]") {
    LEAF_SETUP();

    tOversampler* os;
    tOversampler_create(&leaf.mempool, &os);

    REQUIRE(os != nullptr);
    REQUIRE_NOTHROW(tOversampler_init(&leaf, os, 2, 1));
    REQUIRE_NOTHROW(tOversampler_free(&os));
}

TEST_CASE("Tests for tWavefolder create/init/free",
          "[leaf-distortion][create-init][tWavefolder]") {
    LEAF_SETUP();

    tWavefolder* wf;
    tWavefolder_create(&leaf.mempool, &wf);

    REQUIRE(wf != nullptr);
    REQUIRE_NOTHROW(tWavefolder_init(&leaf, wf, 0.5f, 0.5f, 1.0f));
    REQUIRE_NOTHROW(tWavefolder_free(&wf));
}

TEST_CASE("Tests for tLockhartWavefolder create/init/free",
          "[leaf-distortion][create-init][tLockhartWavefolder]") {
    LEAF_SETUP();

    tLockhartWavefolder* wf;
    tLockhartWavefolder_create(&leaf.mempool, &wf);

    REQUIRE(wf != nullptr);
    REQUIRE_NOTHROW(tLockhartWavefolder_init(&leaf, wf));
    REQUIRE_NOTHROW(tLockhartWavefolder_free(&wf));
}

TEST_CASE("Tests for tCrusher create/init/free",
          "[leaf-distortion][create-init][tCrusher]") {
    LEAF_SETUP();

    tCrusher* crusher;
    tCrusher_create(&leaf.mempool, &crusher);

    REQUIRE(crusher != nullptr);
    REQUIRE_NOTHROW(tCrusher_init(&leaf, crusher));
    REQUIRE_NOTHROW(tCrusher_free(&crusher));
}
TEST_CASE("Tests for tCompressor create/init/free",
          "[leaf-dynamics][create-init][tCompressor]") {

    LEAF_SETUP();

    tCompressor* comp;
    tCompressor_create(&leaf.mempool, &comp);

    REQUIRE(comp != nullptr);
    REQUIRE_NOTHROW(tCompressor_init(&leaf, comp));
    REQUIRE_NOTHROW(tCompressor_free(&comp));
}

TEST_CASE("Tests for tFeedbackLeveler create/init/free",
          "[leaf-dynamics][create-init][tFeedbackLeveler]") {

    LEAF_SETUP();

    tFeedbackLeveler* lvl;
    tFeedbackLeveler_create(&leaf.mempool, &lvl);

    REQUIRE(lvl != nullptr);
    REQUIRE_NOTHROW(tFeedbackLeveler_init(&leaf, lvl, 0.5f, 0.5f, 0.5f, 1));
    REQUIRE_NOTHROW(tFeedbackLeveler_free(&lvl));
}

TEST_CASE("Tests for tThreshold create/init/free",
          "[leaf-dynamics][create-init][tThreshold]") {

    LEAF_SETUP();

    tThreshold* thr;
    tThreshold_create(&leaf.mempool, &thr);

    REQUIRE(thr != nullptr);
    REQUIRE_NOTHROW(tThreshold_init(&leaf, thr, -0.5f, 0.5f));
    REQUIRE_NOTHROW(tThreshold_free(&thr));
}
TEST_CASE("Tests for tEnvelope create/init/free",
          "[leaf-envelopes][create-init][tEnvelope]") {

    LEAF_SETUP();

    tEnvelope* env;
    tEnvelope_create(&leaf.mempool, &env);

    REQUIRE(env != nullptr);
    REQUIRE_NOTHROW(tEnvelope_init(&leaf, env, 0.1f, 0.2f, 0));
    REQUIRE_NOTHROW(tEnvelope_free(&env));
}

TEST_CASE("Tests for tExpSmooth create/init/free",
          "[leaf-envelopes][create-init][tExpSmooth]") {

    LEAF_SETUP();

    tExpSmooth* sm;
    tExpSmooth_create(&leaf.mempool, &sm);

    REQUIRE(sm != nullptr);
    REQUIRE_NOTHROW(tExpSmooth_init(&leaf, sm, 0.0f, 0.5f));
    REQUIRE_NOTHROW(tExpSmooth_free(&sm));
}

TEST_CASE("Tests for tADSR create/init/free",
          "[leaf-envelopes][create-init][tADSR]") {

    LEAF_SETUP();

    tADSR* adsr;
    tADSR_create(&leaf.mempool, &adsr);

    REQUIRE(adsr != nullptr);
    REQUIRE_NOTHROW(tADSR_init(&leaf, adsr, 0.1f, 0.1f, 0.7f, 0.1f));
    REQUIRE_NOTHROW(tADSR_free(&adsr));
}

TEST_CASE("Tests for tADSRT create/init/free",
          "[leaf-envelopes][create-init][tADSRT]") {

    LEAF_SETUP();

    float dummyBuffer[32] = {0};

    tADSRT* env;
    tADSRT_create(&leaf.mempool, &env);

    REQUIRE(env != nullptr);
    REQUIRE_NOTHROW(tADSRT_init(&leaf, env, 0.1f, 0.1f, 0.7f, 0.1f, dummyBuffer, 32));
    REQUIRE_NOTHROW(tADSRT_free(&env));
}

TEST_CASE("Tests for tADSRS create/init/free",
          "[leaf-envelopes][create-init][tADSRS]") {

    LEAF_SETUP();

    tADSRS* env;
    tADSRS_create(&leaf.mempool, &env);

    REQUIRE(env != nullptr);
    REQUIRE_NOTHROW(tADSRS_init(&leaf, env, 0.1f, 0.1f, 0.7f, 0.1f));
    REQUIRE_NOTHROW(tADSRS_free(&env));
}

TEST_CASE("Tests for tRamp create/init/free",
          "[leaf-envelopes][create-init][tRamp]") {

    LEAF_SETUP();

    tRamp* r;
    tRamp_create(&leaf.mempool, &r);

    REQUIRE(r != nullptr);
    REQUIRE_NOTHROW(tRamp_init(&leaf, r, 50.0f, 64));
    REQUIRE_NOTHROW(tRamp_free(&r));
}

TEST_CASE("Tests for tRampUpDown create/init/free",
          "[leaf-envelopes][create-init][tRampUpDown]") {

    LEAF_SETUP();

    tRampUpDown* r;
    tRampUpDown_create(&leaf.mempool, &r);

    REQUIRE(r != nullptr);
    REQUIRE_NOTHROW(tRampUpDown_init(&leaf, r, 50.0f, 100.0f, 64));
    REQUIRE_NOTHROW(tRampUpDown_free(&r));
}

TEST_CASE("Tests for tSlide create/init/free",
          "[leaf-envelopes][create-init][tSlide]") {

    LEAF_SETUP();

    tSlide* s;
    tSlide_create(&leaf.mempool, &s);

    REQUIRE(s != nullptr);
    REQUIRE_NOTHROW(tSlide_init(&leaf, s, 10.0f, 20.0f));
    REQUIRE_NOTHROW(tSlide_free(&s));
}
// tCycle
TEST_CASE("tCycle create/init/free", "[leaf-filters][create-init][tCycle]")
{
    LEAF_SETUP();

    tCycle* osc = nullptr;

    REQUIRE_NOTHROW(tCycle_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tCycle_init(&leaf, osc));
    REQUIRE_NOTHROW(tCycle_free(&osc));
}

// tTriangle
TEST_CASE("tTriangle create/init/free", "[leaf-filters][create-init][tTriangle]")
{
    LEAF_SETUP();

    tTriangle* osc = nullptr;

    REQUIRE_NOTHROW(tTriangle_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tTriangle_init(&leaf, osc));
    REQUIRE_NOTHROW(tTriangle_free(&osc));
}

// tSquare
TEST_CASE("tSquare create/init/free", "[leaf-filters][create-init][tSquare]")
{
    LEAF_SETUP();

    tSquare* osc = nullptr;

    REQUIRE_NOTHROW(tSquare_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSquare_init(&leaf, osc));
    REQUIRE_NOTHROW(tSquare_free(&osc));
}

// tSawtooth
TEST_CASE("tSawtooth create/init/free", "[leaf-filters][create-init][tSawtooth]")
{
    LEAF_SETUP();

    tSawtooth* osc = nullptr;

    REQUIRE_NOTHROW(tSawtooth_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSawtooth_init(&leaf, osc));
    REQUIRE_NOTHROW(tSawtooth_free(&osc));
}

// tPBSineTriangle
TEST_CASE("tPBSineTriangle create/init/free", "[leaf-filters][create-init][tPBSineTriangle]")
{
    LEAF_SETUP();

    tPBSineTriangle* osc = nullptr;

    REQUIRE_NOTHROW(tPBSineTriangle_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBSineTriangle_init(&leaf, osc));
    REQUIRE_NOTHROW(tPBSineTriangle_free(&osc));
}

// tPBTriangle
TEST_CASE("tPBTriangle create/init/free", "[leaf-filters][create-init][tPBTriangle]")
{
    LEAF_SETUP();

    tPBTriangle* osc = nullptr;

    REQUIRE_NOTHROW(tPBTriangle_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBTriangle_init(&leaf, osc));
    REQUIRE_NOTHROW(tPBTriangle_free(&osc));
}

// tPBPulse
TEST_CASE("tPBPulse create/init/free", "[leaf-filters][create-init][tPBPulse]")
{
    LEAF_SETUP();

    tPBPulse* osc = nullptr;

    REQUIRE_NOTHROW(tPBPulse_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBPulse_init(&leaf, osc));
    REQUIRE_NOTHROW(tPBPulse_free(&osc));
}

// tPBSaw
TEST_CASE("tPBSaw create/init/free", "[leaf-filters][create-init][tPBSaw]")
{
    LEAF_SETUP();

    tPBSaw* osc = nullptr;

    REQUIRE_NOTHROW(tPBSaw_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBSaw_init(&leaf, osc));
    REQUIRE_NOTHROW(tPBSaw_free(&osc));
}

// tPBSawSquare
TEST_CASE("tPBSawSquare create/init/free", "[leaf-filters][create-init][tPBSawSquare]")
{
    LEAF_SETUP();

    tPBSawSquare* osc = nullptr;

    REQUIRE_NOTHROW(tPBSawSquare_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPBSawSquare_init(&leaf, osc));
    REQUIRE_NOTHROW(tPBSawSquare_free(&osc));
}

// tSawOS
TEST_CASE("tSawOS create/init/free", "[leaf-filters][create-init][tSawOS]")
{
    LEAF_SETUP();

    tSawOS* osc = nullptr;

    REQUIRE_NOTHROW(tSawOS_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSawOS_init(&leaf, osc, 2, 2)); // small, valid OS ratio & filter order
    REQUIRE_NOTHROW(tSawOS_free(&osc));
}

// tPhasor
TEST_CASE("tPhasor create/init/free", "[leaf-filters][create-init][tPhasor]")
{
    LEAF_SETUP();

    tPhasor* osc = nullptr;

    REQUIRE_NOTHROW(tPhasor_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPhasor_init(&leaf, osc));
    REQUIRE_NOTHROW(tPhasor_free(&osc));
}

// tNoise
TEST_CASE("tNoise create/init/free", "[leaf-filters][create-init][tNoise]")
{
    LEAF_SETUP();

    tNoise* noise = nullptr;

    REQUIRE_NOTHROW(tNoise_create(&leaf.mempool, &noise));
    REQUIRE(noise != nullptr);
    REQUIRE_NOTHROW(tNoise_init(&leaf, noise, WhiteNoise));
    REQUIRE_NOTHROW(tNoise_free(&noise));
}

// tNeuron
TEST_CASE("tNeuron create/init/free", "[leaf-filters][create-init][tNeuron]")
{
    LEAF_SETUP();

    tNeuron* neuron = nullptr;

    REQUIRE_NOTHROW(tNeuron_create(&leaf.mempool, &neuron));
    REQUIRE(neuron != nullptr);
    REQUIRE_NOTHROW(tNeuron_init(&leaf, neuron));
    REQUIRE_NOTHROW(tNeuron_free(&neuron));
}

// tMBPulse
TEST_CASE("tMBPulse create/init/free", "[leaf-filters][create-init][tMBPulse]")
{
    LEAF_SETUP();

    tMBPulse* osc = nullptr;

    REQUIRE_NOTHROW(tMBPulse_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBPulse_init(&leaf, osc));
    REQUIRE_NOTHROW(tMBPulse_free(&osc));
}

// tMBTriangle
TEST_CASE("tMBTriangle create/init/free", "[leaf-filters][create-init][tMBTriangle]")
{
    LEAF_SETUP();

    tMBTriangle* osc = nullptr;

    REQUIRE_NOTHROW(tMBTriangle_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBTriangle_init(&leaf, osc));
    REQUIRE_NOTHROW(tMBTriangle_free(&osc));
}

// tMBSineTri
TEST_CASE("tMBSineTri create/init/free", "[leaf-filters][create-init][tMBSineTri]")
{
    LEAF_SETUP();

    tMBSineTri* osc = nullptr;

    REQUIRE_NOTHROW(tMBSineTri_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBSineTri_init(&leaf, osc));
    REQUIRE_NOTHROW(tMBSineTri_free(&osc));
}

// tMBSaw
TEST_CASE("tMBSaw create/init/free", "[leaf-filters][create-init][tMBSaw]")
{
    LEAF_SETUP();

    tMBSaw* osc = nullptr;

    REQUIRE_NOTHROW(tMBSaw_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBSaw_init(&leaf, osc));
    REQUIRE_NOTHROW(tMBSaw_free(&osc));
}

// tMBSawPulse
TEST_CASE("tMBSawPulse create/init/free", "[leaf-filters][create-init][tMBSawPulse]")
{
    LEAF_SETUP();

    tMBSawPulse* osc = nullptr;

    REQUIRE_NOTHROW(tMBSawPulse_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tMBSawPulse_init(&leaf, osc));
    REQUIRE_NOTHROW(tMBSawPulse_free(&osc));
}

// tTable
TEST_CASE("tTable create/init/free", "[leaf-filters][create-init][tTable]")
{
    LEAF_SETUP();

    tTable* osc = nullptr;
    float table[16] = { 0.0f };

    REQUIRE_NOTHROW(tTable_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tTable_init(&leaf, osc, table, 16));
    REQUIRE_NOTHROW(tTable_free(&osc));
}

// tWaveTable
TEST_CASE("tWaveTable create/init/free", "[leaf-filters][create-init][tWaveTable]")
{
    LEAF_SETUP();

    tWaveTable* wt = nullptr;
    float table[16] = { 0.0f };

    REQUIRE_NOTHROW(tWaveTable_create(&leaf.mempool, &wt));
    REQUIRE(wt != nullptr);
    REQUIRE_NOTHROW(tWaveTable_init(&leaf, wt, table, 16, 20000.0f));
    REQUIRE_NOTHROW(tWaveTable_free(&wt));
}

// tWaveOsc
TEST_CASE("tWaveOsc create/init/free", "[leaf-filters][create-init][tWaveOsc]")
{
    LEAF_SETUP();

    // Two simple wavetables
    float table1[16] = { 0.0f };
    float table2[16] = { 0.0f };

    tWaveTable* wt1 = nullptr;
    tWaveTable* wt2 = nullptr;

    REQUIRE_NOTHROW(tWaveTable_create(&leaf.mempool, &wt1));
    REQUIRE_NOTHROW(tWaveTable_create(&leaf.mempool, &wt2));
    REQUIRE(wt1 != nullptr);
    REQUIRE(wt2 != nullptr);

    REQUIRE_NOTHROW(tWaveTable_init(&leaf, wt1, table1, 16, 20000.0f));
    REQUIRE_NOTHROW(tWaveTable_init(&leaf, wt2, table2, 16, 20000.0f));

    tWaveTable* tables[2] = { wt1, wt2 };
    tWaveOsc* osc = nullptr;

    REQUIRE_NOTHROW(tWaveOsc_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tWaveOsc_init(&leaf, osc, tables, 2));
    REQUIRE_NOTHROW(tWaveOsc_free(&osc));

    REQUIRE_NOTHROW(tWaveTable_free(&wt1));
    REQUIRE_NOTHROW(tWaveTable_free(&wt2));
}

// tWaveTableS
TEST_CASE("tWaveTableS create/init/free", "[leaf-filters][create-init][tWaveTableS]")
{
    LEAF_SETUP();

    tWaveTableS* wt = nullptr;
    float table[16] = { 0.0f };

    REQUIRE_NOTHROW(tWaveTableS_create(&leaf.mempool, &wt));
    REQUIRE(wt != nullptr);
    REQUIRE_NOTHROW(tWaveTableS_init(&leaf, wt, table, 16, 20000.0f));
    REQUIRE_NOTHROW(tWaveTableS_free(&wt));
}

// tWaveOscS
TEST_CASE("tWaveOscS create/init/free", "[leaf-filters][create-init][tWaveOscS]")
{
    LEAF_SETUP();

    float table1[16] = { 0.0f };
    float table2[16] = { 0.0f };

    tWaveTableS* wt1 = nullptr;
    tWaveTableS* wt2 = nullptr;

    REQUIRE_NOTHROW(tWaveTableS_create(&leaf.mempool, &wt1));
    REQUIRE_NOTHROW(tWaveTableS_create(&leaf.mempool, &wt2));
    REQUIRE(wt1 != nullptr);
    REQUIRE(wt2 != nullptr);

    REQUIRE_NOTHROW(tWaveTableS_init(&leaf, wt1, table1, 16, 20000.0f));
    REQUIRE_NOTHROW(tWaveTableS_init(&leaf, wt2, table2, 16, 20000.0f));

    tWaveTableS* tables[2] = { wt1, wt2 };
    tWaveOscS* osc = nullptr;

    REQUIRE_NOTHROW(tWaveOscS_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tWaveOscS_init(&leaf, osc, tables, 2));
    REQUIRE_NOTHROW(tWaveOscS_free(&osc));

    REQUIRE_NOTHROW(tWaveTableS_free(&wt1));
    REQUIRE_NOTHROW(tWaveTableS_free(&wt2));
}

// tIntPhasor
TEST_CASE("tIntPhasor create/init/free", "[leaf-filters][create-init][tIntPhasor]")
{
    LEAF_SETUP();

    tIntPhasor* osc = nullptr;

    REQUIRE_NOTHROW(tIntPhasor_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tIntPhasor_init(&leaf, osc));
    REQUIRE_NOTHROW(tIntPhasor_free(&osc));
}

// tSquareLFO
TEST_CASE("tSquareLFO create/init/free", "[leaf-filters][create-init][tSquareLFO]")
{
    LEAF_SETUP();

    tSquareLFO* osc = nullptr;

    REQUIRE_NOTHROW(tSquareLFO_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSquareLFO_init(&leaf, osc));
    REQUIRE_NOTHROW(tSquareLFO_free(&osc));
}

// tSawSquareLFO
TEST_CASE("tSawSquareLFO create/init/free", "[leaf-filters][create-init][tSawSquareLFO]")
{
    LEAF_SETUP();

    tSawSquareLFO* osc = nullptr;

    REQUIRE_NOTHROW(tSawSquareLFO_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSawSquareLFO_init(&leaf, osc));
    REQUIRE_NOTHROW(tSawSquareLFO_free(&osc));
}

// tTriLFO
TEST_CASE("tTriLFO create/init/free", "[leaf-filters][create-init][tTriLFO]")
{
    LEAF_SETUP();

    tTriLFO* osc = nullptr;

    REQUIRE_NOTHROW(tTriLFO_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tTriLFO_init(&leaf, osc));
    REQUIRE_NOTHROW(tTriLFO_free(&osc));
}

// tSineTriLFO
TEST_CASE("tSineTriLFO create/init/free", "[leaf-filters][create-init][tSineTriLFO]")
{
    LEAF_SETUP();

    tSineTriLFO* osc = nullptr;

    REQUIRE_NOTHROW(tSineTriLFO_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tSineTriLFO_init(&leaf, osc));
    REQUIRE_NOTHROW(tSineTriLFO_free(&osc));
}

// tDampedOscillator
TEST_CASE("tDampedOscillator create/init/free", "[leaf-filters][create-init][tDampedOscillator]")
{
    LEAF_SETUP();

    tDampedOscillator* osc = nullptr;

    REQUIRE_NOTHROW(tDampedOscillator_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tDampedOscillator_init(&leaf, osc));
    REQUIRE_NOTHROW(tDampedOscillator_free(&osc));
}

// tPlutaQuadOsc
TEST_CASE("tPlutaQuadOsc create/init/free", "[leaf-filters][create-init][tPlutaQuadOsc]")
{
    LEAF_SETUP();

    tPlutaQuadOsc* osc = nullptr;

    REQUIRE_NOTHROW(tPlutaQuadOsc_create(&leaf.mempool, &osc));
    REQUIRE(osc != nullptr);
    REQUIRE_NOTHROW(tPlutaQuadOsc_init(&leaf, osc, 4)); // small oversampling ratio
    REQUIRE_NOTHROW(tPlutaQuadOsc_free(&osc));
}


//==============================================================================
// leaf-effects create / init / free tests
//==============================================================================

// tTalkbox
TEST_CASE("tTalkbox create/init/free", "[leaf-effects][create-init][tTalkbox]")
{
    LEAF_SETUP();

    tTalkbox* fx = nullptr;
    REQUIRE_NOTHROW(tTalkbox_create(&leaf.mempool, &fx));
    REQUIRE(fx != nullptr);
    REQUIRE_NOTHROW(tTalkbox_init(&leaf, fx, 1024));
    REQUIRE_NOTHROW(tTalkbox_free(&fx));
}

// tTalkboxfloat
TEST_CASE("tTalkboxfloat create/init/free", "[leaf-effects][create-init][tTalkboxfloat]")
{
    LEAF_SETUP();

    tTalkboxfloat* fx = nullptr;
    REQUIRE_NOTHROW(tTalkboxfloat_create(&leaf.mempool, &fx));
    REQUIRE(fx != nullptr);
    REQUIRE_NOTHROW(tTalkboxfloat_init(&leaf, fx, 1024));
    REQUIRE_NOTHROW(tTalkboxfloat_free(&fx));
}

// tVocoder
TEST_CASE("tVocoder create/init/free", "[leaf-effects][create-init][tVocoder]")
{
    LEAF_SETUP();

    tVocoder* fx = nullptr;
    REQUIRE_NOTHROW(tVocoder_create(&leaf.mempool, &fx));
    REQUIRE(fx != nullptr);
    REQUIRE_NOTHROW(tVocoder_init(&leaf, fx));
    REQUIRE_NOTHROW(tVocoder_free(&fx));
}

// tRosenbergGlottalPulse
TEST_CASE("tRosenbergGlottalPulse create/init/free", "[leaf-effects][create-init][tRosenbergGlottalPulse]")
{
    LEAF_SETUP();

    tRosenbergGlottalPulse* fx = nullptr;
    REQUIRE_NOTHROW(tRosenbergGlottalPulse_create(&leaf.mempool, &fx));
    REQUIRE(fx != nullptr);
    REQUIRE_NOTHROW(tRosenbergGlottalPulse_init(&leaf, fx));
    REQUIRE_NOTHROW(tRosenbergGlottalPulse_free(&fx));
}

// tSOLAD
TEST_CASE("tSOLAD create/init/free", "[leaf-effects][create-init][tSOLAD]")
{
    LEAF_SETUP();

    tSOLAD* fx = nullptr;
    REQUIRE_NOTHROW(tSOLAD_create(&leaf.mempool, &fx));
    REQUIRE(fx != nullptr);
    REQUIRE_NOTHROW(tSOLAD_init(&leaf, fx, 4096));
    REQUIRE_NOTHROW(tSOLAD_free(&fx));
}

// tPitchShift
TEST_CASE("tPitchShift create/init/free", "[leaf-effects][create-init][tPitchShift]")
{
    LEAF_SETUP();

    constexpr int bufSize = 1024;
    float inBuffer[bufSize] = { 0.0f };

    tDualPitchDetector* pd = nullptr;
    REQUIRE_NOTHROW(tDualPitchDetector_create(&leaf.mempool, &pd));
    REQUIRE(pd != nullptr);

    REQUIRE_NOTHROW(tDualPitchDetector_init(&leaf, pd, 50.0f, 2000.0f, inBuffer, bufSize));


    tPitchShift* fx = nullptr;
    REQUIRE_NOTHROW(tPitchShift_create(&leaf.mempool, &fx));
    REQUIRE(fx != nullptr);
    REQUIRE_NOTHROW(tPitchShift_init(&leaf, fx, pd, 1024));
    REQUIRE_NOTHROW(tPitchShift_free(&fx));

    REQUIRE_NOTHROW(tDualPitchDetector_free(&pd));
}

// tSimpleRetune
TEST_CASE("tSimpleRetune create/init/free", "[leaf-effects][create-init][tSimpleRetune]")
{
    LEAF_SETUP();

    tSimpleRetune* fx = nullptr;
    REQUIRE_NOTHROW(tSimpleRetune_create(&leaf.mempool, &fx));
    REQUIRE(fx != nullptr);
    REQUIRE_NOTHROW(tSimpleRetune_init(&leaf, fx, 4, 50.0f, 2000.0f, 1024));
    REQUIRE_NOTHROW(tSimpleRetune_free(&fx));
}

// tRetune
TEST_CASE("tRetune create/init/free", "[leaf-effects][create-init][tRetune]")
{
    LEAF_SETUP();

    tRetune* fx = nullptr;
    REQUIRE_NOTHROW(tRetune_create(&leaf.mempool, &fx));
    REQUIRE(fx != nullptr);
    REQUIRE_NOTHROW(tRetune_init(&leaf, fx, 2, 50.0f, 2000.0f, 128));
    REQUIRE_NOTHROW(tRetune_free(&fx));
}

// tFormantShifter
TEST_CASE("tFormantShifter create/init/free", "[leaf-effects][create-init][tFormantShifter]")
{
    LEAF_SETUP();

    tFormantShifter* fx = nullptr;
    REQUIRE_NOTHROW(tFormantShifter_create(&leaf.mempool, &fx));
    REQUIRE(fx != nullptr);
    REQUIRE_NOTHROW(tFormantShifter_init(&leaf, fx, 32));
    REQUIRE_NOTHROW(tFormantShifter_free(&fx));
}

//==============================================================================
// leaf-electrical: create / init / free only (REQUIRE_NOTHROW)
//==============================================================================

TEST_CASE("leaf-electrical: tWDF create/init/free", "[leaf][electrical]")
{
LEAF_SETUP();
    tWDF* wdf = nullptr;


    REQUIRE_NOTHROW(tWDF_create(&leaf.mempool, &wdf));
    REQUIRE(wdf != nullptr);

    // children can be null for leaf components (Resistor/Capacitor/etc),
    // so just pass nullptrs here.
    REQUIRE_NOTHROW(tWDF_init(&leaf, wdf, Resistor, 1000.0f, nullptr, nullptr));

    REQUIRE_NOTHROW(tWDF_free(&wdf));
}
//==============================================================================
// leaf-instruments: create / init / free only (REQUIRE_NOTHROW)
//==============================================================================

/*==============================================================================
   t808Cowbell
==============================================================================*/

TEST_CASE("t808Cowbell create/init/free", "[leaf-instruments][create-init][t808Cowbell]") {
    LEAF_SETUP();

    t808Cowbell* inst = nullptr;
    REQUIRE_NOTHROW(t808Cowbell_create(&leaf.mempool, &inst));
    REQUIRE(inst != nullptr);
    REQUIRE_NOTHROW(t808Cowbell_init(&leaf, inst, /*useStick=*/1));
    REQUIRE_NOTHROW(t808Cowbell_free(&inst));
}

/*==============================================================================
   t808Hihat
==============================================================================*/

TEST_CASE("t808Hihat create/init/free", "[leaf-instruments][create-init][t808Hihat]") {
    LEAF_SETUP();

    t808Hihat* inst = nullptr;
    REQUIRE_NOTHROW(t808Hihat_create(&leaf.mempool, &inst));
    REQUIRE(inst != nullptr);
    REQUIRE_NOTHROW(t808Hihat_init(&leaf, inst));
    REQUIRE_NOTHROW(t808Hihat_free(&inst));
}

/*==============================================================================
   t808Snare
==============================================================================*/

TEST_CASE("t808Snare create/init/free", "[leaf-instruments][create-init][t808Snare]") {
    LEAF_SETUP();

    t808Snare* inst = nullptr;
    REQUIRE_NOTHROW(t808Snare_create(&leaf.mempool, &inst));
    REQUIRE(inst != nullptr);
    REQUIRE_NOTHROW(t808Snare_init(&leaf, inst));
    REQUIRE_NOTHROW(t808Snare_free(&inst));
}

/*==============================================================================
   t808SnareSmall
==============================================================================*/

TEST_CASE("t808SnareSmall create/init/free", "[leaf-instruments][create-init][t808SnareSmall]") {
    LEAF_SETUP();

    t808SnareSmall* inst = nullptr;
    REQUIRE_NOTHROW(t808SnareSmall_create(&leaf.mempool, &inst));
    REQUIRE(inst != nullptr);
    REQUIRE_NOTHROW(t808SnareSmall_init(&leaf, inst));
    REQUIRE_NOTHROW(t808SnareSmall_free(&inst));
}

/*==============================================================================
   t808Kick
==============================================================================*/

TEST_CASE("t808Kick create/init/free", "[leaf-instruments][create-init][t808Kick]") {
    LEAF_SETUP();

    t808Kick* inst = nullptr;
    REQUIRE_NOTHROW(t808Kick_create(&leaf.mempool, &inst));
    REQUIRE(inst != nullptr);
    REQUIRE_NOTHROW(t808Kick_init(&leaf, inst));
    REQUIRE_NOTHROW(t808Kick_free(&inst));
}

/*==============================================================================
   t808KickSmall
==============================================================================*/

TEST_CASE("t808KickSmall create/init/free", "[leaf-instruments][create-init][t808KickSmall]") {
    LEAF_SETUP();

    t808KickSmall* inst = nullptr;
    REQUIRE_NOTHROW(t808KickSmall_create(&leaf.mempool, &inst));
    REQUIRE(inst != nullptr);
    REQUIRE_NOTHROW(t808KickSmall_init(&leaf, inst));
    REQUIRE_NOTHROW(t808KickSmall_free(&inst));
}
//==============================================================================
// leaf-midi: create / init / free only (REQUIRE_NOTHROW)
// (Assumes your common includes + LEAF_SETUP() macro are already present.)
//==============================================================================

/*==============================================================================
   tStack
==============================================================================*/

TEST_CASE("tStack create/init/free", "[leaf-midi][create-init][tStack]")
{
    LEAF_SETUP();

    tStack* s = nullptr;
    REQUIRE_NOTHROW(tStack_create(&leaf.mempool, &s));
    REQUIRE(s != nullptr);

    REQUIRE_NOTHROW(tStack_init(&leaf, s));

    REQUIRE_NOTHROW(tStack_free(&s));
}

/*==============================================================================
   tPoly
==============================================================================*/

TEST_CASE("tPoly create/init/free", "[leaf-midi][create-init][tPoly]")
{
    LEAF_SETUP();

    tPoly* poly = nullptr;
    REQUIRE_NOTHROW(tPoly_create(&leaf.mempool, &poly));
    REQUIRE(poly != nullptr);

    REQUIRE_NOTHROW(tPoly_init(&leaf, poly, /*maxNumVoices=*/8));

    REQUIRE_NOTHROW(tPoly_free(&poly));
}

/*==============================================================================
   tSimplePoly
==============================================================================*/

TEST_CASE("tSimplePoly create/init/free", "[leaf-midi][create-init][tSimplePoly]")
{
    LEAF_SETUP();

    tSimplePoly* poly = nullptr;
    REQUIRE_NOTHROW(tSimplePoly_create(&leaf.mempool, &poly));
    REQUIRE(poly != nullptr);

    REQUIRE_NOTHROW(tSimplePoly_init(&leaf, poly, /*maxNumVoices=*/8));

    REQUIRE_NOTHROW(tSimplePoly_free(&poly));
}
