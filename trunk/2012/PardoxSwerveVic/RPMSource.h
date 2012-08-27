#ifndef RPMSOURCE_H_
#define RPMSOURCE_H_

class RPMSource : public PIDSource
{
public:
	RPMSource();
	virtual ~RPMSource();
	void inputRPM(double input);
	double PIDGet();
private:
	double rpm;
};

#endif
