/*
 *  Universe.h
 *  PID
 *
 *  Created by Anshu Chimala on 1/5/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#ifndef __UNIVERSE_H
#define __UNIVERSE_H

#include "Definitions.h"
#include "Atom.h"
#include "Molecule.h"

// Comment this out if you don't want verbosity
#define VERBOSE 1

#ifndef VERBOSE
#	define ULOG(s, ...)		;
#else
#	define ULOG				printf
#endif

#endif
