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
		// Use double dabble
		const bool is_negative = (val[0] == '-') ? true : false;
		if (is_negative)
			val = val.substr(1);

		const int string_size = static_cast<int>(val.length());
		const int required_auxiliary_size = static_cast<int>(string_size + 1) / 2;
		const int required_data_size = size_ = 1 + (3 * string_size) / (8 * sizeof(ull));

		if (required_data_size == 1)
			data_ = &small_integer_;
		else
			data_ = new ull[required_data_size];

		ull* data = data_;
		std::unique_ptr<unsigned char> unique_auxiliary{ new unsigned char[required_auxiliary_size] };
		unsigned char* auxiliary = unique_auxiliary.get();

		{
			int i = required_auxiliary_size - 1;
			int j = 0;
			if (string_size % 2)
				auxiliary[i--] = (val[j++] - '0') & 0xf;
			while (i >= 0)
				auxiliary[i--] = ((val[j++] - '0') << 4) + (val[j++] - '0');
			for (int i = 0; i < required_data_size; ++i)
				data[i] = 0;
		}

		// Borrow flag
		uint bf = 0;
		for (int i = 0; i < 8 * sizeof(ull) * required_data_size; ++i)
		{
			bf = 0;
			for (int k = required_auxiliary_size - 1; k >= 0; --k)
			{
				uint tmp = (bf << 7) + (auxiliary[k] >> 1);
				bf = auxiliary[k] & 1;
				if ((tmp & 0xf) > 7)
					tmp -= 3;
				if ((tmp & 0xf0) > (7 << 4))
					tmp -= (3 << 4);
				auxiliary[k] = tmp;
			}
			for (int k = required_data_size - 1; k >= 0; --k)
			{
				ull tmp = (static_cast<ull>(bf) << (8 * sizeof(ull) - 1)) + (data[k] >> 1);
				bf = data[k] & 1;
				data[k] = tmp;
			}
		}

		if(is_negative)
			*this = -*this;
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
		return (data_[size_ - 1] & (1ull << (8 * sizeof(ull) - 1)));
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
	inline size_t BigInteger::getSize(void) const noexcept
	{
		return size_;
	}
	inline ull BigInteger::getAcutalSizeOfData(void) const noexcept
	{
		ull size = 0;
		for (size_t i = 0; i < size_; ++i)
			size += popcount(data_[i]);
		return size;
	}
	std::string BigInteger::getNumberByDecimalString(void) const
	{
	// Use double dabble
		const int required_string_size = static_cast<int>(1.f + 8 * sizeof(ull) * size_ / 3.f);
		const int required_auxiliary_size = static_cast<int>(1.f + required_string_size) / 2;

		std::string s;
		s.reserve(required_string_size + 1);
		std::unique_ptr<unsigned char> unique_auxiliary{ new unsigned char[required_auxiliary_size] };
		unsigned char* auxiliary = unique_auxiliary.get();

		for (int i = 0; i < required_auxiliary_size; ++i)
			auxiliary[i] = 0;
		
		BigInteger temp_instance{*this};
		if (isNegative())
			temp_instance = -temp_instance;
		const ull* data = temp_instance.data_;

		// Carry flag
		uint cf = 0;
		for (int i = static_cast<int>(size_) - 1; i >= 0; --i)
		{
			for (int j = 8 * sizeof(ull) - 1; j >= 0; --j)
			{
				cf = (data[i] >> j) & 1;
				for (int k = 0; k < required_auxiliary_size; ++k)
				{
					uint tmp = auxiliary[k];
					if ((tmp & 0xf) > 4)
						tmp += 3;
					if ((tmp & 0xf0) > (4 << 4))
						tmp += (3 << 4);
					tmp = (tmp << 1) + cf;
					cf = tmp >> 8;
					auxiliary[k] = static_cast<char>(tmp);
				}
			}
		}

		if (isNegative())
			s += '-';

		int index_to_print_0 = required_auxiliary_size;
		while (index_to_print_0-- > 0)
		{
			if (auxiliary[index_to_print_0] & 0xf0)
				break;
			else if (auxiliary[index_to_print_0] & 0xf)
			{
				s += ('0' + (auxiliary[index_to_print_0--] & 0xf));
				break;
			}
		}

		while (index_to_print_0 >= 0)
		{
			s += ('0' + ((auxiliary[index_to_print_0] & 0xf0) >> 4));
			s += ('0' + (auxiliary[index_to_print_0--] & 0xf));
		}
		if (!s.length())
			s = "0";

		return std::move(s);
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
	BigInteger& BigInteger::operator=(const BigInteger& val)
	{
		
		if (val.isSmalltIntegerOptimized())
		{
			data_[0] = val.data_[0];
			for (size_t i = 1; i < size_; ++i)
				data_[i] = 0;
		}
		else
		{
			if (size_ < val.size_)
			{
				size_ = val.size_;
				if (!isSmalltIntegerOptimized())
					delete data_;
				data_ = new ull[size_];
			}
			memcpy(data_, val.data_, 8 * size_);
			memset(data_ + size_, 0, size_ - val.size_);
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
			cf = static_cast<bool>(tmp >> 8 * sizeof(uint));
		}
		size_t new_size = std::max(size0 / 2, size1 / 2);
		if (same_sign && (cf || isNegative()))
			++new_size;
		reserve(new_size);
		size0 = 2 * size_;
		data0 = reinterpret_cast<uint*>(data_);
		for (; i0 < size0; ++i0)
		{
			ull tmp = data0[i0] + cf;
			data0[i0] = static_cast<uint>(tmp & UINT_MAX);
			cf = static_cast<bool>(tmp >> 8 * sizeof(uint));
		}
		for (; i1 < size1; ++i1)
		{
			ull tmp = data1[i1] + cf;
			data0[i1] = static_cast<uint>(tmp & UINT_MAX);
			cf = static_cast<bool>(tmp >> 8 * sizeof(uint));
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
			else if (size_ == 2)
			{
				const uint data_at_index_2 = static_cast<uint>(data_[1] >> (8 * sizeof(ull) - small_shift));
				if (data_at_index_2)
					reserve(size_ + 1);
				data_[2] = data_at_index_2;
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
				const ull real_size = getAcutalSizeOfData();
				if (val + real_size > 8 * size_)
					reserve(size_ + large_shift + 1);
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
	// Cannot insert inline. why?!?!
	BigInteger operator-(const BigInteger& a)
	{
		return (~BigInteger{ a } + 1);
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
	inline BigInteger operator/(const BigInteger& a, const BigInteger& b)
	{
		return (BigInteger{ a } /= b);
	}
	inline BigInteger operator%(const BigInteger& a, const BigInteger& b)
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
	std::istream& operator>>(std::istream& is, BigInteger& val)
	{
		std::string s;
		is >> s;
		val = BigInteger{ s };
		return is;
	}
	std::ostream& operator<<(std::ostream& os, const BigInteger& val)
	{
		return (os << val.getNumberByDecimalString());
	}
}