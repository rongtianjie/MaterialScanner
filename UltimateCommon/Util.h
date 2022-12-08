#pragma once
#include <cmath>

namespace util
{

	class NoCopyable
	{
	protected:
		NoCopyable() = default;

	private:
		NoCopyable(const NoCopyable&) = delete;
		NoCopyable& operator=(const NoCopyable& ) = delete;
	};


	template<typename T>
	class Singleton : public NoCopyable
	{
	public:
		Singleton() = default;

		static T* instance() {
			static T t;
			return &t;
		}
	};


	template <typename T>
	std::vector<T> parse(unsigned char const* buf, std::uint32_t nval)
	{
		constexpr std::size_t const type_size = sizeof(T);
		std::vector<T> result(nval);
		std::uint32_t i = 0; T* source = static_cast<T*>(buf);
		for (; i < nval; ++i, ++source) {
			std::memcpy(&result[i], source, type_size);
		}
	}

	inline double Round(double value, int figure)
	{
		bool isNagative = (value < 0);
		if (isNagative == true) 
			value = -value;

		double rate = std::pow(10.0, figure);
		//double value1 = value * rate + 0.5;

		long tmp = (long)(value * rate + 0.5);
		value = tmp / rate;

		if (isNagative == true) 
			value = -value;

		return value;
	}

	// Retrieve the hi-order word (16-bit value) of a dword (32-bit value)
	inline std::uint16_t hiword(std::uint32_t dword)
	{
		return static_cast<std::uint16_t>((dword >> 16) & 0xFFFF);
	}

	// Retrieve the low-order word (16-bit value) of a dword (32-bit value)
	inline std::uint16_t loword(std::uint32_t dword)
	{
		return static_cast<std::uint16_t>(dword & 0xFFFF);
	}

} // namespace util



