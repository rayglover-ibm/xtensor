/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef BENCHMARK_CONTAINER_HPP
#define BENCHMARK_CONTAINER_HPP

#include "xtensor/xarray.hpp"
#include "xtensor/xtensor.hpp"
#include <cstddef>
#include <chrono>
#include <string>

namespace xt
{
    template <class T, class A>
    std::string get_container_name(const std::vector<T, A>&)
    {
        return "std::vector";
    }

    template <class T, class A>
    std::string get_container_name(const uvector<T, A>&)
    {
        return "xt::uvector";
    }

    namespace axpy_1d
    {
        template <class E>
        inline auto benchmark_iteration(const E& x, const E& y, E& res, typename E::value_type a, std::size_t number)
        {
            auto start = std::chrono::steady_clock::now();
            for (std::size_t i = 0; i < number; ++i)
            {
                auto iterx = x.begin();
                auto itery = y.begin();
                for (auto iter = res.begin(); iter != res.end(); ++iter, ++iterx, ++itery)
                {
                    *iter = a * (*iterx) + (*itery);
                }
            }
            auto end = std::chrono::steady_clock::now();
            auto diff = end - start;
            return diff;
        }

        template <class E>
        inline auto benchmark_xiteration(const E& x, const E& y, E& res, typename E::value_type a, std::size_t number)
        {
            auto start = std::chrono::steady_clock::now();
            for (std::size_t i = 0; i < number; ++i)
            {
                auto iterx = x.xbegin();
                auto itery = y.xbegin();
                for (auto iter = res.xbegin(); iter != res.xend(); ++iter, ++iterx, ++itery)
                {
                    *iter = a * (*iterx) + (*itery);
                }
            }
            auto end = std::chrono::steady_clock::now();
            auto diff = end - start;
            return diff;
        }

        template <class E>
        inline auto benchmark_indexing(const E& x, const E& y, E& res, typename E::value_type a, std::size_t number)
        {
            using size_type = typename E::size_type;
            auto start = std::chrono::steady_clock::now();
            for (std::size_t i = 0; i < number; ++i)
            {
                size_type n = x.size();
                for (size_type i = 0; i < n; ++i)
                {
                    res(i) = a * x(i) + y(i);
                }
            }
            auto end = std::chrono::steady_clock::now();
            auto diff = end - start;
            return diff;
        }

        template <class E>
        inline void init_benchmark(E& x, E& y, E& res, typename E::size_type size)
        {
            x.reshape({ size });
            y.reshape({ size });
            res.reshape({ size });

            using value_type = typename E::value_type;
            using size_type = typename E::size_type;
            for (size_type i = 0; i < size; ++i)
            {
                x(i) = 0.5 + value_type(i);
                y(i) = 0.25 * value_type(i);
            }
        }

        template <class C, class OS>
        void benchmark(OS& out)
        {
            using duration_type = std::chrono::duration<double, std::milli>;
            using size_type = xarray<double>::size_type;
            constexpr size_type size = 1000;
            constexpr size_type number = 10000;
            double a = 2.7;

            using array_type = xarray_container<C>;
            array_type ax, ay, ares;
            init_benchmark(ax, ay, ares, size);

            using tensor_type = xtensor_container<C, 1>;
            tensor_type tx, ty, tres;
            init_benchmark(tx, ty, tres, size);

            duration_type aiter = benchmark_iteration(ax, ay, ares, a, number);
            duration_type titer = benchmark_iteration(tx, ty, tres, a, number);
            duration_type axiter = benchmark_xiteration(ax, ay, ares, a, number);
            duration_type txiter = benchmark_xiteration(tx, ty, tres, a, number);
            duration_type aindex = benchmark_indexing(ax, ay, ares, a, number);
            duration_type tindex = benchmark_indexing(tx, ty, tres, a, number);

            std::string cname = get_container_name(C());
            out << "************************" << std::string(cname.size(), '*') << std::endl;
            out << "* AXPY 1D BENCHMARK : " << cname << " *" << std::endl;
            out << "************************" << std::string(cname.size(), '*') << std::endl << std::endl;

            out << "xarray   iteration: " << aiter.count() << "ms" << std::endl;
            out << "xtensor  iteration: " << titer.count() << "ms" << std::endl;
            out << "xarray  xiteration: " << axiter.count() << "ms" << std::endl;
            out << "xtensor xiteration: " << txiter.count() << "ms" << std::endl;
            out << "xarray    indexing: " << aindex.count() << "ms" << std::endl;
            out << "xtensor   indexing: " << tindex.count() << "ms" << std::endl;
            out << std::endl;
        }
    }

    namespace func
    {
        template <class E>
        inline auto benchmark_assign(const E& x, const E& y, const E& z, E& res, std::size_t number)
        {
            auto start = std::chrono::steady_clock::now();
            for (std::size_t i = 0; i < number; ++i)
            {
                res = 3 * x - 2 * y * z;
            }
            auto end = std::chrono::steady_clock::now();
            auto diff = end - start;
            return diff;
        }

        template <class E>
        inline void init_benchmark(E& x, E& y, E& z, E& res)
        {
            using value_type = typename E::value_type;
            using size_type = typename E::size_type;
            using shape_type = typename E::shape_type;

            shape_type shape = { 4, 3, 5 };

            x.reshape(shape);
            y.reshape(shape);
            z.reshape(shape);
            res.reshape(shape);

            for (size_type i = 0; i < shape[0]; ++i)
            {
                for (size_type j = 0; j < shape[1]; ++j)
                {
                    for (size_type k = 0; k < shape[2]; ++k)
                    {
                        x(i, j, k) = 0.25 * value_type(i) + 0.5 * value_type(j) - 0.01 * value_type(k);
                        y(i, j, k) = 0.31 * value_type(i) - 0.2 * value_type(j) + 0.07 * value_type(k);
                        z(i, j, k) = 0.27 * value_type(i) + 0.4 * value_type(j) - 0.03 * value_type(k);
                    }
                }
            }
        }

        template <class C, class OS>
        void benchmark(OS& out)
        {
            using duration_type = std::chrono::duration<double, std::milli>;
            std::size_t number = 2000;

            using array_type = xarray_container<C>;
            array_type ax, ay, az, ares;
            init_benchmark(ax, ay, az, ares);

            using tensor_type = xtensor_container<C, 3>;
            tensor_type tx, ty, tz, tres;
            init_benchmark(tx, ty, tz, tres);

            duration_type aassign = benchmark_assign(ax, ay, az, ares, number);
            duration_type tassign = benchmark_assign(tx, ty, tz, tres, number);

            std::string cname = get_container_name(C());
            out << "*********************************" << std::string(cname.size(), '*') << std::endl;
            out << "* XFUNCTION ASSIGN BENCHMARK : " << cname << " *" << std::endl;
            out << "*********************************" << std::string(cname.size(), '*') << std::endl << std::endl;

            out << "benchmark: res = 3 * x - 2 * y * z; - res resized out of the benchmark" << std::endl;
            out << "xarray : " << aassign.count() << "ms" << std::endl;
            out << "xtensor: " << tassign.count() << "ms" << std::endl;
            out << std::endl;
        }
    }

    namespace sum_assign
    {
        template <class E>
        inline auto benchmark_assign(const E& x, const E& y, E& res)
        {
            auto start = std::chrono::steady_clock::now();
            res = 3 * x - 2 * y;
            auto end = std::chrono::steady_clock::now();
            auto diff = end - start;
            return diff;
        }

        template <class E>
        inline void init_benchmark(E& x, E& y)
        {
            using value_type = typename E::value_type;
            using size_type = typename E::size_type;
            using shape_type = typename E::shape_type;

            shape_type shape = { 100, 100 };

            x.reshape(shape);
            y.reshape(shape);

            for (size_type i = 0; i < shape[0]; ++i)
            {
                for (size_type j = 0; j < shape[1]; ++j)
                {
                    x(i, j) = 0.25 * value_type(i) + 0.5 * value_type(j);
                    y(i, j) = 0.31 * value_type(i) - 0.2 * value_type(j);
                }
            }
        }

        template <class C, class OS>
        void benchmark(OS& out)
        {
            using duration_type = std::chrono::duration<double, std::milli>;

            using array_type = xarray_container<C>;
            array_type ax, ay, ares;
            init_benchmark(ax, ay);

            using tensor_type = xtensor_container<C, 2>;
            tensor_type tx, ty, tres;
            init_benchmark(tx, ty);

            duration_type aassign = benchmark_assign(ax, ay, ares);
            duration_type tassign = benchmark_assign(tx, ty, tres);

            std::string cname = get_container_name(C());
            out << "***************************************" << std::string(cname.size(), '*') << std::endl;
            out << "* XFUNCTION FULL ASSIGN BENCHMARK : " << cname << " *" << std::endl;
            out << "***************************************" << std::string(cname.size(), '*') << std::endl << std::endl;

            out << "benchmark: res = 3 * x - 2 * y; - res resized inside the benchmark" << std::endl;
            out << "xarray : " << aassign.count() << "ms" << std::endl;
            out << "xtensor: " << tassign.count() << "ms" << std::endl;
            out << std::endl;
        }
    }
}

#endif
