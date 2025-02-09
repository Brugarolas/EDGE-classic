//------------------------------------------------------------------------
//  SOUND Definitions
//------------------------------------------------------------------------
//
//  DEH_EDGE  Copyright (C) 2004-2024 The EDGE Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License (in COPYING.txt) for more details.
//
//------------------------------------------------------------------------
//
//  DEH_EDGE is based on:
//
//  +  DeHackEd source code, by Greg Lewis.
//  -  DOOM source code (C) 1993-1996 id Software, Inc.
//  -  Linux DOOM Hack Editor, by Sam Lantinga.
//  -  PrBoom's DEH/BEX code, by Ty Halderman, TeamTNT.
//
//------------------------------------------------------------------------

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "deh_i_defs.h"
#include "deh_edge.h"

#include "deh_buffer.h"
#include "deh_patch.h"
#include "deh_sounds.h"
#include "deh_system.h"
#include "deh_util.h"
#include "deh_wad.h"

namespace Deh_Edge
{

//
// SoundFX struct.
//
struct sfxinfo_t
{
    // up to 6-character name
    char name[8];

    // Sfx singularity (only one at a time), 0 = normal
    int singularity;

    // Sfx priority (lower is MORE important)
    int priority;
};

//------------------------------------------------------------------------
//
// Information about all the sfx
//

const sfxinfo_t S_sfx_orig[NUMSFX_COMPAT] = {
    // S_sfx[0] needs to be a dummy for odd reasons.
    {"", 0, 127},

    {"pistol", 0, 64},
    {"shotgn", 0, 64},
    {"sgcock", 0, 64},
    {"dshtgn", 0, 64},
    {"dbopn", 0, 64},
    {"dbcls", 0, 64},
    {"dbload", 0, 64},
    {"plasma", 0, 64},
    {"bfg", 0, 64},
    {"sawup", 2, 64},
    {"sawidl", 2, 118},
    {"sawful", 2, 64},
    {"sawhit", 2, 64},
    {"rlaunc", 0, 64},
    {"rxplod", 0, 70},
    {"firsht", 0, 70},
    {"firxpl", 0, 70},
    {"pstart", 18, 100},
    {"pstop", 18, 100},
    {"doropn", 0, 100},
    {"dorcls", 0, 100},
    {"stnmov", 18, 119},
    {"swtchn", 0, 78},
    {"swtchx", 0, 78},
    {"plpain", 0, 96},
    {"dmpain", 0, 96},
    {"popain", 0, 96},
    {"vipain", 0, 96},
    {"mnpain", 0, 96},
    {"pepain", 0, 96},
    {"slop", 0, 78},
    {"itemup", 20, 78},
    {"wpnup", 21, 78},
    {"oof", 0, 96},
    {"telept", 0, 32},
    {"posit1", 3, 98},
    {"posit2", 3, 98},
    {"posit3", 3, 98},
    {"bgsit1", 4, 98},
    {"bgsit2", 4, 98},
    {"sgtsit", 5, 98},
    {"cacsit", 6, 98},
    {"brssit", 7, 94},
    {"cybsit", 8, 92},
    {"spisit", 9, 90},
    {"bspsit", 10, 90},
    {"kntsit", 11, 90},
    {"vilsit", 12, 90},
    {"mansit", 13, 90},
    {"pesit", 14, 90},
    {"sklatk", 0, 70},
    {"sgtatk", 0, 70},
    {"skepch", 0, 70},
    {"vilatk", 0, 70},
    {"claw", 0, 70},
    {"skeswg", 0, 70},
    {"pldeth", 0, 32},
    {"pdiehi", 0, 32},
    {"podth1", 0, 70},
    {"podth2", 0, 70},
    {"podth3", 0, 70},
    {"bgdth1", 0, 70},
    {"bgdth2", 0, 70},
    {"sgtdth", 0, 70},
    {"cacdth", 0, 70},
    {"skldth", 0, 70},
    {"brsdth", 0, 32},
    {"cybdth", 0, 32},
    {"spidth", 0, 32},
    {"bspdth", 0, 32},
    {"vildth", 0, 32},
    {"kntdth", 0, 32},
    {"pedth", 0, 32},
    {"skedth", 0, 32},
    {"posact", 3, 120},
    {"bgact", 4, 120},
    {"dmact", 15, 120},
    {"bspact", 10, 100},
    {"bspwlk", 16, 100},
    {"vilact", 12, 100},
    {"noway", 0, 78},
    {"barexp", 0, 60},
    {"punch", 0, 64},
    {"hoof", 0, 70},
    {"metal", 0, 70},
    {"chgun", 0, 64},
    {"tink", 0, 60},
    {"bdopn", 0, 100},
    {"bdcls", 0, 100},
    {"itmbk", 0, 100},
    {"flame", 0, 32},
    {"flamst", 0, 32},
    {"getpow", 0, 60},
    {"bospit", 0, 70},
    {"boscub", 0, 70},
    {"bossit", 0, 70},
    {"bospn", 0, 70},
    {"bosdth", 0, 70},
    {"manatk", 0, 70},
    {"mandth", 0, 70},
    {"sssit", 0, 70},
    {"ssdth", 0, 70},
    {"keenpn", 0, 70},
    {"keendt", 0, 70},
    {"skeact", 0, 70},
    {"skesit", 0, 70},
    {"skeatk", 0, 70},
    {"radio", 0, 60},

    // MBF sounds...
    {"dgsit", 0, 98},
    {"dgatk", 0, 70},
    {"dgact", 0, 120},
    {"dgdth", 0, 70},
    {"dgpain", 0, 96},

    // other source ports...
    {"secret", 0, 60},
    {"gibdth", 0, 60},
    {"scrsht", 0, 0},
};

// DEHEXTRA : 500 to 699
const sfxinfo_t S_sfx_dehextra[200] = {
    {"fre000", 0, 127}, {"fre001", 0, 127}, {"fre002", 0, 127}, {"fre003", 0, 127}, {"fre004", 0, 127},
    {"fre005", 0, 127}, {"fre006", 0, 127}, {"fre007", 0, 127}, {"fre008", 0, 127}, {"fre009", 0, 127},
    {"fre010", 0, 127}, {"fre011", 0, 127}, {"fre012", 0, 127}, {"fre013", 0, 127}, {"fre014", 0, 127},
    {"fre015", 0, 127}, {"fre016", 0, 127}, {"fre017", 0, 127}, {"fre018", 0, 127}, {"fre019", 0, 127},
    {"fre020", 0, 127}, {"fre021", 0, 127}, {"fre022", 0, 127}, {"fre023", 0, 127}, {"fre024", 0, 127},
    {"fre025", 0, 127}, {"fre026", 0, 127}, {"fre027", 0, 127}, {"fre028", 0, 127}, {"fre029", 0, 127},

    {"fre030", 0, 127}, {"fre031", 0, 127}, {"fre032", 0, 127}, {"fre033", 0, 127}, {"fre034", 0, 127},
    {"fre035", 0, 127}, {"fre036", 0, 127}, {"fre037", 0, 127}, {"fre038", 0, 127}, {"fre039", 0, 127},
    {"fre040", 0, 127}, {"fre041", 0, 127}, {"fre042", 0, 127}, {"fre043", 0, 127}, {"fre044", 0, 127},
    {"fre045", 0, 127}, {"fre046", 0, 127}, {"fre047", 0, 127}, {"fre048", 0, 127}, {"fre049", 0, 127},
    {"fre050", 0, 127}, {"fre051", 0, 127}, {"fre052", 0, 127}, {"fre053", 0, 127}, {"fre054", 0, 127},
    {"fre055", 0, 127}, {"fre056", 0, 127}, {"fre057", 0, 127}, {"fre058", 0, 127}, {"fre059", 0, 127},

    {"fre060", 0, 127}, {"fre061", 0, 127}, {"fre062", 0, 127}, {"fre063", 0, 127}, {"fre064", 0, 127},
    {"fre065", 0, 127}, {"fre066", 0, 127}, {"fre067", 0, 127}, {"fre068", 0, 127}, {"fre069", 0, 127},
    {"fre070", 0, 127}, {"fre071", 0, 127}, {"fre072", 0, 127}, {"fre073", 0, 127}, {"fre074", 0, 127},
    {"fre075", 0, 127}, {"fre076", 0, 127}, {"fre077", 0, 127}, {"fre078", 0, 127}, {"fre079", 0, 127},
    {"fre080", 0, 127}, {"fre081", 0, 127}, {"fre082", 0, 127}, {"fre083", 0, 127}, {"fre084", 0, 127},
    {"fre085", 0, 127}, {"fre086", 0, 127}, {"fre087", 0, 127}, {"fre088", 0, 127}, {"fre089", 0, 127},

    {"fre090", 0, 127}, {"fre091", 0, 127}, {"fre092", 0, 127}, {"fre093", 0, 127}, {"fre094", 0, 127},
    {"fre095", 0, 127}, {"fre096", 0, 127}, {"fre097", 0, 127}, {"fre098", 0, 127}, {"fre099", 0, 127},
    {"fre100", 0, 127}, {"fre101", 0, 127}, {"fre102", 0, 127}, {"fre103", 0, 127}, {"fre104", 0, 127},
    {"fre105", 0, 127}, {"fre106", 0, 127}, {"fre107", 0, 127}, {"fre108", 0, 127}, {"fre109", 0, 127},
    {"fre110", 0, 127}, {"fre111", 0, 127}, {"fre112", 0, 127}, {"fre113", 0, 127}, {"fre114", 0, 127},
    {"fre115", 0, 127}, {"fre116", 0, 127}, {"fre117", 0, 127}, {"fre118", 0, 127}, {"fre119", 0, 127},
    {"fre120", 0, 127}, {"fre121", 0, 127}, {"fre122", 0, 127},

    {"fre123", 0, 127}, {"fre124", 0, 127}, {"fre125", 0, 127}, {"fre126", 0, 127}, {"fre127", 0, 127},
    {"fre128", 0, 127}, {"fre129", 0, 127}, {"fre130", 0, 127}, {"fre131", 0, 127}, {"fre132", 0, 127},
    {"fre133", 0, 127}, {"fre134", 0, 127}, {"fre135", 0, 127}, {"fre136", 0, 127}, {"fre137", 0, 127},
    {"fre138", 0, 127}, {"fre139", 0, 127}, {"fre140", 0, 127}, {"fre141", 0, 127}, {"fre142", 0, 127},
    {"fre143", 0, 127}, {"fre144", 0, 127}, {"fre145", 0, 127}, {"fre146", 0, 127}, {"fre147", 0, 127},
    {"fre148", 0, 127}, {"fre149", 0, 127},

    {"fre150", 0, 127}, {"fre151", 0, 127}, {"fre152", 0, 127}, {"fre153", 0, 127}, {"fre154", 0, 127},
    {"fre155", 0, 127}, {"fre156", 0, 127}, {"fre157", 0, 127}, {"fre158", 0, 127}, {"fre159", 0, 127},
    {"fre160", 0, 127}, {"fre161", 0, 127}, {"fre162", 0, 127}, {"fre163", 0, 127}, {"fre164", 0, 127},
    {"fre165", 0, 127}, {"fre166", 0, 127}, {"fre167", 0, 127}, {"fre168", 0, 127}, {"fre169", 0, 127},
    {"fre170", 0, 127}, {"fre171", 0, 127}, {"fre172", 0, 127}, {"fre173", 0, 127}, {"fre174", 0, 127},
    {"fre175", 0, 127}, {"fre176", 0, 127}, {"fre177", 0, 127}, {"fre178", 0, 127}, {"fre179", 0, 127},

    {"fre180", 0, 127}, {"fre181", 0, 127}, {"fre182", 0, 127}, {"fre183", 0, 127}, {"fre184", 0, 127},
    {"fre185", 0, 127}, {"fre186", 0, 127}, {"fre187", 0, 127}, {"fre188", 0, 127}, {"fre189", 0, 127},
    {"fre190", 0, 127}, {"fre191", 0, 127}, {"fre192", 0, 127}, {"fre193", 0, 127}, {"fre194", 0, 127},
    {"fre195", 0, 127}, {"fre196", 0, 127}, {"fre197", 0, 127}, {"fre198", 0, 127}, {"fre199", 0, 127}};

//
// all the modified entries.
// NOTE: some pointers may be NULL!
//
std::vector<sfxinfo_t *> S_sfx;

//------------------------------------------------------------------------

namespace Sounds
{
void BeginLump();
void FinishLump();

void             MarkSound(int s_num);
const sfxinfo_t *GetOriginalSFX(int num);
const char      *GetEdgeSfxName(int sound_id);
void             WriteSound(int s_num);
} // namespace Sounds

void Sounds::Init()
{
    S_sfx.clear();
}

void Sounds::Shutdown()
{
    for (size_t i = 0; i < S_sfx.size(); i++)
        if (S_sfx[i] != NULL)
            delete S_sfx[i];

    S_sfx.clear();
}

void Sounds::BeginLump()
{
    WAD::NewLump(DDF_SFX);

    WAD::Printf("<SOUNDS>\n\n");
}

void Sounds::FinishLump()
{
    WAD::Printf("\n");
}

const sfxinfo_t *Sounds::GetOriginalSFX(int num)
{
    if (0 <= num && num < NUMSFX_COMPAT)
        return &S_sfx_orig[num];

    if (sfx_fre000 <= num && num <= sfx_fre199)
        return &S_sfx_dehextra[num - sfx_fre000];

    // no actual original, return the dummy template
    return &S_sfx_orig[0];
}

void Sounds::MarkSound(int num)
{
    // can happen since the binary patches contain the dummy sound
    if (num == sfx_None)
        return;

    // fill any missing slots with NULLs, including the one we want
    while ((int)S_sfx.size() < num + 1)
    {
        S_sfx.push_back(NULL);
    }

    // already have a modified entry?
    if (S_sfx[num] != NULL)
        return;

    sfxinfo_t *entry = new sfxinfo_t;
    S_sfx[num]       = entry;

    // copy the original info
    const sfxinfo_t *orig = GetOriginalSFX(num);

    strcpy(entry->name, orig->name);

    entry->singularity = orig->singularity;
    entry->priority    = orig->priority;
}

void Sounds::AlterSound(int new_val)
{
    int         s_num     = Patch::active_obj;
    const char *deh_field = Patch::line_buf;

    assert(s_num >= 0);

    if (StrCaseCmpPartial(deh_field, "Zero") == 0 || StrCaseCmpPartial(deh_field, "Neg. One") == 0)
        return;

    if (StrCaseCmp(deh_field, "Zero/One") == 0) // singularity, ignored
        return;

    if (StrCaseCmp(deh_field, "Offset") == 0)
    {
        I_Debugf("Dehacked: Warning - Line %d: raw sound Offset not supported.\n", Patch::line_num);
        return;
    }

    if (StrCaseCmp(deh_field, "Value") == 0) // priority
    {
        if (new_val < 0)
        {
            I_Debugf("Dehacked: Warning - Line %d: bad sound priority value: %d.\n", Patch::line_num, new_val);
            new_val = 0;
        }

        MarkSound(s_num);

        S_sfx[s_num]->priority = new_val;
        return;
    }

    I_Debugf("Dehacked: Warning - UNKNOWN SOUND FIELD: %s\n", deh_field);
}

const char *Sounds::GetEdgeSfxName(int sound_id)
{
    if (sound_id == sfx_None)
        return NULL;

    switch (sound_id)
    {
    // EDGE uses different names for the DOG sounds
    case sfx_dgsit:
        return "DOG_SIGHT";
    case sfx_dgatk:
        return "DOG_BITE";
    case sfx_dgact:
        return "DOG_LOOK";
    case sfx_dgdth:
        return "DOG_DIE";
    case sfx_dgpain:
        return "DOG_PAIN";

    default:
        break;
    }

    const sfxinfo_t *orig = GetOriginalSFX(sound_id);

    if (orig->name[0] != 0)
        return StrUpper(orig->name);

    // we get here for sounds with no original name (only possible
    // for DSDehacked / MBF21).  check if modified name is empty too.

    if (sound_id >= (int)S_sfx.size())
        return NULL;

    const sfxinfo_t *mod = S_sfx[sound_id];
    if (mod == NULL || mod->name[0] == 0)
        return NULL;

    // create a suitable name
    static char name_buf[64];
    snprintf(name_buf, sizeof(name_buf), "BEX_%d", sound_id);
    return name_buf;
}

const char *Sounds::GetSound(int sound_id)
{
    if (sound_id == sfx_None)
        return "NULL";

    // handle random sounds
    switch (sound_id)
    {
    case sfx_podth1:
    case sfx_podth2:
    case sfx_podth3:
        return "PODTH?";

    case sfx_posit1:
    case sfx_posit2:
    case sfx_posit3:
        return "POSIT?";

    case sfx_bgdth1:
    case sfx_bgdth2:
        return "BGDTH?";

    case sfx_bgsit1:
    case sfx_bgsit2:
        return "BGSIT?";

    default:
        break;
    }

    // if something uses DEHEXTRA sounds (+ a few others), ensure we
    // generate DDFSFX entries for them.
    if ((sfx_fre000 <= sound_id && sound_id <= sfx_fre199) || (sound_id == sfx_gibdth) || (sound_id == sfx_scrsht))
    {
        MarkSound(sound_id);
    }

    const char *name = GetEdgeSfxName(sound_id);
    if (name == NULL)
        return "NULL";

    return name;
}

void Sounds::WriteSound(int sound_id)
{
    const sfxinfo_t *sound = S_sfx[sound_id];

    // in the unlikely event the sound did not get a name (which is
    // only possible with DSDehacked / MBF21), just skip it.
    const char *lump = sound->name;

    if (lump[0] == 0)
        return;

    const char *ddf_name = GetEdgeSfxName(sound_id);
    if (ddf_name == NULL)
        I_Error("Dehacked: Error - No DDF name for sound %d ??\n", sound_id);

    WAD::Printf("[%s]\n", ddf_name);

    // only one sound has a `link` field in standard DOOM.
    // we emulate that here.
    if (sound_id == sfx_chgun)
    {
        const sfxinfo_t *link = &S_sfx_orig[sfx_pistol];

        if (sfx_pistol < (int)S_sfx.size() && S_sfx[sfx_pistol] != NULL)
            link = S_sfx[sfx_pistol];

        if (link->name[0] != 0)
            lump = link->name;
    }

    WAD::Printf("LUMP_NAME = \"DS%s\";\n", StrUpper(lump));
    WAD::Printf("PRIORITY = %d;\n", sound->priority);

    if (sound->singularity != 0)
        WAD::Printf("SINGULAR = %d;\n", sound->singularity);

    if (sound_id == sfx_stnmov)
        WAD::Printf("LOOP = TRUE;\n");

    WAD::Printf("\n");
}

void Sounds::ConvertSFX(void)
{
    if (all_mode)
    {
        for (int i = 1; i < NUMSFX_COMPAT; i++)
            MarkSound(i);

        /* this is debatable....
        for (int i = sfx_fre000 ; i <= sfx_fre199 ; i++)
            MarkSound(i);
        */
    }

    bool got_one = false;

    for (int i = 1; i < (int)S_sfx.size(); i++)
    {
        if (S_sfx[i] == NULL)
            continue;

        if (!got_one)
        {
            BeginLump();
            got_one = true;
        }

        WriteSound(i);
    }

    if (got_one)
        FinishLump();
}

bool Sounds::ReplaceSound(const char *before, const char *after)
{
    assert(strlen(before) <= 6);
    assert(strlen(after) <= 6);

    for (int i = 1; i < NUMSFX_DEHEXTRA; i++)
    {
        const sfxinfo_t *orig = GetOriginalSFX(i);

        if (orig->name[0] == 0)
            continue;

        if (StrCaseCmp(orig->name, before) != 0)
            continue;

        MarkSound(i);

        strcpy(S_sfx[i]->name, after);
        return true;
    }

    return false;
}

void Sounds::AlterBexSound(const char *new_val)
{
    const char *old_val = Patch::line_buf;

    if (strlen(new_val) < 1 || strlen(new_val) > 6)
    {
        I_Debugf("Dehacked: Warning - Bad length for sound name '%s'.\n", new_val);
        return;
    }

    // for DSDehacked, support a numeric target
    if (isdigit(old_val[0]))
    {
        int num = atoi(old_val);
        if (num < 1 || num > 32767)
        {
            I_Debugf("Dehacked: Warning - Line %d: illegal sound number '%s'.\n", Patch::line_num, old_val);
        }
        else
        {
            MarkSound(num);
            strcpy(S_sfx[num]->name, new_val);
        }
        return;
    }

    if (strlen(old_val) < 1 || strlen(old_val) > 6)
    {
        I_Debugf("Dehacked: Warning - Bad length for sound name '%s'.\n", old_val);
        return;
    }

    if (!ReplaceSound(old_val, new_val))
        I_Debugf("Dehacked: Warning - Line %d: unknown sound name '%s'.\n", Patch::line_num, old_val);
}

} // namespace Deh_Edge
