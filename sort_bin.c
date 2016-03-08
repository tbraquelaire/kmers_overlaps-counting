#include "sort_bin.h"

void usage(){
	printf("require a binary file, number of blocks to fit in memory, and an output file\n");
}

/* dump tab */


/*static void tab_dump (uint64_t *a, size_t n)
{
   size_t i;
   int counter = 0;
   for (i = 0; i < n; i++)
   {
      printf ("c = %d\t%"PRIu64"\n", counter, a[i]);
      ++counter;
   }
   printf ("\n");
}*/

/* compare function to give to qsort() */
static int compare (void const *a, void const *b)
{

   uint64_t const *pa = a;
   uint64_t const *pb = b;

   //TEST
   uint64_t uint64a = *pa;
   uint64_t uint64b = *pb;

   //number of group of 15 bits
   int cpt = 0;

   int inta = (uint64a << (cpt * 15)) >> (64 - 15);
   int intb = (uint64b << (cpt * 15)) >> (64 - 15);

   int res = 0;

   while ( ((res = (inta - intb)) == 0) && cpt < 5){
   	++cpt;
   	inta = (uint64a << (cpt * 15)) >> (64 - 15);
   	intb = (uint64b << (cpt * 15)) >> (64 - 15);
   }

   /*

	if (a<b){
		return -1;
	}
	if (a>b){
		return 1;
	}
	return 0;

   */

   return res;
   	//return *pa - *pb;
}

/*
** sort binary file
**file must contains code blocks of 8 bytes each
**TODO may generelize to all code blocks type
*/
void sort_bin(FILE* ifp, int nb_allowed_blocks, FILE* ofp){

	uint64_t tab[nb_allowed_blocks];
	uint64_t id_tmp_file = 0;
	uint64_t nb_tmp_files = 0;
	char* tmp_file_name = "sort_tmp_file";
	char id[1024]; 
	char tmp_file[1024];

	//uint64_t bit_vector;
	size_t nb_blocks_read = 0;

	//bool end_of_file = false;
	while(!feof(ifp)){

		memset(tmp_file, '\0', strlen(tmp_file));
		memset(id, '\0', strlen(id));
		strcpy(tmp_file, tmp_file_name);
		sprintf(id,"%"PRIu64"",id_tmp_file);
		strcat(tmp_file, id);
		FILE* f_save = fopen(tmp_file, "w");
		nb_blocks_read = 0;
/*		while (*/nb_blocks_read = fread(tab, sizeof(uint64_t), nb_allowed_blocks, ifp);// && nb_blocks_read < nb_allowed_blocks){
		for (int i = 0; i<10; ++i){
			uint64_t bit_vector;
			bit_vector = tab[i];
			printf("kmer : %"PRIu64" pos : %"PRIu64" lettre : %"PRIu64" count overlaps : %"PRIu64"\n",
			bit_vector>>10, (bit_vector<<54)>>63, (bit_vector<<55)>>60, (bit_vector<<59)>>59);
			printf("%"PRIu64"\n", bit_vector);		
		}
		fflush(stdout);
//			tab[nb_blocks_read] = bit_vector;
//			++nb_blocks_read;
//		}
//		if (nb_blocks_read != nb_allowed_blocks){
//			end_of_file = true;
//		}

		//printf("before\n\n\n");
		//tab_dump(tab, sizeof tab / sizeof *tab);
		//TODO clean sizeof(tab)/sierof etc...

		qsort(tab, nb_blocks_read, sizeof(*tab), compare);

//printf("after\n\n\n");
		//TODO clean sizeof(tab)/sierof etc...
		//tab_dump(tab, sizeof tab / sizeof *tab);

		//fprintf(stdout, "%d block sorted\n");

		//write sorted tab in a tmp file
		/*for (int i=0; i<nb_blocks_read; ++i){
			bit_vector = tab[i];*/
			fwrite(tab, sizeof(uint64_t), nb_blocks_read, f_save);
		//}

		++id_tmp_file;
		++nb_tmp_files;
		nb_blocks_read = 0;

		fclose(f_save);
	}

}

int main(int argc, char const *argv[])
{
	if (argc != 4){
		usage();
		return EXIT_FAILURE;
	}

	if (argv[2] <= 0){
		fprintf(stderr, "can't give sort with 0 blocks\n");
		return EXIT_FAILURE;
	}

	FILE* ifp = fopen(argv[1], "r");
	int nb_allowed_blocks = atoll(argv[2]);
	FILE* ofp = fopen(argv[3], "w");

	struct rlimit rl;
	getrlimit(RLIMIT_STACK, &rl);
	//approximative max size...
	const rlim_t maxstacksize = 1024*1024*1024+1000000000000000000*9;
	rl.rlim_cur = maxstacksize;
	int result = setrlimit(RLIMIT_STACK, &rl);
    if (result != 0){
	    fprintf(stderr, "setrlimit returned result = %d\n", result);
    }

	sort_bin(ifp, nb_allowed_blocks, ofp);

	return 0;
}