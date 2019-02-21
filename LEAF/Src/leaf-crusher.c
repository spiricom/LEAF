/*==============================================================================

    leaf-crusher.c
    Created: 30 Nov 2018 11:56:49am
    Author:  airship

==============================================================================*/

#if _WIN32 || _WIN64

#include "..\Inc\leaf-crusher.h"

#else

#include "../Inc/leaf-crusher.h"

#endif

//==============================================================================

#define SCALAR 5000.f

void    tCrusher_init    (tCrusher* const c)
{
    c->op = 4;
    c->div = SCALAR;
    c->rnd = 0.25f;
    c->srr = 0.25f;

    c->gain = (c->div / SCALAR) * 0.7f + 0.3f;
}

void    tCrusher_free    (tCrusher* const c)
{
    leaf_free(c);
}

float   tCrusher_tick    (tCrusher* const c, float input)
{
    float sample = input;

    sample *= SCALAR; // SCALAR is 5000 by default

    sample = (int32_t) sample;

    sample /= c->div;

    sample = LEAF_bitwise_xor(sample, c->op << 23);

    sample = LEAF_clip(-1.f, sample, 1.f);

    sample = LEAF_round(sample, c->rnd);

    sample = LEAF_reduct(sample, c->srr);

    return sample * c->gain;

}

void    tCrusher_setOperation (tCrusher* const c, float op)
{
	c->op = (uint32_t) (op * 8.0f);
}

// 0.0 - 1.0
void    tCrusher_setQuality (tCrusher* const c, float val)
{
    val = LEAF_clip(0.0f, val, 1.0f);

    c->div = 0.01f + val * SCALAR;

    c->gain = (c->div / SCALAR) * 0.7f + 0.3f;
}

// what decimal to round to
void    tCrusher_setRound (tCrusher* const c, float rnd)
{
    c->rnd = fabsf(rnd);
}

void    tCrusher_setSamplingRatio (tCrusher* const c, float ratio)
{
    c->srr = ratio;
}
