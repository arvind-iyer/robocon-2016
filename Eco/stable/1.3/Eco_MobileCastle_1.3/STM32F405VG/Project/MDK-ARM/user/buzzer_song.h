#ifndef	__BUZZER_SONG_H
#define	__BUZZER_SONG_H

#include "buzzer.h"

extern const MUSIC_NOTE START_UP[];
extern const MUSIC_NOTE START_UP2[];
extern const MUSIC_NOTE SUCCESSFUL_SOUND[];
extern const MUSIC_NOTE FAIL_SOUND[];
extern const MUSIC_NOTE BIRTHDAY_SONG[];
extern const MUSIC_NOTE CLICK[];

extern const MUSIC_NOTE MARIO_BEGIN[];
extern const MUSIC_NOTE MARIO[];

extern const MUSIC_NOTE HIGH_1[];
extern const MUSIC_NOTE HIGH_2[];
extern const MUSIC_NOTE HIGH_3[];
extern const MUSIC_NOTE HIGH_4[];
extern const MUSIC_NOTE RIVER_1[];
extern const MUSIC_NOTE RIVER_2[];


#define	SUCCESSFUL_MUSIC			buzzer_play_song(SUCCESSFUL_SOUND, 100, 0)
#define	FAIL_MUSIC 						buzzer_play_song(FAIL_SOUND, 120, 100)
#define	CLICK_MUSIC						buzzer_play_song(CLICK, 10, 0)
#endif /* __BUZZER_SONG_H */