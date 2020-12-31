#pragma once

#include <iostream>
#include <string>
#include <vector>

class Number
{
private:
	const static unsigned int base = 10;
	const static unsigned int basePowerLimit = 1000000000;

	std::vector<unsigned int> parts;

	void copyData(const Number&);
	void simplifyNumber();
	void multiplyByBase();
	void divideByBase();
	void extendParts(size_t);

	size_t numberOfParts() const;

public:
	Number(unsigned int x = 0);
	Number(const std::string&);
	Number(const Number&);
	Number& operator=(const Number&);

	Number operator+(const Number&) const;
	Number operator-(const Number&) const;
	Number operator*(const Number&) const;
	Number operator/(const Number&) const;
	Number operator%(const Number&) const;

	bool operator<(const Number&) const;
	bool operator>(const Number&) const;
	bool operator<=(const Number&) const;
	bool operator>=(const Number&) const;
	bool operator==(const Number&) const;
	bool operator!=(const Number&) const;

	friend std::ostream& operator<<(std::ostream&, const Number&);
};

std::ostream& operator<<(std::ostream&, const Number&);
