#pragma once

class logistic_regression
{
  public:
       explicit logistic_regression(
             const double *coef,
             const unsigned short nof_rows,
             const unsigned short nof_columns,
             const double euler, double (*pow)(double, double)) : coef_{coef},
                                                                                                     nof_rows_{nof_rows},
                                                                                                     nof_columns_{nof_columns},
                                                                                                     euler_{euler},
                                                                                                     pow_{pow}
       {
       }
       double predict(double **matrix) const;
       double predict_compressed(unsigned char* matrix) const;

  private:
       const double *coef_;
       const unsigned short nof_rows_{};
       const unsigned short nof_columns_{};
       const double euler_{};
       double (*pow_)(double, double);
};
