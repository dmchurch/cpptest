#pragma once

#include "LibraryTester.h"
#include "variants.h"

using namespace std;

struct __EqT1 {};

template <class T1, class T2i, class TExPo, class URBG, class T_refs_in_callbacks = true_type, class T2 = conditional_t<is_same_v<T2i,__EqT1>, T1, T2i>>
struct proto_algorithm {
	const static bool refs_in_callbacks = T_refs_in_callbacks::value;
	T1 o1;
	T2 o2;
	T1* p1;
	T2* p2;
	initializer_list<T1> ilist;
	TExPo expo;
	using R1 = conditional_t<refs_in_callbacks, const T1&, T1>;
	using R2 = conditional_t<refs_in_callbacks, const T2&, T2>;
	using NCR1 = conditional_t<refs_in_callbacks, T1&, T1>;
	using NCR2 = conditional_t<refs_in_callbacks, T2&, T2>;
	bool (*uPred)(R1);
	bool (*biPred11)(R1, R1);
	bool (*biPred12)(R1, R2);
	void (*uAct)(R1);
	void (*biAct)(R1, R2);
	NCR1(*gen)();
	NCR2(*trans12)(R1);
	NCR2(*trans122)(R1, R2);
	int (*fRand)();
	URBG *pUrbg;
	int i;

	~proto_algorithm() { foo(i); }

	void foo(int i) {
		SINCE_11(ECALL(all_of, p1, p1, uPred));
		SINCE_11(ECALL(any_of, p1, p1, uPred));
		SINCE_11(ECALL(none_of, p1, p1, uPred));

		CALL(for_each, p1, p1, uAct);
		VCALL(for_each, expo, p1, p1, uAct);
		SINCE_17(ECALL(for_each_n, p1, i, uAct));

		ECALL(count, p1, p1, o1);
		ECALL(count_if, p1, p1, uPred);

		ECALL(mismatch, p1, p1, p2);
		ECALL(mismatch, p1, p1, p2, biPred12);

		SINCE_14(ECALL(mismatch, p1, p1, p2, p2));
		SINCE_14(ECALL(mismatch, p1, p1, p2, p2, biPred12));


		ECALL(find, p1, p1, o1);
		ECALL(find_if, p1, p1, uPred);
		SINCE_11(ECALL(find_if_not, p1, p1, uPred));

		ECALL(find_end, p1, p1, p2, p2);
		ECALL(find_end, p1, p1, p2, p2, biPred12);

		ECALL(find_first_of, p1, p1, p2, p2);
		ECALL(find_first_of, p1, p1, p2, p2, biPred12);

		ECALL(adjacent_find, p1, p1);
		ECALL(adjacent_find, p1, p1, biPred11);

		
		ECALL(search, p1, p1, p2, p2);
		ECALL(search, p1, p1, p2, p2, biPred12);

		SINCE_17(CALL(search, p1, p1, std::default_searcher(p1, p1)));
		SINCE_17(CALL(search, p1, p1, std::boyer_moore_searcher(p1, p1)));
		SINCE_17(CALL(search, p1, p1, std::boyer_moore_horspool_searcher(p1, p1)));

		ECALL(search_n, p1, p1, i, o1);
		ECALL(search_n, p1, p1, i, o1, biPred11);

		ECALL(copy, p1, p1, p1);
		SINCE_11(ECALL(copy_if, p1, p1, p1, uPred));
		SINCE_11(ECALL(copy_n, p1, i, p1));
		CALL(copy_backward, p1, p1, p1);

		ECALL(move, p1, p1, p1);
		CALL(move_backward, p1, p1, p1);

		VECALL(fill, p1, p1, o1);
		VECALL(fill_n, p1, i, o1);

		ECALL(transform, p1, p1, p2, trans12);
		ECALL(transform, p1, p1, p2, p2, trans122);

		VECALL(generate, p1, p1, gen);
		VECALL(generate_n, p1, i, gen);

		ECALL(remove, p1, p1, o1);
		ECALL(remove_if, p1, p1, uPred);

		ECALL(remove_copy, p1, p1, p1, o1);
		ECALL(remove_copy_if, p1, p1, p1, uPred);

		VECALL(replace, p1, p1, o1, o1);
		VECALL(replace_if, p1, p1, uPred, o1);

		ECALL(replace_copy, p1, p1, p1, o1, o1);
		ECALL(replace_copy_if, p1, p1, p1, uPred, o1);

		VCALL(swap, o1, o1);
		VCALL(iter_swap, p1, p1);

		ECALL(swap_ranges, p1, p1, p1);

		VECALL(reverse, p1, p1);
		ECALL(reverse_copy, p1, p1, p1);

		VECALL(rotate, p1, p1, p1);
		ECALL(rotate_copy, p1, p1, p1, p1);

		SINCE_20(ECALL(shift_left, p1, p1, i));
		SINCE_20(ECALL(shift_right, p1, p1, i));

		UNTIL_17(VCALL(random_shuffle, p1, p1));
		UNTIL_17(VCALL(random_shuffle, p1, p1, fRand));
		SINCE_11(VCALL(shuffle, p1, p1, *pUrbg));

		SINCE_17(CALL(sample, p1, p1, p1, i, *pUrbg));

		ECALL(unique, p1, p1);
		ECALL(unique, p1, p1, biPred11);
		ECALL(unique_copy, p1, p1, p1);
		ECALL(unique_copy, p1, p1, p1, biPred11);

		ECALL(is_partitioned, p1, p1, uPred);
		ECALL(partition, p1, p1, uPred);
		ECALL(partition_copy, p1, p1, p1, p1, uPred);
		ECALL(stable_partition, p1, p1, uPred);
		CALL(partition_point, p1, p1, uPred);
#define COMP(m, f, ...) m(f, __VA_ARGS__); m(f, __VA_ARGS__, biPred11);
		SINCE_11(COMP(ECALL, is_sorted, p1, p1));
		SINCE_11(COMP(ECALL, is_sorted_until, p1, p1));

		COMP(VECALL, sort, p1, p1);
		COMP(VECALL, partial_sort, p1, p1, p1);
		COMP(VECALL, partial_sort_copy, p1, p1, p1, p1);
		COMP(VECALL, stable_sort, p1, p1);

		COMP(CALL, lower_bound, p1, p1, o1);
		COMP(CALL, upper_bound, p1, p1, o1);

		COMP(CALL, binary_search, p1, p1, o1);
		COMP(CALL, equal_range, p1, p1, o1);

		COMP(ECALL, merge, p1, p1, p1, p1, p1);
		COMP(VECALL, inplace_merge, p1, p1, p1);

		COMP(ECALL, includes, p1, p1, p1, p1);
		COMP(ECALL, set_difference, p1, p1, p1, p1, p1);
		COMP(ECALL, set_intersection, p1, p1, p1, p1, p1);
		COMP(ECALL, set_symmetric_difference, p1, p1, p1, p1, p1);
		COMP(ECALL, set_union, p1, p1, p1, p1, p1);

		SINCE_11(COMP(ECALL, is_heap, p1, p1));
		SINCE_11(COMP(ECALL, is_heap_until, p1, p1));
		COMP(VCALL, make_heap, p1, p1);
		COMP(VCALL, push_heap, p1, p1);
		COMP(VCALL, pop_heap, p1, p1);
		COMP(VCALL, sort_heap, p1, p1);

		COMP(CALL, max, o1, o1);
		COMP(CALL, max, ilist);
		COMP(ECALL, max_element, p1, p1);
		COMP(CALL, min, o1, o1);
		COMP(CALL, min, ilist);
		COMP(ECALL, min_element, p1, p1);
		SINCE_11(COMP(CALL, minmax, o1, o1));
		SINCE_11(COMP(CALL, minmax, ilist));
		SINCE_11(COMP(ECALL, minmax_element, p1, p1));
		SINCE_17(COMP(CALL, clamp, o1, o1, o1));

		COMP(ECALL, equal, p1, p1, p1);
		COMP(ECALL, equal, p1, p1, p1, p1);
		SINCE_20(COMP(ECALL, lexicographical_compare, p1, p1, p1, p1));

		SINCE_11(COMP(CALL, is_permutation, p1, p1, p1));
		SINCE_14(COMP(CALL, is_permutation, p1, p1, p1, p1));
		COMP(CALL, next_permutation, p1, p1);
		COMP(CALL, prev_permutation, p1, p1);
	}
};

template <class VaryT1, class VaryT2, class T_refs_in_callbacks=true_type, class VaryTExPo = vary_execpol, class VaryURBG = vary_random>
class alg {
	typename Varying<VaryT1, VaryT2, VaryTExPo, VaryURBG, T_refs_in_callbacks>::template over<proto_algorithm> v;
};

alg<vary_numerics, vary_numerics, vary_bool> a;
alg<vary_strings, __EqT1> b;