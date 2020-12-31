#include "Number.h"

void Number::copyData(const Number& other)
{
	parts.resize(other.numberOfParts());
	for (size_t i = 0; i < other.numberOfParts(); i++) parts[i] = other.parts[i];
}

void Number::simplifyNumber()
{
	while (numberOfParts() > 1 && parts.back() == 0) parts.pop_back();
}

void Number::multiplyByBase()
{
	unsigned long long int current, carry = 0;

	for (size_t i = 0; i < numberOfParts(); i++)
	{
		current = parts[i];
		current *= base;
		current += carry;

		carry = current / basePowerLimit;

		parts[i] = current % basePowerLimit;
	}

	if (carry > 0) parts.push_back(carry);
}

void Number::divideByBase()
{
	unsigned long long int current, carry = 0, multiplier = basePowerLimit / base;

	for (int i = numberOfParts() - 1; i >= 0; i--)
	{
		current = parts[i] / base;
		current += carry * multiplier;

		carry = parts[i] % base;

		parts[i] = current;
	}

	simplifyNumber();
}

void Number::extendParts(size_t numberOfExtraParts)
{
	size_t len = numberOfParts();
	parts.resize(numberOfExtraParts + len);
	for (int i = len - 1; i >= 0; i--) parts[numberOfExtraParts + i] = parts[i];
	for (size_t i = 0; i < numberOfExtraParts; i++) parts[i] = 0;
}

size_t Number::numberOfParts() const
{
	return parts.size();
}

Number::Number(unsigned int x)
{
	parts.push_back(x);
}

Number::Number(const std::string& s)
{
	unsigned int number = 0;
	unsigned int multiplier = 1;

	for (int i = s.size() - 1; i >= 0; i--)
	{
		number += multiplier * (s[i] - '0');
		multiplier *= base;

		if (multiplier >= basePowerLimit)
		{
			parts.push_back(number);
			number = 0;
			multiplier = 1;
		}
	}

	parts.push_back(number);
	simplifyNumber();
}

Number::Number(const Number& other)
{
	copyData(other);
}

Number& Number::operator=(const Number& other)
{
	if (this != &other)
	{
		parts.clear();
		copyData(other);
	}
	return *this;
}

Number Number::operator+(const Number& other) const
{
	Number result;
	result.parts.clear();

	size_t i = 0;
	unsigned long long int sum;
	unsigned long long int carry = 0, first, second;

	while (i < numberOfParts() || i < other.numberOfParts())
	{
		if (i < numberOfParts()) first = parts[i];
		else first = 0;

		if (i < other.numberOfParts()) second = other.parts[i];
		else second = 0;

		sum = first + second + carry;

		result.parts.push_back(sum % basePowerLimit);
		carry = sum / basePowerLimit;

		i++;
	}

	if (carry > 0) result.parts.push_back(carry);

	return result;
}

Number Number::operator-(const Number& other) const
{
	if (*this < other) return 0;

	Number result;
	result.parts.resize(numberOfParts());

	unsigned long long int difference;
	unsigned long long int carry = 0, first, second;

	for (size_t i = 0; i < numberOfParts(); i++)
	{
		first = parts[i];
		if (i < other.numberOfParts()) second = other.parts[i];
		else second = 0;
		second += carry;

		if (second > first)
		{
			carry = 1;
			first += basePowerLimit;
		}
		else carry = 0;

		difference = first - second;

		result.parts[i] = difference;
	}

	result.simplifyNumber();
	return result;
}

Number Number::operator*(const Number& other) const
{
	Number result, temp;
	unsigned long long int product, first, second;

	for (size_t i = 0; i < numberOfParts(); i++)
	{
		for (size_t j = 0; j < other.numberOfParts(); j++)
		{
			first = parts[i];
			second = other.parts[j];
			product = first * second;

			temp.parts.resize(2);
			temp.parts[0] = product % basePowerLimit;
			temp.parts[1] = product / basePowerLimit;
			temp.simplifyNumber();
			temp.extendParts(i + j);

			result = result + temp;
		}
	}

	return result;
}

Number Number::operator/(const Number& other) const
{
	if (other == Number(0)) return Number(0);
	if (*this < other) return Number(0);

	Number result, remainder = *this, divider = other;
	int power = -1;
	unsigned int counter;

	while (!(divider > remainder))
	{
		power++;
		divider.multiplyByBase();
	}
	divider.divideByBase();

	do
	{
		power--;
		result.multiplyByBase();
		counter = 0;
		while (!(remainder < divider))
		{
			remainder = remainder - divider;
			counter++;
		}
		result.parts[0] += counter;
		divider.divideByBase();

	} while (power >= 0);

	return result;
}

Number Number::operator%(const Number& other) const
{
	if (other == Number(0)) return Number(0);
	if (*this < other) return *this;

	Number remainder = *this, divider = other;
	int power = -1;

	while (!(divider > remainder))
	{
		power++;
		divider.multiplyByBase();
	}
	divider.divideByBase();

	do
	{
		power--;
		while (!(remainder < divider)) remainder = remainder - divider;
		divider.divideByBase();

	} while (power >= 0);

	return remainder;
}

bool Number::operator<(const Number& other) const
{
	if (numberOfParts() < other.numberOfParts()) return true;
	if (numberOfParts() > other.numberOfParts()) return false;

	for (int i = other.numberOfParts() - 1; i >= 0; i--)
	{
		if (parts[i] < other.parts[i]) return true;
		if (parts[i] > other.parts[i]) return false;
	}
	return false;
}

bool Number::operator>(const Number& other) const
{
	return (other < *this);
}

bool Number::operator<=(const Number& other) const
{
	return !(*this > other);
}

bool Number::operator>=(const Number& other) const
{
	return !(*this < other);
}

bool Number::operator==(const Number& other) const
{
	if (numberOfParts() != other.numberOfParts()) return false;
	for (size_t i = 0; i < numberOfParts(); i++)
	{
		if (parts[i] != other.parts[i]) return false;
	}
	return true;
}

bool Number::operator!=(const Number& other) const
{
	return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Number& num)
{
	os << num.parts.back();
	for (int i = num.numberOfParts() - 2; i >= 0; i--)
	{
		unsigned int threshold = Number::basePowerLimit / Number::base;
		while (threshold > num.parts[i])
		{
			os << 0;
			threshold /= Number::base;
		}
		if (num.parts[i] > 0) os << num.parts[i];
	}

	return os;
}