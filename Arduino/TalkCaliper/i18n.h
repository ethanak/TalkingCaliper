#ifndef XI18N_H
#define XI18N_H

// uncomment only one language definition
// or leave commented for English

// odkomentuj dla języka polskiego
//#define VOICE_IS_PL

// descomentar para español
//#define VOICE_IS_ES


// uncomment for German
//#define VOICE_IS_DE

#if defined(VOICE_IS_PL)
#define VOXDEFS "voxdefs_pl.h"
#define VOXH "vox_pl.h"

#elif defined(VOICE_IS_ES)
#define VOXDEFS "voxdefs_es.h"
#define VOXH "vox_es.h"

#elif defined(VOICE_IS_DE)
#define VOXDEFS "voxdefs_de.h"
#define VOXH "vox_de.h"

#else
#define VOXDEFS "voxdefs_en.h"
#define VOXH "vox_en.h"
#endif
#endif
