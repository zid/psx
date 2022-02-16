#include <stdarg.h>
#include "print.h"

#define BUF_SIZ 64

static void putchar(unsigned int c)
{
	*((volatile unsigned char *)(0x1f802080)) = c;
}

static void putstr(const char *s)
{
	while(*s)
		putchar(*s++);
}

static void put_number(unsigned long n, int islong, int base, char pad_char, int width)
{
	unsigned char digit;
	int len;
	char buf[BUF_SIZ];
	char *p = &buf[BUF_SIZ-1];

	if(!islong)
		n = (unsigned int)n;

	do {
		digit = n % base;
		n /= base;
		*p-- = digit["0123456789ABCDEF"];
	} while(n);

	len = &buf[BUF_SIZ-1] - p;
	while(width && len < width)
	{
		*p-- = pad_char;
		len++;
	}

	while(++p != &buf[BUF_SIZ])
		putchar(*p);
}

static void vprint(const char *fmt, va_list ap)
{
	const char *p;
	unsigned char c;
	unsigned long l;
	int long_int;
	char pad_char;
	int width;
	int base;
	p = fmt;

	while(1)
	{
		width = 0;
		pad_char = ' ';
		long_int = 0;

		for(; *p && *p != '%'; p++)
			putchar(*p);
		if(!*p)
			return;
		p++;
again:		c = *p++;

		if(c == '0')
		{
			pad_char = '0';
			c = *p++;
		}

		while(c >= '0' && c <= '9')
		{
			width = width*10 + c - '0';
			c = *p++;
		}

		switch(c)
		{
			case '%':
				putchar(c);
			break;
			case 'l':
				long_int = 1;
				goto again;
			break;
			case 'X':
			case 'x':
				base = 16;
				goto print_num;
			case 'd':
				base = 10;
				goto print_num;


			case 's':
				putstr(va_arg(ap, const char *));
			break;
			case 'c':
				putchar(va_arg(ap, int));
			break;
			print_num:
				if(long_int)
					l = va_arg(ap, long);
				else
					l = va_arg(ap, int);
				put_number(l, long_int, base, pad_char, width);
			break;
		}

	}
}

void printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprint(fmt, ap);
	va_end(ap);
}
