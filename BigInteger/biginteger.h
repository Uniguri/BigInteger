#pragma once

#include <iostream>
#include <cstring>
#include <limits>
#include <memory>
#include <string_view>

#include "usefulfunctions.hpp"
#include "types.h"

namespace mylib
{
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

		bool isSmalltIntegerOptimized(void) const noexcept;
		bool isNegative(void) const noexcept;
		bool isPositive(void) const noexcept;
		void clear(void) const noexcept;
		size_t getSize(void) const noexcept;
		ull getAcutalSizeOfData(void) const noexcept;
		std::string getNumberByDecimalString(void) const;
		void reserve(size_t size);

		BigInteger& operator=(const BigInteger& val);
		BigInteger& operator+=(const BigInteger& val);
		BigInteger& operator-=(const BigInteger& val);
		BigInteger& operator*=(const BigInteger& val);
		BigInteger& operator/=(const BigInteger& val);
		BigInteger& operator%=(const BigInteger& val);

		BigInteger& operator&=(const BigInteger& val);
		BigInteger& operator|=(const BigInteger& val);
		BigInteger& operator^=(const BigInteger& val);

		BigInteger& operator<<=(const ull& val);
		BigInteger& operator>>=(const ull& val);

		BigInteger& operator++();
		BigInteger operator++(int);
		BigInteger& operator--();
		BigInteger operator--(int);

		friend BigInteger operator-(const BigInteger& a);
		friend BigInteger operator+(const BigInteger& a, const BigInteger& b);
		friend BigInteger operator-(const BigInteger& a, const BigInteger& b);
		friend BigInteger operator*(const BigInteger& a, const BigInteger& b);
		friend BigInteger operator/(const BigInteger& a, const BigInteger& b);
		friend BigInteger operator%(const BigInteger& a, const BigInteger& b);

		friend BigInteger operator~(const BigInteger& a);
		friend BigInteger operator!(const BigInteger& a);
		friend BigInteger operator&(const BigInteger& a, const BigInteger& b);
		friend BigInteger operator|(const BigInteger& a, const BigInteger& b);
		friend BigInteger operator^(const BigInteger& a, const BigInteger& b);

		friend BigInteger operator<<(const BigInteger& a, const ull& b);
		friend BigInteger operator>>(const BigInteger& a, const ull& b);

		friend bool operator&&(const BigInteger& a, const BigInteger& b);
		friend bool operator||(const BigInteger& a, const BigInteger& b);
		friend bool operator==(const BigInteger& a, const BigInteger& b);
		friend bool operator!=(const BigInteger& a, const BigInteger& b);
		friend bool operator>(const BigInteger& a, const BigInteger& b);
		friend bool operator<(const BigInteger & a, const BigInteger & b);
		friend bool operator>=(const BigInteger& a, const BigInteger& b);
		friend bool operator<=(const BigInteger & a, const BigInteger & b);

		friend std::istream& operator>>(std::istream& is, BigInteger& val);
		friend std::ostream& operator<<(std::ostream& os, const BigInteger& val);
	private:
		// Save address of actual data
		ull* data_;
		// Save data when data is short (actually data can represented by int)
		ull small_integer_;
		// Represent how many unsigneds are needed.
		size_t size_;

		// Check whether needs more space when adding.
		bool NeedsMoreSpace(void) const noexcept;
	};
};