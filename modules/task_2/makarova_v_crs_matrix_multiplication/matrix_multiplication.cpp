// Copyright 2020 Makarova Viktoria
#include <ctime>
#include <random>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>

#include <omp.h>

#include "../../modules/task_2/makarova_v_crs_matrix_multiplication/matrix_multiplication.h"

Matrix generateRandomMat(int rows, int cols) {
    std::mt19937 gen;
    gen.seed((unsigned)time(0));
    std::uniform_int_distribution<int> dis(-255, 255);
    Matrix result(rows, cols);
    for (int i = 0; i < rows * cols; ++i) {
        if (dis(gen) > 200) {
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

MatrixCRS transpOMP(const MatrixCRS &inMat) {
    // generate out mat
    Matrix just_mat(inMat.rows, inMat.cols);

#pragma omp parallel for
    for (size_t i = 0; i < inMat.ptrs.size() - 1; ++i) {
        // i its rows
        for (int j = inMat.ptrs[i]; j < inMat.ptrs[i + 1]; ++j) {
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

    return result;
}


MatrixCRS transp(const MatrixCRS &inMat) {
    // generate out mat
    Matrix just_mat(inMat.rows, inMat.cols);

    for (size_t i = 0; i < inMat.ptrs.size() - 1; ++i) {
        // i its rows
        for (int j = inMat.ptrs[i]; j < inMat.ptrs[i + 1]; ++j) {
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


MatrixCRS matrixCRSMultOMP(const MatrixCRS &first, const MatrixCRS &second_a) {

    if (first.cols != second_a.rows)
        throw std::runtime_error("Matrix dimensions do not match");

    MatrixCRS second = transp(second_a);

    if (first.ptrs.size() != second.ptrs.size())
            throw std::runtime_error("AAAAAA");

    MatrixCRS out;
    out.rows = first.rows;
    out.cols = second.cols;

// MPI -технология распараллеливания уровня процессов 
// OMP и TBB - уровня потоков
// В нашей ситуации основное различия процессов и потоков заключается в том,
// что процессы имеют собственное адресное пространство, а потоки - общее
// Из-за этого в MPI мы посылали данные к процессам, а потом собирали результаты
// Сейчас же потоки будут работать над одной памятью и будут конкурировать за нее
// Наша задача - организовать рабоу потоков так, чтобы они меньше всего менаши друг другу

// Впрочем, подход распарралеливания MPI и OMP достаточно похожи
// Код парралельной области в OMP будт выполняться параллельно всеми потоками

// Идея алгоритма заключается в том, чтобы создать несколько массивово vals и col_pos для аждого потока,
// заполнить их и потом собрать обратно.

    // Временный масссив ptrs
    std::vector<int> tmpResultPtrs(first.ptrs.size(), 0);
    // Временный масссив массивово col_pos
    std::vector<int>* tmpResultColsPose = new std::vector<int>[first.ptrs.size() - 1];
    // Временный масссив ассивов vals
    std::vector<std::complex<int>>* tmpResultValue = new std::vector<std::complex<int>>[first.ptrs.size() - 1];

    omp_set_num_threads(4);
    int k;
    size_t j;

// Начало параллельной области
// Даем кажому потоку обрабатывать отдельную строку матрицы first
// "schedule(static, 32)" означает, что каждый поток будео обрабатывать пепрерывный участок размером 32 строки
#pragma omp for private(j, k) schedule(static, 32)
    for (size_t i = 0; i < first.ptrs.size() - 1; ++i) { // START PRALLEL
        // Здесь все очень похоже на последовательную реализацию
        std::vector<std::complex<int>> tmpVals(first.ptrs.size() - 1, std::complex<int>(0, 0));

        int rowNZ = 0;
        // Скалярное умножение векторов (замена цикла while() в последовательной версии)
        for (j = 0; j < second.ptrs.size() - 1; j++) {
            std::complex<int> sum(0, 0);
            for (k = first.ptrs[i]; k < first.ptrs[i + 1]; k++) {
                for (int l = second.ptrs[j]; l < second.ptrs[j + 1]; l++) {
                    if (first.cols_pos[k] == second.cols_pos[l]) {
                        sum += first.val[k] * second.val[l];
                        break;
                    }
                }
            }

            if (sum != std::complex<int>(0, 0)) {
                // Здесь i означает некоторую область юрисдикции каждого потока
                // i каждый поток в один момент времени "захватывает" только один конкретный i
                tmpResultValue[i].push_back(sum);
                tmpResultColsPose[i].push_back(j);
                rowNZ++;
            }
        }
        tmpResultPtrs[i] = rowNZ;        
    } // END PRALLEL
    // Здесь у нас есть готовые массивы ptrs col_pos vals, осталось только аккуратно их проинициализировать

    // Инициализируем ptrs
    int tmpRows = 0;
    out.ptrs.resize(first.ptrs.size());
    for (size_t i = 0; i < out.ptrs.size(); ++i) {
        int tmp = tmpResultPtrs[i];
        out.ptrs[i] = tmpRows;
        tmpRows += tmp;

    }
    out.ptrs[first.ptrs.size()] = tmpRows;

    // Выделяем место в резуьтирующей матрице, чтобы скопировать
    out.cols_pos.resize(tmpRows);
    out.val.resize(tmpRows);
    int count = 0;
    // инициализируем col_pos и vals
    for (size_t i = 0; i < out.ptrs.size() - 1; ++i) {
        int size = tmpResultColsPose[i].size();
        std::memcpy(&out.cols_pos[count], &tmpResultColsPose[i][0], size * sizeof(int));        
        std::memcpy(&out.val[count], &tmpResultValue[i][0], size * sizeof(std::complex<int>));
        count += size;
    }

    // Возможно, это можно закомментировать, можшь проверить ))))))))
    delete[]tmpResultColsPose;
    delete[]tmpResultValue;

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
