#include "logistic_regression.h"
#include "Debug.h"

double logistic_regression::predict(double **matrix) const
{
       double sum = 0;
       for (auto row_index = 0; row_index < nof_rows_; row_index++)
       {
             for (auto column_index = 0; column_index < nof_columns_; column_index++)
             {
                    sum -= matrix[row_index][column_index] * coef_[nof_columns_ * row_index + column_index];
             }
       }
       return 1 / (1 + pow_(euler_, sum));
}


double logistic_regression::predict_compressed(unsigned char* matrix) const
{
       double sum = 0;
       double tmp = 0;
       for (auto column_index = 0; column_index < nof_columns_; column_index++)
       {
       

             for (auto row_index = 0; row_index < nof_rows_; row_index++)
             {
                   
                    tmp += matrix[row_index*nof_columns_+ column_index] / nof_rows_;
             }
             sum -= tmp * coef_[column_index];
       }
       return 1 / (1 + pow_(euler_, sum));
}
