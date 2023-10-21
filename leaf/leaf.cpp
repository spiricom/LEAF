#if _WIN32 || _WIN64

#include ".\leaf.h"
#include ".\Src\leaf-math.c"
#include ".\Src\leaf-mempool.c"
#include ".\Src\leaf-tables.c"
#include ".\Src\leaf-distortion.c"
#include ".\Src\leaf-oscillators.c"
#include ".\Src\leaf-filters.c"
#include ".\Src\leaf-delay.c"
#include ".\Src\leaf-reverb.c"
#include ".\Src\leaf-effects.c"
#include ".\Src\leaf-envelopes.c"
#include ".\Src\leaf-dynamics.c"
#include ".\Src\leaf-analysis.c"
#include ".\Src\leaf-instruments.c"
#include ".\Src\leaf-midi.c"
#include ".\Src\leaf-sampling.c"
#include ".\Src\leaf-physical.c"
#include ".\Src\leaf-electrical.c"
#include ".\Src\leaf.c"

#include ".\Externals\d_fft_mayer.c"

#else

#include "./leaf.h"
#include "./Src/leaf-math.c"
#include "./Src/leaf-mempool.c"
#include "./Src/leaf-tables.c"
#include "./Src/leaf-distortion.c"
#include "./Src/leaf-dynamics.c"
#include "./Src/leaf-oscillators.c"
#include "./Src/leaf-filters.c"
#include "./Src/leaf-delay.c"
#include "./Src/leaf-reverb.c"
#include "./Src/leaf-effects.c"
#include "./Src/leaf-envelopes.c"
#include "./Src/leaf-analysis.c"
#include "./Src/leaf-instruments.c"
#include "./Src/leaf-midi.c"
#include "./Src/leaf-sampling.c"
#include "./Src/leaf-physical.c"
#include "./Src/leaf-electrical.c"
#include "./Src/leaf-vocal.c"
#include "./Src/leaf.c"


#include "./Externals/d_fft_mayer.c"

#endif
