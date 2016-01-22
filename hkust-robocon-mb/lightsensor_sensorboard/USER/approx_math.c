#include "approx_math.h"

s16 tan_val[91] = {		//scale 100
0,2,3,5,7,9,11,12,14,16,18,19,21,23,25,27,29,31,32,34,36,38,40,42,
45,47,49,51,53,55,58,60,62,65,67,70,73,75,78,81,84,87,90,93,97,100,104,107,
111,115,119,123,128,133,138,143,148,154,160,166,173,180,188,196,205,214,225,236,248,261,275,290,
308,327,349,373,401,433,470,514,567,631,712,814,951,1143,1430,1908,2864,5729};

s16 cos_val[91] = {
10000,9998,9994,9986,9976,9962,9945,9925,9903,9877,9848,9816,9781,9744,9703,9659,9613,9563,9511,
9455,9397,9336,9272,9205,9135,9063,8988,8910,8829,8746,8660,8572,8480,8387,8290,8192,8090,7986,
7880,7771,7660,7547,7431,7314,7193,7071,6947,6820,6691,6561,6428,6293,6157,6018,5878,5736,5592,
5446,5299,5150,5000,4848,4695,4540,4384,4226,4067,3907,3746,3584,3420,3256,3090,2924,2756,2588,
2419,2250,2079,1908,1736,1564,1392,1219,1045,872,698,523,349,175,0};




s32 get_circle_X(s32 a, s32 radius) {
	a *= 10;
	while (a < 0)	a += 3600;
	while (a > 3600) a -= 3600;

	return (radius * int_cos(a) / 10000); // (rcosT)
	
}

s32 get_circle_Y(s32 a, s32 radius) {
	a *= 10;
	while (a < 0)	a += 3600;
	while (a > 3600) a -= 3600;

	return (radius * int_sin(a) / 10000);	// rsinT
	
}


/**
  * @brief  Approximation of sin function
  * @param  a: angle in degree scaled by 10 (0-3600)
  * @retval sin(a) scaled by 10000 (0-10000)
  */
s32 int_sin(s32 a)
{
	s16 ta;
	while(a < 0)
		a += 3600;
	a = a % 3600;
	ta = a;
	if (a > 2700) {
		a = 3600-a;
	} else if (a > 1800) {
		a = a-1800;
	} else if (a > 900) {
		a = 1800-a;
	}
	a = cos_val[90-(a/10)] + (((a%10)*cos_val[a/10]+286)/573);
	return ta > 1800 ? -a : a;
}

/**
  * @brief  Approximation of cos function
  * @param  a: angle in degree scaled by 10 (0-3600)
  * @retval cos(a) scaled by 10000 (0-10000)
  */
s32 int_cos(s32 a)
{
	return int_sin(900-a);
}

/**
  * @brief  Approximation of tan function
  * @param  a: angle in degree WITHOUT scaling
  * @retval tan(a) scaled by 100
  */
s16 int_tan(s16 a)
{
	u8 neg = 0;
	if (a < 0) {
		neg = 1;
		a = -a;
	}
	while (a < 0)
		a += 180;
	a = a % 180;
	if (a > 90) {
		return neg ? tan_val[180-a] : -tan_val[180-a];
	} else if (a < 90) {
		return neg ? -tan_val[a] : tan_val[a];
	} else {
		return -1;
	}
}

/**
  * @brief  Quick asin function approximation with 
  * @param  sin_val: input scaled by 10000 (-10000-10000)
  * @retval asin(sin_val) in degree WITHOUT scaling (-90-90)
  */
s16 int_arc_sin(s16 sin_val)
{
	s32 search_angle = 0;

	s16 lower_bound = 0, upper_bound = 900;
	s16 interval = 450, index = 0;
	u8 neg_val = 0;


	if (sin_val < 0)	neg_val = 1;
	sin_val = Abs(sin_val);

	while (1)
	{
		index = lower_bound + interval;
		search_angle = int_sin(index);

		if (interval == 0 || sin_val == search_angle)
		{
			index = (index+4)/10;
			break;
		}
		else if (sin_val < search_angle)
			upper_bound = index;
		else//						(sin_val > search_angle)
			lower_bound = index;
		interval = (upper_bound - lower_bound) / 2;

	}

	if (neg_val)	return -index;
	else			return index;
}


/**
  * @brief  Approximation of acos function
  * @param  cos_val: input scaled by 10000 (0-10000)
  * @retval acos(cos_val) in degree WITHOUT scaling
  */
s16 int_arc_cos(s16 cos_val)
{
	return 90-int_arc_sin(cos_val);
}

/**
  * @brief  Approximation of atan function
  * @param  tan_val: input scaled by 100
  * @retval atan(y/x) in degree from -89 to 90 (ignore quarter)
  */
s16 int_arc_tan(s32 tan_val)
{
	s16 angle = 0;
	s16 pre_tan = 0;
	s16 cur_tan = 0;
	if (tan_val >= 0) {
		while (angle < 90) {
			pre_tan = cur_tan;
			cur_tan = int_tan(angle);
			if (tan_val <= cur_tan && tan_val >= pre_tan) break;
			angle++;
		}
	} else {
		while (angle >= -89) {
			pre_tan = cur_tan;
			cur_tan = int_tan(angle);
			if (tan_val >= cur_tan && tan_val <= pre_tan) break;
			angle--;
		}
	}
	return angle;
}

/**
  * @brief  Approximation of atan2 function
  * @param  y: input y
  * @param  x: input x
  * @retval atan(y/x) in degree from 0 to 359 WITHOUT scaling (correct quarter)
  */
s16 int_arc_tan2(s32 y, s32 x)
{
	if (x == 0) {
		if (y < 0)
			return -90;
		else if (y == 0)
			return 0;
		else
			return 90;
	} else if (y == 0) {
		return x < 0 ? 180 : 0;
	} else if (x < 0) {
		return 180+int_arc_tan((s32)(100*y+x/2)/x);
	} else {
		return int_arc_tan((s32)(100*y+x/2)/x);
	}
}

/**
  * @brief  Matrix rotation
  * @param  x-coor (Pass by REF), y-coor (Pass by REF), angle to rotate (anti-clockwise)
  * @retval none
  */
void xy_rotate(s32 *x, s32 *y, s32 w) {
	s32 new_x = int_cos(w) * *x - int_sin(w) * *y;
	s32 new_y = int_sin(w) * *x + int_cos(w) * *y;
	*x = new_x / 10000;
	*y = new_y / 10000;
}


s32 p_mod(s32 dividor, s32 divisor) {
		while (dividor < 0) {dividor += divisor;}
		return dividor % divisor;
}

/**
  * @brief  Absolute value function
  * @param  v: input
  * @retval absolute value |v|
  */
s32 Abs(s32 v)
{
	return v < 0 ? -v : v;
}

s32 s_Abs (s32 v) {
	return v < 0 ? -v : v;
}

void limit_num(s32 limit, s32 *num) { 
	limit = Abs(limit);
	if (*num > limit) {*num = limit;}
	else if (*num < -limit) {*num = -limit;}
}

void limit_nums(s32 limit, u8 count, ...)
{
	u8 i, limit_required = 0;
	s32 max = 0;
	va_list vl, vl2;
	va_start(vl,count);
	va_copy(vl2, vl);

	limit = Abs(limit);
	
  for (i = 0;i < count; ++i) {
		s32* val=va_arg(vl,s32*);
		if (Abs(*val) > max) {max = Abs(*val);}
		if (Abs(*val) > limit) {limit_required = 1;}
  }

  if (!limit_required) {return;}

  for (i = 0;i < count; ++i) {
		s32* val=va_arg(vl2,s32*);
		if (max == 0) {*val = 0;}
		else {*val = *val * limit / max;}
  }
  va_end(vl);
  va_end(vl2);
}


s32 Sqr (s32 x) {
	return x * x;
}

/**
  * @brief  Rapid sqrt approximation with maximum 3.46% deviation
  * @param  v: input
  * @retval square root of 100*v
  */
s32 Sqrt(s32 num) {
	s32 upper_sqrt = 1, lower_sqrt, range, tmp_sqrt, tmp_sqr;
	//u8 i = 0;
	if (num == 0) return 0;

	while (Sqr(upper_sqrt) <= num) {
		if ((upper_sqrt <<= 1) < 0) {
			return 0;	// Overflow happened
		}
	}

	lower_sqrt = upper_sqrt >> 1;
	range = upper_sqrt - lower_sqrt;
	do {
		tmp_sqrt = lower_sqrt + (range >>= 1);
		tmp_sqr = Sqr(tmp_sqrt);
		if (tmp_sqr < num) {lower_sqrt = tmp_sqrt;}
		else if (tmp_sqr > num) {upper_sqrt = tmp_sqrt;}
		else {return tmp_sqrt;}
		//printf("%d to %d \n", lower_sqrt, upper_sqrt);
	} while (range >= 2);

	//printf("%d, %d\n", num, (Sqr(lower_sqrt) + Sqr(upper_sqrt)) >> 1);
	if (num < (Sqr(lower_sqrt) + Sqr(upper_sqrt)) >> 1) {
		return lower_sqrt;
	} else {
		return upper_sqrt;
	}
}

s32 Hyp(s32 x, s32 y) {
	return Sqrt(x*x + y*y);
}

/**
	* @brief Calculate angle difference with the smallest possible angle 
	* @param current_angle, target_angle
	* @param angle_at_a_point: reference of 360 degrees (i.e., take X if X represent 360 degrees)
	* @sample (20, 55, 360) returns 35, (20, 355, 360) returns 35, (355, -20, 360) returns -15
	* @sample (300, 2900, 3600) returns -1000, (0, 3555, 3600) returns 45
	* @return The angle difference (possibly positive or negative, taking the smallest possible angle)
	*/
s32 angle_diff(s32 current_angle, s32 target_angle, s32 angle_at_a_point) 
{
	s32 diff = target_angle - current_angle;
	while (diff > angle_at_a_point / 2) {diff -= angle_at_a_point;}
	while (diff < -angle_at_a_point / 2) {diff += angle_at_a_point;}
	return diff;
}

s32 Acc_limit(s32 prev_speed, s32 new_speed, const s32 ACC, const s32 DEC) 
{
	//new_speed = prev_speed;
	
	
	if (prev_speed > 0) {
		if (new_speed > prev_speed + ACC) {
			new_speed = prev_speed + ACC;
		} else if (new_speed < prev_speed - DEC) {
			new_speed = prev_speed - DEC;
		}
	}
	
	else if (prev_speed < 0) {
		if (new_speed < prev_speed - ACC) {
			new_speed = prev_speed - ACC;
		} else if (new_speed > prev_speed + DEC) {
			new_speed = prev_speed + DEC;
		}
	}
	
	else {
		if (new_speed > ACC) {
			new_speed = ACC;
		}	else if (new_speed < -ACC) {
			new_speed = -ACC;
		}
	}

	return new_speed;

}

