#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#define TYPE_NAME_MAX_LEN		48

typedef struct _type_range_info
{
	char name[TYPE_NAME_MAX_LEN];
	ssize_t type_bytes;
	__int128_t type_max;
	__int128_t type_min;
	__int128_t type_capacity;
} type_range_info;

type_range_info tprng[] = {
	{
		.name = "char",
		.type_bytes = sizeof(char),
		.type_max = CHAR_MAX,
		.type_min = CHAR_MIN,
		.type_capacity = UCHAR_MAX,
	},
	{
		.name = "int",
		.type_bytes = sizeof(int),
		.type_max = INT_MAX,
		.type_min = INT_MIN,
		.type_capacity = UINT_MAX,
	},
	{
		.name = "long",
		.type_bytes = sizeof(long),
		.type_max = LONG_MAX,
		.type_min = LONG_MIN,
		.type_capacity = ULLONG_MAX,
	},
};
ssize_t types_count = sizeof(tprng) / sizeof(type_range_info);

static void range_get(void)
{
	int i, bits;
	__int128_t capacity, max, min;

	i = 0;
	printf("\ntyps range get by limits.h\n");
	while (i < types_count) {
		printf("\t%s max = %lld\n", tprng[i].name, tprng[i].type_max);
		printf("\t%s min = %lld\n", tprng[i].name, tprng[i].type_min);
		printf("\t%s capacity = %llu\n", tprng[i].name, tprng[i].type_capacity);

		i++;
	}

	i = 0;
	printf("\ntyps range get by bits\n");
	while (i < types_count) {
		bits = tprng[i].type_bytes * CHAR_BIT;
		max = (1ULL << (bits - 1)) - 1;
		min = -(1ULL << (bits - 1));

		printf("\tbits = %04d ; %s max = %lld\n", bits, tprng[i].name, max);
		printf("\tbits = %04d ; %s min = %lld\n", bits, tprng[i].name, min);

		i++;
	}
}

int main(void)
{
	printf("number of bits occupied by a byte = %ld\n", CHAR_BIT);

	range_get();
}
