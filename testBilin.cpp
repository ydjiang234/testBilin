#include <elementAPI.h>
#include "testBilin.h"
#include <Vector.h>
#include <Channel.h>
#include <math.h>
#include <float.h>

#ifdef _USRDLL
#define OPS_Export extern "C" _declspec(dllexport)
#elif _MACOSX
#define OPS_Export extern "C" __attribute__((visibility("default")))
#else
#define OPS_Export extern "C"
#endif


static int numtestBilin = 0;

OPS_Export void *OPS_testBilin()
{
	// print out some KUDO's
	if (numtestBilin == 0) {
		opserr << "testBilin unaxial material - Written by Yadong Jiang\n";
		numtestBilin = 1;
	}

	// Pointer to a uniaxial material that will be returned
	UniaxialMaterial *theMaterial = 0;

	//
	// parse the input line for the material parameters
	//

	int    iData[1];
	double dData[5];
	int numData;
	numData = 1;
	if (OPS_GetIntInput(&numData, iData) != 0) {
		opserr << "WARNING invalid uniaxialMaterial testBilin tag" << endln;
		return 0;
	}

	numData = 5;
	if (OPS_GetDoubleInput(&numData, dData) != 0) {
		opserr << "WARNING invalid E & fyt & fyn & bt & bn\n";
		return 0;
	}

	// 
	// create a new material
	//

	theMaterial = new testBilin(iData[0], dData[0], dData[1], dData[2], dData[3], dData[4]);

	if (theMaterial == 0) {
		opserr << "WARNING could not create uniaxialMaterial of type testBilin\n";
		return 0;
	}

	// return the material
	return theMaterial;
}


testBilin::testBilin(int tag, double E, double fyt, double fyn, double bt, double bn):UniaxialMaterial(tag, 0)
{
	this->E = E;
	this->bt = bt;
	this->bn = bn;
	this->fyt_ini = fyt;
	this->fyn_ini = fyn;
	this->strain = 0.0;
	this->stress = 0.0;
	this->tangent = E;
	this->strain_next = 0.0;
	this->stress_next = 0.0;
	this->tangent_next = E;
	this->ep = 0.0;
	this->fyt = fyt;
	this->fyn = fyn;
	this->ep_next = 0.0;
	this->fyt_next = fyt;
	this->fyn_next = fyn;
}


testBilin::testBilin() :UniaxialMaterial(0, 0)
{
	this->E = 0.0;
	this->bt = 0.0;
	this->bn = 0.0;
	this->fyt_ini = 0.0;
	this->fyn_ini = 0.0;
	this->strain = 0.0;
	this->stress = 0.0;
	this->tangent = 0.0;
	this->strain_next = 0.0;
	this->stress_next = 0.0;
	this->tangent_next = 0.0;
	this->ep = 0.0;
	this->fyt = 0.0;
	this->fyn = 0.0;
	this->ep_next = 0.0;
	this->fyt_next = 0.0;
	this->fyn_next = 0.0;
}

testBilin::~testBilin()
{
	//do nothing here
}

int testBilin::setTrialStrain(double strain, double strainRate)
{
	if (fabs(strain - this->strain) < DBL_EPSILON)
	{
		return 0;
	} else {
		this->nextStress(strain);
		return 0;
	}
}

double testBilin::getStrain(void)
{
	return this->strain_next;
}

double testBilin::getStress(void)
{
	return this->stress_next;
}

double testBilin::getTangent(void)
{
	return this->tangent_next;
}

double testBilin::getInitialTangent(void)
{
	return this->E;
}

int testBilin::commitState(void)
{
	this->gotoNext();
	return 0;
}

int testBilin::revertToLastCommit(void)
{
	
	this->strain_next = this->strain;
	this->strain_next = this->stress;
	this->tangent_next = this->tangent;
	this->ep = 0.0;
	this->fyt = 0.0;
	this->fyn = 0.0;
	this->ep_next = 0.0;
	this->fyt_next = 0.0;
	this->fyn_next = 0.0;
	return 0;
}

int testBilin::revertToStart(void)
{
	this->strain_next = this->strain = 0.0;
	this->strain_next = this->stress = 0.0;
	this->tangent_next = this->tangent = this->E;
	this->ep_next = this->ep = 0.0;
	this->fyt_next = this->fyt = this->fyt_ini;
	this->fyn_next = this->fyn = this->fyn_ini;
	return 0;
}

UniaxialMaterial *testBilin::getCopy(void)
{
	testBilin *theCopy = new testBilin(this->getTag(), this->E, this->fyt_ini, this->fyn_ini, this->bt, this->bn);
	theCopy->strain = this->strain;
	theCopy->stress = this->stress;
	theCopy->tangent = this->tangent;
	theCopy->strain_next = this->strain_next;
	theCopy->stress_next = this->stress_next;
	theCopy->tangent_next = this->tangent_next;
	theCopy->ep = this->ep;
	theCopy->fyt = this->fyt;
	theCopy->fyn = this->fyn;
	theCopy->ep_next = this->ep_next;
	theCopy->fyt_next = this->fyt_next;
	theCopy->fyn_next = this->fyn_next;
	//opserr << this->i << endln;
	return theCopy;
}

int testBilin::sendSelf(int commitTag, Channel &theChannel)
{
	int res = 0;
	static Vector data(12);
	data(0) = this->getTag();
	data(1) = this->E;
	data(2) = this->bt;
	data(3) = this->bn;
	data(4) = this->fyt_ini;
	data(5) = this->fyn_ini;
	data(6) = this->strain;
	data(7) = this->stress;
	data(8) = this->tangent;
	data(9) = this->ep;
	data(10) = this->fyt;
	data(11) = this->fyn;
	res = theChannel.sendVector(this->getDbTag(), commitTag, data);
	if (res < 0)
		opserr << "testBilin::sendSelf() - failed to send data\n";
	return res;
}
int testBilin::recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker)
{
	int res = 0;
	static Vector data(12);
	res = theChannel.recvVector(this->getDbTag(), commitTag, data);
	if (res < 0) {
		opserr << "testBilin::recvSelf() - failed to receive data\n";
	}
	else {
		this->setTag(data(0));
		this->E = data(1);
		this->bt = data(2);
		this->bn = data(3);
		this->fyt_ini = data(4);
		this->fyn_ini = data(5);
		this->strain = data(6);
		this->stress = data(7);
		this->tangent = data(8);
		this->ep = data(9);
		this->fyt = data(10);
		this->fyn = data(11);
		this->strain_next = this->strain;
		this->stress_next = this->stress;
		this->tangent_next = this->tangent;
		this->ep_next = this->ep;
		this->fyt_next = this->fyt;
		this->fyn_next = this->fyn;
	}
	return res;
}

void testBilin::Print(OPS_Stream &s, int flag)
{
	s << "testBilin tag: " << this->getTag() << endln;
}


int testBilin::nextStress(double input_strain)
{
	double e_new = input_strain;
	double sig_new, fyt_new, fyn_new, ep_new, tangent_new, eyt_new, eyn_new;
	double ep = this->ep;
	double fyt = this->fyt;
	double fyn = this->fyn;
	double eyt = ep + fyt / this->E; //Yield strain under tension
	double eyn = ep + fyn / this->E; //Yield strain under compression
	if ((e_new - eyt) > DBL_EPSILON) {
		//Now the material is under tenile and is in plastic range
		sig_new = this->bt * this->E * (e_new - eyt) + fyt;
		fyt_new = sig_new;
		fyn_new = fyt_new - (fyt - fyn);
		eyt_new = e_new;
		ep_new = eyt_new - fyt_new / this->E;
		tangent_new = this->bt * this->E;
	} else if ((eyn - e_new) > DBL_EPSILON) {
		//Now the material is under compression and is in plastic range
		sig_new = this->bn * this->E * (e_new - eyn) + fyn;
		fyn_new = sig_new;
		fyt_new = fyn_new - (fyn - fyt);
		eyn_new = e_new;
		ep_new = eyn_new - fyn_new / this->E;
		tangent_new = this->bn * this->E;
	} else {
		//Now the material is linear
		sig_new = (e_new- ep) * this->E;
		fyn_new = fyn;
		fyt_new = fyt;
		ep_new = ep;
		
		tangent_new = this->E;
		
	}
	this->strain_next = e_new;
	this->stress_next = sig_new;
	this->tangent_next = tangent_new;
	this->ep_next = ep_new;
	this->fyt_next = fyt_new;
	this->fyn_next = fyn_new;
	
	return 0;
}

int testBilin::gotoNext(void)
{
	this->strain = this->strain_next;
	this->stress = this->stress_next;
	this->tangent = this->tangent_next;
	this->ep = this->ep_next;
	this->fyt = this->fyt_next;
	this->fyn = this->fyn_next;
	//opserr << curve_parameters[0] << endln;
	return 0;
}