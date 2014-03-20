/*
 *  Licensed under the terms of the GNU GPL License version 2.
 *
 *  Generic routines for retrieving cpuid registers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <x86utils.h>

/* returns zero on success */
int native_cpuid(unsigned int cpunr, unsigned long long idx,
	unsigned int *eax, unsigned int *ebx,
	unsigned int *ecx, unsigned int *edx)
{
	unsigned int a = 0, b = 0, c = 0, d = 0;

	if (eax != NULL)
		a = *eax;
	if (ebx != NULL)
		b = *ebx;
	if (ecx != NULL)
		c = *ecx;
	if (edx != NULL)
		d = *edx;

	bind_cpu(cpunr);

	asm("cpuid"
		: "=a" (a),
		  "=b" (b),
		  "+c" (c),
		  "=d" (d)
		: "0" ((unsigned int)idx));

	if (eax != NULL)
		*eax = a;
	if (ebx != NULL)
		*ebx = b;
	if (ecx != NULL)
		*ecx = c;
	if (edx != NULL)
		*edx = d;

	return 0;
}

void cpuid4(unsigned int CPU_number, unsigned long long idx,
	unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx)
{
	cpuid(CPU_number, 4 | (idx << 32), eax, ebx, ecx, edx);
}

/* Some CPUID calls want 'count' to be placed in ecx */
void cpuid_count(unsigned int CPU_number, unsigned int op, int count,
	unsigned int *eax, unsigned int *ebx,
	unsigned int *ecx, unsigned int *edx)
{
	*ecx = count;
	cpuid(CPU_number, op, eax, ebx, ecx, edx);
}

unsigned int cpuid_ebx(unsigned int CPU_number, unsigned int op)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(CPU_number, op, &eax, &ebx, &ecx, &edx);

	return ebx;
}
