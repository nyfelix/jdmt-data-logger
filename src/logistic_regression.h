#pragma once

class logistic_regression
{
public:
	explicit logistic_regression(
		const double intercept,
		const double* coef,
		const unsigned short nof_coef,
		const double euler, double (*pow)(double, double)) : intercept{ intercept },
		coef_{ coef },
		nof_coef{ nof_coef },
		euler_{ euler },
		pow_{ pow }
	{
	}
	double predict(unsigned char* matrix) const;

private:
	const double intercept{};
	const double* coef_;
	const unsigned short nof_coef{};
	const double euler_{};
	double (*pow_)(double, double);
};