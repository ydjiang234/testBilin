#ifndef testBilin_h
#define testBilin_h


#include <UniaxialMaterial.h>

class testBilin : public UniaxialMaterial
{
public:
    testBilin(int tag, double E, double fyt, double fyn, double bt, double bn);
    testBilin();
    ~testBilin();
    UniaxialMaterial *getCopy(void);

    int setTrialStrain(double strain, double strainRate = 0);

    double getStrain(void);
    double getStress(void);
    double getTangent(void);
    double getInitialTangent(void);

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);
    
    void Print(OPS_Stream &s, int flag = 0);
    


protected:

private:
    double E, bt, bn; //fyn should be negative
    double fyt_ini, fyn_ini;//initial fyt and fyn
    double strain, stress, tangent;
    double strain_next, stress_next, tangent_next;
    double ep, fyt, fyn; //ep, fyt, fyn
    double ep_next, fyt_next, fyn_next;

    int nextStress(double input_strain);
    int gotoNext(void);

};



#endif // !testBilin_h
