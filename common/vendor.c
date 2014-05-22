/* inspired from coreboot. */

/* List of cpu vendor strings along with their normalized
 * id values.
 */

#include <string.h>
#include <vendors.h>
#include <x86utils.h>

static struct {
        int vendor;
        const char *name;
} x86_vendors[] = {
        { VENDOR_INTEL,     "GenuineIntel", },
        { VENDOR_CYRIX,     "CyrixInstead", },
        { VENDOR_AMD,       "AuthenticAMD", },
        { VENDOR_UMC,       "UMC UMC UMC ", },
        { VENDOR_NEXGEN,    "NexGenDriven", },
        { VENDOR_CENTAUR,   "CentaurHauls", },
        { VENDOR_RISE,      "RiseRiseRise", },
        { VENDOR_TRANSMETA, "GenuineTMx86", },
        { VENDOR_TRANSMETA, "TransmetaCPU", },
        { VENDOR_NSC,       "Geode by NSC", },
        { VENDOR_SIS,       "SiS SiS SiS ", },
};

static const char *x86_vendor_name[] = {
        [VENDOR_INTEL]     = "Intel",
        [VENDOR_CYRIX]     = "Cyrix",
        [VENDOR_AMD]       = "AMD",
        [VENDOR_UMC]       = "UMC",
        [VENDOR_NEXGEN]    = "NexGen",
        [VENDOR_CENTAUR]   = "Centaur",
        [VENDOR_RISE]      = "Rise",
        [VENDOR_TRANSMETA] = "Transmeta",
        [VENDOR_NSC]       = "NSC",
        [VENDOR_SIS]       = "SiS",
};

const char *cpu_vendor_name(int vendor)
{
	const char *name = "<invalid cpu vendor>";

	if ((vendor < (ARRAY_SIZE(x86_vendor_name))) && (x86_vendor_name[vendor] != 0))
		name = x86_vendor_name[vendor];

	return name;
}


int get_vendor(const char *vendorstr)
{
	unsigned int i = 0;

	while (i < ARRAY_SIZE(x86_vendors)) {
		if (strcmp(vendorstr, x86_vendors[i].name)) {
			return x86_vendors[i].vendor;
		}
		i++;
	}
	return -1;
}
