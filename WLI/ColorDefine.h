#pragma once

// CImage is BGR so defs of red and blue are switched from GetXValue() defs
#define BLU(rgb)	(LOBYTE((rgb) >> 16))
#define GRN(rgb)	(LOBYTE((rgb) >> 8))
#define RED(rgb)	(LOBYTE(rgb))
#define rBLU(rgb)	(LOBYTE(rgb) << 16)
#define rGRN(rgb)	(LOBYTE(rgb) << 8)
#define rRED(rgb)	(LOBYTE(rgb))
#define BGR(b,g,r)	RGB(b,g,r)
