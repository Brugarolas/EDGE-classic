//----------------------------------------------------------------------------
//  EDGE RAD Music Player (HEADER)
//----------------------------------------------------------------------------
//
//  Copyright (c) 2023 - The EDGE Team.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//----------------------------------------------------------------------------

#ifndef __RAD_PLAYER_H__
#define __RAD_PLAYER_H__

#include "i_defs.h"

#include "sound_data.h"

/* FUNCTIONS */

abstract_music_c *S_PlayRADMusic(uint8_t *data, int length, bool looping);

#endif /* __RAD_PLAYER_H__ */

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
