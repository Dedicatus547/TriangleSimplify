#ifndef MATRIX_H_
#define MATRIX_H_
#include <array>

template<typename ty, size_t n, size_t m>
struct Matrix : public std::array<std::array<ty, m>, n> {
    Matrix<ty, n, m> operator*(double a) const {
        Matrix<ty, n, m> ret;
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < m; ++j) {
                ret[i][j] = (*this)[i][j]*a;
            }
        return ret;
    }
    Matrix<ty, n, m> operator+(const Matrix<ty, n, m>& a) const {
        Matrix<ty, n, m> ret;
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < m; ++j) {
                ret[i][j] = (*this)[i][j] + a[i][j];
            }
        return ret;
    }
    Matrix<ty, n, m> operator-(const Matrix<ty, n, m>& a) const {
        Matrix<ty, n, m> ret;
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < m; ++j) {
                ret[i][j] = (*this)[i][j] - a[i][j];
            }
        return ret;
    }
    template<size_t l>
    Matrix<ty, n, l> operator*(const Matrix<ty, m, l>& a) const {
        Matrix<ty, n, l> ret;
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < l; ++j) {
                ret[i][j] = 0;
                for (size_t k = 0; k < m; ++k) 
                    ret[i][j] += (*this)[i][k]*a[k][j];
            }
        return ret;
    }
    Matrix<ty, m, n> transpose() const {
        Matrix<ty, m, n> ret;
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < m; ++j) 
                ret[j][i] = (*this)[i][j];
        return ret;
    }
};

#endif