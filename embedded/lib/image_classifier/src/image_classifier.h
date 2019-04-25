#pragma once

class image_classifier
{
public:
	explicit image_classifier(const double* coef, const double euler, double (*pow)(double, double)) :
		coef_{ coef },
		euler_{ euler },
		pow_{ pow }
	{
	}

	double predict(double** img, unsigned short nof_rows, unsigned short nof_columns) const;
	static void compress_image(double** img, unsigned short nof_rows, unsigned short nof_columns);
	static double calculate_treshhold(double** image, unsigned short nof_rows, unsigned short nof_columns);
	static char* convert_to_number(const bool* binary_image, unsigned short nof_rows, unsigned short nof_columns);
	static bool* convert_to_binary(double** image, unsigned short nof_rows, unsigned short nof_columns);

private:
	const double* coef_;
	const double euler_{};
	double (*pow_)(double, double);
};


inline void image_classifier::compress_image(double** img, const unsigned short nof_rows, const unsigned short nof_columns)
{
	for (auto column_index = 0; column_index < nof_columns; column_index++)
	{
		double row_sum = 0;
		for (auto row_index = 0; row_index < nof_rows; row_index++)

		{
			row_sum += img[row_index][column_index];
		}
		img[0][column_index] = row_sum / nof_rows;
	}
}

inline double image_classifier::calculate_treshhold(double** image, const unsigned short nof_rows, const unsigned short nof_columns)
{
	double sum = 0;
	for (auto row_index = 0; row_index < nof_rows; row_index++)
	{
		for (auto column_index = 0; column_index < nof_columns; column_index++)
		{
			sum += image[row_index][column_index];
		}
	}
	return sum / nof_rows / nof_columns;
}

inline bool* image_classifier::convert_to_binary(double** image, const unsigned short nof_rows, const unsigned short nof_columns)
{
	const auto treshold = calculate_treshhold(image, nof_rows, nof_columns);
	const auto binary_image = new bool[nof_rows * nof_columns];

	for (auto row_index = 0; row_index < nof_rows; row_index++)
	{
		for (auto column_index = 0; column_index < nof_columns; column_index++)
		{
			binary_image[nof_columns * row_index + column_index] = image[row_index][column_index] <= treshold;
		}
	}

	return binary_image;
}

inline char* image_classifier::convert_to_number(const bool* binary_image, const unsigned short nof_rows, const unsigned short nof_columns)
{
	const auto digits = "0123456789ABCDEF";
	const auto int_size = sizeof(int);
	const auto nof_cells = nof_rows * nof_columns;
	const auto image_as_numbers = new char[nof_cells / int_size + 1];
	for (auto cell_index = 0; cell_index < nof_cells; cell_index += 4)
	{
		const auto add_8 = binary_image[cell_index];
		const auto add_4 = cell_index + 1 < nof_cells && binary_image[cell_index + 1];
		const auto add_2 = cell_index + 2 < nof_cells && binary_image[cell_index + 2];
		const auto add_1 = cell_index + 3 < nof_cells && binary_image[cell_index + 3];

		image_as_numbers[cell_index / int_size] = digits[(add_8 ? 8 : 0) | (add_4 ? 4 : 0) | (add_2 ? 2 : 0) | (add_1 ? 1 : 0)];
	}
	return image_as_numbers;
}
