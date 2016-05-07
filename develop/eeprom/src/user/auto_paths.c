/**
  ******************************************************************************
  * _____  ________________   ________      ______                           
  * __  / / /_  ___/__  __/   ___  __ \________  /__________________________ 
  * _  / / /_____ \__  /      __  /_/ /  __ \_  __ \  __ \  ___/  __ \_  __ \
  * / /_/ / ____/ /_  /       _  _, _// /_/ /  /_/ / /_/ / /__ / /_/ /  / / /
  * \____/  /____/ /_/        /_/ |_| \____//_.___/\____/\___/ \____//_/ /_/ 
  *                                                                
  * @file    auto_paths.c
  * @author  Pang Hong Wing
  * @version V1.0.0
  * @date    16 Feb, 2016
  * @brief   Library for storing hybrid PID paths.
  ******************************************************************************
  */

#include "auto_paths.h"

const TARGET STRAIGHT[] = {
	{NODE_STOP, 0, 1500, 0, 0.0},
	{NODE_END}
};

const TARGET CIRCLE[] = {
	{NODE_PASS, 1000, 1000, 0, 1.0},
	{NODE_PASS, 2000, 0, 0, 1.0},
	{NODE_PASS, 1000, -1000, 0, 1.0},
	{NODE_STOP, 0, 0, 0, 1.0},
	{NODE_END}
};

const TARGET EIGHT_FIG[] = {
	{NODE_PASS, 500, 0, 0, 0.0},
	{NODE_PASS, 1000, 500, 0, -2.0},
	{NODE_PASS, 500, 1000, 0, -2.0},
	{NODE_PASS, 0, 500, 0, -2.0},
	{NODE_PASS, 0, -500, 0, 0.0},
	{NODE_PASS, -500, -1000, 0, 2.0},
	{NODE_PASS, -1000, -500, 0, 2.0},
	{NODE_PASS, -500, 0, 0, 2.0},
	{NODE_STOP, 0, 0, 0, 0.0},
	{NODE_END}
};

const TARGET DEM_PID[] = {
	{NODE_STOP, -2870, 0, 330, 0.0},
	{NODE_PASS, -2870, 718, 330, 0.0},
	{NODE_PASS, -1793, 3318, 15, 0.272},
	{NODE_PASS, -427, 5684, 345, -0.189},
	{NODE_STOP, -247, 6800, 270, 0.0},
	{NODE_PASS, -247, 8800, 270, 0.0},
	{NODE_PASS, -1970, 12293, 270, -0.227},
	{NODE_PASS, -4650, 13000, 270, -0.184},
	{NODE_STOP, -5500, 13000, 270, 0.0},
	{NODE_END}
};