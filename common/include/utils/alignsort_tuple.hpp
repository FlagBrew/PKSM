/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#ifndef ALIGNSORT_TUPLE_HPP
#define ALIGNSORT_TUPLE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

namespace internal
{
    template <typename ISeq>
    struct SequenceDetails
    {
    private:
        template <std::size_t Head, std::size_t... Tail>
        static std::integral_constant<std::size_t, Head> IndexSequenceHead(
            std::index_sequence<Head, Tail...>);

        template <std::size_t Head, std::size_t... Tail>
        static std::index_sequence<Tail...> IndexSequenceTail(std::index_sequence<Head, Tail...>);

    public:
        static constexpr std::size_t head = decltype(IndexSequenceHead(std::declval<ISeq>())){};
        using tail                        = decltype(IndexSequenceTail(std::declval<ISeq>()));
    };

    template <std::size_t Idx, typename ISeq>
    struct SequencePrepend
    {
    private:
        template <std::size_t... Tail>
        static std::index_sequence<Idx, Tail...> Prepend(std::index_sequence<Tail...>);

    public:
        using type = decltype(Prepend(std::declval<ISeq>()));
    };

    template <bool IsGEqual, typename details, typename origSubCalc, typename Arg0,
        typename... Args>
    struct calc_helper;

    template <typename seq, typename... Args>
    struct alignsort_tuple_calc_help;

    template <typename details, typename origSubCalc, typename Arg0, typename... Args>
    struct calc_helper<true, details, origSubCalc, Arg0, Args...>
    {
        using indices =
            typename SequencePrepend<details::head, typename origSubCalc::indices>::type;
        using type                             = decltype(std::tuple_cat(
            std::declval<std::tuple<Arg0>>(), std::declval<typename origSubCalc::type>()));
        static constexpr std::size_t max_align = alignof(Arg0);
    };

    template <typename details, typename origSubCalc, typename Arg0, typename... Args>
    struct calc_helper<false, details, origSubCalc, Arg0, Args...>
    {
    private:
        using oldCalcDetails      = SequenceDetails<typename origSubCalc::indices>;
        using oldCalcMaxAlignType = std::tuple_element_t<0, typename origSubCalc::type>;

        using newSubCalcIndices =
            typename SequencePrepend<details::head, typename oldCalcDetails::tail>::type;

        template <typename OldTupleHead, typename... OldTupleTail>
        static std::tuple<OldTupleTail...> GetTupleTail(std::tuple<OldTupleHead, OldTupleTail...>);

        using oldTupleTail = decltype(GetTupleTail(std::declval<typename origSubCalc::type>()));

        template <typename... OldTail>
        static alignsort_tuple_calc_help<newSubCalcIndices, Arg0, OldTail...> GetHelp(
            std::tuple<OldTail...>);

        using newSubCalc = decltype(GetHelp(std::declval<oldTupleTail>()));

    public:
        using type = decltype(std::tuple_cat(std::declval<std::tuple<oldCalcMaxAlignType>>(),
            std::declval<typename newSubCalc::type>()));
        using indices =
            typename SequencePrepend<oldCalcDetails::head, typename newSubCalc::indices>::type;
        static constexpr std::size_t max_align = alignof(oldCalcMaxAlignType);
    };

    template <typename Indices, typename Arg0, typename... Args>
    struct alignsort_tuple_calc_help<Indices, Arg0, Args...>
    {
    private:
        using details     = SequenceDetails<Indices>;
        using origSubCalc = alignsort_tuple_calc_help<typename details::tail, Args...>;

        using helper = calc_helper<alignof(Arg0) >= origSubCalc::max_align, details, origSubCalc,
            Arg0, Args...>;

    public:
        using indices                          = typename helper::indices;
        using type                             = typename helper::type;
        static constexpr std::size_t max_align = helper::max_align;
    };

    template <typename Indices, typename Arg>
    struct alignsort_tuple_calc_help<Indices, Arg>
    {
        using type                             = std::tuple<Arg>;
        using indices                          = Indices;
        static constexpr std::size_t max_align = alignof(Arg);
    };

    template <typename Indices>
    struct alignsort_tuple_calc_help<Indices>
    {
        using type                             = std::tuple<>;
        using indices                          = Indices;
        static constexpr std::size_t max_align = 0;
    };

    template <typename... Args>
    struct alignsort_tuple_calc
    {
    private:
        using helper = alignsort_tuple_calc_help<std::index_sequence_for<Args...>, Args...>;

    public:
        using type                             = typename helper::type;
        using indices                          = typename helper::indices;
        static constexpr std::size_t max_align = helper::max_align;
    };

    template <typename... Ts>
    using alignsort_tuple_calc_t = typename alignsort_tuple_calc<Ts...>::type;

    template <std::size_t Which, typename IdxSeq>
        requires (Which < IdxSeq::size())
    struct IdxAccess
    {
    private:
        using details = SequenceDetails<IdxSeq>;

    public:
        static constexpr std::size_t value = IdxAccess<Which - 1, typename details::tail>::value;
    };

    template <typename IdxSeq>
    struct IdxAccess<0, IdxSeq>
    {
    private:
        using details = SequenceDetails<IdxSeq>;

    public:
        static constexpr std::size_t value = details::head;
    };

    template <std::size_t Which, typename IdxSeq>
    static constexpr std::size_t idx_access_v = IdxAccess<Which, IdxSeq>::value;
} // namespace internal

template <typename... Ts>
struct alignsort_tuple;

namespace std
{
    template <std::size_t I, typename... Args>
    struct tuple_element<I, alignsort_tuple<Args...>> : tuple_element<I, tuple<Args...>>
    {
    };

    template <typename... Args>
    struct tuple_size<alignsort_tuple<Args...>> : tuple_size<tuple<Args...>>
    {
    };
} // namespace std

template <typename... Ts>
struct alignsort_tuple : public internal::alignsort_tuple_calc_t<Ts...>
{
    template <std::size_t I, typename... Args>
    constexpr friend std::tuple_element_t<I, alignsort_tuple<Args...>>& std::get(
        alignsort_tuple<Args...>&);
    template <std::size_t I, typename... Args>
    constexpr friend std::tuple_element_t<I, alignsort_tuple<Args...>>&& std::get(
        alignsort_tuple<Args...>&&);
    template <std::size_t I, typename... Args>
    constexpr friend const std::tuple_element_t<I, alignsort_tuple<Args...>>& std::get(
        const alignsort_tuple<Args...>&);
    template <std::size_t I, typename... Args>
    constexpr friend const std::tuple_element_t<I, alignsort_tuple<Args...>>& std::get(
        const alignsort_tuple<Args...>&&);

private:
    using calc = internal::alignsort_tuple_calc<Ts...>;

    template <typename... Args, std::size_t... Indices>
    constexpr alignsort_tuple(std::index_sequence<Indices...>, std::tuple<Args...>&& argsAsTuple)
        : calc::type(std::get<Indices>(std::forward<std::tuple<Args...>>(argsAsTuple))...)
    {
    }

public:
    template <typename... Args>
    constexpr alignsort_tuple(Args&&... args)
        : alignsort_tuple(
              typename calc::indices{}, std::tuple<Args&&...>(std::forward<Args>(args)...))
    {
    }

    constexpr alignsort_tuple() = default;

    constexpr alignsort_tuple(const alignsort_tuple&)            = default;
    constexpr alignsort_tuple(alignsort_tuple&&)                 = default;
    constexpr alignsort_tuple& operator=(const alignsort_tuple&) = default;
    constexpr alignsort_tuple& operator=(alignsort_tuple&&)      = default;
    constexpr alignsort_tuple(alignsort_tuple&)                  = default;
    constexpr alignsort_tuple& operator=(alignsort_tuple&)       = default;
};

namespace std
{
    template <std::size_t I, typename... Args>
    constexpr tuple_element_t<I, alignsort_tuple<Args...>>& get(alignsort_tuple<Args...>& self)
    {
        return std::get<
            internal::idx_access_v<I, typename alignsort_tuple<Args...>::calc::indices>>(
            static_cast<typename alignsort_tuple<Args...>::calc::type&>(self));
    }

    template <std::size_t I, typename... Args>
    constexpr tuple_element_t<I, alignsort_tuple<Args...>>&& get(alignsort_tuple<Args...>&& self)
    {
        return std::get<
            internal::idx_access_v<I, typename alignsort_tuple<Args...>::calc::indices>>(
            static_cast<typename alignsort_tuple<Args...>::calc::type&&>(self));
    }

    template <std::size_t I, typename... Args>
    constexpr const tuple_element_t<I, alignsort_tuple<Args...>>& get(
        const alignsort_tuple<Args...>& self)
    {
        return std::get<
            internal::idx_access_v<I, typename alignsort_tuple<Args...>::calc::indices>>(
            static_cast<const typename alignsort_tuple<Args...>::calc::type&>(self));
    }

    template <std::size_t I, typename... Args>
    constexpr const tuple_element_t<I, alignsort_tuple<Args...>>&& get(
        const alignsort_tuple<Args...>&& self)
    {
        return std::get<
            internal::idx_access_v<I, typename alignsort_tuple<Args...>::calc::indices>>(
            static_cast<const typename alignsort_tuple<Args...>::calc::type&&>(self));
    }
} // namespace std

template <typename... Args>
alignsort_tuple(Args...) -> alignsort_tuple<Args...>;

template <typename... Args>
alignsort_tuple(alignsort_tuple<Args...>) -> alignsort_tuple<Args...>;

template <typename T1, typename T2>
alignsort_tuple(std::pair<T1, T2>) -> alignsort_tuple<T1, T2>;

template <typename... Args>
alignsort_tuple(std::tuple<Args...>) -> alignsort_tuple<Args...>;

template <typename Alloc, typename... Args>
alignsort_tuple(std::allocator_arg_t, Alloc, Args...) -> alignsort_tuple<Args...>;

template <typename Alloc, typename T1, typename T2>
alignsort_tuple(std::allocator_arg_t, Alloc, std::pair<T1, T2>) -> alignsort_tuple<T1, T2>;

template <typename Alloc, typename... Types>
alignsort_tuple(std::allocator_arg_t, Alloc, std::tuple<Types...>) -> alignsort_tuple<Types...>;

template <typename Alloc, typename... Args>
alignsort_tuple(std::allocator_arg_t, Alloc, alignsort_tuple<Args...>) -> alignsort_tuple<Args...>;

#endif
