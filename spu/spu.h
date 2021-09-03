//
// Based on PCSX-r code, modified for use in PCSX-HDBG
//
// PCSX-HDBG - PCSX-based hack debugger
// Copyright (C) 2019-2021  "chekwob" <chek@wobbyworks.com>
//
// GNU GPL v3 or later
//

#include <hdbg_common.h>

/***************************************************************************
                            spu.h  -  description
                             -------------------
    begin                : Wed May 15 2002
    copyright            : (C) 2002 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

void SetupTimer(void);
void RemoveTimer(void);
void SPUplayADPCMchannel(xa_decode_t *xap);
void SPUplayCDDAchannel(short *pcm, int bytes);
