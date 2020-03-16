// Copyright 2020 Makarova Viktoria

#include <vector>
#include <iostream>

#include <gtest/gtest.h>
#include "./matrix_multiplication.h"

TEST(Gaussian_Image_Filtering_seq, Can_Filter_Small_Image) {
    const int rows = 3, cols = 4;
    Matrix mat(rows, cols);

    std::vector<int> mat_val = {1, 0, 2, 0,
                                0, 3, 0, 0,
                                4, 0, 5, 0};

    std::vector<int> c_vals = {1, 2, 3, 4, 5};
    std::vector<int> c_cols = {0 ,2, 1, 0, 2};
    std::vector<int> c_ptrs = {3, 4, 6}; // !!!

    mat.val = mat_val;
    MatrixCRS cRSMat = convert(mat);

    ASSERT_EQ(cRSMat.val, c_vals);
    ASSERT_EQ(cRSMat.cols_pos, c_cols);
    ASSERT_EQ(cRSMat.ptrs, c_ptrs);
}

TEST(Gaussian_Image_Filtering_seq, Convert_first) {
    const int rows = 4, cols = 4;
    Matrix mat(rows, cols);

    std::vector<int> mat_val = {1, 0, 0, 0,
                                0, 1, 0, 0,
                                0, 0, 1, 0,
                                0, 0, 0, 1};

    std::vector<int> c_vals = {1, 1, 1, 1};
    std::vector<int> c_cols = {0, 1, 2, 3};
    std::vector<int> c_ptrs = {2, 3, 4, 5}; // !!!

    mat.val = mat_val;
    MatrixCRS cRSMat = convert(mat);

    ASSERT_EQ(cRSMat.val, c_vals);
    ASSERT_EQ(cRSMat.cols_pos, c_cols);
    ASSERT_EQ(cRSMat.ptrs, c_ptrs);
}

TEST(Gaussian_Image_Filtering_seq, Random_Mat) {
    const int rows = 3;
    const int cols = 4;

    Matrix matrix = generateRandomMat(rows, cols);

    ASSERT_EQ(matrix.val.size(), static_cast<size_t>(rows * cols));
}

TEST(Gaussian_Image_Filtering_seq, Convert) {
    const int rows = 3;
    const int cols = 4;

    Matrix matrix = generateRandomMat(rows, cols);

    size_t count = 0;
    std::vector<int> c_val;
    for (size_t i = 0; i < matrix.val.size(); ++i)
        if (matrix.val[i] != 0){
            ++count;
            c_val.emplace_back(matrix.val[i]);
        }

    MatrixCRS matrixCRS = convert(matrix);

    ASSERT_EQ(matrixCRS.val.size(), count);
    ASSERT_EQ(matrixCRS.val, c_val);
}

TEST(Gaussian_Image_Filtering_seq, Transponation) {
    const int rows = 4, cols = 4;
    Matrix mat(rows, cols);

    std::vector<int> mat_val = {1, 0, 0, 0,
                                0, 0, 1, 0,
                                0, 0, 1, 0,
                                0, 0, 0, 1};

    mat.val = mat_val;

    MatrixCRS matrixCRS = convert(mat);

    MatrixCRS matrixCRS_tr = transp(matrixCRS);

    // 1, 0, 0, 0
    // 0, 0, 0, 0
    // 0, 1, 1, 0
    // 0, 0, 0, 1

    std::vector<int> c_vals = {1, 1, 1, 1};
    std::vector<int> c_cols = {0 ,1, 2, 3};
    std::vector<int> c_ptrs = {2, 2, 4, 5}; // !!!

    ASSERT_EQ(matrixCRS_tr.val, c_vals);
    ASSERT_EQ(matrixCRS_tr.cols_pos, c_cols);
    ASSERT_EQ(matrixCRS_tr.ptrs, c_ptrs);
}

TEST(Gaussian_Image_Filtering_seq, B) {
    Matrix first(3, 2);
    Matrix second(2, 3);

    std::vector<int> first_val = {1, 0,
                                  2, 1,
                                 -1, 1};

    std::vector<int> second_val = {1, 2, 0,
                                   0,-1, 1};

    std::vector<int> res_val = {1, 2, 0,
                                2, 3, 1,
                               -1,-3, 1};

    first.val = first_val;
    second.val = second_val;

    Matrix res = matrixMult(first, second);

    ASSERT_EQ(res.val, res_val);
}

/*
TEST(Gaussian_Image_Filtering_seq, Can_Handle_Incorrect_Image_Height) {


    const int width = 10;
    const int height = 10;

    Image img = generateRandomImage(width, height);

    ASSERT_ANY_THROW(gaussianFilter(img, width, height - 1));
}

TEST(Gaussian_Image_Filtering_seq, Can_Handle_Incorrect_Image_Width) {
    const int width = 10;
    const int height = 10;

    Image img = generateRandomImage(width, height);

    ASSERT_ANY_THROW(gaussianFilter(img, width + 1, height));
}
*/

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
