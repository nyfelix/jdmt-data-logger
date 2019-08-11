
class image_manipulator
{
public:
       static double calculate_treshold(unsigned char* image, unsigned short nof_cells);
       static unsigned short compromise_image_return_new_index(unsigned char* image, const unsigned short nof_cells);

private:
};

inline double image_manipulator::calculate_treshold(unsigned char* image, const unsigned short nof_cells)
{
       double sum = 0;
       for (auto index = 0; index < nof_cells; index++)
       {
             sum += image[index];
       }
       return sum / nof_cells;
}

inline bool is_cell_lte_treshold(const unsigned char* image, const double treshold, const unsigned short index)
{
       return image[index] <= treshold;
}

inline unsigned short image_manipulator::compromise_image_return_new_index(unsigned char* image, const unsigned short nof_cells)
{
       const auto digits = "0123456789ABCDEF";
       const auto int_size = sizeof(int);
       const auto treshold = calculate_treshold(image, nof_cells);

       for (auto index = 0; index < nof_cells; index += 4)
       {
             const auto add_8 = is_cell_lte_treshold(image, treshold, index);
             const auto add_4 = index + 1 < nof_cells && is_cell_lte_treshold(image, treshold, index + 1);
             const auto add_2 = index + 2 < nof_cells && is_cell_lte_treshold(image, treshold, index + 2);
             const auto add_1 = index + 3 < nof_cells && is_cell_lte_treshold(image, treshold, index + 3);

             image[index / int_size] = digits[(add_8 ? 8 : 0) | (add_4 ? 4 : 0) | (add_2 ? 2 : 0) | (add_1 ? 1 : 0)];
       }

       return nof_cells / int_size + 1;
}
