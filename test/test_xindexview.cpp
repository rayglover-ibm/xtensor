/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"
#include "xtensor/xarray.hpp"
#include "xtensor/xrandom.hpp"
#include "xtensor/xindexview.hpp"
#include "xtensor/xbroadcast.hpp"
#include "xtensor/xview.hpp"
#include "test_common.hpp"

namespace xt
{
    using std::size_t;

    TEST(xindexview, indices)
    {
        xarray<double> e = xt::random::rand<double>({3, 3});
        xarray<double> e_copy = e;
        auto v = make_xindexview(e, {{1, 1}, {1, 2}, {2, 2}});

        using shape_type = typename decltype(v)::shape_type;
        EXPECT_EQ(shape_type{3}, v.shape());
        
        EXPECT_EQ(e(1, 1), v(0));
        EXPECT_EQ(e(1, 2), v[{1}]);

        std::vector<size_t> idx = {2};
        EXPECT_EQ(e(2, 2), v.element(idx.begin(), idx.end()));

        v += 3;
        auto expected = e_copy(1, 1) + 3;
        EXPECT_EQ(expected, e(1, 1));

        auto t = v + 3;
        EXPECT_EQ((e_copy(1, 1) + 6), t(0));
        EXPECT_EQ((e(1, 1) + 3), t(0));

        v = broadcast(123, v.shape());
        EXPECT_EQ(123, e(1, 1));
        EXPECT_EQ(123, e(1, 2));
        EXPECT_EQ(123, e(2, 2));

        xarray<double> as = {3, 3, 3};
        v = as;
        EXPECT_TRUE(all(equal(v, as)));
        EXPECT_EQ(3, e(2, 2));
    }

    TEST(xindexview, boolean)
    {
        xarray<double> e = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        auto v = make_xfilter(e, e > 0);
        EXPECT_EQ(1, v(0));

        v += 2;
        EXPECT_EQ(3, e(1, 1));
        EXPECT_EQ(3, v(1));

        v += xarray<double>{1, 2, 3};
        EXPECT_EQ(5, e(1, 1));
        EXPECT_EQ(6, e(2, 2));

        xarray<double> e2 = random::rand<double>({3, 3, 3, 3});
        auto v2 = make_xfilter(e2, e2 > 0.5);
        v2 *= 0;
        EXPECT_TRUE(!any(e2 > 0.5));
    }

    TEST(xindexview, indices_on_function)
    {
        xarray<double> e = xt::random::rand<double>({3, 3});
        auto fn = e * 3 - 120;
        auto v = make_xindexview(fn, {{1, 1}, {1, 2}, {2, 2}});
        EXPECT_EQ(fn(1, 1), v(0));
        EXPECT_EQ(fn(1, 2), v[{1}]);

        std::vector<size_t> idx = {2};
        EXPECT_EQ(fn(2, 2), v.element(idx.begin(), idx.end()));

        auto it = v.begin();
        EXPECT_EQ(fn(1, 1), *it);

        EXPECT_EQ(fn(1, 2), *(++it));
        EXPECT_EQ(fn(2, 2), *(++it));
        EXPECT_EQ(++it, v.end());
    }

    TEST(xindexview, view_on_view)
    {
        xarray<double> e = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        auto v = make_xfilter(e, e > 0);
        auto v_on_v = make_xview(v, 1);
        v_on_v(0) = 10;
        EXPECT_EQ(10, e(1, 1));
    }
}