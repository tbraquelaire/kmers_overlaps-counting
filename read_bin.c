#include "read_bin.h"

void usage(){
	fprintf(stderr, "require an input file (binary)\n");
}

void read_bin(FILE* ifp){
	fseek(ifp, 0, SEEK_END);
  	long cur = ftell(ifp);
  	cur -= 100*sizeof(uint64_t);
  	fseek(ifp, cur, SEEK_SET);


	uint64_t bit_vector = 0;

	int vectors_read = 0;
	printf("vectors_read : %d\n", vectors_read);
	while (fread(&bit_vector, sizeof(uint64_t), 1, ifp) && vectors_read<100){
		++vectors_read;
		printf("vectors_read : %d\n", vectors_read);
		printf("kmer : %"PRIu64" pos : %"PRIu64" lettre : %"PRIu64" count overlaps : %"PRIu64"\n",
			bit_vector>>10, (bit_vector<<54)>>63, (bit_vector<<55)>>60, (bit_vector<<59)>>59);
		printf("%"PRIu64"\n", bit_vector);
	}
}

int main(int argc, char const *argv[])
{
	if (argc != 2){
		usage();
		return EXIT_FAILURE;
	}

	FILE* ifp = fopen(argv[1], "r");

	read_bin(ifp);

	return 0;
}
