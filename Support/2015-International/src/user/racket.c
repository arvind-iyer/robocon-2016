#include "racket.h"





// flags
static bool is_pneu_extended = 1; 			// true if pneumatic piston is extended
static u32 racket_pneu_start_time = 0;
static bool racket_delayed_hit_queued=0;
static u32 racket_delayed_hit_start_time=0;


static bool is_pneu_2_extended = 1;		// FOR ROBOT C
static u32 racket_pneu_2_start_time = 0;

u32 racket_hit_time=500;
u32 racket_last_hit_time=0;

/********************** here all getters and setters ends **************************/



void racket_pneumatic_set(bool data)		// 
{
	gpio_write(PNEU_GPIO, data);
	//log("pneu",is_pneu_extended);
}


void racket_pneumatic_2_set(bool data)	// for robot C
{
	gpio_write(PNEU_GPIO_DOWN, data);
	//log("pneu2",is_pneu_2_extended);
}

//Called ever 10ms to check and update racket data and redirect to starting and stopping racket at required points
void racket_update(void)
{
	if (ROBOT=='D')
		serve_update();
		
	if (racket_delayed_hit_queued && get_full_ticks() > racket_delayed_hit_start_time + RACKET_HIT_DELAY_TIME)
	{
		racket_delayed_hit_queued=0;
		racket_hit(500);
	}
	
	if(is_pneu_extended && (get_full_ticks() > racket_pneu_start_time + racket_hit_time/*RACKET_SERVE_DELAY*/))
	{
		is_pneu_extended = 0;
		racket_pneumatic_set(is_pneu_extended);
	}
	if (is_pneu_2_extended && (get_full_ticks() > racket_pneu_2_start_time + RACKET_DOWN_HIT_DELAY_TIME))
	{
		is_pneu_2_extended = 0;
		racket_pneumatic_2_set(is_pneu_2_extended);
	}
	
}

void racket_delayed_hit(int delay)
{
	if(!is_pneu_extended && !racket_delayed_hit_queued)
	{
		racket_delayed_hit_queued=1;
		racket_delayed_hit_start_time=get_full_ticks();
		RACKET_HIT_DELAY_TIME = delay;
	}
	
	
}

void racket_hit(u32 hit_time)
{
	if (!is_pneu_extended && racket_last_hit_time+700 <get_full_ticks())
	{
		racket_last_hit_time=get_full_ticks();
		is_pneu_extended = 1;
		racket_pneumatic_set(is_pneu_extended);
		racket_pneu_start_time = get_full_ticks();
		racket_hit_time=hit_time;
	}
	
}
void racket_down_hit(void)
{
	if (!is_pneu_2_extended)
	{
		is_pneu_2_extended = 1;
		racket_pneumatic_2_set(is_pneu_2_extended);
		racket_pneu_2_start_time = get_full_ticks();
		
	}
	
}
