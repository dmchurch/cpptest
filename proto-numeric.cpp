#pragma once

#include "LibraryTester.h"
#include "variants.h"

using namespace std;

template <class T1, class T2, class T3, class TExPo, class T_refs_in_callbacks = true_type>
struct proto_numeric {
	const static bool refs_in_callbacks = T_refs_in_callbacks::value;
	T1 o1;
	T2 o2;
	T3 o3;
	T1* p1;
	T2* p2;
	T3* p3;
	initializer_list<T1> ilist;
	TExPo expo;
	using R1 = conditional_t<refs_in_callbacks, const T1&, T1>;
	using R2 = conditional_t<refs_in_callbacks, const T2&, T2>;
	using R3 = conditional_t<refs_in_callbacks, const T3&, T3>;
	using NCR1 = conditional_t<refs_in_callbacks, T1&, T1>;
	using NCR2 = conditional_t<refs_in_callbacks, T2&, T2>;
	using NCR3 = conditional_t<refs_in_callbacks, T3&, T3>;
	bool (*uPred)(R1);
	bool (*biPred11)(R1, R1);
	bool (*biPred12)(R1, R2);
	void (*uAct)(R1);
	void (*biAct)(R1, R2);
	NCR1(*gen)();
	NCR1(*trans11)(R1);
	NCR2(*trans12)(R1);
	NCR2(*trans22)(R2);
	NCR1(*trans111)(R1, R1);
	NCR2(*trans112)(R1, R1);
	NCR2(*trans122)(R1, R2);
	NCR3(*trans123)(R1, R2);
	NCR2(*trans222)(R2, R2);
	NCR3(*trans333)(R3, R3);
	int (*fRand)();
	int i;

	~proto_numeric() { foo(i); }

	void foo(int i) {
		VCALL(iota, p1, p1, o1);
		CALL(accumulate, p1, p1, o1);
		CALL(accumulate, p1, p1, o1, trans122);
		SINCE_17(ECALL(reduce, p1, p1));
		SINCE_17(ECALL(reduce, p1, p1, o1));
		SINCE_17(ECALL(reduce, p1, p1, o1, trans122));
		SINCE_17(ECALL(transform_reduce, p1, p1, p1, o1));
		SINCE_17(ECALL(transform_reduce, p1, p1, p2, o3, trans333, trans123));
		SINCE_17(ECALL(transform_reduce, p1, p1, o2, trans222, trans12));
	}
};

template <class VaryT1, class VaryT2, class VaryT3, class T_refs_in_callbacks=true_type, class VaryTExPo = vary_execpol>
class num {
public:
	typename Varying<VaryT1, VaryT2, VaryT3, VaryTExPo, T_refs_in_callbacks>::template over<proto_numeric> v;
};

extern int i;
num<vary_numerics, vary_numerics, vary_numerics, vary_bool> a;
