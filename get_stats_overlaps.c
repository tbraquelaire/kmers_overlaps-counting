#include "get_stats_overlaps.h"

void usage(){
	fprintf(stderr, 
		"require an input (sorted and counted binary file), number max over possible overlaps, and a output file\n");
}

void get_stats_overlaps(FILE* ifp, int nb_max_overlaps, FILE* ofp){

	uint64_t nb_nodes = 0;
	uint64_t bit_vector = 0;
	uint64_t mask_nb_overlaps = ((1 << 5)-1);
	int nb_overlaps = 0;

	uint64_t* tab_nb_overlaps = malloc(sizeof(*tab_nb_overlaps)*(nb_max_overlaps+1));
	memset (tab_nb_overlaps, 0, (nb_max_overlaps+1)*(sizeof(*tab_nb_overlaps)));

	while (fread(&bit_vector, sizeof(uint64_t), 1, ifp)){
		++nb_nodes;
		nb_overlaps = (int) bit_vector & mask_nb_overlaps;


		//printf("nb overlaps : %d\n", nb_overlaps);
		//printf("%"PRIu64"\n", tab_nb_overlaps[nb_overlaps]);
		tab_nb_overlaps[nb_overlaps] += 1;
		//printf("%"PRIu64"\n", tab_nb_overlaps[nb_overlaps]);
	}

	fprintf(ofp, "nb nodes : %"PRIu64"\n", nb_nodes);
	for (int actual_nb_overlaps = 0; actual_nb_overlaps <= nb_max_overlaps; ++actual_nb_overlaps){
		fprintf(ofp, "nb of kmers with %d overlaps : %"PRIu64"\n", 
			actual_nb_overlaps, 
			tab_nb_overlaps[actual_nb_overlaps]);
	}

	free(tab_nb_overlaps);
	
}

int main(int argc, char const *argv[])
{
	
	if (argc != 4){
		usage();
		return EXIT_FAILURE;
	}

	FILE* ifp = fopen(argv[1],"r"); // read mode
	int nb_max_overlaps = atoi(argv[2]);
	FILE* ofp = fopen(argv[3], "w");

	get_stats_overlaps(ifp, nb_max_overlaps, ofp);

	return EXIT_SUCCESS;
}