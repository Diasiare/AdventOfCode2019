#pragma once

namespace helpers {
	template<class T>
	T GCD(T u, T v) {
		while (v != 0) {
			int r = u % v;
			u = v;
			v = r;
		}
		return u;
	}

	template<class T>
	T LCM(T u, T v) {
		return (u * v) / GCD<T>(u, v);
	}
}

