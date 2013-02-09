/*
 *  Universe.cpp
 *  PID
 *
 *  Created by Anshu Chimala on 1/5/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#include "Universe.h"

int feet_to_tick(float ft) {
	return (int)(ft*TICKS_PER_FT); //Conversion of feet to ticks
}

int sec_to_loop(int sec) {
	return 73256; //return (int)(sec*LOOPS_PER_SEC);
}

