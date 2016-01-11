#ifndef	__BUZZER_SONG_H
#define	__BUZZER_SONG_H

#include "buzzer.h"

extern const MUSIC_NOTE_DUR STARWAR[];

#define ONCE_UPON_A_TIME_IN_A_GALAXY_FAR_FAR_AWAY	buzzer_play_song(STARWAR, 10, 0)

#endif /* __BUZZER_SONG_H */
