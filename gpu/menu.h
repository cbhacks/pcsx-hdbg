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
                           menu.h  -  description
                             -------------------
    begin                : Sun Mar 08 2009
    copyright            : (C) 1999-2009 by Pete Bernert
    web                  : www.pbernert.com   
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

//*************************************************************************// 
// History of changes:
//
// 2009/03/08 - Pete  
// - generic cleanup for the Peops release
//
//*************************************************************************// 

#ifndef _GL_MENU_H_
#define _GL_MENU_H_

void DisplayText(void);
void HideText(void);
void KillDisplayLists(void);
void MakeDisplayLists(void);
void BuildDispMenu(int iInc);
void SwitchDispMenu(int iStep);
void CreatePic(unsigned char * pMem);
void DisplayPic(void);
void DestroyPic(void);
void ShowGunCursor(void);

#endif // _GL_MENU_H_
