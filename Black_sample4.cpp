#include <iostream>
#include <cmath>
#include <string>
#include <ctime>
#include <windows.h>
using namespace std;

const double PI = 3.1415;
const int PUT = 0;
const int CALL = 1;

double N(double myNum);
double NormDist(double myNum);

double Black(double opt, double price, double strike, double
	expiry, double volatility, double sensitivity, double
	interestRate = 0)
{
	double Result;

	const int PREMIUM = 0;
	const int DELTA = 1;
	const int GAMMA = 2;
	const int VEGA = 3;
	const int THETA = 4;

	double d1, d2;

	//Error check
	if (strike < 0 || price < 0 || expiry < 0 || volatility < 0)
	{
		Result = (double)-9999;
		return Result;
	}
	else if (volatility == 0)
	{
		switch ((int)sensitivity){
		case PREMIUM:
			//Return intrinsic value only
			switch ((int)opt){
			case PUT:
				Result = exp(-interestRate * expiry) * max(strike - price, 0);
				break;
			case CALL:
				Result = exp(-interestRate * expiry) * max(price - strike, 0);
				break;
			default:
				Result = (double)-9999;
			}
			break;
		default:
			Result = (double)0;
		}
		return Result;
	}
	else
	{
		d1 = (log(price / strike) + expiry * pow(volatility, 2) / 2) / (volatility * sqrt(expiry));
		d2 = d1 - volatility * sqrt(expiry);
	}

	switch ((int)sensitivity){

	case PREMIUM:
		switch ((int)opt){
		case PUT:  //Put option premium
			Result = exp(-interestRate * expiry) * (strike * N(-d2) - price * N(-d1));
			break;
		case CALL:  //Call option premium
			Result = exp(-interestRate * expiry) * (price * N(d1) - strike * N(d2));
			break;
		default:
			Result = (double)-9999;
		}
		break;

	case DELTA:
		switch ((int)opt){
		case PUT:  //Delta of Put
			if (expiry == 0){
				if (price < strike){
					Result = (double)-1;
				}
				else{
					Result = (double)0;
				}
			}
			else{
				Result = exp(-interestRate * expiry) * (N(d1) - 1);
			}
			break;

		case CALL:  //Delta of Call
			if (expiry == 0){
				if (price > strike){
					Result = (double)1;
				}
				else{
					Result = (double)0;
				}
			}
			else{
				Result = exp(-interestRate * expiry) * N(d1);
			}
			break;

		default:
			Result = (double)-9999;
		}
		break;

	case GAMMA:
		//Gamma of Call/Put
		Result = exp(-interestRate * expiry) * NormDist(d1) / (price * volatility * sqrt(expiry));
		break;

	case VEGA:
		//Vega of Call/Put
		Result = exp(-interestRate * expiry) * NormDist(d1) * price * sqrt(expiry);
		break;

	case THETA:
		switch ((int)opt){
		case PUT:  //Theta of Put
			Result = -1 * price * NormDist(d1) * volatility * exp(-interestRate * expiry) / (2 * sqrt(expiry)) -
				interestRate * price * N(-d1) * exp(-interestRate * expiry) +
				interestRate * strike * exp(-interestRate * expiry) * N(-d2);
			break;
		case CALL:  //Theta of Call
			Result = -1 * price * NormDist(d1) * volatility * exp(-interestRate * expiry) / (2 * sqrt(expiry)) +
				interestRate * price * N(d1) * exp(-interestRate * expiry) -
				interestRate * strike * exp(-interestRate * expiry) * N(d2); \
				break;
		default:
			Result = (double)-9999;
		}
		break;
	default:
		Result = (double)-9999;
	}


	return Result;
}

/************************************************
Standard normal cummulative distribution function
************************************************/
double N(double myNum)
{
	if (myNum > 6.0) { return 1.0; };
	if (myNum < -6.0) { return 0.0; };

	double b1 = 0.3193;
	double b2 = -0.3565;
	double b3 = 1.7814;
	double b4 = -1.8212;
	double b5 = 1.3302;
	double p = 0.2316;
	double c2 = 0.3989;
	double a = fabs(myNum);
	double t = 1.0 / (1.0 + a*p);
	double b = c2*exp((-myNum)*(myNum / 2.0));
	double n = ((((b5*t + b4)*t + b3)*t + b2)*t + b1)*t;

	n = 1.0 - b*n;
	if (myNum < 0.0) n = 1.0 - n;
	return n;
}

/*************************************
Standard normal distribution function
*************************************/
double NormDist(double myNum)
{
	return exp(-pow(myNum, 2) / 2.0) / sqrt(2.0*PI);
}
