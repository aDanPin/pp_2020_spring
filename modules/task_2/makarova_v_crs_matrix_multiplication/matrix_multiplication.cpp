// Copyright 2020 Makarova Viktoria
#include <ctime>
#include <random>
#include <stdexcept>
#include <vector>
#include <iostream>

#include <omp.h>

#include "../../modules/task_2/makarova_v_crs_matrix_multiplication/matrix_multiplication.h"

Matrix generateRandomMat(int rows, int cols) {
    std::mt19937 gen;
    gen.seed((unsigned)time(0));
    std::uniform_int_distribution<int> dis(-255, 255);
    Matrix result(rows, cols);
    for (int i = 0; i < rows * cols; ++i) {
        if (dis(gen) > 128) {
            result.val[i] = std::complex<int>(dis(gen), dis(gen));
        }
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

    count = 0;
    result.ptrs.emplace_back(count);
    for (int i = 0; i < inMat.rows; ++i) {
        count = 0;
        for (int j = 0; j < inMat.cols; ++j) {
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

#pragma omp parallel for
    for (size_t i = 0; i < inMat.ptrs.size() - 1; ++i) {
        // i its rows
        for (int j = inMat.ptrs[i]; j < inMat.ptrs[i + 1]; ++j) {
//            std::cout<< "I : " << i<< " J : "<< j<< " index: "<< just_mat.cols * (i) + inMat.cols_pos[j] <<std::endl;
            just_mat.val[just_mat.cols * (i) + inMat.cols_pos[j]] = inMat.val[j];
        }
    }

    // get tr matCRS from out mat
    MatrixCRS result;
    result.cols = inMat.cols;
    result.rows = inMat.rows;
    int count;

    count = 0;
    result.ptrs.emplace_back(0);
    for (int j = 0; j < just_mat.rows; ++j) {
        count = 0;
        for (int i = 0; i < just_mat.cols; ++i) {
            if (just_mat.val[just_mat.cols * i + j] != 0) {
                ++count;
                result.val.emplace_back(just_mat.val[inMat.cols * i + j]);
                result.cols_pos.emplace_back(i);
            }
        }

        count += result.ptrs.back();
        result.ptrs.emplace_back(count);
    }

    std::cout<< 3 <<std::endl;

    return result;
}

Matrix matrixMult(const Matrix &first, const Matrix &second) {
    if (first.cols != second.rows)
        throw std::runtime_error("Matrix dimensions do not match");

    Matrix out(first.rows, second.cols);

    // Now!!! i, j - its out matrix
    for (int i = 0; i < first.rows; ++i)
        for (int j = 0; j < second.cols; ++j) {
            std::complex<int> res = 0;
            for (int k = 0; k < first.cols; ++k)
                res = res + first.val[first.cols * i + k] *
                       second.val[second.cols * k + j];
            out.val[out.cols * i + j] = res;
        }

    return out;
}

MatrixCRS matrixCRSMult(const MatrixCRS &first, const MatrixCRS &second_a) {
    if (first.cols != second_a.rows)
        throw std::runtime_error("Matrix dimensions do not match");

    MatrixCRS second = transp(second_a);

    if (first.ptrs.size() != second.ptrs.size())
            throw std::runtime_error("AAAAAA");

    MatrixCRS out;
    out.rows = first.rows;
    out.cols = second.cols;

    print(first);
    print(second);
    // int first_it;
    // int second_it;
    int rowNZ = 0;
    out.ptrs.emplace_back(0);
    for (size_t i = 0; i < first.ptrs.size() - 1; i++) {
        rowNZ = 0;
        for (size_t j = 0; j < second.ptrs.size() - 1; j++) {
            std::complex<int> sum(0, 0);
            for (int k = first.ptrs[i]; k < first.ptrs[i + 1]; k++) {
                for (int l = second.ptrs[j]; l < second.ptrs[j + 1]; l++) {
                    if (first.cols_pos[k] == second.cols_pos[l]) {
                        sum += first.val[k] * second.val[l];

                        std::cout<<"col pos "<< first.cols_pos[k] << ' '<< second.cols_pos[l] <<std::endl;
                        std::cout<<"K L "<< k << ' '<< l <<std::endl;

                        std::cout<<sum.real() << ' '<< sum.imag()<<std::endl;


                        break;
                    }
                }
            }


            if (sum != std::complex<int>(0, 0)) {
                out.val.push_back(sum);
                out.cols_pos.push_back(j);
                rowNZ++;
            }
        }
        out.ptrs.push_back(rowNZ + out.ptrs[i]);
    }

    return out;
}

void print(const MatrixCRS &in) {
    std::cout <<"vals: "<< std::endl;
    for (size_t i = 0; i < in.val.size(); i++)
        std::cout << in.val[i]<< ' ';
    std::cout << std::endl;

    std::cout <<"cols: "<< std::endl;
    for (size_t i = 0; i < in.cols_pos.size(); i++)
        std::cout << in.cols_pos[i] << ' ';
    std::cout<< std::endl;

    std::cout <<"ptrs: "<< std::endl;
    for (size_t i = 0; i < in.ptrs.size(); i++)
        std::cout << in.ptrs[i] << ' ';
    std::cout<< std::endl;
}
