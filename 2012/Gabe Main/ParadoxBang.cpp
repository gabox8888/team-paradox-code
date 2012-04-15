#include "WPILib.h"
#include "ParadoxBang.h"
#include "math.h"

ParadoxBang::ParadoxBang(UINT32 sensor)
{
       time		= new Timer();
       count	= new Counter(sensor);// not sure how to initialize this quite yet ask Geoff for help
       
       K = 0.0;//tune here value from 0 to 1
       max_change = 1.0;//also tune this guy
       
}
void ParadoxBang::SetRPM(float RPM)
{
	the_RPM = RPM;
}
float ParadoxBang::Speed(void)
{
	 new_time = time->GetPPCTimestamp();
	 counts = count->Get();
	 count->Reset();
	 shooterRPM = (60/3)*counts/(new_time-previous_time);
	 filteredRPM = K*filteredRPM + (1-K)*shooterRPM;
	 previous_time = new_time;
	 if(shooterRPM >= the_RPM) new_motor = 0.0;
	 else new_motor = 1.0;
	 if (new_motor > previous_motor + max_change)new_motor=previous_motor + max_change;
	 previous_motor = new_motor;
	 return new_motor;
	 
 }
 
