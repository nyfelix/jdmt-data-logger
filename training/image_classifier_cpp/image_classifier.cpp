#include "pch.h"
#include "image_classifier.h"

void image_classifier::compress_image(double **img) const
{
	for (uint16_t column_index = 0; column_index < nof_columns_; column_index++)
	{
		double row_sum = 0;
		for (uint16_t row_index = 0; row_index < nof_rows_; row_index++)

		{
			row_sum += img[row_index][column_index];
		}
		img[0][column_index] = row_sum / nof_rows_;
	}
}

double image_classifier::predict(double **img, const unsigned short nof_rows, const unsigned short nof_columns) const
{
	double sum = 0;
	for (auto row_index = 0; row_index < nof_rows; row_index++)
	{
		for (uint16_t column_index = 0; column_index < nof_columns; column_index++)
		{
			sum -= img[row_index][column_index] * coef_[nof_columns*row_index + column_index];
		}
	}
	return 1 / (1 + pow(euler_, sum));
}

double image_classifier::calculate_treshhold(double** image) const
{
	double sum = 0;
	for (auto row_index = 0; row_index < nof_rows_; row_index++)
	{
		for (auto column_index = 0; column_index < nof_columns_; column_index++)
		{
			sum += image[row_index][column_index];
		}
	}
	return sum / (nof_rows_ * nof_columns_);
}

void image_classifier::convert_to_binary(double**  image) const
{
	const auto treshold = calculate_treshhold(image);
	for (auto row_index = 0; row_index < nof_rows_; row_index++)
	{
		for (auto column_index = 0; column_index < nof_columns_; column_index++)
		{
			image[row_index][column_index] = image[row_index][column_index] <= treshold ? 1 : 0;
		}
	}
}
