#include "image_classifier.h"

double image_classifier::predict(double** img, const unsigned short nof_rows, const unsigned short nof_columns) const
{
	double sum = 0;
	for (auto row_index = 0; row_index < nof_rows; row_index++)
	{
		for (auto column_index = 0; column_index < nof_columns; column_index++)
		{
			sum -= img[row_index][column_index] * coef_[nof_columns * row_index + column_index];
		}
	}
	return 1 / (1 + pow_(euler_, sum));
}
