/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include <cstddef>
#include "xtensor/xarray.hpp"
#include "xtensor/xtensor.hpp"

namespace xt
{
    template <class C, std::size_t>
    struct redim_container
    {
        using type = C;
    };

    template <class T, std::size_t N, layout_type L, std::size_t NN>
    struct redim_container<xtensor<T, N, L>, NN>
    {
        using type = xtensor<T, NN, L>;
    };

    template <class C, std::size_t N>
    using redim_container_t = typename redim_container<C, N>::type;

    template <class C, class T>
    struct rebind_container;

    template <class T, class NT>
    struct rebind_container<xarray<T>, NT>
    {
        using type = xarray<NT>;
    };

    template <class T, std::size_t N, class NT>
    struct rebind_container<xtensor<T, N>, NT>
    {
        using type = xtensor<NT, N>;
    };

    template <class C, class T>
    using rebind_container_t = typename rebind_container<C, T>::type;

    template <class C>
    class operation : public ::testing::Test
    {
    public:
        using container_type = C;
    };

    using testing_types = ::testing::Types<xarray<double>, xtensor<double, 2>>;
    TYPED_TEST_CASE(operation, testing_types);

    TYPED_TEST(operation, plus)
    {
        using shape_type = typename TypeParam::shape_type;
        shape_type shape = {3, 2};
        TypeParam a(shape, 4.5);
        double ref = +(a(0, 0));
        double actual = (+a)(0, 0);
        EXPECT_EQ(ref, actual);
    }

    TYPED_TEST(operation, minus)
    {
        using shape_type = typename TypeParam::shape_type;
        shape_type shape = {3, 2};
        TypeParam a(shape, 4.5);
        double ref = -(a(0, 0));
        double actual = (-a)(0, 0);
        EXPECT_EQ(ref, actual);
    }

    TYPED_TEST(operation, add)
    {
        using shape_type = typename TypeParam::shape_type;
        shape_type shape = {3, 2};
        TypeParam a(shape, 4.5);
        TypeParam b(shape, 1.3);
        EXPECT_EQ((a + b)(0, 0), a(0, 0) + b(0, 0));

        double sb = 1.2;
        EXPECT_EQ((a + sb)(0, 0), a(0, 0) + sb);

        double sa = 4.6;
        EXPECT_EQ((sa + b)(0, 0), sa + b(0, 0));
    }

    TYPED_TEST(operation, subtract)
    {
        using shape_type = typename TypeParam::shape_type;
        shape_type shape = {3, 2};
        TypeParam a(shape, 4.5);
        TypeParam b(shape, 1.3);
        EXPECT_EQ((a - b)(0, 0), a(0, 0) - b(0, 0));

        double sb = 1.2;
        EXPECT_EQ((a - sb)(0, 0), a(0, 0) - sb);

        double sa = 4.6;
        EXPECT_EQ((sa - b)(0, 0), sa - b(0, 0));
    }

    TYPED_TEST(operation, multiply)
    {
        using shape_type = typename TypeParam::shape_type;
        shape_type shape = {3, 2};
        TypeParam a(shape, 4.5);
        TypeParam b(shape, 1.3);
        EXPECT_EQ((a * b)(0, 0), a(0, 0) * b(0, 0));

        double sb = 1.2;
        EXPECT_EQ((a * sb)(0, 0), a(0, 0) * sb);

        double sa = 4.6;
        EXPECT_EQ((sa * b)(0, 0), sa * b(0, 0));
    }

    TYPED_TEST(operation, divide)
    {
        using shape_type = typename TypeParam::shape_type;
        shape_type shape = {3, 2};
        TypeParam a(shape, 4.5);
        TypeParam b(shape, 1.3);
        EXPECT_EQ((a / b)(0, 0), a(0, 0) / b(0, 0));

        double sb = 1.2;
        EXPECT_EQ((a / sb)(0, 0), a(0, 0) / sb);

        double sa = 4.6;
        EXPECT_EQ((sa / b)(0, 0), sa / b(0, 0));
    }

    TYPED_TEST(operation, less)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using bool_container = rebind_container_t<container_1d, bool>;
        container_1d a = {1, 2, 3, 4, 5};
        bool_container expected = {1, 1, 1, 0, 0};
        bool_container b = a < 4;
        EXPECT_EQ(expected, b);
    }

    TYPED_TEST(operation, less_equal)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using bool_container = rebind_container_t<container_1d, bool>;
        container_1d a = {1, 2, 3, 4, 5};
        bool_container expected = {1, 1, 1, 1, 0};
        bool_container b = a <= 4;
        EXPECT_EQ(expected, b);
    }

    TYPED_TEST(operation, greater)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using bool_container = rebind_container_t<container_1d, bool>;
        container_1d a = {1, 2, 3, 4, 5};
        bool_container expected = {0, 0, 0, 0, 1};
        bool_container b = a > 4;
        EXPECT_EQ(expected, b);
    }

    TYPED_TEST(operation, greater_equal)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using bool_container = rebind_container_t<container_1d, bool>;
        container_1d a = {1, 2, 3, 4, 5};
        bool_container expected = {0, 0, 0, 1, 1};
        bool_container b = a >= 4;
        EXPECT_EQ(expected, b);
    }

    TYPED_TEST(operation, negate)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using bool_container = rebind_container_t<container_1d, bool>;
        container_1d a = {1, 2, 3, 4, 5};
        bool_container expected = {1, 1, 1, 0, 0};
        bool_container b = !(a >= 4);
        EXPECT_EQ(expected, b);
    }

    TYPED_TEST(operation, equal)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using bool_container = rebind_container_t<container_1d, bool>;
        container_1d a = {1, 2, 3, 4, 5};
        bool_container expected = {0, 0, 0, 1, 0};
        bool_container b = equal(a, 4);
        EXPECT_EQ(expected, b);

        container_1d other = {1, 2, 3, 0, 0};
        bool_container b_2 = equal(a, other);
        bool_container expected_2 = {1, 1, 1, 0, 0};
        EXPECT_EQ(expected_2, b_2);
    }

    TYPED_TEST(operation, not_equal)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using bool_container = rebind_container_t<container_1d, bool>;
        container_1d a = {1, 2, 3, 4, 5};
        bool_container expected = {1, 1, 1, 0, 1};
        bool_container b = not_equal(a, 4);
        EXPECT_EQ(expected, b);

        container_1d other = {1, 2, 3, 0, 0};
        bool_container b_2 = not_equal(a, other);
        bool_container expected_2 = {0, 0, 0, 1, 1};
        EXPECT_EQ(expected_2, b_2);
    }

    TYPED_TEST(operation, logical_and)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using bool_container = rebind_container_t<container_1d, bool>;
        bool_container a = {0, 0, 0, 1, 0};
        bool_container expected = {0, 0, 0, 0, 0};
        bool_container b = a && 0;
        bool_container c = a && a;
        EXPECT_EQ(expected, b);
        EXPECT_EQ(c, a);
    }

    TYPED_TEST(operation, logical_or)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using bool_container = rebind_container_t<container_1d, bool>;
        bool_container a = {0, 0, 0, 1, 0};
        bool_container other = {0, 0, 0, 0, 0};
        bool_container b = a || other;
        bool_container c = a || 0;
        bool_container d = a || 1;
        EXPECT_EQ(b, a);
        EXPECT_EQ(c, a);

        bool_container expected = {1, 1, 1, 1, 1};
        EXPECT_EQ(expected, d);
    }

    TYPED_TEST(operation, any)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using int_container = rebind_container_t<container_1d, int>;
        using int_container_2d = rebind_container_t<TypeParam, int>;
        int_container a = {0, 0, 3};
        EXPECT_EQ(true, any(a));
        int_container_2d b = {{0, 0, 0}, {0, 0, 0}};
        EXPECT_EQ(false, any(b));
    }

    TYPED_TEST(operation, minimum)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using int_container = rebind_container_t<container_1d, int>;
        int_container a = {0, 0, 3};
        int_container b = {-1, 0, 10};
        int_container expected = {-1, 0, 3};
        EXPECT_TRUE(all(equal(minimum(a, b), expected)));
    }

    TYPED_TEST(operation, maximum)
    {
        using container_1d = redim_container_t<TypeParam, 1>;
        using int_container = rebind_container_t<container_1d, int>;
        int_container a = {0, 0, 3};
        int_container b = {-1, 0, 10};
        int_container expected = {0, 0, 10};
        int_container expected_2 = {0, 1, 10};
        EXPECT_TRUE(all(equal(maximum(a, b), expected)));
        EXPECT_TRUE(all(equal(maximum(arange(0, 3), b), expected_2)));
    }

    TYPED_TEST(operation, amax)
    {
        using int_container_2d = rebind_container_t<TypeParam, int>;
        using container_1d = redim_container_t<TypeParam, 1>;
        using int_container_1d = rebind_container_t<container_1d, int>;
        int_container_2d a = {{0, 0, 3}, {1, 2, 10}};
        EXPECT_EQ(10, amax(a)());
        int_container_1d e1 = {1, 2, 10};
        EXPECT_EQ(e1, amax(a, {0}));
        int_container_1d e2 = {3, 10};
        EXPECT_EQ(e2, amax(a, {1}));
    }

    TYPED_TEST(operation, amin)
    {
        using int_container_2d = rebind_container_t<TypeParam, int>;
        using container_1d = redim_container_t<TypeParam, 1>;
        using int_container_1d = rebind_container_t<container_1d, int>;
        int_container_2d a = {{0, 0, 3}, {1, 2, 10}};
        EXPECT_EQ(0, amin(a)());
        int_container_1d e1 = {0, 0, 3};
        EXPECT_EQ(e1, amin(a, {0}));
        int_container_1d e2 = {0, 1};
        EXPECT_EQ(e2, amin(a, {1}));
    }

    TYPED_TEST(operation, all)
    {
        using int_container_2d = rebind_container_t<TypeParam, int>;
        using container_1d = redim_container_t<TypeParam, 1>;
        using int_container_1d = rebind_container_t<container_1d, int>;
        int_container_1d a = {1, 1, 3};
        EXPECT_EQ(true, all(a));
        int_container_2d b = {{0, 2, 1}, {2, 1, 0}};
        EXPECT_EQ(false, all(b));
    }

    TYPED_TEST(operation, all_layout)
    {
        xarray<int, layout_type::row_major> a = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
        xarray<int, layout_type::column_major> b = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
        EXPECT_EQ(a(0, 1), b(0, 1));
        EXPECT_TRUE(all(equal(a, b)));
    }

    TYPED_TEST(operation, nonzero)
    {
        using int_container_2d = rebind_container_t<TypeParam, int>;
        using container_1d = redim_container_t<TypeParam, 1>;
        using int_container_1d = rebind_container_t<container_1d, int>;
        using container_3d = redim_container_t<TypeParam, 3>;
        using bool_container = rebind_container_t<container_3d, bool>;
        using shape_type = typename container_3d::shape_type;

        int_container_1d a = {1, 0, 3};
        std::vector<xindex_type_t<typename int_container_1d::shape_type>> expected = {{0}, {2}};
        EXPECT_EQ(expected, nonzero(a));

        int_container_2d b = {{0, 2, 1}, {2, 1, 0}};
        std::vector<xindex_type_t<typename int_container_2d::shape_type>> expected_b = {{0, 1}, {0, 2}, {1, 0}, {1, 1}};
        EXPECT_EQ(expected_b, nonzero(b));

        auto c = equal(b, 0);
        std::vector<xindex_type_t<typename int_container_2d::shape_type>> expected_c = {{0, 0}, {1, 2}};
        EXPECT_EQ(expected_c, nonzero(c));

        shape_type s = {3, 3, 3};
        bool_container d(s);
        std::fill(d.begin(), d.end(), true);

        auto d_nz = nonzero(d);
        EXPECT_EQ(3 * 3 * 3, d_nz.size());
        xindex_type_t<typename container_3d::shape_type> last_idx = {2, 2, 2};
        EXPECT_EQ(last_idx, d_nz.back());
    }

    TYPED_TEST(operation, where_only_condition)
    {
        using int_container_2d = rebind_container_t<TypeParam, int>;
        int_container_2d a = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        std::vector<xindex_type_t<typename int_container_2d::shape_type>> expected = {{0, 0}, {1, 1}, {2, 2}};
        EXPECT_EQ(expected, where(a));
    }
}
