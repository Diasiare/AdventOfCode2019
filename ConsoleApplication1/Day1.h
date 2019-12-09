#pragma 

#include <array>

namespace day1 {

	class Day1
	{
	private:
		static int GetFuleReq(int mass);
		static int GetItterativeFuelReq(int mass);
	public:
		template<std::size_t N>
		static int GetSumOfFuelReqs(std::array<int, N> masses);
		static int GetSumOfFuelReqs();
		static int GetItterativeSumOfFuelReqs();


	};

	template<std::size_t N>
	int Day1::GetSumOfFuelReqs(std::array<int, N> masses) {
		int n = 0;
		for (int i = 0; i < N; i++) {
			n += Day1::GetFuleReq(masses[i]);
		}
		return n;
	}

}