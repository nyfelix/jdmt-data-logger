#pragma once
#include <cmath>

class image_classifier
{
public:
	explicit image_classifier(const double* coef, const unsigned short nof_rows, const unsigned short nof_columns) :
		coef_{ coef },
		nof_rows_{ nof_rows },
		nof_columns_{ nof_columns }
	{	}
	void compress_image(double **img) const;
	double predict(double **img, unsigned short nof_rows, unsigned short nof_columns) const;
	double calculate_treshhold(double **image) const;
	void convert_to_binary(double **image) const;

private:
	const double*coef_;
	unsigned short nof_rows_;
	unsigned short nof_columns_;
	const double euler_ = std::exp(1.0);
};

