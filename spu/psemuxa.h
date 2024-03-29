//
// Based on PCSX-r code, modified for use in PCSX-HDBG
//
// PCSX-HDBG - PCSX-based hack debugger
// Copyright (C) 2019-2021  "chekwob" <chek@wobbyworks.com>
//
// GNU GPL v3 or later
//

#include <hdbg_common.h>

//============================================
//=== Audio XA decoding
//=== Kazzuya
//============================================

#ifndef DECODEXA_H
#define DECODEXA_H

typedef struct
{
	int	y0, y1;
} ADPCM_Decode_t;

typedef struct
{                                                                   
	int				freq;
	int				nbits;
	int				stereo;
	int				nsamples;
	ADPCM_Decode_t	left, right;
	short			pcm[16384];
} xa_decode_t;

long xa_decode_sector( xa_decode_t *xdp,
					   unsigned char *sectorp,
					   int is_first_sector );

#endif
