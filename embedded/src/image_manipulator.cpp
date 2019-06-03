#include "image_manipulator.h"
#include "Debug.h"

float** image_manipulator::compress() const
{
	debug("compress");
	for (auto column_index = 0; column_index < nof_columns_; column_index++)
	{
		float row_sum = 0;
		for (auto row_index = 0; row_index < nof_rows_; row_index++)

		{
			debug(row_index);
			debug(image_[0][0]);
			row_sum += image_[row_index][column_index];
			image_[row_index][column_index] = 0;
		}
		debug(column_index);
		image_[0][column_index] = row_sum / nof_rows_;
	}
	return get();
}

float** image_manipulator::get() const
{
	return image_;
}

float image_manipulator::treshhold() const
{
	float sum = 0;
	for (auto row_index = 0; row_index < nof_rows_; row_index++)
	{
		for (auto column_index = 0; column_index < nof_columns_; column_index++)
		{
			sum += image_[row_index][column_index];
		}
	}
	return sum / nof_rows_ / nof_columns_;
}

bool* image_manipulator::binary_array() const
{
	const auto treshold = treshhold();
	const auto nof_items = nof_rows_ * nof_columns_;
	const auto binary_array_image = new bool[nof_items];

	for (auto row_index = 0; row_index < nof_rows_; row_index++)
	{
		for (auto column_index = 0; column_index < nof_columns_; column_index++)
		{
			const auto index = nof_columns_ * row_index + column_index;
			binary_array_image[index] = image_[row_index][column_index] <= treshold;
		}
	}

	return binary_array_image;
}

char* image_manipulator::hex_array() const
{
	const auto binary_array_image = binary_array();
	const auto digits = "0123456789ABCDEF";
	const auto int_size = sizeof(int);
	const auto nof_cells = nof_rows_ * nof_columns_;
	const auto image_as_numbers = new char[nof_cells / int_size + 1];
	for (auto cell_index = 0; cell_index < nof_cells; cell_index += 4)
	{
		const auto add_8 = binary_array_image[cell_index];
		const auto add_4 = cell_index + 1 < nof_cells && binary_array_image[cell_index + 1];
		const auto add_2 = cell_index + 2 < nof_cells && binary_array_image[cell_index + 2];
		const auto add_1 = cell_index + 3 < nof_cells && binary_array_image[cell_index + 3];

		image_as_numbers[cell_index / int_size] = digits[(add_8 ? 8 : 0) | (add_4 ? 4 : 0) | (add_2 ? 2 : 0) | (add_1 ? 1 : 0)];
	}
	return image_as_numbers;
}
