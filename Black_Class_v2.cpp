#include <iostream>
#include <cmath>
#include <iomanip>
#include <string.h>
#include <windows.h>
#define PI 3.1415
using namespace std;

const int PUT = 0;
const int CALL = 1;

class Distribution
{
public:
	virtual double N(double Num) = 0;
	virtual double NormDist(double Num) = 0;
};

class StandardNormal : public Distribution
{
public:
	//standard normal cumulative distribution function
	double N(double Num)
	{
		if (Num > 6.0) { return 1.0; };
		if (Num < -6.0) { return 0.0; };

		double const b1 = 0.3193;
		double const b2 = -0.3565;
		double const b3 = 1.7814;
		double const b4 = -1.8212;
		double const b5 = 1.3302;
		double const p = 0.2316;
		double const c2 = 0.3989;
		double a = fabs(Num);
		double t = 1.0 / (1.0 + a*p);
		double b = c2*exp((-Num)*(Num / 2.0));
		double n = ((((b5*t + b4)*t + b3)*t + b2)*t + b1)*t;

		n = 1.0 - b*n;
		if (Num < 0.0) n = 1.0 - n;
		return n;
	}

	//standard normal distribution function
	double NormDist(double Num)
	{
		return exp(-pow(Num, 2) / 2.0) / sqrt(2.0*PI);
	}
};

class Black : public StandardNormal
{
private:
	int opt;
	double price;
	double strike;
	double expiry;
	double volatility;
	double interestRate;
	double d1, d2;

public:
	Black(int m_opt, double m_price, double m_strike, double m_expiry, double m_volatility, double m_interestRate = 0)
		: opt(m_opt), price(m_price), strike(m_strike), expiry(m_expiry), volatility(m_volatility), interestRate(m_interestRate)
	{
		if (strike < 0 || price < 0 || expiry < 0 || volatility < 0)
		{
			cout << "The input value is not valid" << endl;
			exit(1);
		}
		else
		{
			d1 = (log(m_price / m_strike) + m_expiry * pow(m_volatility, 2) / 2) / (m_volatility * sqrt(m_expiry));
			d2 = d1 - m_volatility * sqrt(m_expiry);
		}
	}

	double getD1() const { return d1; }
	double getD2() const { return d2; }
	double setInc()
	{
		strike++;
		return premium();
	}

	double setDec()
	{
		if (strike < 0)
		{
			cout << "Can't decrease the strike price any more" << endl;
			exit(1);
		}
		else
		{
			strike--;
			return premium();
		}
	}

	double setStrike(double m_strike)
	{
		this->strike = m_strike;
		return premium();
	}

	double premium()
	{
		double result;

		if (volatility == 0)
		{
			switch ((int)opt)
			{
			case PUT:
				result = exp(-interestRate * expiry) * max(strike - price, 0);
				break;
			case CALL:
				result = exp(-interestRate * expiry) * max(price - strike, 0);
				break;
			default:
				result = (double)-9999;
			}
		}
		else
		{
			switch ((int)opt)
			{
			case PUT:
				result = exp(-interestRate * expiry) * (strike * N(-d2) - price * N(-d1));
				break;
			case CALL:
				result = exp(-interestRate * expiry) * (price * N(d1) - strike * N(d2));
				break;
			default:
				result = (double)-9999;
			}
		}
		return result;
	}

	double delta()
	{
		double result;

		switch ((int)opt)
		{
		case PUT:
			if (expiry == 0)
			{
				if (price < strike)
				{
					result = (double)-1;
				}
				else
				{
					result = (double)0;
				}
			}
			else
			{
				result = exp(-interestRate * expiry) * (d1 - 1);
			}
			break;

		case CALL:
			if (expiry == 0)
			{
				if (price > strike)
				{
					result = (double)1;
				}
				else
				{
					result = (double)0;
				}
			}
			else
			{
				result = exp(-interestRate * expiry) * N(d1);
			}
			break;
		default:
			result = (double)-9999;
		}
		return result;
	}

	double gamma()
	{
		double result = exp(-interestRate * expiry) * NormDist(d1) / (price * volatility * sqrt(expiry));
		return result;
	}

	double vegga()
	{
		double result = exp(-interestRate * expiry) * NormDist(d1) * price * sqrt(expiry);
		return result;
	}

	double theta()
	{
		double result;

		switch ((int)opt)
		{
		case PUT:
			result = -1 * price * NormDist(d1) * volatility * exp(-interestRate * expiry) / (2 * sqrt(expiry)) -
					interestRate * price * N(-d1) * exp(-interestRate * expiry) +
				    interestRate * strike * exp(-interestRate * expiry) * N(-d2);
			break;
		case CALL:
			result = -1 * price * NormDist(d1) * volatility * exp(-interestRate * expiry) / (2 * sqrt(expiry)) +
				    interestRate * price * N(d1) * exp(-interestRate * expiry) -
				    interestRate * strike * exp(-interestRate * expiry) * N(d2);
			break;
		default:
			result = (double)-9999;
		}
		return result;
	}

};

// put  = 0, call = 1
void main()
{
	int opt = 1; double price = 110.70; double strike = 100.50; double expiry = 0.55; double vol = 0.22;
	double start_strike = 96.50; double end_strike = 104.20;

	int size = floor(end_strike) - floor(start_strike);
	double interval = (end_strike - start_strike) / size;
	double * premium_compare = new double[size + 1];
	memset(premium_compare, 0, _msize(premium_compare));

	Black B(opt, price, strike, expiry, vol);
	cout << "Call option premium is: " << B.premium() << endl;
	cout << "Call option delta is: " << B.delta() << endl;
	cout << "Call option gamma is: " << B.gamma() << endl;
	cout << "Call option vegga is: " << B.vegga() << endl;
	cout << "Call option theta is: " << B.theta() << endl;
	cout << "Call option d1 is: " << B.getD1() << endl;
	cout << "Call option d2 is: " << B.getD2() << endl;
	cout << "Call option premium on 99 strike is: " << B.setDec() << endl;
	B.setInc();
	cout << "Call option premium on 101 strike is: " << B.setInc() << endl;
	cout << "============================================== " << endl;

	cout << "Call option premiums of strike prices from 96.50 to 104.20 are as follows: " << endl;
	double new_strike = start_strike;
	for (int i = 0; i < size + 1; i++)
	{
		premium_compare[i] = B.setStrike(new_strike);
		cout.setf(ios::showpoint);
		cout.precision(6);
		cout << "strike price: " << new_strike << ", " << "option premium: " << *(premium_compare + i) << endl;
		new_strike = start_strike + (i + 1)*interval;
	}
	cout << endl;

	delete[] premium_compare;
}

