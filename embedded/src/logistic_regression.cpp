#include "logistic_regression.h"

double logistic_regression::predict(unsigned char* matrix) const
{
	double sum = -intercept;
	for (auto index = 0; index < nof_coef; index++)
	{
		sum -= matrix[index] * coef_[index];
	}
	return 1 / (1 + pow_(euler_, sum));
}