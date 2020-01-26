/*
 * Print.cpp
 *
 * Created: 1/25/2020 1:11:22 PM
 *  Author: Andrew
 */ 

#include "Print.h"
//#include "ObsoleteTX.h"

// Public Methods //////////////////////////////////////////////////////////////

/* default implementation: may be overridden */
size_t Print::write(const uint8_t *buffer, size_t size)
{
	size_t n = 0;
	while (size--) {
		if (write(*buffer++)) n++;
		else break;
	}
	return n;
}

size_t Print::print(const __FlashStringHelper *ifsh)
{
	PGM_P p = reinterpret_cast<PGM_P>(ifsh);
	size_t n = 0;
	while (1) {
		unsigned char c = pgm_read_byte(p++);
		if (c == 0) break;
		if (write(c)) n++;
		else break;
	}
	return n;
}

size_t Print::print(const char str[])
{
	return write(str);
}

size_t Print::print(char c)
{
	return write(c);
}

size_t Print::print(unsigned char b, int base)
{
	return print((unsigned long) b, base);
}

size_t Print::print(int n, int base)
{
	return print((long) n, base);
}

size_t Print::print(unsigned int n, int base)
{
	return print((unsigned long) n, base);
}

size_t Print::print(long n, int base)
{
	if (base == 0) {
		return write(n);
		} else if (base == 10) {
		if (n < 0) {
			int t = print('-');
			n = -n;
			return printNumber(n, 10) + t;
		}
		return printNumber(n, 10);
		} else {
		return printNumber(n, base);
	}
}

size_t Print::print(unsigned long n, int base)
{
	if (base == 0) return write(n);
	else return printNumber(n, base);
}

size_t Print::println(const __FlashStringHelper *ifsh)
{
	size_t n = print(ifsh);
	n += println();
	return n;
}

size_t Print::println(const char c[])
{
	size_t n = print(c);
	n += println();
	return n;
}

size_t Print::println(char c)
{
	size_t n = print(c);
	n += println();
	return n;
}

size_t Print::println(unsigned char b, int base)
{
	size_t n = print(b, base);
	n += println();
	return n;
}

size_t Print::println(int num, int base)
{
	size_t n = print(num, base);
	n += println();
	return n;
}

size_t Print::println(unsigned int num, int base)
{
	size_t n = print(num, base);
	n += println();
	return n;
}

size_t Print::println(long num, int base)
{
	size_t n = print(num, base);
	n += println();
	return n;
}

size_t Print::println(unsigned long num, int base)
{
	size_t n = print(num, base);
	n += println();
	return n;
}

size_t Print::println(void)
{
	return print(F("\r\n"));
}

// Private Methods /////////////////////////////////////////////////////////////

size_t Print::printNumber(unsigned long n, uint8_t base)
{
	char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
	char *str = &buf[sizeof(buf) - 1];

	*str = '\0';

	// prevent crash if called with base == 1
	if (base < 2) base = 10;

	do {
		char c = n % base;
		n /= base;

		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	} while(n);

	return write(str);
}

