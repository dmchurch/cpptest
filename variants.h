#pragma once

#include "LibraryTester.h"

// vary_* classes are "variances" and take an N-ary class template and instantiate it many times with different arguments.
// They can be passed around in class form by wrapping them in a Variance<vary_etc>.
// Variances are cardinality multipliers. The end goal is to instantiate a multi-parameter template a great many times
// with different parameters each time, so it's useful to think of a few different dimensions of cardinality:
// 
//   Width: also "arity", this is the number of parameters to a given template
//     - normally this means "output width", i.e. the arity of a template that a variance wants to apply to
//     - however we also have "input width", the number of extra arguments that must be provided *to* a variance (normally 0)
//   Height: also "stored cardinality", this is the number of potential activations of a given variance
//   Depth: also "actual cardinality", this is the number of actual classes instantiated
//
// This has the following implications:
// - When a variance is still in its holder class, i.e. when it has not yet been instantiated, it has a depth of 1, since
//   the holder class is the only class that actually exists in concrete form.
// - Execution/instantiation of a variance converts its height into depth, without altering its width.
// - Two variances with the same width(s) can be "added", adding their heights together.
// - Two variances of equal input widths can be "crossed", multiplying their heights while adding their output widths.
// - A variance with output width n can be "applied" to a variance width input width n, multiplying their heights.
// - Variances with input width 0 are called "output variances"
// - Variances with input width 1 are called "adaptor variances"
// - Variances with input width >1 are called "input variances"
// - All variances have output width >= 1
// 
// Width-matching is important, and can be corrected by a number of mechanisms:
// - Crossing variances with equal input width creates an output variance of summed output width but the same input width
// - A variance with I/O widths i/n can be applied to a variance with widths n/o for a variance of width i/o
//
// Examples:
// 
// vary_numerics is a unary (width 1) variance of height 8.
// vary_strings is a unary variance of height 2.
// vary_nums_cross_strings is a variance with width 2, height 16.
// vary_list_types has widths 1/1 and height 6
// vary_numeric_lists 
// vary_map_types has widths 2/1 and height 4
// I think I need the concepts of "input width" vs "output width"
// vary_num_string_maps has width 
//

// In this file, I attempt to use the following conventions for template parameter names, since things get pretty weird.
// T, T1, T2, TT... : arguments to the target class template ("output width")
// I, I1, I2, II... : parameters to a given variance ("input width")
// Tpl : the target class template
// TVary* : a raw variance template
// V, V1, V2, VV, V* : a variance holder class (the ::variance member should be a TVary)
//

class Variance;
template<size_t ow, size_t iw = 0>
class NVariance;

#if STD >= 20
template<class V>
concept VaryClass = std::is_base_of_v<Variance, V>;
template<class V, size_t ow, size_t iw>
concept NVaryClass = std::is_base_of_v<NVariance<ow, iw>, V>;
template<class V, size_t ow>
concept OVaryClass = std::is_base_of_v<Variance, V> && V::output_width == ow;
template<class V, size_t iw>
concept IVaryClass = std::is_base_of_v<Variance, V> && V::input_width == ow;

#define NVaryClass(ow, iw) NVaryClass<ow, iw>
#define OVaryClass(ow) OVaryClass<ow>
#define IVaryClass(iw) IVaryClass<iw>
#else
#define VaryClass class
#define NVaryClass(ow, iw) class
#define OVaryClass(ow) class
#define IVaryClass(iw) class
#endif

//base class for variance holders
class Variance {
public:
	template<VaryClass V, class... IIPre> class Closure;
	template<VaryClass V, class... IIPre> class Prefix;
	template<VaryClass... VV> class Cross;
	template<VaryClass V, VaryClass... VV> class Add;
	template<VaryClass VI, VaryClass VO> class Apply;
	template<class... TT> class Over;
	template<template<class> class... TT> class Over1;
	template<template<class, class> class... TT> class Over2;

	// Execute this variance over the target Tpl with inputs II
	template<template<class... TT> class Tpl, class... II>
	class variance {};
};

template<size_t ow, size_t iw>
class NVariance : public Variance {
protected:
	using NVBase = NVariance;
public:
	const static size_t output_width = ow;
	const static size_t input_width = iw;

	template<template<class... TT> class Tpl, class... II>
	class variance : public Variance::template variance<Tpl, II...> {
		static_assert(sizeof...(II) == input_width, "Wrong number of inputs passed to variance");
	};
};

template<VaryClass V, class... IIPre>
class Variance::Closure : public NVariance<V::output_width, V::input_width - sizeof...(IIPre)> {
	using NVBase = typename Closure::NVBase;
	const static size_t stored_args = sizeof...(IIPre);
	static_assert(V::input_width >= stored_args, "Trying to close too many arguments onto a variance");
public:
	template<template<class... TT> class Tpl, class... II>
	class variance : public NVBase::template variance<Tpl, IIPre..., II...> {
		typename V::template variance<Tpl, IIPre..., II...> v;
	};
};

template<VaryClass V, class... TTPre>
class Variance::Prefix : public NVariance<V::output_width + sizeof...(TTPre), V::input_width> {
public:
	const static size_t height = V::height;
	template<template<class... TT> class Tpl, class... II>
	class variance : public V::template variance<Tpl, II...> {
		template<class... TT>
		class variant : public V {};
	};
};

template<VaryClass V, VaryClass... VV>
class Variance::Cross<V, VV...> : public NVariance<V::output_width + Cross<VV...>::output_width, V::input_width> {
	using NVBase = typename Cross::NVBase;
	using VCBase = Cross;
	using nextCross = Cross<VV...>;
	static_assert(V::input_width == nextCross::input_width, "Cannot cross variances with different inwidths");

public:
	const static size_t height = V::height * nextCross::height;

	template<class... TTOut>
	class Subcross : public NVariance<NVBase::output_width + sizeof...(TTOut), NVBase::input_width> {
		using NVBase = typename Subcross::NVBase;
	public:
		const static size_t height = VCBase::height;
		template<template<class... TT> class Tpl, class... II>
		class variance : public NVBase::template variance<Tpl, II...> {
		public:
			template<class... TT>
			class variant : public nextCross::template Subcross<TTOut..., TT...>::template variance<Tpl, II...> {
			public:
				//variant() {
				//	std::cout << typeid(*this).name() << std::endl;
				//}
			};

			typename V::template variance<variant, II...> v;

			//variance() {
			//	std::cout << typeid(*this).name() << std::endl;
			//}
		};
	};

	//Cross() {
	//	std::cout << typeid(*this).name() << std::endl;
	//}

	template<template<class... TT> class Tpl, class... II>
	using variance = typename Subcross<>::template variance<Tpl, II...>;
};

template<>
class Variance::Cross<> : public NVariance<0, 0> {
	using NVBase = typename Cross::NVBase;
	using VCBase = Cross;
public:
	const static size_t height = 0;

	template<class... TTOut>
	class Subcross : public NVariance<sizeof...(TTOut)> {
	public:
		const static size_t height = 1;

		template<template<class... TT> class Tpl, class... II>
		class variance : public Tpl<TTOut...> {};
	};

	//Cross() {
	//	std::cout << typeid(*this).name() << std::endl;
	//}
};


template<VaryClass V, VaryClass... VV>
class Variance::Add : public NVariance<V::output_width, V::input_width> {
	using NVBase = typename Add::NVBase;
	using nextStack = Add<VV...>;
	static_assert(V::input_width == nextStack::input_width, "Cannot add variances with different inwidths");
	static_assert(V::output_width == nextStack::output_width, "Cannot add variances with different outwidths");

public:
	const static size_t height = V::height + nextStack::height;
	template<template<class... TT> class Tpl, class... II>
	class variance : public NVBase::template variance<Tpl, II...> {
		typename V::template variance<Tpl, II...> v;
		typename nextStack::template variance<Tpl, II...> vv;
	};
};

template<VaryClass V>
class Variance::Add<V> : public V {};

template<VaryClass VI, VaryClass VO>
class Variance::Apply : public NVariance<VO::output_width, VI::input_width> {
	using NVBase = typename Apply::NVBase;
	static_assert(VI::output_width == VO::input_width, "Cannot apply variance with mismatched widths");

public:
	const static size_t height = VI::height * VO::height;
	template<template<class... TT> class Tpl, class... II>
	class variance : public NVBase::template variance<Tpl, II...> {
		template<class... TT>
		class variant : public VO::template variance<Tpl, TT...> {};

		typename VI::template variance<variant, II...> v;
	};
};


template<class... TT>
class Variance::Over : public NVariance<1> {
public:
	const static size_t height = 0;

	template<template<class T> class Tpl>
	class variance : public NVBase::template variance<Tpl> {};
};

template<class T, class... TT>
class Variance::Over<T, TT...> : public NVariance<1> {
public:
	const static size_t height = sizeof...(TT) + 1;

	template<template<class T> class Tpl>
	class variance : public Variance::Over<TT...>::template variance<Tpl> {
		Tpl<T> v;
	};
};

template<template<class> class... TT>
class Variance::Over1 : public NVariance<1, 1> {
public:
	const static size_t height = 0;

	template<template<class T> class Tpl, class I>
	class variance : public NVBase::template variance<Tpl, I> {};
};

template<template<class> class T, template<class> class... TT>
class Variance::Over1<T, TT...> : public NVariance<1, 1> {
public:
	const static size_t height = sizeof...(TT) + 1;

	template<template<class T> class Tpl, class I>
	class variance : public Variance::Over1<TT...>::template variance<Tpl, I> {
		Tpl<T<I>> v;
	};
};

template<template<class, class> class... TT>
class Variance::Over2 : public NVariance<1, 2> {
public:
	const static size_t height = 0;

	template<template<class T> class Tpl, class I1, class I2>
	class variance : public NVBase::template variance<Tpl, I1, I2> {};
};

template<template<class, class> class T, template<class, class> class... TT>
class Variance::Over2<T, TT...> : public NVariance<1, 2> {
public:
	const static size_t height = sizeof...(TT) + 1;

	template<template<class T> class Tpl, class I1, class I2>
	class variance : public Variance::Over2<TT...>::template variance<Tpl, I1, I2> {
		Tpl<T<I1, I2>> v;
	};
};

template<class T>
class Nonvariant : public Variance::Over<T> {};

class vary_none : public Variance::Over<> {};

class vary_bool : public Variance::Over<std::true_type, std::false_type> {};

class vary_execpol : public Variance::Over<
	UNTIL_17(std::nullptr_t)
	SINCE_17(std::execution::sequenced_policy,)
	SINCE_17(std::execution::parallel_policy,)
	SINCE_17(std::execution::parallel_unsequenced_policy)
	SINCE_20(,std::execution::unsequenced_policy)
> {};

class vary_random : public Variance::Over<
#if STD >= 11
	std::random_device,
	std::minstd_rand0,
	std::minstd_rand,
	std::mt19937,
	std::mt19937_64,
	std::ranlux24_base,
	std::ranlux48_base
#else
	std::nullptr_t
#endif
> {};

class vary_numerics : public Variance::Over<
	uint8_t, int8_t,
	uint16_t,int16_t,
	uint32_t,int32_t,
	uint64_t,int64_t
> {};

class vary_strings : public Variance::Over<std::string, std::wstring, std::u16string, std::u32string> {};

using vary_all_simple = Variance::Add<vary_numerics, vary_strings, Nonvariant<bool>>;
using vary_all_pairs = Variance::Apply<Variance::Cross<vary_all_simple, vary_all_simple>, Variance::Over2<std::pair>>;
using vary_simple_and_pairs = Variance::Add<vary_all_simple, vary_all_pairs>;

//template<template<class T> class Tpl, class T>
template<class T>
class array16 : public std::array<T, 16> {};

class vary_container_types : public Variance::Over1<
	array16,
	std::deque,
	std::forward_list,
	std::list,
	std::multiset,
	std::set,
	std::unordered_set,
	std::unordered_multiset,
	std::vector
>{};

class vary_map_types : public Variance::Over2<
	std::map,
	std::multimap,
	std::unordered_map,
	std::unordered_multimap
>{};

class vary_containers : public Variance::Apply<vary_simple_and_pairs, vary_container_types> {};

class vary_maps : public Variance::Apply<
	Variance::Cross<vary_simple_and_pairs, Variance::Add<vary_simple_and_pairs, vary_containers>>,
	vary_map_types> {};

class vary_all_maps : public Variance::Add<
	vary_maps,
	Variance::Apply<
		Variance::Cross<vary_simple_and_pairs, vary_maps>,
		vary_map_types>
> {};

using vary_all = Variance::Add<vary_simple_and_pairs, vary_containers, vary_all_maps>;


// template class which makes it much easier to use Variance types. It expects a list of
// Variance types, but a non-Variance type will get wrapped in a Nonvariant class.

template<class... VClass_or_nonvar>
class Varying;

template<VaryClass... VV>
class _Varying;

template<class... VClass_or_nonvar>
class _Varying_input {
public:
	using varying = typename _Varying_input<_Varying<>, VClass_or_nonvar...>::varying;
};

template<VaryClass... VV1, VaryClass... VV2, class... VVCCNNVV>
class _Varying_input<_Varying<VV1...>, _Varying<VV2...>, VVCCNNVV...> {
public:
	using varying = typename _Varying_input<_Varying<VV1..., VV2...>, VVCCNNVV...>::varying;
};

template<VaryClass... VV, class VCNV, class... VVCCNNVV>
class _Varying_input<_Varying<VV...>, VCNV, VVCCNNVV...> {
public:
	using V = std::conditional_t<std::is_base_of_v<Variance, VCNV>, VCNV, Nonvariant<VCNV>>;
	using varying = typename _Varying_input<_Varying<VV..., V>, VVCCNNVV...>::varying;
};

template<VaryClass... VV>
class _Varying_input<_Varying<VV...>> {
public:
	using varying = _Varying<VV...>;
};

template<class... VClass_or_nonvar>
class Varying : public _Varying_input<VClass_or_nonvar...>::varying {};

template<VaryClass... VV>
class _Varying {
	using VBase = _Varying;

	template<class>
	class _shift {
	public:
		using VNext = VBase;
	};
	template<VaryClass VPre>
	class _shift<_Varying<VPre>> {
	public:
		using VNext = _Varying<>;
	};
	template<VaryClass VPre, VaryClass... VVPost>
	class _shift<_Varying<VPre, VVPost...>> {
	public:
		using VNext = _Varying<VVPost...>;
	};


	template<class>
	class _pop {
	public:
		using VNext = VBase;
	};
	template<VaryClass VPost>
	class _pop<_Varying<VPost>> {
	public:
		using VNext = _Varying<>;
	};
	template<VaryClass... VVPre, VaryClass VPost>
	class _pop<_Varying<VVPre..., VPost>> {
	public:
		using VNext = _Varying<VVPre...>;
	};

	template<class VStore, class... VVRev>
	class _reverse {
	public:
		using VNext = VStore;
	};

	template<class... VVStore, class VRev, class... VVRev>
	class _reverse<_Varying<VVStore...>, VRev, VVRev...> {
	public:
		using VNext = typename _reverse<_Varying<VRev, VVStore...>, VVRev...>::VNext;
	};

	template<VaryClass... VVPre>
	using _add = _Varying<VVPre..., VV...>;

	using _cross = Variance::Cross<VV...>;

public:

	using reverse = typename _reverse<_Varying<>, VV...>::VNext;

	using shift = typename _shift<_Varying>::VNext;
	template<int n, int actual_n = (n < 0 ? sizeof...(VV) + n : n)>
	using shift_n = typename std::conditional_t<actual_n == 0, VBase, typename shift::template shift_n<actual_n - 1>>;

	using pop = typename _pop<_Varying>::VNext;
	template<int n>
	using pop_n = typename reverse::template shift_n<n>::reverse;


	template<class... CC>
	using add = typename Varying<CC...>::template _add<VV...>;

	using cross = std::conditional_t<(sizeof...(VV) <= 1), VBase, _Varying<_cross>>;

	template<template<class... TT> class Tpl>
	using over = typename _cross::template variance<Tpl>;
	//template<VaryClass V, class... IIPre> class Closure;
	//template<VaryClass V, VaryClass... VV> class Cross;
	//template<VaryClass V, VaryClass... VV> class Add;
	//template<VaryClass VI, VaryClass VO> class Apply;
};

// Degenerate empty case
template<>
class _Varying<> {
public:
	template<int> using shift_n = _Varying;
	template<int> using pop_n = _Varying;

	using shift = shift_n<1>;
	using pop = pop_n<1>;
};

