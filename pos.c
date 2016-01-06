//global target_x target_y
//global track_m track_c


void get_ppcor(s16 current_x,s16 current_y,s16* ppd_x,s16* ppd_y)
{
	&ppd_x = ((current_y - track_c)* track_m + current_x)/(1+track_m*track_m);
	&ppd_y = track_m * &ppd_x + track_c;
}

s32 get_distance(s16 x1, s16 y1, s16 x2, s16 y2)
{
	return sqrt((y1-y2)*(y1-y2)+(x1-x2)*(x1-x2))
}

void move(s16 target_x, s16 target_y)
{
	s32 last_plerror = 0;
	s32 last_pperror = 0;
	s32 plerror = 0;
	s32 pperror = 0;
	s16 ppd_x;
	s16 ppd_y;
	while(1)
	{
		bot_xcor = get_X();
		bot_ycor = get_Y();	
		if (get_distance(bot_xcor,bot_ycor,target_x,target_y)<300) break;
		get_ppcor(bot_xcor,bot_ycor,&ppd_x,&ppd_y);
		plerror = get_distance(ppd_x,ppd_y,target_x,target_y);
		d_plerror = plerror - last_plerror;
		pperror = get_distance(bot_xcor,bot_ycor,ppd_x,ppd_y);
		d_pperror = pperror - last_pperror;
		v_pl = Kp*plerror + Kd*d_plerror;
		v_pp = Kp*pperror + Kd*d_pperror;		
		angle = (tan(v_pl / v_pp) + tan(track_m)) % 360
	}
}

