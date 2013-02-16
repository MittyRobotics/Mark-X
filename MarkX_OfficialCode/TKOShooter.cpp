//Last edited by Vadim Korolik
//on 02/09/2013
#include "TKOShooter.h"

///Constructor for the TKOAutonomous class

TKOShooter::TKOShooter(int port1) :
	j(port1, CANJaguar::kSpeed)
{
}
///Destructor for the TKOAutonomous class
TKOShooter::~TKOShooter()
{
}
