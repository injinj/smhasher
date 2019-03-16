#pragma once

#include "Types.h"

void BulkSpeedTest ( pfHash hash, uint32_t seed, bool plot );
void TinySpeedTest ( pfHash hash, int hashsize, int keysize, uint32_t seed, double & outCycles, bool plot, bool speedseed );

//-----------------------------------------------------------------------------
