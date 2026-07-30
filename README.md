# LEAF — fork contributions (Sean Xue)

This is a fork of [spiricom/LEAF](https://github.com/spiricom/LEAF), the Lightweight Embedded
Audio Framework out of Princeton's New Instrument Research Lab. Upstream's own README (library
overview, naming conventions, embedded/JUCE setup) is preserved as
[README-upstream.md](README-upstream.md) and still applies — this file only covers what I changed
in the fork.

Fork: [anytroops/LEAF](https://github.com/anytroops/LEAF) · Work done May–July 2025.

There are two separate workstreams:

1. **API modernization** — replacing LEAF's hidden-pointer typedef convention with explicit
   pointer types across the whole library. *Merged into `master`.*
2. **Usage documentation** — adding runnable Doxygen examples to the public headers, 66 objects
   so far. *Lives on the `seanRenaming` branch.*

---

## 1. API modernization: explicit pointers instead of hidden ones

### The problem

Every LEAF object used a two-typedef pattern where the public type name was secretly a pointer:

```c
typedef struct _tCompressor { ... } _tCompressor;
typedef _tCompressor* tCompressor;          // tCompressor IS a pointer

void   tCompressor_init (tCompressor* const comp, LEAF* const leaf);   // really tCompressor**
Lfloat tCompressor_tick (tCompressor const c, Lfloat in);              // really tCompressor*
```

This reads badly in two directions. `tCompressor* const` in `_init` looks like "pointer to object"
but is actually pointer-to-pointer, while `tCompressor const c` in `_tick` looks like a by-value
struct but is actually the pointer. Callers can't tell the ownership story from the signature, IDE
navigation lands on the wrong type, and the `_t`-prefixed struct names collide with the reserved
identifier convention.

### The change

Collapse to a single struct type and make every level of indirection visible at the call site:

```c
typedef struct tCompressor { ... } tCompressor;   // a struct, and it says so

void   tCompressor_init (tCompressor** const comp, LEAF* const leaf);  // takes the handle to fill
Lfloat tCompressor_tick (tCompressor*  const c,    Lfloat in);         // takes the object
```

Rules applied uniformly:

| Function class | Before | After |
| --- | --- | --- |
| `_init`, `_initToPool`, `_free` | `tFoo* const` | `tFoo** const` |
| `_tick`, setters, getters | `tFoo const` | `tFoo* const` |
| struct tag / typedef | `_tFoo` + `tFoo` alias | `tFoo` only |

Call sites change from `tFoo x; tFoo_init(&x, ...)` to `tFoo* x = NULL; tFoo_init(&x, ...)` —
the null-initialized handle is now explicit, which is also why every example in section 2 starts
that way.

### Scope

- **33 files** across `leaf/Inc/` and `leaf/Src/` — every module except the pure-header ones
  (`leaf-math`, `leaf-tables`, `leaf-mempool`).
- ~4,400 lines added / ~5,100 removed; the net reduction is the deleted alias typedefs plus a
  stale `leaf-midi copy.h` that was carrying a divergent duplicate of the MIDI API.
- Commits: `239db70`, `93d1e9a` (bulk conversion), `d53a4c1` (follow-ups).
- Cleaned up by Davis Polito in `d2aa5aa` and merged; `master` now carries the new API.

`d53a4c1` also fixed real bugs the conversion surfaced — several dereferences had been written as
`&*ptr`, a no-op that silently kept the old pointer level and would have handed `_free` and the
zero-crossing period math the wrong address once the types changed:

```c
- tTriLFO* c = &*cy;    // still the handle, not the object
+ tTriLFO* c = *cy;
```

---

## 2. Usage documentation for the public headers

Upstream headers document *signatures* — every function has an `@fn` / `@brief` / `@param` block,
so the generated Doxygen tells you what arguments exist but never how the object is meant to be
driven. Nothing showed the init → tick → free lifecycle, the mempool handshake, or which setters
matter in an audio loop.

I added a prose explanation of the algorithm plus a compiling `@code{.c}` example to each object's
`@defgroup` block, following the object's actual usage pattern:

```c
/*!
 @defgroup tzerocrossingcounter tZeroCrossingCounter
 @ingroup analysis
 @brief Count the amount of zero crossings within a window of the input audio data

The zero crossing counter keeps a circular buffer of the most recent samples, up to 'maxWindowSize'.
On each tick it:
1. Inserts the new sample into the buffer, overwriting the current oldest sample
2. Checks if the new sample and what it replaced have zero crossing
3. Updates internal counter
4. Returns the count normalized by window size, from [0.0 to 1.0]
Higher counts indicate rapid sign changes
Lower counts indicate steady signals

Example
@code{.c}
//initialize
tZeroCrossingCounter* zc = NULL;
tZeroCrossingCounter_init(&zc,
                          1024,     //max window size
                          leaf);

//audio loop
float r = tZeroCrossingCounter_tick(zc, inputSample);   //fraction of zero crossings in last window

//changing window size
tZeroCrossingCounter_setWindowSize(zc, 512);

//when done
tZeroCrossingCounter_free(&zc);
@endcode
*/
```

Each example shows the null handle, the constructor with every argument annotated inline, the
per-sample or per-block call in context, the setters worth reaching for, and the teardown.
Where an object has a block-processing path (`tEnvPD`) or multiple tick variants, both are shown.

### Coverage — 66 objects across 12 modules

| Module | Objects documented |
| --- | --- |
| `leaf-filters.h` (16) | tAllpassSO, tThiranAllpassSOCascade, tOnePole, tTwoPole, tOneZero, tTwoZero, tPoleZero, tBiQuad, tSVF, tEfficientSVF, tHighpass, tButterworth, tFIR, tMedianFilter, tVZFilter, tDiodeFilter |
| `leaf-effects.h` (9) | tTalkbox, tTalkboxLfloat, tVocoder, tRosenbergGlottalPulse, tSOLAD, tPitchShift, tSimpleRetune, tRetune, tFormantShifter |
| `leaf-analysis.h` (8) | tEnvelopeFollower, tZeroCrossingCounter, tPowerFollower, tEnvPD, tAttackDetection, tSNAC, tPeriodDetection, tDualPitchDetector |
| `leaf-oscillators.h` (8) | tCycle, tSawtooth, tPBTriangle, tPBPulse, tPBSaw, tPhasor, tNoise, tNeuron |
| `leaf-delay.h` (6) | tDelay, tLinearDelay, tHermiteDelay, tAllpassDelay, tTapeDelay, tRingBuffer |
| `leaf-instruments.h` (6) | t808Cowbell, t808Hihat, t808Snare, t808SnareSmall, t808Kick, t808KickSmall |
| `leaf-distortion.h` (5) | tSampleReducer, tOversampler, tWavefolder, tLockhartWavefolder, tCrusher |
| `leaf-dynamics.h` (3) | tCompressor, tFeedbackLeveler, tThreshold |
| `leaf-midi.h` (2) | tStack, tSimplePoly |
| `leaf-envelopes.h` (1) | tEnvelope |
| `leaf-electrical.h` (1) | tWDF |
| `leaf-physical.h` (1) | tPluck *(uncommitted in the working tree)* |

Roughly 2,000 lines of documentation across 11 headers, one commit per module
(`9706116` … `f726a36`, July 15–29 2025).

**Not yet covered:** `leaf-reverb.h`, `leaf-sampling.h`, `leaf-vocal.h`, and the rest of
`leaf-physical.h` (the string/brass/bowed models) — plus `leaf-math`, `leaf-tables`, `leaf-global`
and `leaf-mempool`, which are utility headers rather than object modules.

---

## Branch map

| Branch | Contents |
| --- | --- |
| `master` | Upstream + the merged API modernization. Head `d53a4c1`. |
| `seanRenaming` | `master` + the 10 documentation commits. Head `f726a36`. **This is the branch with the full body of work.** |
| `Sean_merge` | Not mine — upstream fixes (warning cleanup, mempool free guards, `allNotesOff` on tSimplePoly) merged forward by Davis Polito. |

Documentation work is unmerged, so `seanRenaming` is what to read or open a PR from.

## Building and regenerating docs

```bash
cmake -B build && cmake --build build
```

```bash
doxygen Doxyfile
```

Doxygen output lands in `docs/`; `EXAMPLE_PATH` is `Examples/`, and the inline `@code{.c}` blocks
above render into each object's group page.

## Related

[anytroops/LEAF-Example1](https://github.com/anytroops/LEAF-Example1) — a JUCE/Pamplejuce plugin
built against this fork, used to verify the converted API works from a real host. `c8ec183` gets a
LEAF oscillator producing sound through the plugin processor.
