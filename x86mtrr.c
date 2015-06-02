/*
 *  Licensed under the terms of the GNU GPL License version 2.
 *
 *  MTRR register dumping.
 */

#include <stdio.h>
#include <asm/mtrr.h>
#include <x86utils.h>

#define X86_FEATURE_MTRR (1<<12)

#define IA32_MTRRCAP_WC   (1 << 10)
#define IA32_MTRRCAP_FIX  (1 << 8)
#define IA32_MTRRCAP_VCNT 0xFF

#define IA32_MTRR_DEFTYPE_E    0x800
#define IA32_MTRR_DEFTYPE_FE   0x400
#define IA32_MTRR_DEFTYPE_TYPE 0xFF

#define IA32_PHYBASE_TYPE   0XFF
#define IA32_PHYMASK_VALID  (1 << 11)

static unsigned int max_phy_addr = 0;

static const char * mtrr_types[MTRR_NUM_TYPES] =
{
	"uncacheable",
	"write-combining",
	"?",
	"?",
	"write-through",
	"write-protect",
	"write-back",
};

static void decode_address(unsigned long long val)
{
	switch (max_phy_addr) {
	case 40:
		printf("0x%07x ", (unsigned int) ((val >> 12) & 0xFFFFFFF));
		break;
	case 36:
	default:
		printf("0x%06x ", (unsigned int) ((val >> 12) & 0xFFFFFF));
		break;
	}
}

static void set_max_phy_addr(unsigned int cpunum)
{
	unsigned int value;

	if (!max_phy_addr) {
		cpuid(cpunum, 0x80000008, &value, NULL, NULL, NULL);
		max_phy_addr = value & 0xFF;
	}
}

static int mtrr_value(unsigned int cpunum, int msr, unsigned long long *val)
{
	if (read_msr(cpunum, msr, val) == 1)
		return 1;
	else
		return 0;
}

static void dump_mtrr(unsigned int cpunum, int msr)
{
	unsigned long long val = 0;

	if (read_msr(cpunum, msr, &val) == 1)
		printf("0x%016llx\n", val);
}

static void decode_mtrrcap(unsigned int cpunum, int msr)
{
	unsigned long long val;
	int ret;

	ret = mtrr_value(cpunum, msr, &val);
	if (ret) {
		printf("0x%016llx ", val);
		printf("wc:%d ",
			!! ((unsigned int) val & IA32_MTRRCAP_WC));
		printf("fix:%d ",
			!! ((unsigned int) val & IA32_MTRRCAP_FIX));
		printf("vcnt:%d\n",
			(unsigned int) val & IA32_MTRRCAP_VCNT);
	}
}

static void decode_mtrr_deftype(unsigned int cpunum, int msr)
{
	unsigned long long val;
	int ret;
	u8 type;

	ret = mtrr_value(cpunum, msr, &val);
	if (ret) {
		printf("0x%016llx ", val);

		printf("(fixed-range flag:%d ",
			!! ((unsigned int) val & (1 << 10)));

		printf("enable flag:%d ",
			!! ((unsigned int) val & (1 << 11)));

		type = val & IA32_MTRR_DEFTYPE_TYPE;
		printf("default type:0x%02x (%s))\n", type, mtrr_types[type]);
	}
}

static void decode_mtrr_physbase(unsigned int cpunum, int msr)
{
	unsigned long long val;
	int ret;
	u8 type;

	ret = mtrr_value(cpunum, msr, &val);
	if (ret) {
		printf("0x%016llx ", val);

		printf("(physbase:");
		decode_address(val);

		type = (unsigned int) val & IA32_PHYBASE_TYPE;
		printf("type: 0x%02x (%s))\n", type, mtrr_types[type]);
	}
}

static void decode_mtrr_physmask(unsigned int cpunum, int msr)
{
	unsigned long long val;
	int ret;

	ret = mtrr_value(cpunum, msr, &val);
	if (ret) {
		printf("0x%016llx ", val);

		printf("(physmask:");
		decode_address(val);

		printf("valid:%d)\n",
			!! ((int) val & IA32_PHYMASK_VALID));
	}
}

struct mtrrdesc {
	const char *name;
	unsigned int reg;
};

static void dump_mtrrs(unsigned int cpunum)
{
	unsigned long long val = 0;
	unsigned int i;
	struct mtrrdesc fixedmtrrs[] = {
		{ "MTRRfix64K_00000", 0x250 },
		{ "MTRRfix16K_80000", 0x258 },
		{ "MTRRfix16K_A0000", 0x259 },
		{ "MTRRfix4K_C8000", 0x269 },
		{ "MTRRfix4K_D0000", 0x26a },
		{ "MTRRfix4K_D8000", 0x26b },
		{ "MTRRfix4K_E0000", 0x26c },
		{ "MTRRfix4K_E8000", 0x26d },
		{ "MTRRfix4K_F0000", 0x26e },
		{ "MTRRfix4K_F8000", 0x26f },
	};

	/*
	 * If MTRR registers are not accessible like in some
	 * virtualization systems then return
	 */
	if (!read_msr(cpunum, 0xfe, &val))
		return;

	printf("MTRR registers for cpu %u:\n", cpunum);

	printf("MTRRcap (0xfe): ");
	decode_mtrrcap(cpunum, 0xfe);

	set_max_phy_addr(cpunum);

	for (i = 0; i < 16; i+=2) {
		printf("MTRRphysBase%u (0x%x): ", i / 2, (unsigned int) 0x200 + i);
		decode_mtrr_physbase(cpunum, 0x200 + i);
		printf("MTRRphysMask%u (0x%x): ", i/2, (unsigned int) 0x201 + i);
		decode_mtrr_physmask(cpunum, 0x201 + i);
	}

	for (i = 0; i < ARRAY_SIZE(fixedmtrrs); i++) {
		unsigned int reg = fixedmtrrs[i].reg;

		printf("%s (0x%x): ", fixedmtrrs[i].name, reg);
		dump_mtrr(cpunum, reg);
	}

	printf("MTRRdefType (0x2ff): ");
	decode_mtrr_deftype(cpunum, 0x2ff);

	printf("\n");
}

static bool check_feature(unsigned int cpunum)
{
	unsigned int maxi, edx;

	cpuid(cpunum, 0, &maxi, NULL, NULL, NULL);
	maxi &= 0xffff;         /* The high-order word is non-zero on some Cyrix CPUs */
	if (maxi < 1)
		return FALSE;

	cpuid(cpunum, 1, NULL, NULL, NULL, &edx);
	if (!(edx & X86_FEATURE_MTRR))
		return TRUE;

	return FALSE;
}

int main(int __unused argc, char __unused **argv)
{
	unsigned int cpunum = 0;

	//TODO: parse args, get cpunum

	bind_cpu(cpunum);

	if (check_feature(cpunum)) {
		printf("cpuid advertises no MTRRs on this processor.\n");
		return 0;
	}

	dump_mtrrs(0);
	return 0;
}
