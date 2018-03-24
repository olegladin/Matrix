// Ladin Oleg, 2018

#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

// Debug print
#ifdef MATRIX_DEBUG
 #define MATRIX_DEBUG_(x) x
 #ifdef _MSC_VER // for Visual Studio
  #define FUNC_NAME_ __FUNCSIG__
 #else
  #define FUNC_NAME_ __func__
 #endif
#else
 #define MATRIX_DEBUG_(x)
#endif

// The maximum size (in bytes) of the matrix being allocated on the stack
#ifdef MATRIX_DATA_STORAGE_STACK_SIZE_MAX
 #define MATRIX_DATA_STORAGE_STACK_SIZE_MAX_ MATRIX_DATA_STORAGE_STACK_SIZE_MAX
#else
 #define MATRIX_DATA_STORAGE_STACK_SIZE_MAX_ 1024
#endif



namespace matrix {

// Type of matrix data storage
enum class MatrixDataStorage {
    UNSPECIFIED, // stack or heap storage will be chosen based on matrix size
    STACK,       // allocates matrix on stack
    HEAP,        // allocates matrix on heap
    USER         // places matrix in the memory given by the pointer
};

// Chooses the best storage for the matrix with giving size
constexpr MatrixDataStorage choose_matrix_data_storage(const size_t size) {
    return (size > MATRIX_DATA_STORAGE_STACK_SIZE_MAX_) ? MatrixDataStorage::HEAP : MatrixDataStorage::STACK;
}

// Chooses storage based on storages of giving two matrices. Decision is done
// according to the following diagram:
//     x   UNSP. STACK HEAP  USER
//   UNSP. unsp. stack heap  unsp.
//   STACK stack stack unsp. stack
//   HEAP  heap  unsp. heap  heap
//   USER  unsp. stack heap  unsp.
// For unspecified result the matrix storage will be chosen based on matrix size.
constexpr MatrixDataStorage result_matrix_data_storage(const MatrixDataStorage lhs, const MatrixDataStorage rhs = MatrixDataStorage::UNSPECIFIED) {
    switch (lhs) {
      case MatrixDataStorage::UNSPECIFIED:
      case MatrixDataStorage::USER:
        return (rhs == MatrixDataStorage::USER) ? MatrixDataStorage::UNSPECIFIED : rhs;
      case MatrixDataStorage::STACK:
        return (rhs == MatrixDataStorage::HEAP) ? MatrixDataStorage::UNSPECIFIED : MatrixDataStorage::STACK;
      case MatrixDataStorage::HEAP:
        return (rhs == MatrixDataStorage::STACK) ? MatrixDataStorage::UNSPECIFIED : MatrixDataStorage::HEAP;
    }
}



// Memory managemant of a matrix
template<typename T, size_t M, size_t N, MatrixDataStorage S>
class MatrixData; // only stack, heap and user types of memory are allowed

// Allocates matrix on stack. Suitable for small matrix size.
template<typename T, size_t M, size_t N>
class MatrixData<T, M, N, MatrixDataStorage::STACK> {
  private:
    T data_[M * N];

  public:
    MatrixData() {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK DATA constructor" << std::endl);
    }
    template<typename T_>
    explicit MatrixData(T_ &&val) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK DATA constructor (val)" << std::endl);
        for (size_t i = 0; i < M * N; ++i) {
            data_[i] = static_cast<T>(val);
        }
    }
    template<typename T_>
    explicit MatrixData(T_ *arr) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK DATA constructor (arr)" << std::endl);
        for (size_t i = 0; i < M * N; ++i) {
            data_[i] = static_cast<T>(arr[i]);
        }
    }
    template<typename T_>
    explicit MatrixData(std::initializer_list<T_> init) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK DATA constructor (init)" << std::endl);
        if (init.size() > M * N) {
            auto end = init.begin();
            std::advance(end, M * N); // prevent overflow
            size_t i = 0;
            for (auto it = init.begin(); it != end; ++it) {
                data_[i++] = static_cast<T>(*it);
            }
        } else {
            size_t i = 0;
            for (auto &val : init) {
                data_[i++] = static_cast<T>(val);
            }
            while (i < M * N) {
                data_[i++] = T(); // fill with default elements
            }
        }
    }

    ~MatrixData() = default;                                  // destructor
    MatrixData(const MatrixData &other) = default;            // copy constructor
    MatrixData(MatrixData &&other) = default;                 // move constructor
    MatrixData& operator=(const MatrixData &other) = default; // copy assignment
    MatrixData& operator=(MatrixData &&other) = default;      // move assignment

    const T* const read() const { return data_; } // read-only access
    T* write() { return data_; }                  // read and write access
};

// Allocates matrix on heap. Suitable for large matrix size.
template<typename T, size_t M, size_t N>
class MatrixData<T, M, N, MatrixDataStorage::HEAP> {
  private:
    T *data_;

  public:
    MatrixData() {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP DATA constructor" << std::endl);
        data_ = new T[M * N];
    }
    template<typename T_>
    explicit MatrixData(T_ &&val) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP DATA constructor (val)" << std::endl);
        data_ = new T[M * N];
        try {
            for (size_t i = 0; i < M * N; ++i) {
                data_[i] = static_cast<T>(val);
            }
        } catch (...) {
            // Free critical resource in case of exception in constructor
            delete[] data_;
            throw;
        }
    }
    template<typename T_>
    explicit MatrixData(T_ *arr) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP DATA constructor (arr)" << std::endl);
        data_ = new T[M * N];
        try {
            for (size_t i = 0; i < M * N; ++i) {
                data_[i] = static_cast<T>(arr[i]);
            }
        } catch (...) {
            // Free critical resource in case of exception in constructor
            delete[] data_;
            throw;
        }
    }
    template<typename T_>
    explicit MatrixData(std::initializer_list<T_> init) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP DATA constructor (init)" << std::endl);
        data_ = new T[M * N];
        try {
            if (init.size() > M * N) {
                auto end = init.begin();
                std::advance(end, M * N); // prevent overflow
                size_t i = 0;
                for (auto it = init.begin(); it != end; ++it) {
                    data_[i++] = static_cast<T>(*it);
                }
            } else {
                size_t i = 0;
                for (auto &val : init) {
                    data_[i++] = static_cast<T>(val);
                }
                while (i < M * N) {
                    data_[i++] = T(); // fill with default elements
                }
            }
        } catch (...) {
            // Free critical resource in case of exception in constructor
            delete[] data_;
            throw;
        }
    }

    ~MatrixData() {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP DATA destructor" << std::endl);
        delete[] data_;
    }
    MatrixData(const MatrixData &other) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP DATA copy constructor" << std::endl);
        data_ = new T[M * N];
        try {
            for (size_t i = 0; i < M * N; ++i) {
                data_[i] = other.data_[i];
            }
        } catch (...) {
            // Free critical resource in case of exception in constructor
            delete[] data_;
            throw;
        }
    }
    MatrixData(MatrixData &&other) : data_(other.data_) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP DATA move constructor" << std::endl);
        other.data_ = nullptr;
    }
    MatrixData& operator=(const MatrixData &other) {
        // Just copy values, matrices have the same size
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP DATA copy assignment" << std::endl);
        if (this != &other) { // avoid self-copy
            for (size_t i = 0; i < M * N; ++i) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }
    MatrixData& operator=(MatrixData &&other) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP DATA move assignment" << std::endl);
        if (this != &other) { // prevent self-move
            // Previous data from current matrix will be automatically destructed
            std::swap(data_, other.data_);
        }
        return *this;
    }

    const T* const read() const { return data_; }
    T* write() { return data_; }
};

// Allocates matrix in the memory given by the pointer. Allocation, deallocation
// and memory size should be handled by user himself.
template<typename T, size_t M, size_t N>
class MatrixData<T, M, N, MatrixDataStorage::USER> {
  private:
    T *data_;

  public:
    explicit MatrixData(T *mem) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER DATA constructor" << std::endl);
        data_ = mem;
    }
    template<typename T_>
    MatrixData(T *mem, T_ &&val) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER DATA constructor (val)" << std::endl);
        data_ = mem;
        for (size_t i = 0; i < M * N; ++i) {
            data_[i] = static_cast<T>(val);
        }
    }
    template<typename T_>
    MatrixData(T *mem, T_ *arr) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER DATA constructor (arr)" << std::endl);
        data_ = mem;
        for (size_t i = 0; i < M * N; ++i) {
            data_[i] = static_cast<T>(arr[i]);
        }
    }
    template<typename T_>
    MatrixData(std::initializer_list<T_> init) = delete; // memory isn't specified

    ~MatrixData() {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER DATA destructor" << std::endl);
    }
    // Copy constructor is prohibited because the destination memory location
    // isn't known.
    MatrixData(const MatrixData &other) = delete;
    MatrixData(MatrixData &&other) : data_(other.data_) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER DATA move constructor" << std::endl);
    }
    MatrixData& operator=(const MatrixData &other) {
        // Just copy values, matrices have the same size
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER DATA copy assignment" << std::endl);
        if (this != &other) { // avoid self-copy
            for (size_t i = 0; i < M * N; ++i) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }
    MatrixData& operator=(MatrixData &&other) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER DATA move assignment" << std::endl);
        if (this != &other) { // prevent self-move
            data_ = other.data_;
        }
        return *this;
    }

    const T* const read() const { return data_; }
    T* write() { return data_; }
};



// Matrix layout:
//         N
//   x - - - >
//   |0,0 0,1
// M |1,0 1,1
//   v
template<typename T, size_t M, size_t N, MatrixDataStorage S = MatrixDataStorage::UNSPECIFIED>
class Matrix; // by default storage is automatically chosen based on matrix size

// Matrix on stack (explicitly set)
template<typename T, size_t M, size_t N>
class Matrix<T, M, N, MatrixDataStorage::STACK> {
  private:
    MatrixData<T, M, N, MatrixDataStorage::STACK> md_;

  public:
    Matrix() : md_() {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK constructor" << std::endl);
    }
    template<typename T_>
    explicit Matrix(T_ &&val) : md_(std::forward<T_>(val)) { // perfect forwarding for large objects
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK constructor (val)" << std::endl);
    }
    template<typename T_>
    explicit Matrix(T_ *arr) : md_(arr) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK constructor (arr)" << std::endl);
    }
    template<typename T_>
    Matrix(std::initializer_list<T_> init) : md_(init) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK constructor (init)" << std::endl);
    }

    ~Matrix() = default;
    Matrix(const Matrix &other) = default;
    Matrix(Matrix &&other) = default;
    Matrix& operator=(const Matrix &other) = default;
    Matrix& operator=(Matrix &&other) = default;

    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::UNSPECIFIED> &other) : md_(other.read()) { // copy from UNSPECIFIED
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK <- UNSPECIFIED" << std::endl);
    }
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::STACK> &other) : md_(other.read()) { // converts value type
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK type converter" << std::endl);
    }
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::HEAP> &other) : md_(other.read()) { // copy from HEAP
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK <- HEAP" << std::endl);
    }
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::USER> &other) : md_(other.read()) { // copy from USER
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  STACK <- USER" << std::endl);
    }

    const T* const read() const { return md_.read(); } // read-only access
    T* write() { return md_.write(); }                 // read and write access
    void print() {
        const T* const arr = read();
        for (size_t i = 0; i < M * N; ) {
            std::cout << arr[i];
            std::cout << (!(++i % N) ? '\n' : ' ');
        }
        std::cout << std::endl;
    }

    template<typename T_, MatrixDataStorage S_>
    Matrix& operator+=(const Matrix<T_, M, N, S_> &other) {
        T *arr = write();
        const T_* const other_arr = other.read();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] += static_cast<T>(other_arr[i]);
        }
        return *this;
    }
    template<typename T_, MatrixDataStorage S_>
    Matrix& operator-=(const Matrix<T_, M, N, S_> &other) {
        T *arr = write();
        const T_* const other_arr = other.read();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] -= static_cast<T>(other_arr[i]);
        }
        return *this;
    }
    template<typename T_>
    Matrix& operator*=(const T_ &other) {
        T *arr = write();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] *= static_cast<T>(other);
        }
        return *this;
    }
    template<typename T_>
    Matrix& operator/=(const T_ &other) {
        T *arr = write();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] /= static_cast<T>(other);
        }
        return *this;
    }
};

// Matrix on heap (explicitly set)
template<typename T, size_t M, size_t N>
class Matrix<T, M, N, MatrixDataStorage::HEAP> {
  private:
    MatrixData<T, M, N, MatrixDataStorage::HEAP> md_;

  public:
    Matrix() : md_() {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP constructor" << std::endl);
    }
    template<typename T_>
    explicit Matrix(T_ &&val) : md_(std::forward<T_>(val)) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP constructor (val)" << std::endl);
    }
    template<typename T_>
    explicit Matrix(T_ *arr) : md_(arr) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP constructor (arr)" << std::endl);
    }
    template<typename T_>
    Matrix(std::initializer_list<T_> init) : md_(init) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP constructor (init)" << std::endl);
    }

    ~Matrix() = default;
    Matrix(const Matrix &other) = default;
    Matrix(Matrix &&other) = default;
    Matrix& operator=(const Matrix &other) = default;
    Matrix& operator=(Matrix &&other) = default;

    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::UNSPECIFIED> &other) : md_(other.read()) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP <- UNSPECIFIED" << std::endl);
    }
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::STACK> &other) : md_(other.read()) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP <- STACK" << std::endl);
    }
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::HEAP> &other) : md_(other.read()) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP type converter" << std::endl);
    }
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::USER> &other) : md_(other.read()) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  HEAP <- USER" << std::endl);
    }

    const T* const read() const { return md_.read(); }
    T* write() { return md_.write(); }
    void print() {
        const T* const arr = read();
        for (size_t i = 0; i < M * N; ) {
            std::cout << arr[i];
            std::cout << (!(++i % N) ? '\n' : ' ');
        }
        std::cout << std::endl;
    }

    template<typename T_, MatrixDataStorage S_>
    Matrix& operator+=(const Matrix<T_, M, N, S_> &other) {
        T *arr = write();
        const T_* const other_arr = other.read();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] += static_cast<T>(other_arr[i]);
        }
        return *this;
    }
    template<typename T_, MatrixDataStorage S_>
    Matrix& operator-=(const Matrix<T_, M, N, S_> &other) {
        T *arr = write();
        const T_* const other_arr = other.read();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] -= static_cast<T>(other_arr[i]);
        }
        return *this;
    }
    template<typename T_>
    Matrix& operator*=(const T_ &other) {
        T *arr = write();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] *= static_cast<T>(other);
        }
        return *this;
    }
    template<typename T_>
    Matrix& operator/=(const T_ &other) {
        T *arr = write();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] /= static_cast<T>(other);
        }
        return *this;
    }
};

// Matrix in user-defined memory. Template parameter should be set explicitly.
template<typename T, size_t M, size_t N>
class Matrix<T, M, N, MatrixDataStorage::USER> {
  private:
    MatrixData<T, M, N, MatrixDataStorage::USER> md_;

  public:
    // First constructor parameter is user memory for matrix data placement. The
    // amount of memory should be big enough to carry M * N * sizeof(T) bytes.
    // Memory is explicitly managed by user himself.
    explicit Matrix(T *mem) : md_(mem) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER constructor" << std::endl);
    }
    template<typename T_>
    Matrix(T *mem, T_ &&val) : md_(mem, std::forward<T_>(val)) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER constructor (val)" << std::endl);
    }
    template<typename T_>
    Matrix(T *mem, T_ *arr) : md_(mem, arr) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  USER constructor (arr)" << std::endl);
    }
    template<typename T_>
    Matrix(std::initializer_list<T_> init) = delete; // memory isn't specified

    ~Matrix() = default;
    // Creating a copy of metrix with user-defined memory is prohibited due to
    // a lack of pointer to memory location for the new matrix. Other copy-like
    // constructors from another storage type or another data type are prohibited
    // for the same reason.
    Matrix(const Matrix &other) = delete;
    Matrix(Matrix &&other) = default;
    Matrix& operator=(const Matrix &other) = default;
    Matrix& operator=(Matrix &&other) = default;

    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::UNSPECIFIED> &other) = delete;
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::STACK> &other) = delete;
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::HEAP> &other) = delete;
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::USER> &other) = delete;

    const T* const read() const { return md_.read(); }
    T* write() { return md_.write(); }
    void print() {
        const T* const arr = read();
        for (size_t i = 0; i < M * N; ) {
            std::cout << arr[i];
            std::cout << (!(++i % N) ? '\n' : ' ');
        }
        std::cout << std::endl;
    }

    template<typename T_, MatrixDataStorage S_>
    Matrix& operator+=(const Matrix<T_, M, N, S_> &other) {
        T *arr = write();
        const T_* const other_arr = other.read();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] += static_cast<T>(other_arr[i]);
        }
        return *this;
    }
    template<typename T_, MatrixDataStorage S_>
    Matrix& operator-=(const Matrix<T_, M, N, S_> &other) {
        T *arr = write();
        const T_* const other_arr = other.read();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] -= static_cast<T>(other_arr[i]);
        }
        return *this;
    }
    template<typename T_>
    Matrix& operator*=(const T_ &other) {
        T *arr = write();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] *= static_cast<T>(other);
        }
        return *this;
    }
    template<typename T_>
    Matrix& operator/=(const T_ &other) {
        T *arr = write();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] /= static_cast<T>(other);
        }
        return *this;
    }
};

// Matrix memory will be chosen based on metrix size. This type of matrix will
// be created by default, but it also could be created explicitly.
template<typename T, size_t M, size_t N>
class Matrix<T, M, N, MatrixDataStorage::UNSPECIFIED> {
  private:
    MatrixData<T, M, N, choose_matrix_data_storage(sizeof(T) * M * N)> md_;

  public:
    Matrix() : md_() {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  UNSPECIFIED constructor" << std::endl);
    }
    template<typename T_>
    explicit Matrix(T_ &&val) : md_(std::forward<T_>(val)) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  UNSPECIFIED constructor (val)" << std::endl);
    }
    template<typename T_>
    explicit Matrix(T_ *arr) : md_(arr) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  UNSPECIFIED constructor (arr)" << std::endl);
    }
    template<typename T_>
    Matrix(std::initializer_list<T_> init) : md_(init) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  UNSPECIFIED constructor (init)" << std::endl);
    }

    ~Matrix() = default;
    Matrix(const Matrix &other) = default;
    Matrix(Matrix &&other) = default;
    Matrix& operator=(const Matrix &other) = default;
    Matrix& operator=(Matrix &&other) = default;

    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::UNSPECIFIED> &other) : md_(other.read()) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  UNSPECIFIED type converter" << std::endl);
    }
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::STACK> &other) : md_(other.read()) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  UNSPECIFIED <- STACK" << std::endl);
    }
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::HEAP> &other) : md_(other.read()) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  UNSPECIFIED <- HEAP" << std::endl);
    }
    template<typename T_>
    Matrix(const Matrix<T_, M, N, MatrixDataStorage::USER> &other) : md_(other.read()) {
        MATRIX_DEBUG_(std::cout << FUNC_NAME_ << std::endl);
        MATRIX_DEBUG_(std::cout << "  UNSPECIFIED <- USER" << std::endl);
    }

    const T* const read() const { return md_.read(); }
    T* write() { return md_.write(); }
    void print() {
        const T* const arr = read();
        for (size_t i = 0; i < M * N; ) {
            std::cout << arr[i];
            std::cout << (!(++i % N) ? '\n' : ' ');
        }
        std::cout << std::endl;
    }

    template<typename T_, MatrixDataStorage S_>
    Matrix& operator+=(const Matrix<T_, M, N, S_> &other) {
        T *arr = write();
        const T_* const other_arr = other.read();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] += static_cast<T>(other_arr[i]);
        }
        return *this;
    }
    template<typename T_, MatrixDataStorage S_>
    Matrix& operator-=(const Matrix<T_, M, N, S_> &other) {
        T *arr = write();
        const T_* const other_arr = other.read();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] -= static_cast<T>(other_arr[i]);
        }
        return *this;
    }
    template<typename T_>
    Matrix& operator*=(const T_ &other) {
        T *arr = write();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] *= static_cast<T>(other);
        }
        return *this;
    }
    template<typename T_>
    Matrix& operator/=(const T_ &other) {
        T *arr = write();
        for (size_t i = 0; i < M * N; ++i) {
            arr[i] /= static_cast<T>(other);
        }
        return *this;
    }
};



// Arithmetic operators are implemented without passing matrix by value to avoid
// copying. As a result elements are created inside function that avoids extra
// algorithmic complexity. But it forbids to use corresponding assignment operators
// from class effectivelly.
// "+matrix"
template<typename T, size_t M, size_t N, MatrixDataStorage S>
Matrix<T, M, N, result_matrix_data_storage(S)> operator+(const Matrix<T, M, N, S> &val) {
    return Matrix<T, M, N, result_matrix_data_storage(S)>(val); // creates a copy
}
// "-matrix"
template<typename T, size_t M, size_t N, MatrixDataStorage S>
Matrix<T, M, N, result_matrix_data_storage(S)> operator-(const Matrix<T, M, N, S> &val) {
    Matrix<T, M, N, result_matrix_data_storage(S)> ret;
    T *arr = ret.write();
    const T* const val_arr = val.read();
    for (size_t i = 0; i < M * N; ++i) {
        arr[i] = -val_arr[i];
    }
    return ret;
}
// "matrix + matrix"
template<typename T, typename T_, size_t M, size_t N, MatrixDataStorage S, MatrixDataStorage S_>
Matrix<std::common_type_t<T, T_>, M, N, result_matrix_data_storage(S, S_)> operator+(const Matrix<T, M, N, S> &lhs, const Matrix<T_, M, N, S_> &rhs) {
    using TT_ = std::common_type_t<T, T_>;
    Matrix<TT_, M, N, result_matrix_data_storage(S, S_)> ret;
    TT_ *arr = ret.write();
    const T* const lhs_arr = lhs.read();
    const T_* const rhs_arr = rhs.read();
    for (size_t i = 0; i < M * N; ++i) {
        arr[i] = static_cast<TT_>(lhs_arr[i] + rhs_arr[i]);
    }
    return ret;
}
// "matrix - matrix"
template<typename T, typename T_, size_t M, size_t N, MatrixDataStorage S, MatrixDataStorage S_>
Matrix<std::common_type_t<T, T_>, M, N, result_matrix_data_storage(S, S_)> operator-(const Matrix<T, M, N, S> &lhs, const Matrix<T_, M, N, S_> &rhs) {
    using TT_ = std::common_type_t<T, T_>;
    Matrix<TT_, M, N, result_matrix_data_storage(S, S_)> ret;
    TT_ *arr = ret.write();
    const T* const lhs_arr = lhs.read();
    const T_* const rhs_arr = rhs.read();
    for (size_t i = 0; i < M * N; ++i) {
        arr[i] = static_cast<TT_>(lhs_arr[i] - rhs_arr[i]);
    }
    return ret;
}
// "matrix * scalar"
template<typename T, typename T_, size_t M, size_t N, MatrixDataStorage S>
Matrix<T, M, N, result_matrix_data_storage(S)> operator*(const Matrix<T, M, N, S> &lhs, const T_ &rhs) {
    Matrix<T, M, N, result_matrix_data_storage(S)> ret;
    T *arr = ret.write();
    const T* const lhs_arr = lhs.read();
    for (size_t i = 0; i < M * N; ++i) {
        arr[i] = lhs_arr[i] * static_cast<T>(rhs);
    }
    return ret;
}
// "scalar * matrix"
template<typename T, typename T_, size_t M, size_t N, MatrixDataStorage S>
Matrix<T, M, N, result_matrix_data_storage(S)> operator*(const T_ &lhs, const Matrix<T, M, N, S> &rhs) {
    // Another arguments order of "matrix * scalar"
    return (rhs * lhs);
}
// "matrix / scalar"
template<typename T, typename T_, size_t M, size_t N, MatrixDataStorage S>
Matrix<T, M, N, result_matrix_data_storage(S)> operator/(const Matrix<T, M, N, S> &lhs, const T_ &rhs) {
    Matrix<T, M, N, result_matrix_data_storage(S)> ret;
    T *arr = ret.write();
    const T* const lhs_arr = lhs.read();
    for (size_t i = 0; i < M * N; ++i) {
        arr[i] = lhs_arr[i] / static_cast<T>(rhs);
    }
    return ret;
}

// Comparison operators
// "matrix == matrix"
template<typename T, typename T_, size_t M, size_t N, MatrixDataStorage S, MatrixDataStorage S_>
bool operator==(const Matrix<T, M, N, S> &lhs, const Matrix<T_, M, N, S_> &rhs) {
    const T* const lhs_arr = lhs.read();
    const T_* const rhs_arr = rhs.read();
    for (size_t i = 0; i < M * N; ++i) {
        if (lhs_arr[i] != rhs_arr[i]) { // bad for FP
            return false;
        }
    }
    return true;
}
// "matrix != matrix"
template<typename T, typename T_, size_t M, size_t N, MatrixDataStorage S, MatrixDataStorage S_>
bool operator!=(const Matrix<T, M, N, S> &lhs, const Matrix<T_, M, N, S_> &rhs) {
    // Opposite to "matrix == matrix"
    return !(lhs == rhs);
}



// Other matrix functions
// Multiplies matrix(m,n) by matrix(n,p)
//     < N >       < P >     < P >
// ^ (a a a a)   ^ (b b)   ^ (r r)
// M (a a a a) x N (b b) = M (r r)
// v (a a a a)   v (b b)   v (r r)
//                 (b b)
template<typename T, typename T_, size_t M, size_t N, size_t P, MatrixDataStorage S, MatrixDataStorage S_>
Matrix<std::common_type_t<T, T_>, M, P, result_matrix_data_storage(S, S_)> mul(const Matrix<T, M, N, S> &lhs, const Matrix<T_, N, P, S_> &rhs) {
    using TT_ = std::common_type_t<T, T_>;
    Matrix<TT_, M, P, result_matrix_data_storage(S, S_)> ret;
    TT_ *arr = ret.write();
    const T* const lhs_arr = lhs.read();
    const T_* const rhs_arr = rhs.read();
    for (size_t i = 0; i < M; ++i) { // rows of result
        size_t iN = i * N; // use const inside a row values
        size_t iP = i * P;
        for (size_t j = 0; j < P; ++j) { // columns of result
            TT_ val = 0; // use local storage for temp value
            for (size_t k = 0; k < N; ++k) { // dot product
                val += static_cast<TT_>(lhs_arr[iN + k] * rhs_arr[k * P + j]);
            }
            arr[iP + j] = val;
        }
    }
    return ret;
}

// Computes determinant of matrix(n,n). Gaussian elimination method is used to
// obtain something close to lower triangular matrix (LTM), thus the complexity
// is O(n^3). This method should be applied to matrices with floating point
// elements, overwise integer division will zero the result. Precise method
// specialization for integers is not implemented due to fast integer overflow.
template<typename T, size_t N, MatrixDataStorage S>
T det(const Matrix<T, N, N, S> &val) {
    Matrix<T, N, N, result_matrix_data_storage(S)> ltm(val); // will be transformed to almost-LTM
    T *arr = ltm.write();

    T factor = 1; // accumulates intermediate multiplyers
    // Since transposed matrix has the same determinant as original we transform
    // given matrix to almost-LTM because of C-style array location in memory.
    for (size_t i = 0; i < N; ++i) { // rows
        size_t iN = i * N; // use const inside a row values
        if (arr[iN + i] == 0) { // diagonal elenment is zero, need to swap columns
            size_t j = i + 1;
            for ( ; j < N; ++j) { // search for not-zero element further in the same row
                if (arr[iN + j] != 0) { // non-zero element found, swap columns (actually, lower parts only)
                    for (size_t k = i; k < N; ++k) {
                        std::swap(arr[k * N + i], arr[k * N + j]);
                    }
                    factor = -factor; // column swap inverts determinant
                    j = i; // used as flag that matrix is nondegenerate
                    break;
                }
            }
            if (j != i) { // determinant of degenerate matrix is zero
                return T(0);
            }
        }
        for (size_t j = i + 1; j < N; ++j) { // further columns
            if (arr[iN + j] != 0) {
                T multiplier = arr[iN + i] / arr[iN + j];
                factor *= multiplier; // column multiplication changes determinant
                for (size_t k = i + 1; k < N; ++k) { // subtract lower part of column
                    arr[k * N + j] = arr[k * N + j] * multiplier - arr[k * N + i];
                }
            }
        }
    }
    T res = arr[0];
    for (size_t i = 1; i < N; ++i) { // diagonal elements represent determinant
        res *= arr[i * N + i];
    }
    res /= factor;
    return res;
}

} // namespace matrix



// Cleanup
#undef MATRIX_DEBUG_
#undef FUNC_NAME_
#undef MATRIX_DATA_STORAGE_STACK_SIZE_MAX_

#endif // #ifndef MATRIX_H
