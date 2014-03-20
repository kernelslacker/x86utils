#pragma once

// TYPES
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char bool;

// HANDY MACROS
#define ARRAY_SIZE(x) ((int)(sizeof(x)/sizeof(x[0])))
#define __stringify_1(x...)     #x
#define __stringify(x...)       __stringify_1(x)
#define __unused __attribute((unused))

#define TRUE 1
#define FALSE 0

// FUNCTION PROTOTYPES
void cpuid_UP(unsigned int idx, unsigned long *eax, unsigned long *ebx, unsigned long *ecx, unsigned long *edx);
void cpuid(unsigned int cpu, unsigned long long idx, unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx);
void cpuid4(unsigned int cpu, unsigned long long idx, unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx);
void cpuid_count(unsigned int cpu, unsigned int op, int count, unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx);
unsigned int cpuid_ebx(unsigned int cpu, unsigned int op);
int native_cpuid(unsigned int cpunr, unsigned long long idx, unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx);
int HaveCPUID(void);

#define family(c) (c->family + c->efamily)
#define model(c) ((c->emodel << 4) + c->model)

#define tuple(c) ((c->family << 8) | (c->model << 4) | (c->stepping))
#define etuple(c) ((c->efamily << 8) | (c->model <<4) | (c->stepping))

void bind_cpu(unsigned int cpunr);

int read_msr(int cpu, unsigned int idx, unsigned long long *val);
