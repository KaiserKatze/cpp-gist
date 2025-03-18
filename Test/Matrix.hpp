#pragma once

#include <cassert>
#include <cstring>
#include <initializer_list>
#include <vector>
#include <iterator> // std::distance
#include <algorithm> // std::copy, std::min
#include <string>
#include <sstream>
#include <stdexcept>

template <class ElemType>
struct Matrix {
    using MatrixRow = std::vector<ElemType>;
    std::vector<MatrixRow> data;
    size_t nRows;
    size_t nColumns;

    Matrix(size_t nRows, size_t nColumns)
        : nRows{ nRows }, nColumns{ nColumns },
        data(nRows, MatrixRow(nColumns)) {
    }

    Matrix(size_t nRows, size_t nColumns, ElemType value)
        : nRows{ nRows }, nColumns{ nColumns },
        data(nRows, MatrixRow(nColumns, value)) {
    }

    MatrixRow& operator[](size_t row) {
        return data[row];
    }

    Matrix sum_all() const {
        Matrix result(1, 1, 0);
        for (size_t i = 0; i < nRows; ++i) {
            for (size_t j = 0; j < nColumns; ++j) {
                result[0][0] += data[i][j];
            }
        }
        return result;
    }

    Matrix sum_row() const {
        Matrix result(nRows, 1, 0);
        for (size_t i = 0; i < nRows; ++i) {
            for (size_t j = 0; j < nColumns; ++j) {
                result[i][0] += data[i][j];
            }
        }
        return result;
    }

    Matrix sum_column() const {
        Matrix result(1, nColumns, 0);
        for (size_t i = 0; i < nRows; ++i) {
            for (size_t j = 0; j < nColumns; ++j) {
                result[0][j] += data[i][j];
            }
        }
        return result;
    }

    Matrix& operator=(std::initializer_list<ElemType> data) {
        if (nRows * nColumns != data.size()) {
            throw std::runtime_error{ "矩阵大小不一致!" };
        }
        auto it = data.begin();
        for (size_t i = 0; i < nRows; ++i) {
            for (size_t j = 0; j < nColumns; ++j) {
                this->data[i][j] = *it;
                ++it;
            }
        }
        return *this;
    }

    Matrix& operator=(std::initializer_list<std::initializer_list<ElemType>> data) {
        size_t nColumns{ 0 };
        for (const auto& row : data) {
            if (nColumns == 0) {
                nColumns = row.size();
            }
            else if (nColumns != row.size()) {
                throw std::runtime_error{ "各行向量维数不同!" };
            }
        }
        this->nRows = data.size();
        this->nColumns = nColumns;
        size_t i{ 0 };
        for (const auto& row : data) {
            this->data[i] = row;
            ++i;
        }
        return *this;
    }

    Matrix& operator+=(const Matrix& other) {
        for (size_t i = 0; i < nRows; ++i) {
            for (size_t j = 0; j < nColumns; ++j) {
                data[i][j] += other.data[i][j];
            }
        }
        return *this;
    }

    Matrix& operator-=(const Matrix& other) {
        for (size_t i = 0; i < nRows; ++i) {
            for (size_t j = 0; j < nColumns; ++j) {
                data[i][j] -= other.data[i][j];
            }
        }
        return *this;
    }

    Matrix operator-(const Matrix& other) const {
        Matrix result(*this); // 复制
        result -= other;
        return result;
    }

    explicit operator std::string() const {
        static constexpr ElemType Infty{ std::numeric_limits<ElemType>::max() };
        std::stringstream ss;
        ss << "{\n";
        for (size_t i = 0; i < nRows; ++i) {
            ss << "\t{";
            for (size_t j = 0; j < nColumns; ++j) {
                if (j > 0) ss << ", ";
                const ElemType& entry{ data[i][j] };
                if (entry == Infty) ss << "∞";
                else                ss << entry;
            }
            ss << "},\n";
        }
        ss << "}";
        return ss.str();
    }
};

template <class ElemType>
std::vector<ElemType>& operator-=(std::vector<ElemType>& vec, Matrix<ElemType>& mat) {
    size_t len;
    if (mat.nRows == 1) { // 矩阵 mat 是一个行向量
        len = mat.nColumns;
    }
    else if (mat.nColumns == 1) { // 矩阵 mat 是一个列向量
        len = mat.nRows;
    }
    else {
        throw std::invalid_argument{ "矩阵不是只有1行或1列!" };
    }
    if (len != vec.size()) { // 向量 vec 与 mat 的长度不一致
        throw std::invalid_argument{ "向量长度不一致!" };
    }
    if (mat.nRows == 1) { // 矩阵 mat 是一个行向量
        for (size_t i = 0; i < len; ++i) {
            vec[i] -= mat[0][i];
        }
    }
    else { // 矩阵 mat 是一个列向量
        for (size_t i = 0; i < len; ++i) {
            vec[i] -= mat[i][0];
        }
    }
    return vec;
}

template <class ElemType>
std::vector<ElemType> operator-(const std::vector<ElemType>& vec, const Matrix<ElemType>& mat) {
    std::vector<ElemType> result{ vec };
    vec -= mat;
    return result;
}

#if 0
template <typename ElemType>
struct Matrix {
    Matrix(int nRows, int nColumns)
        : nr{ nRows }, nc{ nColumns } {
        data.resize(nr * nc);
    }
    Matrix(int nRows, int nColumns, ElemType value)
        : nr{ nRows }, nc{ nColumns }, data(nr* nc, value) {
    }
    Matrix(int nRows, int nColumns, std::initializer_list<ElemType> il) : Matrix(nRows, nColumns) {
        CloneMatrixData(il);
    }
    Matrix(const Matrix& other) : Matrix(other.nr, other.nc) {
        CloneMatrixData(other.data);
    }
    void Fill(ElemType value) {
        for (int i = 0; i < nr * nc; ++i) data[i] = value;
    }
    int GetNRow() { return nr; }
    int GetNCol() { return nc; }
    ElemType& At(int row, int column) {
        return this->data[row * this->nc + column];
    }

    struct MatrixRow {
        MatrixRow(Matrix* matrix, int row) : matrix{ matrix }, row{ row } {}
        ElemType& operator[](int column) {
            if (0 <= column && column < matrix->nc)
                return matrix->At(row, column);
            throw "out of range!";
        }
        ElemType Sum() {
            ElemType result{ (*this)[0] };
            for (int i = 1; i < matrix->nc; ++i) result += (*this)[i];
            return result;
        }
    private:
        Matrix* matrix;
        int row;
    };
    MatrixRow operator[](int row) {
        if (0 <= row && row < nr)
            return MatrixRow{ this, row };
        throw "out of range!";
    }
    ElemType Sum() { // 求出矩阵中各元素之和
        ElemType result{ (*this)[0].Sum() };
        for (int i = 1; i < nr; ++i) result += (*this)[i].Sum();
        return result;
    }
    template <class Callable>
    int Count(Callable callable) const {
        int result{};
        for (ElemType e : data)
            if (callable(e))
                ++result;
        return result;
    }
    ElemType Max() const { // 求出矩阵中各元素的最大值
        ElemType result{ data[0] };
        for (auto it = std::cbegin(data);
            ++it < std::cend(data);)
            if (*it > result)
                result = *it;
        return result;
    }
    void Replace(ElemType pattern, ElemType value) { // 将矩阵中等于 `pattern` 的元素置为 `value`
        for (auto it = std::begin(data); it < std::end(data); ++it)
            if (*it == pattern)
                *it = value;
    }
    explicit operator std::string() {
        std::stringstream ss;
        ss << "{\n";
        for (int i = 0; i < nr; ++i) {
            ss << "\t{";
            for (int j = 0; j < nc; ++j) {
                if (j > 0) ss << ", ";
                ss << this->At(i, j);
            }
            ss << "},\n";
        }
        ss << "}";
        return ss.str();
    }
private:
    int nr; // 行数
    int nc; // 列数
    std::vector<ElemType> data;

    template <class DataSource>
    void CloneMatrixData(const DataSource& dataSource) {
        auto minDistance{ std::min(
            std::distance(std::begin(dataSource), std::end(dataSource)),
            std::distance(std::begin(data), std::end(data))
        ) };
        auto itBegin{ std::begin(dataSource) };
        auto itEnd{ itBegin + minDistance };
        std::copy(itBegin, itEnd, std::begin(data));
    }
};

#endif
