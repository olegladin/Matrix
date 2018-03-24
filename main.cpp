// Ladin Oleg, 2018

#include "matrix.h"     // matrix "library"
using namespace matrix; // use shortened names

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    std::cout << "[BEGIN TESTING]\n" << std::endl;

    // Creating an empty matrix and accessing its storage
    {
        std::string fails;

        // Create matrix 3*4 (rows*columns) of integers with auto-defined storage
        // (depends on "MATRIX_DATA_STORAGE_STACK_SIZE_MAX" compiler parameter:
        // uses stack in case of matrix size in bytes is smaller than it, and
        // heap overwise).
        Matrix<int, 3, 4> ma0; // #A0

        // Read and write access to matrix array (one-dimentional)
        int *ma0_arr_w = ma0.write(); // #A1
        ma0_arr_w[1] = 2;

        // Read-only access to matrix array
        const int* const ma0_arr_r = ma0.read(); // #A2
        if (ma0_arr_r[1] != 2) {
            fails += " #A0-#A2 ";
        }

        // As previous but explicitly ask for to inference the best data storage.
        // The 4th matrix data storage parameter is UNSPECIFIED by default (but
        // its constructors are tested independently here).
        Matrix<char, 3, 4, MatrixDataStorage::UNSPECIFIED> ma1; // #A3
        ma1.write()[3] = 'a'; // #A4
        if (ma1.read()[3] != 'a') { // #A5
            fails += " #A3-#A5 ";
        }

        // Create matrix on stack
        Matrix<int, 4, 4, MatrixDataStorage::STACK> ma2; // #A6
        ma2.write()[4] = -1; // #A7
        if (ma2.read()[4] != -1) { // #A8
            fails += " #A6-#A8 ";
        }

        // Create matrix on heap
        Matrix<int, 3, 5, MatrixDataStorage::HEAP> ma3; // #A9
        ma3.write()[5] = 5; // #A10
        if (ma3.read()[5] != 5) { // #A11
            fails += " #A9-#A11 ";
        }

        // Create matrix in existing memory. Size of the memory should be big
        // enough to store the matrix. User should manage the memory by himself.
        // A pointer to this memory should be passed as a constructor parameter.
        // Memory type should be the same as the matrix elements type.
        double *mem = new double[32];
        mem[2] = 1.5;
        Matrix<double, 5, 6, MatrixDataStorage::USER> ma4(mem); // #A12
        ma4.write()[1] = 3.5; // #A13
        if ((ma4.read()[1] != 3.5) || (ma4.read()[2] != 1.5)) { // #A14
            fails += " #A12-#A14 ";
        }
        delete[] mem;

        // WRONG CODE:
        //m.read()[i] = val; // read-only access
        //Matrix<T, M, N, MatrixDataStorage::USER> m; // needs pointer to memory
        //Matrix<T, M, N, MatrixDataStorage::USER> m = mem; // needs explicit creation

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Creating an empty matrix and accessing its storage)" << std::endl;
    }
    
    // Creating and initializing a matrix with values
    {
        std::string fails;

        // Create and initialize all matrix elements with a value
        Matrix<int, 3, 2> m0(1); // #B0
        auto m0_arr = m0.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m0_arr[i] != 1) {
                fails += " #B0 ";
                break;
            }
        }

        // Initialize matrix with different type rather than matrix elements have
        Matrix<int, 3, 2> m1(8.25); // #B1
        auto m1_arr = m1.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m1_arr[i] != static_cast<int>(8.25)) {
                fails += " #B1 ";
                break;
            }
        }

        // Initialize UNSPECIFIED matrix
        Matrix<int, 3, 2, MatrixDataStorage::UNSPECIFIED> m2(13); // #B2
        auto m2_arr = m2.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m2_arr[i] != 13) {
                fails += " #B2 ";
                break;
            }
        }

        // Initialize UNSPECIFIED matrix with type convertion
        Matrix<int, 3, 2, MatrixDataStorage::UNSPECIFIED> m3(-8.25); // #B3
        auto m3_arr = m3.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m3_arr[i] != static_cast<int>(-8.25)) {
                fails += " #B3 ";
                break;
            }
        }

        // Initialize STACK matrix
        Matrix<int, 3, 2, MatrixDataStorage::STACK> m4(8); // #B4
        auto m4_arr = m4.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m4_arr[i] != 8) {
                fails += " #B4 ";
                break;
            }
        }

        // Initialize STACK matrix with type convertion
        Matrix<int, 3, 2, MatrixDataStorage::STACK> m5(-2.2); // #B5
        auto m5_arr = m5.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m5_arr[i] != static_cast<int>(-2.2)) {
                fails += " #B5 ";
                break;
            }
        }

        // Initialize HEAP matrix
        Matrix<int, 3, 2, MatrixDataStorage::HEAP> m6(80); // #B6
        auto m6_arr = m6.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m6_arr[i] != 80) {
                fails += " #B6 ";
                break;
            }
        }

        // Initialize HEAP matrix with type convertion
        Matrix<int, 3, 2, MatrixDataStorage::HEAP> m7(-2.02); // #B7
        auto m7_arr = m7.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m7_arr[i] != static_cast<int>(-2.02)) {
                fails += " #B7 ";
                break;
            }
        }

        // Initialize USER matrix
        int val = 7;
        int mem[12];
        Matrix<int, 3, 2, MatrixDataStorage::USER> m8(mem, val); // #B8
        auto m8_arr = m8.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m8_arr[i] != 7) {
                fails += " #B8 ";
                break;
            }
        }

        // Initialize USER matrix with type convertion
        Matrix<int, 3, 2, MatrixDataStorage::USER> m9(mem + 3 * 2, 4.2); // #B9
        auto m9_arr = m9.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (m9_arr[i] != static_cast<int>(4.2)) {
                fails += " #B9 ";
                break;
            }
        }

        // Initializing matrix with values from an array
        int arr0[]{ 0, 1, 2, 3, 4, 5, 6, 7 };
        Matrix<int, 2, 3> m10(arr0); // #B10
        arr0[2] *= -1;
        auto m10_arr = m10.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (m10_arr[i] != i) {
                fails += " #B10 ";
                break;
            }
        }

        // Initializing matrix with an array of other type
        double arr1[]{ 0.5, 1.5, 2.5, 3.5 };
        Matrix<int, 2, 2> m11(arr1); // #B11
        arr1[0] *= -1;
        auto m11_arr = m11.read();
        if ((m11_arr[0] != static_cast<int>(0.5)) || (m11_arr[1] != static_cast<int>(1.5)) ||
            (m11_arr[2] != static_cast<int>(2.5)) || (m11_arr[3] != static_cast<int>(3.5))) {
            fails += " #B11 ";
        }

        // Initializing UNSPECIFIED matrix with values from an array
        Matrix<int, 2, 3, MatrixDataStorage::UNSPECIFIED> m12(arr0); // #B12
        auto m12_arr = m12.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (i == 2) {
                if (m12_arr[2] != -2) {
                    fails += " #B12 ";
                    break;
                }
                continue;
            }
            if (m12_arr[i] != i) {
                fails += " #B12 ";
                break;
            }
        }

        // Initializing UNSPECIFIED matrix with an array of other type
        Matrix<int, 2, 2, MatrixDataStorage::UNSPECIFIED> m13(arr1); // #B13
        auto m13_arr = m13.read();
        if ((m13_arr[0] != static_cast<int>(-0.5)) || (m13_arr[1] != static_cast<int>(1.5)) ||
            (m13_arr[2] != static_cast<int>(2.5)) || (m13_arr[3] != static_cast<int>(3.5))) {
            fails += " #B13 ";
        }

        // Initializing STACK matrix with values from an array
        arr0[2] *= -1;
        Matrix<int, 2, 3, MatrixDataStorage::STACK> m14(arr0); // #B14
        auto m14_arr = m14.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (m14_arr[i] != i) {
                fails += " #B14 ";
                break;
            }
        }

        // Initializing STACK matrix with an array of other type
        arr1[0] *= -1;
        Matrix<int, 2, 2, MatrixDataStorage::STACK> m15(arr1); // #B15
        auto m15_arr = m15.read();
        if ((m15_arr[0] != static_cast<int>(0.5)) || (m15_arr[1] != static_cast<int>(1.5)) ||
            (m15_arr[2] != static_cast<int>(2.5)) || (m15_arr[3] != static_cast<int>(3.5))) {
            fails += " #B15 ";
        }

        // Initializing HEAP matrix with values from an array
        Matrix<int, 2, 3, MatrixDataStorage::HEAP> m16(arr0); // #B16
        auto m16_arr = m16.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (m16_arr[i] != i) {
                fails += " #B16 ";
                break;
            }
        }

        // Initializing HEAP matrix with an array of other type
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> m17(arr1); // #B17
        auto m17_arr = m17.read();
        if ((m17_arr[0] != static_cast<int>(0.5)) || (m17_arr[1] != static_cast<int>(1.5)) ||
            (m17_arr[2] != static_cast<int>(2.5)) || (m17_arr[3] != static_cast<int>(3.5))) {
            fails += " #B17 ";
        }

        // Initializing USER matrix with values from an array
        int mem1[6] = { 9, 8, 7, 6, 5, 4 };
        Matrix<int, 2, 3, MatrixDataStorage::USER> m18(mem1, arr0); // #B18
        auto m18_arr = m18.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (m18_arr[i] != i) {
                fails += " #B18 ";
                break;
            }
        }

        // Initializing USER matrix with an array of other type
        Matrix<int, 2, 2, MatrixDataStorage::USER> m19(mem1, arr1); // #B19
        auto m19_arr = m19.read();
        if ((m19_arr[0] != static_cast<int>(0.5)) || (m19_arr[1] != static_cast<int>(1.5)) ||
            (m19_arr[2] != static_cast<int>(2.5)) || (m19_arr[3] != static_cast<int>(3.5))) {
            fails += " #B19 ";
        }

        // WRONG CODE:
        //Matrix<T, M, N, T> m = (T_)val; // needs explicit creation
        //Matrix<T, M, N, T> m = (T_*)arr; // needs explicit creation

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Creating and initializing a matrix with values)" << std::endl;
    }
    
    // Initializer lists
    {
        std::string fails;

        // Initializer list. If the amount of elements exceeds matrix size, they
        // will be ignored. If the amount is less than the matrix size, it will
        // be filled with zeroes.
        Matrix<int, 3, 1> m0{ 2, 1 }; // #C0
        auto m0_arr = m0.read();
        if ((m0_arr[0] != 2) || (m0_arr[1] != 1) || (m0_arr[2] != 0)) {
            fails += " #C0 ";
        }

        // Initializer list with assignment
        Matrix<int, 3, 1> m1 = { 0, -1, 2, 5 }; // #C1
        auto m1_arr = m1.read();
        if ((m1_arr[0] != 0) || (m1_arr[1] != -1) || (m1_arr[2] != 2)) {
            fails += " #C1 ";
        }

        // Initializer list with convertion
        Matrix<int, 3, 1> m2{ 0.5, 1.5, 2.5 }; // #C2
        auto m2_arr = m2.read();
        if ((m2_arr[0] != static_cast<int>(0.5)) ||
            (m2_arr[1] != static_cast<int>(1.5)) ||
            (m2_arr[2] != static_cast<int>(2.5))) {
            fails += " #C2 ";
        }

        // Initializer list with convertion and assignment
        Matrix<int, 3, 1> m3 = { 0.5, -1.5, 2.5 }; // #C3
        auto m3_arr = m3.read();
        if ((m3_arr[0] != static_cast<int>(0.5)) ||
            (m3_arr[1] != static_cast<int>(-1.5)) ||
            (m3_arr[2] != static_cast<int>(2.5))) {
            fails += " #C3 ";
        }

        // Initializer list for UNSPECIFIED
        Matrix<int, 3, 1, MatrixDataStorage::UNSPECIFIED> m4{ 3, 2, 1 }; // #C4
        auto m4_arr = m4.read();
        if ((m4_arr[0] != 3) || (m4_arr[1] != 2) || (m4_arr[2] != 1)) {
            fails += " #C4 ";
        }

        // Initializer list with assignment for UNSPECIFIED
        Matrix<int, 3, 1, MatrixDataStorage::UNSPECIFIED> m5 = { 2, -1, 2 }; // #C5
        auto m5_arr = m5.read();
        if ((m5_arr[0] != 2) || (m5_arr[1] != -1) || (m5_arr[2] != 2)) {
            fails += " #C5 ";
        }

        // Initializer list with convertion for UNSPECIFIED
        Matrix<int, 3, 1, MatrixDataStorage::UNSPECIFIED> m6{ 1.5, 1., 1.5 }; // #C6
        auto m6_arr = m6.read();
        if ((m6_arr[0] != static_cast<int>(1.5)) ||
            (m6_arr[1] != static_cast<int>(1.)) ||
            (m6_arr[2] != static_cast<int>(1.5))) {
            fails += " #C6 ";
        }

        // Initializer list with convertion and assignment for UNSPECIFIED
        Matrix<int, 3, 1, MatrixDataStorage::UNSPECIFIED> m7 = { 0.5, -1., 2.5 }; // #C7
        auto m7_arr = m7.read();
        if ((m7_arr[0] != static_cast<int>(0.5)) ||
            (m7_arr[1] != static_cast<int>(-1.)) ||
            (m7_arr[2] != static_cast<int>(2.5))) {
            fails += " #C7 ";
        }

        // Initializer list for STACK
        Matrix<int, 3, 1, MatrixDataStorage::STACK> m8{ 0, 2, 1 }; // #C8
        auto m8_arr = m8.read();
        if ((m8_arr[0] != 0) || (m8_arr[1] != 2) || (m8_arr[2] != 1)) {
            fails += " #C8 ";
        }

        // Initializer list with assignment for STACK
        Matrix<int, 3, 1, MatrixDataStorage::STACK> m9 = { 6, -1, 2 }; // #C9
        auto m9_arr = m9.read();
        if ((m9_arr[0] != 6) || (m9_arr[1] != -1) || (m9_arr[2] != 2)) {
            fails += " #C9 ";
        }

        // Initializer list with convertion for STACK
        Matrix<int, 3, 1, MatrixDataStorage::STACK> m10{ 9.5, 1., 1.5 }; // #C10
        auto m10_arr = m10.read();
        if ((m10_arr[0] != static_cast<int>(9.5)) ||
            (m10_arr[1] != static_cast<int>(1.)) ||
            (m10_arr[2] != static_cast<int>(1.5))) {
            fails += " #C10 ";
        }

        // Initializer list with convertion and assignment for STACK
        Matrix<int, 3, 1, MatrixDataStorage::STACK> m11 = { 7.5, -1., 2.5 }; // #C11
        auto m11_arr = m11.read();
        if ((m11_arr[0] != static_cast<int>(7.5)) ||
            (m11_arr[1] != static_cast<int>(-1.)) ||
            (m11_arr[2] != static_cast<int>(2.5))) {
            fails += " #C11 ";
        }

        // Initializer list for HEAP
        Matrix<int, 3, 1, MatrixDataStorage::HEAP> m12{ 0, 0, 1 }; // #C12
        auto m12_arr = m12.read();
        if ((m12_arr[0] != 0) || (m12_arr[1] != 0) || (m12_arr[2] != 1)) {
            fails += " #C12 ";
        }

        // Initializer list with assignment for HEAP
        Matrix<int, 3, 1, MatrixDataStorage::HEAP> m13 = { 6, 12, 2 }; // #C13
        auto m13_arr = m13.read();
        if ((m13_arr[0] != 6) || (m13_arr[1] != 12) || (m13_arr[2] != 2)) {
            fails += " #C13 ";
        }

        // Initializer list with convertion for HEAP
        Matrix<int, 3, 1, MatrixDataStorage::HEAP> m14{ 9.5, 1.9, 1.5 }; // #C14
        auto m14_arr = m14.read();
        if ((m14_arr[0] != static_cast<int>(9.5)) ||
            (m14_arr[1] != static_cast<int>(1.9)) ||
            (m14_arr[2] != static_cast<int>(1.5))) {
            fails += " #C14 ";
        }

        // Initializer list with convertion and assignment for HEAP
        Matrix<int, 3, 1, MatrixDataStorage::HEAP> m15 = { 7.5, 1.8, 2.5 }; // #C15
        auto m15_arr = m15.read();
        if ((m15_arr[0] != static_cast<int>(7.5)) ||
            (m15_arr[1] != static_cast<int>(1.8)) ||
            (m15_arr[2] != static_cast<int>(2.5))) {
            fails += " #C15 ";
        }

        // WRONG CODE:
        //Matrix<T, M, N, MatrixDataStorage::USER> m{ val }; // needs pointer to memory
        //Matrix<T, M, N, MatrixDataStorage::USER> m = { val }; // needs pointer to memory

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Initializer lists)" << std::endl;
    }

    // Type and storage converters
    {
        std::string fails;

        Matrix<double, 2, 3> mx(1.5);
        Matrix<double, 2, 3, MatrixDataStorage::STACK> ms(2.5);
        Matrix<double, 2, 3, MatrixDataStorage::HEAP> mh(3.5);
        double mem[6];
        Matrix<double, 2, 3, MatrixDataStorage::USER> mu(mem, 4.5);
        // UNSPECIFIED type converter
        Matrix<int, 2, 3> mxx = mx; // #D0
        auto mxx_arr = mxx.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (mxx_arr[i] != static_cast<int>(1.5)) {
                fails += " #D0 ";
                break;
            }
        }

        // UNSPECIFIED <- STACK
        Matrix<int, 2, 3> mxs = ms; // #D1
        auto mxs_arr = mxs.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (mxs_arr[i] != static_cast<int>(2.5)) {
                fails += " #D1 ";
                break;
            }
        }

        // UNSPECIFIED <- HEAP
        Matrix<int, 2, 3> mxh = mh; // #D2
        auto mxh_arr = mxh.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (mxh_arr[i] != static_cast<int>(3.5)) {
                fails += " #D2 ";
                break;
            }
        }

        // UNSPECIFIED <- USER
        Matrix<int, 2, 3> mxu = mu; // #D3
        auto mxu_arr = mxu.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (mxu_arr[i] != static_cast<int>(4.5)) {
                fails += " #D3 ";
                break;
            }
        }

        // STACK <- UNSPECIFIED
        Matrix<int, 2, 3, MatrixDataStorage::STACK> msx = mx; // #D4
        auto msx_arr = msx.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (msx_arr[i] != static_cast<int>(1.5)) {
                fails += " #D4 ";
                break;
            }
        }

        // STACK type converter
        Matrix<int, 2, 3, MatrixDataStorage::STACK> mss = ms; // #D5
        auto mss_arr = mss.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (mss_arr[i] != static_cast<int>(2.5)) {
                fails += " #D5 ";
                break;
            }
        }

        // STACK <- HEAP
        Matrix<int, 2, 3, MatrixDataStorage::STACK> msh = mh; // #D6
        auto msh_arr = msh.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (msh_arr[i] != static_cast<int>(3.5)) {
                fails += " #D6 ";
                break;
            }
        }

        // STACK <- USER
        Matrix<int, 2, 3, MatrixDataStorage::STACK> msu = mu; // #D7
        auto msu_arr = msu.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (msu_arr[i] != static_cast<int>(4.5)) {
                fails += " #D7 ";
                break;
            }
        }

        // HEAP <- UNSPECIFIED
        Matrix<int, 2, 3, MatrixDataStorage::HEAP> mhx = mx; // #D8
        auto mhx_arr = mhx.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (mhx_arr[i] != static_cast<int>(1.5)) {
                fails += " #D8 ";
                break;
            }
        }

        // HEAP <- STACK
        Matrix<int, 2, 3, MatrixDataStorage::HEAP> mhs = ms; // #D9
        auto mhs_arr = mhs.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (mhs_arr[i] != static_cast<int>(2.5)) {
                fails += " #D9 ";
                break;
            }
        }

        // HEAP type converter
        Matrix<int, 2, 3, MatrixDataStorage::HEAP> mhh = mh; // #D10
        auto mhh_arr = mhh.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (mhh_arr[i] != static_cast<int>(3.5)) {
                fails += " #D10 ";
                break;
            }
        }

        // HEAP <- USER
        Matrix<int, 2, 3, MatrixDataStorage::HEAP> mhu = mu; // #D11
        auto mhu_arr = mhu.read();
        for (size_t i = 0; i < 2 * 3; ++i) {
            if (mhu_arr[i] != static_cast<int>(4.5)) {
                fails += " #D11 ";
                break;
            }
        }

        // WRONG CODE:
        //Matrix<T, M, N, S> m(m_); // conflicts with value-initialization constructor
        //Matrix<T, M, N, MatrixDataStorage::USER> m = m_; // needs another pointer to memory

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Type and storage converters)" << std::endl;
    }

    // Copy, move constructor, assignment
    {
        std::string fails;

        Matrix<int, 2, 2> mx(1);
        Matrix<int, 2, 2, MatrixDataStorage::STACK> ms(2);
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> mh(3);
        int mem[4];
        Matrix<int, 2, 2, MatrixDataStorage::USER> mu(mem, 4);
        // UNSPECIFIED copy constructor
        Matrix<int, 2, 2> mxcc = mx; // #E0
        auto mxcc_arr = mxcc.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxcc_arr[i] != 1) {
                fails += " #E0 ";
                break;
            }
        }

        // UNSPECIFIED move constructor
        Matrix<int, 2, 2> mxmc = std::move(mx); // #E1
        auto mxmc_arr = mxmc.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxmc_arr[i] != 1) {
                fails += " #E1 ";
                break;
            }
        }

        // UNSPECIFIED copy assignment
        Matrix<int, 2, 2> mxca(-1);
        mxca = mxcc; // #E2
        auto mxca_arr = mxca.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxca_arr[i] != 1) {
                fails += " #E2 ";
                break;
            }
        }

        // UNSPECIFIED move assignment
        Matrix<int, 2, 2> mxma(-1);
        mxma = std::move(mxmc); // #E3
        auto mxma_arr = mxma.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxma_arr[i] != 1) {
                fails += " #E3 ";
                break;
            }
        }

        // STACK copy constructor
        Matrix<int, 2, 2, MatrixDataStorage::STACK> mscc = ms; // #E4
        auto mscc_arr = mscc.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mscc_arr[i] != 2) {
                fails += " #E4 ";
                break;
            }
        }

        // STACK move constructor
        Matrix<int, 2, 2, MatrixDataStorage::STACK> msmc = std::move(ms); // #E5
        auto msmc_arr = msmc.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msmc_arr[i] != 2) {
                fails += " #E5 ";
                break;
            }
        }

        // STACK copy assignment
        Matrix<int, 2, 2, MatrixDataStorage::STACK> msca(-1);
        msca = mscc; // #E6
        auto msca_arr = msca.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msca_arr[i] != 2) {
                fails += " #E6 ";
                break;
            }
        }

        // STACK move assignment
        Matrix<int, 2, 2, MatrixDataStorage::STACK> msma(-1);
        msma = std::move(msmc); // #E7
        auto msma_arr = msma.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msma_arr[i] != 2) {
                fails += " #E7 ";
                break;
            }
        }

        // HEAP copy constructor
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> mhcc = mh; // #E8
        auto mhcc_arr = mhcc.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhcc_arr[i] != 3) {
                fails += " #E8 ";
                break;
            }
        }

        // HEAP move constructor
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> mhmc = std::move(mh); // #E9
        auto mhmc_arr = mhmc.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhmc_arr[i] != 3) {
                fails += " #E9 ";
                break;
            }
        }

        // HEAP copy assignment
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> mhca(-1);
        mhca = mhcc; // #E10
        auto mhca_arr = mhca.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhca_arr[i] != 3) {
                fails += " #E10 ";
                break;
            }
        }

        // HEAP move assignment
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> mhma(-1);
        mhma = std::move(mhmc); // #E11
        auto mhma_arr = mhma.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhma_arr[i] != 3) {
                fails += " #E11 ";
                break;
            }
        }

        // USER move constructor
        Matrix<int, 2, 2, MatrixDataStorage::USER> mumc = std::move(mu); // #E12
        auto mumc_arr = mumc.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mumc_arr[i] != 4) {
                fails += " #E12 ";
                break;
            }
        }

        int muca_mem[4];
        // USER copy assignment
        Matrix<int, 2, 2, MatrixDataStorage::USER> muca(muca_mem, -1);
        muca = mumc; // #E13
        auto muca_arr = muca.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (muca_arr[i] != 4) {
                fails += " #E13 ";
                break;
            }
        }

        int muma_mem[4];
        // USER move assignment
        Matrix<int, 2, 2, MatrixDataStorage::USER> muma(muma_mem, -1);
        muma = std::move(muca); // #E14
        auto muma_arr = muma.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (muma_arr[i] != 4) {
                fails += " #E14 ";
                break;
            }
        }

        // WRONG CODE:
        //Matrix<T, M, N, MatrixDataStorage::USER> m = m_; // needs another pointer to memory

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Copy, move constructor, assignment)" << std::endl;
    }

    // Assignment with arithmetics operators (sum and sub)
    {
        std::string fails;

        Matrix<double, 3, 2> mx(1.5);
        Matrix<double, 3, 2, MatrixDataStorage::STACK> ms(2.5);
        Matrix<double, 3, 2, MatrixDataStorage::HEAP> mh(3.5);
        double mem[6];
        Matrix<double, 3, 2, MatrixDataStorage::USER> mu(mem, 4.5);

        // Add one matrix to another with possible type cast
        Matrix<int, 3, 2> mx0(7);
        mx0 += mx; // #F0
        auto mx0_arr = mx0.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != (7 + static_cast<int>(1.5))) {
                fails += " #F0 ";
                break;
            }
        }
        mx0 += ms; // #F1
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != (7 + static_cast<int>(1.5) + static_cast<int>(2.5))) {
                fails += " #F1 ";
                break;
            }
        }
        mx0 += mh; // #F2
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != (7 + static_cast<int>(1.5) + static_cast<int>(2.5) + static_cast<int>(3.5))) {
                fails += " #F2 ";
                break;
            }
        }
        mx0 += mu; // #F3
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != (7 + static_cast<int>(1.5) + static_cast<int>(2.5) + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F3 ";
                break;
            }
        }

        // Add one matrix to another with possible type cast (STACK)
        Matrix<int, 3, 2, MatrixDataStorage::STACK> ms0(-3);
        ms0 += mx; // #F4
        auto ms0_arr = ms0.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != (-3 + static_cast<int>(1.5))) {
                fails += " #F4 ";
                break;
            }
        }
        ms0 += ms; // #F5
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != (-3 + static_cast<int>(1.5) + static_cast<int>(2.5))) {
                fails += " #F5 ";
                break;
            }
        }
        ms0 += mh; // #F6
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != (-3 + static_cast<int>(1.5) + static_cast<int>(2.5) + static_cast<int>(3.5))) {
                fails += " #F6 ";
                break;
            }
        }
        ms0 += mu; // #F7
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != (-3 + static_cast<int>(1.5) + static_cast<int>(2.5) + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F7 ";
                break;
            }
        }

        // Add one matrix to another with possible type cast (HEAP)
        Matrix<int, 3, 2, MatrixDataStorage::HEAP> mh0(11);
        mh0 += mx; // #F8
        auto mh0_arr = mh0.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != (11 + static_cast<int>(1.5))) {
                fails += " #F8 ";
                break;
            }
        }
        mh0 += ms; // #F9
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != (11 + static_cast<int>(1.5) + static_cast<int>(2.5))) {
                fails += " #F9 ";
                break;
            }
        }
        mh0 += mh; // #F10
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != (11 + static_cast<int>(1.5) + static_cast<int>(2.5) + static_cast<int>(3.5))) {
                fails += " #F10 ";
                break;
            }
        }
        mh0 += mu; // #F11
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != (11 + static_cast<int>(1.5) + static_cast<int>(2.5) + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F11 ";
                break;
            }
        }

        // Add one matrix to another with possible type cast (USER)
        int mem0[6];
        Matrix<int, 3, 2, MatrixDataStorage::USER> mu0(mem0, 2);
        mu0 += mx; // #F12
        auto mu0_arr = mu0.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != (2 + static_cast<int>(1.5))) {
                fails += " #F12 ";
                break;
            }
        }
        mu0 += ms; // #F13
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != (2 + static_cast<int>(1.5) + static_cast<int>(2.5))) {
                fails += " #F13 ";
                break;
            }
        }
        mu0 += mh; // #F14
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != (2 + static_cast<int>(1.5) + static_cast<int>(2.5) + static_cast<int>(3.5))) {
                fails += " #F14 ";
                break;
            }
        }
        mu0 += mu; // #F15
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != (2 + static_cast<int>(1.5) + static_cast<int>(2.5) + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F15 ";
                break;
            }
        }

        // Subtract one matrix from another with possible type cast
        mx0 -= mx; // #F16
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != (7 + static_cast<int>(2.5) + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F16 ";
                break;
            }
        }
        mx0 -= ms; // #F17
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != (7 + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F17 ";
                break;
            }
        }
        mx0 -= mh; // #F18
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != (7 + static_cast<int>(4.5))) {
                fails += " #F18 ";
                break;
            }
        }
        mx0 -= mu; // #F19
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != 7) {
                fails += " #F19 ";
                break;
            }
        }

        // Subtract one matrix from another with possible type cast (STACK)
        ms0 -= mx; // #F20
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != (-3 + static_cast<int>(2.5) + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F20 ";
                break;
            }
        }
        ms0 -= ms; // #F21
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != (-3 + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F21 ";
                break;
            }
        }
        ms0 -= mh; // #F22
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != (-3 + static_cast<int>(4.5))) {
                fails += " #F22 ";
                break;
            }
        }
        ms0 -= mu; // #F23
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != -3) {
                fails += " #F23 ";
                break;
            }
        }

        // Subtract one matrix from another with possible type cast (HEAP)
        mh0 -= mx; // #F24
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != (11 + static_cast<int>(2.5) + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F24 ";
                break;
            }
        }
        mh0 -= ms; // #F25
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != (11 + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F25 ";
                break;
            }
        }
        mh0 -= mh; // #F26
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != (11 + static_cast<int>(4.5))) {
                fails += " #F26 ";
                break;
            }
        }
        mh0 -= mu; // #F27
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != 11) {
                fails += " #F27 ";
                break;
            }
        }

        // Subtract one matrix from another with possible type cast (USER)
        mu0 -= mx; // #F28
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != (2 + static_cast<int>(2.5) + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F28 ";
                break;
            }
        }
        mu0 -= ms; // #F29
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != (2 + static_cast<int>(3.5) + static_cast<int>(4.5))) {
                fails += " #F29 ";
                break;
            }
        }
        mu0 -= mh; // #F30
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != (2 + static_cast<int>(4.5))) {
                fails += " #F30 ";
                break;
            }
        }
        mu0 -= mu; // #F31
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != 2) {
                fails += " #F31 ";
                break;
            }
        }

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Assignment with arithmetics operators (sum and sub))" << std::endl;
    }

    // Assignment with arithmetics operators (mul and div)
    {
        std::string fails;

        // Multiply matrix by value (all elements) with possible type cast
        Matrix<int, 2, 3> mx(2);
        mx *= 2.5; // #G0
        auto mx0_arr = mx.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != 2 * static_cast<int>(2.5)) {
                fails += " #G0 ";
                break;
            }
        }
        // Divide matrix by value (all elements) with possible type cast
        mx /= 4.2; // #G1
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mx0_arr[i] != 2 * static_cast<int>(2.5) / static_cast<int>(4.2)) {
                fails += " #G1 ";
                break;
            }
        }

        // Multiply matrix by value (all elements) with possible type cast (STACK)
        Matrix<int, 2, 3, MatrixDataStorage::STACK> ms(4);
        ms *= 2.5; // #G2
        auto ms0_arr = ms.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != 4 * static_cast<int>(2.5)) {
                fails += " #G2 ";
                break;
            }
        }
        // Divide matrix by value (all elements) with possible type cast (STACK)
        ms /= 3.2; // #G3
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (ms0_arr[i] != 4 * static_cast<int>(2.5) / static_cast<int>(3.2)) {
                fails += " #G3 ";
                break;
            }
        }

        // Multiply matrix by value (all elements) with possible type cast (HEAP)
        Matrix<int, 2, 3, MatrixDataStorage::HEAP> mh(42);
        mh *= 3.3; // #G4
        auto mh0_arr = mh.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != 42 * static_cast<int>(3.3)) {
                fails += " #G4 ";
                break;
            }
        }
        // Divide matrix by value (all elements) with possible type cast (STACK)
        mh /= 10.2; // #G5
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mh0_arr[i] != 42 * static_cast<int>(3.3) / static_cast<int>(10.2)) {
                fails += " #G5 ";
                break;
            }
        }

        int mem[6];
        // Multiply matrix by value (all elements) with possible type cast (USER)
        Matrix<int, 2, 3, MatrixDataStorage::USER> mu(mem, 2);
        mu *= 10.3; // #G6
        auto mu0_arr = mu.read();
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != 2 * static_cast<int>(10.3)) {
                fails += " #G6 ";
                break;
            }
        }
        // Divide matrix by value (all elements) with possible type cast (STACK)
        mu /= 2.5; // #G7
        for (size_t i = 0; i < 3 * 2; ++i) {
            if (mu0_arr[i] != 2 * static_cast<int>(10.3) / static_cast<int>(2.5)) {
                fails += " #G7 ";
                break;
            }
        }

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Assignment with arithmetics operators (mul and div))" << std::endl;
    }

    // Arithmetic operators (unary plus and minus, binary sum and sub)
    {
        std::string fails;

        Matrix<int, 2, 2> mx(1);
        Matrix<int, 2, 2, MatrixDataStorage::STACK> ms(2);
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> mh(3);
        int mem[4];
        Matrix<int, 2, 2, MatrixDataStorage::USER> mu(mem, 4);

        // "+matrix" for UNSPECIFIED
        auto mxup = (+mx); // #H0
        auto mxup_arr = mxup.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxup_arr[i] != 1) {
                fails += " #H0 ";
                break;
            }
        }
        // "+matrix" for STACK
        auto msup = (+ms); // #H1
        auto msup_arr = msup.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msup_arr[i] != 2) {
                fails += " #H1 ";
                break;
            }
        }
        // "+matrix" for HEAP
        auto mhup = (+mh); // #H2
        auto mhup_arr = mhup.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhup_arr[i] != 3) {
                fails += " #H2 ";
                break;
            }
        }
        // "+matrix" for USER
        auto muup = (+mu); // #H3
        auto muup_arr = muup.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (muup_arr[i] != 4) {
                fails += " #H3 ";
                break;
            }
        }

        // "-matrix" for UNSPECIFIED
        auto mxum = (-mx); // #H4
        auto mxum_arr = mxum.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxum_arr[i] != -1) {
                fails += " #H4 ";
                break;
            }
        }
        // "-matrix" for STACK
        auto msum = (-ms); // #H5
        auto msum_arr = msum.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msum_arr[i] != -2) {
                fails += " #H5 ";
                break;
            }
        }
        // "-matrix" for HEAP
        auto mhum = (-mh); // #H6
        auto mhum_arr = mhum.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhum_arr[i] != -3) {
                fails += " #H6 ";
                break;
            }
        }
        // "-matrix" for USER
        auto muum = (-mu); // #H7
        auto muum_arr = muum.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (muum_arr[i] != -4) {
                fails += " #H7 ";
                break;
            }
        }

        Matrix<double, 2, 2> mx2(5.3);
        Matrix<double, 2, 2, MatrixDataStorage::STACK> ms2(6.3);
        Matrix<double, 2, 2, MatrixDataStorage::HEAP> mh2(7.3);
        double mem2[4];
        Matrix<double, 2, 2, MatrixDataStorage::USER> mu2(mem2, 8.3);

        // "matrix + matrix" for UNSPECIFIED and UNSPECIFIED
        auto mxpx = mx + mx2; // #H8
        auto mxpx_arr = mxpx.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxpx_arr[i] != 1 + 5.3) {
                fails += " #H8 ";
                break;
            }
        }
        // "matrix + matrix" for UNSPECIFIED and STACK
        auto mxps = mx + ms2; // #H9
        auto mxps_arr = mxps.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxps_arr[i] != 1 + 6.3) {
                fails += " #H9 ";
                break;
            }
        }
        // "matrix + matrix" for UNSPECIFIED and HEAP
        auto mxph = mx + mh2; // #H10
        auto mxph_arr = mxph.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxph_arr[i] != 1 + 7.3) {
                fails += " #H10 ";
                break;
            }
        }
        // "matrix + matrix" for UNSPECIFIED and USER
        auto mxpu = mx + mu2; // #H11
        auto mxpu_arr = mxpu.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxpu_arr[i] != 1 + 8.3) {
                fails += " #H11 ";
                break;
            }
        }
        // "matrix + matrix" for STACK and STACK
        auto msps = ms + ms2; // #H12
        auto msps_arr = msps.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msps_arr[i] != 2 + 6.3) {
                fails += " #H12 ";
                break;
            }
        }
        // "matrix + matrix" for STACK and HEAP
        auto msph = ms + mh2; // #H13
        auto msph_arr = msph.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msph_arr[i] != 2 + 7.3) {
                fails += " #H13 ";
                break;
            }
        }
        // "matrix + matrix" for STACK and USER
        auto mspu = ms + mu2; // #H14
        auto mspu_arr = mspu.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mspu_arr[i] != 2 + 8.3) {
                fails += " #H14 ";
                break;
            }
        }
        // "matrix + matrix" for HEAP and HEAP
        auto mhph = mh + mh2; // #H15
        auto mhph_arr = mhph.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhph_arr[i] != 3 + 7.3) {
                fails += " #H15 ";
                break;
            }
        }
        // "matrix + matrix" for HEAP and USER
        auto mhpu = mh + mu2; // #H16
        auto mhpu_arr = mhpu.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhpu_arr[i] != 3 + 8.3) {
                fails += " #H16 ";
                break;
            }
        }
        // "matrix + matrix" for USER and USER
        auto mupu = mu + mu2; // #H17
        auto mupu_arr = mupu.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mupu_arr[i] != 4 + 8.3) {
                fails += " #H17 ";
                break;
            }
        }

        // "matrix - matrix" for UNSPECIFIED and UNSPECIFIED
        auto mxmx = mx - mx2; // #H18
        auto mxmx_arr = mxmx.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxmx_arr[i] != 1 - 5.3) {
                fails += " #H18 ";
                break;
            }
        }
        // "matrix - matrix" for UNSPECIFIED and STACK
        auto mxms = mx - ms2; // #H19
        auto mxms_arr = mxms.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxms_arr[i] != 1 - 6.3) {
                fails += " #H19 ";
                break;
            }
        }
        // "matrix - matrix" for UNSPECIFIED and HEAP
        auto mxmh = mx - mh2; // #H20
        auto mxmh_arr = mxmh.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxmh_arr[i] != 1 - 7.3) {
                fails += " #H20 ";
                break;
            }
        }
        // "matrix - matrix" for UNSPECIFIED and USER
        auto mxmu = mx - mu2; // #H21
        auto mxmu_arr = mxmu.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxmu_arr[i] != 1 - 8.3) {
                fails += " #H21 ";
                break;
            }
        }
        // "matrix - matrix" for STACK and STACK
        auto msms = ms - ms2; // #H22
        auto msms_arr = msms.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msms_arr[i] != 2 - 6.3) {
                fails += " #H22 ";
                break;
            }
        }
        // "matrix - matrix" for STACK and HEAP
        auto msmh = ms - mh2; // #H23
        auto msmh_arr = msmh.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msmh_arr[i] != 2 - 7.3) {
                fails += " #H23 ";
                break;
            }
        }
        // "matrix - matrix" for STACK and USER
        auto msmu = ms - mu2; // #H24
        auto msmu_arr = msmu.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msmu_arr[i] != 2 - 8.3) {
                fails += " #H24 ";
                break;
            }
        }
        // "matrix - matrix" for HEAP and HEAP
        auto mhmh = mh - mh2; // #H25
        auto mhmh_arr = mhmh.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhmh_arr[i] != 3 - 7.3) {
                fails += " #H25 ";
                break;
            }
        }
        // "matrix - matrix" for HEAP and USER
        auto mhmu = mh - mu2; // #H26
        auto mhmu_arr = mhmu.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhmu_arr[i] != 3 - 8.3) {
                fails += " #H26 ";
                break;
            }
        }
        // "matrix - matrix" for USER and USER
        auto mumu = mu - mu2; // #H27
        auto mumu_arr = mumu.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mumu_arr[i] != 4 - 8.3) {
                fails += " #H27 ";
                break;
            }
        }

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Arithmetic operators (unary plus and minus, binary sum and sub))" << std::endl;
    }

    // Arithmetic operators (mul and div)
    {
        std::string fails;

        Matrix<int, 2, 2> mx(2);
        Matrix<int, 2, 2, MatrixDataStorage::STACK> ms(3);
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> mh(4);
        int mem[4];
        Matrix<int, 2, 2, MatrixDataStorage::USER> mu(mem, 5);

        // "matrix * scalar" for UNSPECIFIED
        auto mxs = mx * 3.1; // #I0
        auto mxs_arr = mxs.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxs_arr[i] != 2 * static_cast<int>(3.1)) {
                fails += " #I0 ";
                break;
            }
        }
        // "matrix * scalar" for STACK
        auto mss = ms * 4.1; // #I1
        auto mss_arr = mss.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mss_arr[i] != 3 * static_cast<int>(4.1)) {
                fails += " #I1 ";
                break;
            }
        }
        // "matrix * scalar" for HEAP
        auto mhs = mh * 5.1; // #I2
        auto mhs_arr = mhs.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhs_arr[i] != 4 * static_cast<int>(5.1)) {
                fails += " #I2 ";
                break;
            }
        }
        // "matrix * scalar" for USER
        auto mus = mu * 6.1; // #I3
        auto mus_arr = mus.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mus_arr[i] != 5 * static_cast<int>(6.1)) {
                fails += " #I3 ";
                break;
            }
        }

        // "scalar * matrix" for UNSPECIFIED
        auto smx = 3.2 * mx; // #I4
        auto smx_arr = smx.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (smx_arr[i] != 2 * static_cast<int>(3.2)) {
                fails += " #I4 ";
                break;
            }
        }
        // "scalar * matrix" for STACK
        auto sms = 4.2 * ms; // #I5
        auto sms_arr = sms.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (sms_arr[i] != 3 * static_cast<int>(4.2)) {
                fails += " #I5 ";
                break;
            }
        }
        // "scalar * matrix" for HEAP
        auto smh = 5.2 * mh; // #I6
        auto smh_arr = smh.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (smh_arr[i] != 4 * static_cast<int>(5.2)) {
                fails += " #I6 ";
                break;
            }
        }
        // "scalar * matrix" for USER
        auto smu = 6.2 * mu; // #I7
        auto smu_arr = smu.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (smu_arr[i] != 5 * static_cast<int>(6.2)) {
                fails += " #I7 ";
                break;
            }
        }

        // "matrix / scalar" for UNSPECIFIED
        auto mxd = mx / 2.1; // #I8
        auto mxd_arr = mxd.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mxd_arr[i] != 2 / static_cast<int>(2.1)) {
                fails += " #I8 ";
                break;
            }
        }
        // "matrix / scalar" for STACK
        auto msd = ms / 2.1; // #I9
        auto msd_arr = msd.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (msd_arr[i] != 3 / static_cast<int>(2.1)) {
                fails += " #I9 ";
                break;
            }
        }
        // "matrix / scalar" for HEAP
        auto mhd = mh / 2.1; // #I10
        auto mhd_arr = mhd.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mhd_arr[i] != 4 / static_cast<int>(2.1)) {
                fails += " #I10 ";
                break;
            }
        }
        // "matrix / scalar" for USER
        auto mud = mu / 2.1; // #I11
        auto mud_arr = mud.read();
        for (size_t i = 0; i < 2 * 2; ++i) {
            if (mud_arr[i] != 5 / static_cast<int>(2.1)) {
                fails += " #I11 ";
                break;
            }
        }

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Arithmetic operators (mul and div))" << std::endl;
    }

    // Comparison operators (eq and neq)
    {
        std::string fails;

        Matrix<int, 2, 2> mx2(2);
        Matrix<int, 2, 2> mx5(5);
        Matrix<int, 2, 2, MatrixDataStorage::STACK> ms2(2);
        Matrix<int, 2, 2, MatrixDataStorage::STACK> ms5(5);
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> mh2(2);
        Matrix<int, 2, 2, MatrixDataStorage::HEAP> mh5(5);
        double mem2[4];
        double mem5[4];
        Matrix<double, 2, 2, MatrixDataStorage::USER> mu2(mem2, 2);
        Matrix<double, 2, 2, MatrixDataStorage::USER> mu5(mem5, 5);

        // "matrix == matrix" for UNSPECIFIED and UNSPECIFIED
        bool j0 = mx2 == mx5; // #J0
        if (j0) {
            fails += " #J0 ";
        }
        // "matrix == matrix" for UNSPECIFIED and STACK
        bool j1 = mx2 == ms2; // #J1
        if (!j1) {
            fails += " #J1 ";
        }
        // "matrix == matrix" for UNSPECIFIED and HEAP
        bool j2 = mx2 == mh5; // #J2
        if (j2) {
            fails += " #J2 ";
        }
        // "matrix == matrix" for UNSPECIFIED and USER
        bool j3 = mx2 == mu2; // #J3
        if (!j3) {
            fails += " #J3 ";
        }
        // "matrix == matrix" for STACK and STACK
        bool j4 = ms2 == ms5; // #J4
        if (j4) {
            fails += " #J4 ";
        }
        // "matrix == matrix" for STACK and HEAP
        bool j5 = ms2 == mh2; // #J5
        if (!j5) {
            fails += " #J5 ";
        }
        // "matrix == matrix" for STACK and USER
        bool j6 = ms2 == mu5; // #J6
        if (j6) {
            fails += " #J6 ";
        }
        // "matrix == matrix" for HEAP and HEAP
        bool j7 = mh2 == mh2; // #J7
        if (!j7) {
            fails += " #J7 ";
        }
        // "matrix == matrix" for HEAP and USER
        bool j8 = mh2 == mu5; // #J8
        if (j8) {
            fails += " #J8 ";
        }
        // "matrix == matrix" for USER and USER
        bool j9 = mu5 == mu2; // #J9
        if (j9) {
            fails += " #J9 ";
        }

        // "matrix == matrix" for UNSPECIFIED and UNSPECIFIED
        bool j10 = mx2 != mx5; // #J10
        if (!j10) {
            fails += " #J10 ";
        }
        // "matrix == matrix" for UNSPECIFIED and STACK
        bool j11 = mx2 != ms2; // #J11
        if (j11) {
            fails += " #J11 ";
        }
        // "matrix == matrix" for UNSPECIFIED and HEAP
        bool j12 = mx2 != mh5; // #J12
        if (!j12) {
            fails += " #J12 ";
        }
        // "matrix == matrix" for UNSPECIFIED and USER
        bool j13 = mx2 != mu2; // #J13
        if (j13) {
            fails += " #J13 ";
        }
        // "matrix == matrix" for STACK and STACK
        bool j14 = ms2 != ms5; // #J14
        if (!j14) {
            fails += " #J14 ";
        }
        // "matrix == matrix" for STACK and HEAP
        bool j15 = ms2 != mh2; // #J15
        if (j15) {
            fails += " #J15 ";
        }
        // "matrix == matrix" for STACK and USER
        bool j16 = ms2 != mu5; // #J16
        if (!j16) {
            fails += " #J16 ";
        }
        // "matrix == matrix" for HEAP and HEAP
        bool j17 = mh2 != mh2; // #J17
        if (j17) {
            fails += " #J17 ";
        }
        // "matrix == matrix" for HEAP and USER
        bool j18 = mh2 != mu5; // #J18
        if (!j18) {
            fails += " #J18 ";
        }
        // "matrix == matrix" for USER and USER
        bool j19 = mu5 != mu2; // #J19
        if (!j19) {
            fails += " #J19 ";
        }

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Comparison operators (eq and neq))" << std::endl;
    }

    // Matrix multiplication
    {
        std::string fails;

        // Test 0 (random multiplication)
        Matrix<int, 5, 3> a = {  4, 7, 2,
                                 0, 1, 2,
                                -2, 1, 7,
                                -1, 0, 4,
                                 0, 1, 5 };
        int arr_b[] = { 4, 2, 5, 0, -2, -1 };
        Matrix<int, 3, 2, MatrixDataStorage::HEAP> b(arr_b);
        auto c = mul(a, b); // #K0
        Matrix<int, 5, 2, MatrixDataStorage::HEAP> d{ 47, 6, 1, -2, -17, -11, -12, -6, -5, -5 };
        if (c != d) {
            fails += " #K0 ";
        }

        // Test 1 (exponentiation)
        Matrix<int, 3, 3> aa = { -2, 4, 5,
                                  1, 3, 0,
                                 -4, 3, 2};
        auto a6 = mul(aa, mul(aa, mul(aa, mul(aa, mul(aa, aa))))); // #K1
        Matrix<int, 3, 3> a6_{ -1652,  3249, -1425,
                                 -54,  2698,  1155,
                                1833, -1086, -3716 };
        if (a6 != a6_) {
            fails += " #K1 ";
        }

        // Test 3 (big matrices)
        Matrix<int, 1000, 1000> x2(2);
        Matrix<int, 1000, 1000> x3(3);
        auto x2x3 = mul(x2, x3); // #K2
        auto x2x3_arr = x2x3.read();
        for (size_t i = 0; i < 1000 * 1000; ++i) {
            if (x2x3_arr[i] != 6000) {
                fails += " #K2 ";
                break;
            }
        }

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Matrix multiplication)" << std::endl;
    }

    // Determinant
    {
        std::string fails;

        // Test 0 (random determinant)
        Matrix<double, 4, 4> m0 = {  3, -2,  1,  1,
                                     5,  1,  2,  0,
                                    -1,  1, -1,  1,
                                     2, -1,  6, -3 };
        auto det_m0 = det(m0); // #L0
        if (static_cast<int>(std::round(det_m0)) != -69) {
            fails += " #L0 ";
        }

        // Test 1 (linear dependence)
        Matrix<double, 4, 4> m1 = {  3, -2,  1,  1, // a
                                     6, -4,  2,  2, // 2a
                                    -1,  1, -1,  1,
                                     2, -1,  6, -3 };
        auto det_m1 = det(m1); // #L1
        if (static_cast<int>(std::round(det_m1)) != 0) {
            fails += " #L1 ";
        }

        // Test 2 (big Identity matrix)
        double *mem = new double[625]();
        for (size_t i = 0; i < 25; ++i) {
            mem[i * 25 + i] = 1;
        }
        Matrix<double, 25, 25, MatrixDataStorage::USER> m2(mem);
        auto det_m2 = det(m2); // #L2
        if (static_cast<int>(std::round(det_m2)) != 1) {
            fails += " #L2 ";
        }
        delete[] mem;

        // Test 3 (determinant of multiplied matrices)
        Matrix<double, 5, 5> m3 = {  1, -4, 2,  5,  7,
                                     0,  0, 3,  5,  3,
                                     3,  1, 7, -3, -2,
                                    -1,  0, 5,  2,  4,
                                     8,  9, 7,  1,  0 }; // det(m3)==6974
        Matrix<double, 5, 5> m4 = {  4, 0, 1,  3,  3,
                                    -3, 9, 0,  4,  0,
                                    -2, 0, 3,  2, -4,
                                     0, 8, 0,  6,  0,
                                     2, 7, 5, -6, -7 }; // det(m4)==-6122
        auto m3m4 = mul(m3, m4);
        if (static_cast<int>(std::round(det(m3) * det(m4))) != static_cast<int>(std::round(det(m3m4)))) { // #L3
            fails += " #L3 ";
        }

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Determinant)" << std::endl;
    }

    // Other
    {
        std::string fails;

        // Printing matrix of std::string (#M0)
        Matrix<std::string, 2, 3, MatrixDataStorage::UNSPECIFIED> m = { "Te", "mat", "p", "Fri", "i", "ma" };
        Matrix<std::string, 2, 3, MatrixDataStorage::STACK> l = { "s", "ri", "rint", "end", "", "g" };
        Matrix<std::string, 2, 3, MatrixDataStorage::HEAP> p = { "ting", "x", "", "ship", "s", "ic" };
        auto mlp = m + l + p;
        mlp.print();

        // Algebra (#M1)
        Matrix<int, 3, 3> a(3);
        Matrix<double, 3, 3, MatrixDataStorage::STACK> b(-6.2);
        Matrix<int, 3, 3, MatrixDataStorage::HEAP> c(12);
        Matrix<float, 3, 3> d(7.5);
        Matrix<int, 3, 3, MatrixDataStorage::STACK> e(2);
        auto x = a - c * 2 - d / 7 + (-b + e); // -13.8714
        auto x_arr = x.read();
        for (size_t i = 0; i < 3 * 3; ++i) {
            auto diff = std::abs(x_arr[i] - (3 - 12 * 2 - (float)7.5 / 7 + (-(-6.2) + 2)));
            auto magnitude = std::abs(x_arr[i] + (3 - 12 * 2 - (float)7.5 / 7 + (-(-6.2) + 2)));
            if (diff > std::numeric_limits<decltype(diff)>::epsilon() * magnitude * 2) {
                fails += " #M1 ";
                break;
            }
        }

        std::cout << (!fails.empty() ? (" !!! FAILED !!! [" + fails + "] ") : "PASSED ")
            << "(Other)" << std::endl;
    }

    std::cout << "\n[END TESTING]" << std::endl;

#ifdef _MSC_VER
    system("pause>nul"); // usefull for Visual Studio
#endif
    return 0;
}
