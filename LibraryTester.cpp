#include "LibraryTester.h"

MSG(c++ standard: __cplusplus)

#include "variants.h"

using namespace std;

int main() {
	cout << "main" << endl;
}

template<class T>
using objforward = T;

vary_all_simple::template variance<std::hash> hashes;
vary_all::template variance<objforward> lists;