/*
 *  Molecule.h
 *  PID
 *
 *  Created by Anshu Chimala on 1/5/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#ifndef __MOLECULE_H
#define __MOLECULE_H

#include "Atom.h"
#include <stdarg.h>
#include <vector>

class Molecule {
private:
	unsigned int _position;
	std::vector<Atom * > _formula;
	bool _done;
	bool _newAtom;

public:
	Molecule(Atom *a1, ...); // Please terminate this with NULL
	void Execute();
	bool Done();
	void Reset();
};

#endif
