#pragma once

#include <cstring>
#include <limits>
#include <algorithm>
#include <string_view>

namespace mylib
{
	using uint = unsigned int;
	using ll = long long;
	using ull = unsigned long long;

	class BigInteger
	{
	public:
		BigInteger() noexcept;
		BigInteger(int val) noexcept;
		BigInteger(uint val) noexcept;
		BigInteger(ll val) noexcept;
		BigInteger(ull val) noexcept;
		BigInteger(std::string_view val);
		BigInteger(const BigInteger& val);
		BigInteger(BigInteger&& val) noexcept;
		~BigInteger();

		inline bool isSmalltIntegerOptimized(void) const noexcept;
		inline bool isNegative(void) const noexcept;
		inline bool isPositive(void) const noexcept;
		inline void clear(void) const noexcept;
		void reserve(size_t size);

		BigInteger& operator=(const BigInteger& b);
		BigInteger& operator+=(const BigInteger& val);
		BigInteger& operator-=(const BigInteger& val);
		BigInteger& operator*=(const BigInteger& val);
		BigInteger& operator/=(const BigInteger& val);
		BigInteger& operator%=(const BigInteger& val);

		inline BigInteger& operator&=(const BigInteger& val);
		inline BigInteger& operator|=(const BigInteger& val);
		inline BigInteger& operator^=(const BigInteger& val);

		BigInteger& operator<<=(const ull& val);
		BigInteger& operator>>=(const ull& val);

		inline BigInteger& operator++();
		inline BigInteger operator++(int);
		inline BigInteger& operator--();
		inline BigInteger operator--(int);

		friend inline BigInteger operator+(const BigInteger& a, const BigInteger& b);
		friend inline BigInteger operator-(const BigInteger& a, const BigInteger& b);
		friend inline BigInteger operator*(const BigInteger& a, const BigInteger& b);
		friend inline BigInteger operator/(const BigInteger& a, const BigInteger& b);
		friend inline BigInteger operator%(const BigInteger& a, const BigInteger& b);

		friend inline BigInteger operator~(const BigInteger& a);
		friend inline BigInteger operator!(const BigInteger& a);
		friend inline BigInteger operator&(const BigInteger& a, const BigInteger& b);
		friend inline BigInteger operator|(const BigInteger& a, const BigInteger& b);
		friend inline BigInteger operator^(const BigInteger& a, const BigInteger& b);

		friend inline BigInteger operator<<(const BigInteger& a, const ull& b);
		friend inline BigInteger operator>>(const BigInteger& a, const ull& b);

		friend inline bool operator&&(const BigInteger& a, const BigInteger& b);
		friend inline bool operator||(const BigInteger& a, const BigInteger& b);
		friend inline bool operator==(const BigInteger& a, const BigInteger& b);
		friend inline bool operator!=(const BigInteger& a, const BigInteger& b);
		friend inline bool operator>(const BigInteger& a, const BigInteger& b);
		friend inline bool operator<(const BigInteger & a, const BigInteger & b);
		friend inline bool operator>=(const BigInteger& a, const BigInteger& b);
		friend inline bool operator<=(const BigInteger & a, const BigInteger & b);

		friend inline std::istream& operator>>(std::istream& is, BigInteger& val);
		friend std::ostream& operator<<(std::ostream& os, const BigInteger& val);
	private:
		// Save address of actual data
		ull* data_;
		// Save data when data is short (actually data can represented by int)
		ull small_integer_;
		// Represent how many unsigneds are needed.
		size_t size_;

		// Check whether needs more space when adding.
		inline bool NeedsMoreSpace(void) const noexcept;
	};
};