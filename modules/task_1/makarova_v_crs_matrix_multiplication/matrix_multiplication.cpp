// Copyright 2020 Makarova Viktoria

#include <ctime>
#include <random>
#include <stdexcept>
#include <vector>

#include <iostream>

#include "../../modules/task_1/makarova_v_crs_matrix_multiplication/matrix_multiplication.h"

Matrix generateRandomMat(int rows, int cols) {
    std::mt19937 gen;
    gen.seed((unsigned)time(0));
    std::uniform_int_distribution<int> dis(-255, 255);
    Matrix result(rows, cols);
    for (int i = 0; i < rows * cols; ++i) {
        if(dis(gen) > 128)
            result.val[i] = dis(gen);
        else
            result.val[i] = 0;
    }
    return result;
}

MatrixCRS generateRandomCRSMat(int rows, int cols) {
    return convert(generateRandomMat(rows, cols));
}

MatrixCRS convert(const Matrix &inMat) {
    MatrixCRS result;
    result.rows = inMat.rows;
    result.cols = inMat.cols;

    int count;

    count = 1;
    for (int j = 0; j < inMat.cols; ++j){
        if (inMat.val[/*inMat.cols * i  +*/ j] != 0) {
            ++count;
            result.val.emplace_back(inMat.val[/*inMat.cols * i +*/ j]);
            result.cols_pos.emplace_back(j);
        }
    }
    result.ptrs.emplace_back(count);

    for (int i = 1; i < inMat.rows; ++i){
        count = 0;
        for (int j = 0; j < inMat.cols; ++j){
            if (inMat.val.at(inMat.cols * i + j) != 0) {
                ++count;

                result.val.emplace_back(inMat.val[inMat.cols * i + j]);
                result.cols_pos.emplace_back(j);
            }
        }

        count += result.ptrs.back();
        result.ptrs.emplace_back(count);
    }

    return result;
}

MatrixCRS transp(const MatrixCRS &inMat) {
    // generate out mat
    Matrix just_mat(inMat.rows, inMat.cols);

    for (int j = 1; j < inMat.ptrs[0]; ++j) {
        just_mat.val[inMat.cols_pos[j - 1]] = inMat.val[j - 1];
    }
    for (size_t i = 1; i < inMat.ptrs.size(); ++i) {
        // i its rows
        for (int j = inMat.ptrs[i - 1]; j < inMat.ptrs[i]; ++j) {
            just_mat.val[just_mat.cols * i + inMat.cols_pos[j - 1]] = inMat.val[j - 1];
        }
    }

    // get tr matCRS from out mat
    MatrixCRS result;
    result.cols = inMat.cols;
    result.rows = inMat.rows;
    int count;

    count = 1;
    for (int i = 0; i < just_mat.rows; ++i){
        if (just_mat.val[just_mat.cols * i] != 0) {
            ++count;
            result.val.emplace_back(just_mat.val[inMat.cols * i]);
            result.cols_pos.emplace_back(i);
        }
    }
    result.ptrs.emplace_back(count);

//    for (size_t i = 0; i < result.val.size(); ++i)
//        std::cout<< result.val[i] << ' ';
//    std::cout << std::endl;

    for (int j = 1; j < just_mat.rows; ++j) {
        count = 0;
        for (int i = 0; i < just_mat.cols; ++i){
            if (just_mat.val[just_mat.cols * i + j] != 0) {
                ++count;
                //std::cout<< 
                result.val.emplace_back(just_mat.val[inMat.cols * i + j]);
                result.cols_pos.emplace_back(i);
            }
        }

        count += result.ptrs.back();
        result.ptrs.emplace_back(count);
    }

    return result;
}

//MatrixCRS matrixCRSMult(const MatrixCRS &first, const MatrixCRS &second);

Matrix matrixMult(const Matrix &first, const Matrix &second) {
    if (first.cols != second.rows)
        throw std::runtime_error("Matrix dimensions do not match");

    Matrix out(first.rows, second.cols);

    // Now!!! i, j - its out matrix
    for (int i = 0; i < out.rows; ++i)
        for (int j = 0; j < out.cols; ++j) {
            int res;
            for (int k = 0; k < first.cols; ++k)
                res += first.val[first.cols * i + k] *
                       second.val[second.cols * k + j];
            out.val[out.cols * i + j];
        }

    return out;
}
