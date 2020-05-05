// Copyright 2020 Makarova Viktoria
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <ctime>
#include "./matrix_multiplication.h"

TEST(Gaussian_Image_Filtering_seq, Simple_Convert) {
    const int rows = 3, cols = 4;

    std::vector<std::complex<int>> mat_val = {1, 0, 2, 0,
                                              0, 3, 0, 0,
                                              4, 0, 5, 0};
    Matrix mat(rows, cols);
    mat.val = mat_val;

    std::vector<std::complex<int>> c_vals = {1, 2, 3, 4, 5};
    std::vector<int> c_cols = {0, 2, 1, 0, 2};
    std::vector<int> c_ptrs = {0, 2, 3, 5};

    MatrixCRS cRSMat = convert(mat);

    EXPECT_EQ(cRSMat.val, c_vals);
    EXPECT_EQ(cRSMat.cols_pos, c_cols);
    EXPECT_EQ(cRSMat.ptrs, c_ptrs);
}

TEST(Gaussian_Image_Filtering_seq, Convert_first) {
    const int rows = 4, cols = 4;
    Matrix mat(rows, cols);

    std::vector<std::complex<int>> mat_val = {1, 0, 0, 0,
                                0, 3, 0, 0,
                                0, 0, 5, 0,
                                0, 0, 0, 4};

    std::vector<std::complex<int>> c_vals = {1, 3, 5, 4};
    std::vector<int> c_cols = {0, 1, 2, 3};
    std::vector<int> c_ptrs = {0, 1, 2, 3, 4};

    mat.val = mat_val;
    MatrixCRS cRSMat = convert(mat);

    EXPECT_EQ(cRSMat.val, c_vals);
    EXPECT_EQ(cRSMat.cols_pos, c_cols);
    EXPECT_EQ(cRSMat.ptrs, c_ptrs);
}

TEST(Gaussian_Image_Filtering_seq, Random_Mat) {
    const int rows = 3;
    const int cols = 4;

    Matrix matrix = generateRandomMat(rows, cols);

    EXPECT_EQ(matrix.val.size(), static_cast<size_t>(rows * cols));
}

TEST(Gaussian_Image_Filtering_seq, Convert) {
    const int rows = 3;
    const int cols = 4;

    Matrix matrix = generateRandomMat(rows, cols);

    size_t count = 0;
    std::vector<std::complex<int>> c_val;
    for (size_t i = 0; i < matrix.val.size(); ++i)
        if (matrix.val[i] != 0) {
            ++count;
            c_val.emplace_back(matrix.val[i]);
        }

    MatrixCRS matrixCRS = convert(matrix);

    EXPECT_EQ(matrixCRS.val.size(), count);
    EXPECT_EQ(matrixCRS.val, c_val);
}

TEST(Gaussian_Image_Filtering_seq, Transponation) {
    const int rows = 4, cols = 4;
    Matrix mat(rows, cols);

    std::vector<std::complex<int>> mat_val = {1, 0, 0, 0,
                                0, 0, 0, 4,
                                0, 0, 0, 0,
                                9, 0, 0, 1};

    mat.val = mat_val;

    MatrixCRS matrixCRS_tr(transp(convert(mat)));

    // 1, 0, 0, 9
    // 0, 0, 0, 0
    // 0, 0, 0, 0
    // 0, 4, 0, 1

    std::vector<std::complex<int>> c_vals = {1, 9, 4, 1};
    std::vector<int> c_cols = {0, 3, 1, 3};
    std::vector<int> c_ptrs = {0, 2, 2, 2, 4};

    EXPECT_EQ(matrixCRS_tr.val, c_vals);
    EXPECT_EQ(matrixCRS_tr.cols_pos, c_cols);
    EXPECT_EQ(matrixCRS_tr.ptrs, c_ptrs);
}

TEST(Gaussian_Image_Filtering_seq, B) {
    Matrix first(3, 3);
    Matrix second(3, 3);

    std::vector<std::complex<int>> first_val = {1, 0, 2,
                                 -1, 3, 0,
                                  0, 0, 3};

    std::vector<std::complex<int>> second_val = {0, 2, 0,
                                   4, 0, 0,
                                   0, 0, 1};

    std::vector<std::complex<int>> res_val = {0, 2, 2,
                               12, -2, 0,
                                0, 0, 3};

    first.val = first_val;
    second.val = second_val;

    Matrix res = matrixMult(first, second);

    EXPECT_EQ(res.val, res_val);
}

TEST(Gaussian_Image_Filtering_seq, C) {
    std::vector<std::complex<int>> first_val = {1, 0, 2,
                                 -1, 3, 0,
                                  0, 0, 3};
    Matrix fir(3, 3);
    fir.val = first_val;
    MatrixCRS first = convert(fir);
//    first.rows = first.cols = 3;
//    first.val = {1, 2, -1, 3, 3};
//    first.cols_pos = {0, 2, 0, 1, 2};
//    first.ptrs = {1, 3, 5, 6};

    std::vector<std::complex<int>> second_val = {0, 2, 0,
                                                 4, 0, 0,
                                                 0, 0, 1};
    Matrix sec(3, 3);
    sec.val = second_val;
    MatrixCRS second = convert(sec);

    Matrix re = matrixMult(fir, sec);
    MatrixCRS res = convert(re);

//    res.cols = res.rows = 3;
//    res.val = {2, 2, 12, -2, 3};
//    res.cols_pos = {1, 2, 0, 1, 2};
//    res.ptrs = {1, 3, 5, 6};

    MatrixCRS multRes = matrixCRSMult(first, second);

    EXPECT_EQ(res.val, multRes.val);
    EXPECT_EQ(res.cols_pos, multRes.cols_pos);
    EXPECT_EQ(res.ptrs, multRes.ptrs);
}

TEST(Gaussian_Image_Filtering_seq, D) {
    std::vector<std::complex<int>> first_val = {1, 0, 2,
                                               -1, 3, 0,
                                                0, 0, 3};
    Matrix fir(3, 3);
    fir.val = first_val;
    MatrixCRS first = convert(fir);

    std::vector<std::complex<int>> second_val = {0, 2, 0,
                                                 4, 0, 0,
                                                 0, 0, 1};
    Matrix sec(3, 3);
    sec.val = second_val;
    MatrixCRS second = convert(sec);

    Matrix re = matrixMult(fir, sec);
    MatrixCRS res = convert(re);


    MatrixCRS multRes = matrixCRSMultOMP(first, second);

    EXPECT_EQ(res.val, multRes.val);
    EXPECT_EQ(res.cols_pos, multRes.cols_pos);
    EXPECT_EQ(res.ptrs, multRes.ptrs);
}

TEST(Gaussian_Image_Filtering_seq, PerfRandMatrixTest) {
    int row, col;
    row = col = 350;

    std::cout<< "Start generating " << std::endl;
    Matrix  firstNorm  = generateRandomMat(row, col);
    Matrix  secondNorm = generateRandomMat(row, col);
    std::cout<< "End generating " << std::endl;

    std::cout<< "Start converting" << std::endl;
    MatrixCRS firstCRS  = convert(firstNorm);
    MatrixCRS secondCRS = convert(secondNorm);
    std::cout<< "End converting" << std::endl;

    std::cout<< "Start sec" << std::endl;
    clock_t secStart, secFinish;
    secStart = clock();
    MatrixCRS multRes = matrixCRSMult(firstCRS, secondCRS);
    secFinish = clock();
    double secTime = (double)(secFinish - secStart) / CLOCKS_PER_SEC; 
    std::cout<< "End sec" << std::endl;

    std::cout<< "Start paral" << std::endl;
    clock_t parStart, parFinish;
    parStart = clock();
    MatrixCRS multResOMP = matrixCRSMultOMP(firstCRS, secondCRS);
    parFinish = clock();
    double parTime = (double)(parFinish - parStart) / CLOCKS_PER_SEC; 
    std::cout<< "End paral" << std::endl;

    std::cout<< "secTime / parTime: " << secTime / parTime << std::endl;
    std::cout<< "secTime: " << secTime << std::endl;
    std::cout<< "parTime: " << parTime << std::endl;

    EXPECT_EQ(multRes.ptrs, multResOMP.ptrs);
    EXPECT_EQ(multRes.val, multResOMP.val);
    EXPECT_EQ(multRes.cols_pos, multResOMP.cols_pos);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
