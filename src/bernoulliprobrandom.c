#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>

SEXP bernoulliprobrandom(SEXP patterns, SEXP outcomex,SEXP lambdacoef, 
	SEXP gh, SEXP momentdata, SEXP probit)
{
	SEXP ans;
	int irow, outcome, index, noutcomes, nrows, ipoint, npoints, level2size, ilambda, lprobit, *rpatterns = INTEGER(patterns);
	double  *routcomex = REAL(outcomex), *rans,
		neww,newp, *rmomentdata=REAL(momentdata),
		*rgh=REAL(gh),*rlambdacoef=REAL(lambdacoef);
	double product, sum, myoutcomex, myoutcomep;
	
	lprobit = asLogical(probit);
	
	noutcomes = LENGTH(outcomex);
	nrows = LENGTH(patterns)/noutcomes;
	npoints = LENGTH(gh)/2;
	level2size=LENGTH(lambdacoef);
	
	PROTECT(ans = allocVector(REALSXP,nrows));
	
	rans = REAL(ans);
	
	
	for (irow=0; irow < nrows; irow++) {
		/* Rprintf("irow  %d\n",irow); */
		sum=0.0;
/* calculate transformed w and p */
		for (ipoint=0; ipoint < npoints; ipoint++) {
			/* Rprintf("momentdata  %f,%f\n",rmomentdata[irow],rmomentdata[nrows+irow]); */
			newp = rmomentdata[irow]+rmomentdata[nrows+irow]*rgh[ipoint];
			neww = log(rmomentdata[nrows+irow])+
				(rgh[ipoint]*rgh[ipoint])/2.0+log(rgh[npoints+ipoint])-
				newp*newp/2.0;
			/* Rprintf("newp,neww  %f,%f\n",newp,neww); */
			ilambda=0;
			product=1.0;
			for (outcome=0; outcome <noutcomes; outcome++) {
				/* calculate outcome probability for this outcome */
				myoutcomex = routcomex[outcome]+
					rlambdacoef[ilambda]*newp;
				if (lprobit)
					myoutcomep=pnorm(myoutcomex,0,1,TRUE,FALSE);
				else
					myoutcomep=1.0/(1+exp(-myoutcomex));
				ilambda=(ilambda+1) % level2size;				
				/* update likelihood for this observation */
			/*  Rprintf("myoutcomep  %f\n",myoutcomep); */
				index = irow+outcome*nrows;
				if (rpatterns[index]!=NA_INTEGER) {
				  if (rpatterns[index]==1) product = product*myoutcomep;
				  else product = product*(1-myoutcomep); 
				}
			}
			sum=sum+product*exp(neww);
		}
		rans[irow]=sum;
	}
	UNPROTECT(1);		
	return ans;
}
