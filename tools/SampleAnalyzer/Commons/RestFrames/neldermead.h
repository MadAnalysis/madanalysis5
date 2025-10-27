#pragma once
# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cmath>
//#include<array>   // for begin and end etc

#include <vector>
#include <functional>
/*
void nelmin ( double fn ( double x[] ), int n, double start[], double xmin[], 
  double *ynewlo, double reqmin, double step[], int konvge, int kcount, 
  int *icount, int *numres, int *ifault );
*/
/*
void nelmin (std::function<double(double x[])> &fn, int n, std::vector<double> &start, std::vector<double> &xmin, 
  double *ynewlo, double reqmin, std::vector<double> &step, int konvge, int kcount, 
  int *icount, int *numres, int *ifault );
*/

/*
void nelmin (auto &fn, int n, std::vector<double> &start, std::vector<double> &xmin, 
  double *ynewlo, double reqmin, std::vector<double> &step, int konvge, int kcount, 
  int *icount, int *numres, int *ifault );
*/

template<typename FUNC> void nelmin (FUNC &fn, int n, double start[], double xmin[], 
  double *ynewlo, double reqmin, std::vector<double> &step, int konvge, int kcount, 
  int *icount, int *numres, int *ifault );

class NELDERMEAD
{
  private:
      //int nvars=0; // number of variables
      std::vector<double> vals;
      std::vector<double> steps;
      //std::vector<double> xmin;
      std::vector<bool> fixed;
      double _eps=0.001;
     int _maxit=1000;
     int _printlevel=0;
     int kcount = 0;
     int icound = 0;
     int _status = 0;


  public:
    NELDERMEAD() {(void)kcount; (void)icound; };
    void Minimize(std::function<double(const double*)> infn);
    //void Minimize(double (*infn) ( const double*));
    void SetVariable(int index, const char* name, double initialval, double step);
    void SetVariableValue( int index, double value);
    void FixVariable(int n);
    void ReleaseVariable(int n);
    void SetMaxFunctionCalls(int ncalls);
    void SetMaxIterations(int nits);
    void SetTolerance(double  tol);
    void SetPrintLevel(int level);

    int Status();

    const double* X();

    ~NELDERMEAD();
};


/*
double ccoeff = 0.5;
  double del;
  double dn;
  double dnn;
  double ecoeff = 2.0;
  double eps = 0.001;
  int i;
  int ihi;
  int ilo;
  int j;
  int jcount;
  int l;
  int nn;
  double *p;
  double *p2star;
  double *pbar;
  double *pstar;
  double rcoeff = 1.0;
  double rq;
  double x;
  double *y;
  double y2star;
  double ylo;
  double ystar;
  double z;


*/
