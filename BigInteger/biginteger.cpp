#include "biginteger.h"

namespace mylib
{
	BigInteger::BigInteger() noexcept
		: BigInteger(static_cast<ull>(0)) {}
	BigInteger::BigInteger(int val) noexcept
		: BigInteger(static_cast<ull>(val)) {}
	BigInteger::BigInteger(uint val) noexcept
		: BigInteger(static_cast<ull>(val)) {}
	BigInteger::BigInteger(ll val) noexcept
		: BigInteger(static_cast<ull>(val)) {}
	BigInteger::BigInteger(ull val) noexcept
		: data_(&small_integer_), small_integer_(val), size_(1) {}
	BigInteger::BigInteger(std::string_view val)
		: BigInteger(static_cast<ull>(0))
	{
		// TODO : Convert string(BCD) to binary using Double dabble algorithm
	}
	BigInteger::BigInteger(const BigInteger& val)
		: data_(&small_integer_), small_integer_(val.small_integer_), size_(val.size_)
	{
		if (!val.isSmalltIntegerOptimized())
		{
			data_ = new ull[size_];
			memcpy(data_, val.data_, 8 * size_);
		}
	}
	BigInteger::BigInteger(BigInteger&& val) noexcept
		: data_(std::move(val.data_)), small_integer_(val.small_integer_), size_(val.size_) 
	{
		if (val.isSmalltIntegerOptimized())
			data_ = &small_integer_;
		val.data_ = &val.small_integer_;
	};
	BigInteger::~BigInteger()
	{
		if (!isSmalltIntegerOptimized())
			delete data_;
	}
	bool BigInteger::isSmalltIntegerOptimized(void) const noexcept
	{
		return (data_ == &small_integer_);
	}
	inline bool BigInteger::isNegative(void) const noexcept
	{
		return (data_[size_ - 1] & (1ull << (8 * sizeof(unsigned) - 1)));
	}
	inline bool BigInteger::isPositive(void) const noexcept
	{
		return !isNegative();
	}
	inline void BigInteger::clear(void) const noexcept
	{
		for (size_t i = 0; i < size_; ++i)
			data_[i] = 0;
	}
	void BigInteger::reserve(size_t size)
	{
		if (size_ == size)
			return;
		ull* tmp = new ull[size];
		if (size_ > size)
			memcpy(tmp, data_, 8 * size);
		else
			memcpy(tmp, data_, 8 * size_);
		if (!isSmalltIntegerOptimized())
			delete[] data_;
		if (size_ < size)
			memset(tmp + size_, 0, 8 * (size - size_));
		data_ = tmp;
		size_ = size;
	}
	BigInteger& BigInteger::operator=(const BigInteger& b)
	{
		
		if (b.isSmalltIntegerOptimized())
		{
			data_[0] = b.data_[0];
			for (size_t i = 1; i < size_; ++i)
				data_[i] = 0;
		}
		else
		{
			if (size_ < b.size_)
			{
				size_ = b.size_;
				if (!isSmalltIntegerOptimized())
					delete data_;
				data_ = new ull[size_];
			}
			memcpy(data_, b.data_, 8 * size_);
			memset(data_ + size_, 0, size_ - b.size_);
		}
		return *this;
	}
	BigInteger& BigInteger::operator+=(const BigInteger& val)
	{
		const bool same_sign = isNegative() ^ val.isPositive();
		size_t size0 = 2 * size_, size1 = 2 * val.size_;
		uint* data0 = reinterpret_cast<uint*>(data_);
		uint* data1 = reinterpret_cast<uint*>(val.data_);
		// Carry Flag
		bool cf = 0;
		size_t i0 = 0, i1 = 0;
		for (; i0 < size0 && i1 < size1; ++i0, ++i1)
		{
			ull tmp = static_cast<ull>(data0[i0]) + data1[i1] + cf;
			data0[i0] = static_cast<uint>(tmp & UINT_MAX);
			cf = static_cast<bool>(tmp >> 8 * (sizeof(uint) - 1));
		}
		size_t new_size = std::max(size0 / 2, size1 / 2);
		if (same_sign && cf)
			++new_size;
		reserve(new_size);
		size0 = 2 * size_;
		data0 = reinterpret_cast<uint*>(data_);
		for (; i0 < size0; ++i0)
		{
			ull tmp = data0[i0] + cf;
			data0[i0] = static_cast<uint>(tmp & UINT_MAX);
			cf = static_cast<bool>(tmp >> 8 * (sizeof(uint) - 1));
		}
		for (; i1 < size1; ++i1)
		{
			ull tmp = data1[i1] + cf;
			data0[i1] = static_cast<uint>(tmp & UINT_MAX);
			cf = static_cast<bool>(tmp >> 8 * (sizeof(uint) - 1));
		}
		return *this;
	}
	BigInteger& BigInteger::operator-=(const BigInteger& val)
	{
		const bool same_sign = isNegative() ^ val.isPositive();
		size_t size0 = 2 * size_, size1 = 2 * val.size_;
		uint* data0 = reinterpret_cast<uint*>(data_);
		uint* data1 = reinterpret_cast<uint*>(val.data_);
		// Borrow Flag
		bool bf = 0;
		size_t i0 = 0, i1 = 0;
		for (; i0 < size0 && i1 < size1; ++i0, ++i1)
		{
			ull tmp = (1ull << 8 * sizeof(uint)) + data0[i0] - data1[i1] - bf;
			data0[i0] = static_cast<uint>(tmp & UINT_MAX);
			bf = !static_cast<bool>(tmp >> 8 * sizeof(uint));
		}
		size_t new_size = std::max(size_, val.size_);
		reserve(new_size);
		size0 = 2 * size_;
		for (; i0 < size0; ++i0)
		{
			ull tmp = (1ull << 8 * sizeof(uint)) + data0[i0] - bf;
			data0[i0] = static_cast<uint>(tmp & UINT_MAX);
			bf = !static_cast<bool>(tmp >> 8 * sizeof(uint));
		}
		for (; i1 < size1; ++i1)
		{
			ull tmp = (1ull << 8 * sizeof(uint)) + data1[i1] - bf;
			data0[i0] = static_cast<uint>(tmp & UINT_MAX);
			bf = !static_cast<bool>(tmp >> 8 * sizeof(uint));
		}
		return *this;
	}
	BigInteger& BigInteger::operator*=(const BigInteger& val)
	{
		// TODO: Implemente using Schonhage-Strassen algorithm or Naive multiplication
		return *this;
	}
	BigInteger& BigInteger::operator/=(const BigInteger& val)
	{
		// TODO: Implement division using https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
		return *this;
	}
	BigInteger& BigInteger::operator%=(const BigInteger& val)
	{
		// TODO: Implement modulo using https://en.wikipedia.org/wiki/Division_algorithm#Integer_division_(unsigned)_with_remainder
		return *this;
	}
	inline BigInteger& BigInteger::operator&=(const BigInteger& val)
	{
		if (size_ < val.size_)
			reserve(val.size_);
		size_t i0 = 0, i1 = 0;
		while(i0<size_ && i1<val.size_)
			data_[i0++] &= val.data_[i1++];
		return *this;
	}
	inline BigInteger& BigInteger::operator|=(const BigInteger& val)
	{
		if (size_ < val.size_)
			reserve(val.size_);
		size_t i0 = 0, i1 = 0;
		while (i0 < size_ && i1 < val.size_)
			data_[i0++] |= val.data_[i1++];
		return *this;
	}
	inline BigInteger& BigInteger::operator^=(const BigInteger& val)
	{
		if (size_ < val.size_)
			reserve(val.size_);
		size_t i0 = 0, i1 = 0;
		while (i0 < size_ && i1 < val.size_)
			data_[i0++] ^= val.data_[i1++];
		return *this;
	}
	BigInteger& BigInteger::operator<<=(const ull& val)
	{
		if (val >= 8 * sizeof(ull) * size_)
			clear();
		else
		{
			const ull large_shift = val / (8 * sizeof(ull));
			const ull small_shift = val % (8 * sizeof(ull));
			if (isSmalltIntegerOptimized())
				data_[0] <<= small_shift;
			// TODO: Do memory alloc and do shift 
			else if (size_ == 2)
			{
				if (large_shift == 0)
				{
					data_[1] = (data_[1] << small_shift) | (data_[0] >> (8 * sizeof(ull) - small_shift));
					data_[0] <<= small_shift;
				}
				else
				{
					data_[1] = data_[0] << small_shift;
					data_[0] = 0;
				}
			}
			else
			{
				for (size_t i = size_ - 1; i > large_shift; --i)
					data_[i] = (data_[i - large_shift] << small_shift) | (data_[i - large_shift - 1] >> (8 * sizeof(ull) - small_shift));
				for (size_t i = 0; i < large_shift; ++i)
					data_[i] = 0;
			}
		}
		return *this;
	}
	BigInteger& BigInteger::operator>>=(const ull& val)
	{
		if (val >= 8 * sizeof(ull) * size_)
			clear();
		else
		{
			const ull large_shift = val / (8 * sizeof(ull));
			const ull small_shift = val % (8 * sizeof(ull));
			ll* signed_data = reinterpret_cast<ll*>(data_);
			if (isSmalltIntegerOptimized())
				signed_data[0] >>= small_shift;
			else if (size_ == 2)
			{
				if (large_shift == 0)
				{
					data_[0] = (signed_data[1] << (8 * sizeof(ull) - small_shift)) | (data_[0] >> small_shift);
					signed_data[1] >>= small_shift;
				}
				else
				{
					data_[0] = signed_data[1] >> small_shift;
					signed_data[1] >>= (8 * sizeof(ull) - 1);
				}
			}
			else
			{
				for (size_t i = 0; i < size_ - large_shift - 1; ++i)
					data_[i] = (signed_data[i + large_shift + 1] << (8 * sizeof(ull) - small_shift)) | (data_[i + large_shift] >> small_shift);
				for (size_t i = size_ - large_shift; i < size_; ++i)
					signed_data[i] >>= (8 * sizeof(ull) - 1);
			}
		}
		return *this;
	}
	inline BigInteger& BigInteger::operator++()
	{
		*this += BigInteger{ static_cast<ull>(1) };
		return *this;
	}
	inline BigInteger BigInteger::operator++(int)
	{
		BigInteger tmp = *this;
		*this += BigInteger{ static_cast<ull>(1) };
		return tmp;
	}
	inline BigInteger& BigInteger::operator--()
	{
		*this -= BigInteger{ static_cast<ull>(1) };
		return *this;
	}
	inline BigInteger BigInteger::operator--(int)
	{
		BigInteger tmp = *this;
		*this -= BigInteger{ static_cast<ull>(1) };
		return tmp;
	}
	inline bool BigInteger::NeedsMoreSpace(void) const noexcept
	{
		return (data_[size_ - 1] & (1 << 8 * (sizeof(unsigned) - 2)));
	}
	inline BigInteger operator+(const BigInteger& a, const BigInteger& b)
	{
		return (BigInteger{ a } += b);
	}
	inline BigInteger operator-(const BigInteger& a, const BigInteger& b)
	{
		return (BigInteger{ a } -= b);
	}
	inline BigInteger operator*(const BigInteger& a, const BigInteger& b)
	{
		return (BigInteger{ a } *= b);
	}
	BigInteger operator/(const BigInteger& a, const BigInteger& b)
	{
		return (BigInteger{ a } /= b);
	}
	BigInteger operator%(const BigInteger& a, const BigInteger& b)
	{
		return (BigInteger{ a } %= b);
	}
	inline BigInteger operator~(const BigInteger& a)
	{
		BigInteger tmp{ a };
		for (size_t i = 0; i < tmp.size_; ++i)
			tmp.data_[i] = ~tmp.data_[i];
		return tmp;
	}
	inline BigInteger operator!(const BigInteger& a)
	{
		size_t i = 0;
		while (i < a.size_ && !a.data_[i])
			++i;
		if (i == a.size_)
			return BigInteger{ 1 };
		return BigInteger{ 0 };
	}
	inline BigInteger operator&(const BigInteger& a, const BigInteger& b)
	{
		return (BigInteger{ a } &= b);
	}
	inline BigInteger operator|(const BigInteger& a, const BigInteger& b)
	{
		return (BigInteger{ a } |= b);
	}
	inline BigInteger operator^(const BigInteger& a, const BigInteger& b)
	{
		return (BigInteger{ a } ^= b);
	}
	inline BigInteger operator<<(const BigInteger& a, const ull& b)
	{
		return (BigInteger{ a } <<= b);
	}
	inline BigInteger operator>>(const BigInteger& a, const ull& b)
	{
		return (BigInteger{ a } >>= b);
	}
	bool operator&&(const BigInteger& a, const BigInteger& b)
	{
		return !(!a).data_ && !(!b).data_;
	}
	inline bool operator||(const BigInteger& a, const BigInteger& b)
	{
		return !((!a).data_ && (!b).data_);
	}
	inline bool operator==(const BigInteger& a, const BigInteger& b)
	{
		return !(a < b) && !(b < a);
	}
	inline bool operator!=(const BigInteger& a, const BigInteger& b)
	{
		return !(a == b);
	}
	inline bool operator>(const BigInteger& a, const BigInteger& b)
	{
		if (a.isNegative() ^ b.isPositive())
			return a.isPositive();
		return (a - b).isPositive();
	}
	inline bool operator<(const BigInteger& a, const BigInteger& b)
	{
		if (a.isNegative() ^ b.isPositive())
			return a.isNegative();
		return (a - b).isNegative();
	}
	inline bool operator>=(const BigInteger& a, const BigInteger& b)
	{
		return !(a < b);
	}
	bool operator<=(const BigInteger& a, const BigInteger& b)
	{
		return !(a > b);
	}
	inline std::istream& operator>>(std::istream& is, BigInteger& val)
	{
		std::string s;
		is >> s;
		val = BigInteger{ s };
		return is;
	}
	std::ostream& operator<<(std::ostream& os, const BigInteger& val)
	{
		// TODO: Convert binary to string(BCD) using Double dabble algorithm.
		return os;
	}
}