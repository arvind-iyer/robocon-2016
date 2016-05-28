#include "buzzer_song.h"

const MUSIC_NOTE START_UP[] = {{NOTE_G,6},{NOTE_B,6},{NOTE_D,7},{NOTE_G,7},{NOTE_G,7},{NOTE_END}};
const MUSIC_NOTE SUCCESSFUL_SOUND[] = {{NOTE_C,7}, {NOTE_D,7}, {NOTE_E,7}, {NOTE_END}};
const MUSIC_NOTE FAIL_SOUND[] = {{NOTE_C, 4}, {NOTE_C, 4}, {NOTE_END}};
const MUSIC_NOTE CLICK[] = {{NOTE_G, 7}, {NOTE_END}};

const MUSIC_NOTE BIRTHDAY_SONG[] = {
	{NOTE_D,6},{NOTE_REST},{NOTE_D,6},
	{NOTE_E,6},{NOTE_E,6},{NOTE_E,6},
	{NOTE_D,6},{NOTE_D,6},{NOTE_D,6},
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},
	{NOTE_Fs,6},{NOTE_Fs,6},{NOTE_Fs,6},
	{NOTE_Fs,6},{NOTE_Fs,6},{NOTE_REST},
	
	{NOTE_D,6},{NOTE_REST},{NOTE_D,6},
	{NOTE_E,6},{NOTE_E,6},{NOTE_E,6},
	{NOTE_D,6},{NOTE_D,6},{NOTE_D,6},
	{NOTE_A,6},{NOTE_A,6},{NOTE_A,6},
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},
	{NOTE_G,6},{NOTE_G,6},{NOTE_REST},
	
	{NOTE_D,6},{NOTE_REST},{NOTE_D,6},
	{NOTE_D,7},{NOTE_D,7},{NOTE_D,7},	
	{NOTE_B,6},{NOTE_B,6},{NOTE_B,6},	
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},	
	{NOTE_Fs,6},{NOTE_Fs,6},{NOTE_Fs,6},
	{NOTE_E,6},{NOTE_E,6},{NOTE_E,6},
	
	{NOTE_C,7},{NOTE_REST},{NOTE_C,7},
	{NOTE_B,6},{NOTE_B,6},{NOTE_B,6},	
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},	
	{NOTE_A,6},{NOTE_A,6},{NOTE_A,6},	
	{NOTE_G,6},{NOTE_G,6},{NOTE_G,6},	
	{NOTE_G,6},{NOTE_G,6},{NOTE_END}
};