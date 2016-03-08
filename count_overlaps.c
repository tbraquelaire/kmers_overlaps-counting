#include "count_overlaps.h"

#define ALPH_SIZE 8

void usage(){
	fprintf(stderr, "require an input file (ordered binary) ans an output file (for the result)\n");
}

int binary_decimal(int n) /* Function to convert binary to decimal.*/

{
    int decimal=0, i=0, rem;
    while (n!=0)
    {
        rem = n%10;
        n/=10;
        decimal += rem*pow(2,i);
        ++i;
    }
    return decimal;
}

/*
moyenne chevauchements
savoir cb à 0, cb à 1,..., cb à 8
*/
void count_overlaps(FILE* ifp, FILE* ofp){

	uint64_t bit_vector1 = 0;
	uint64_t bit_vector1_old = -1;
	uint64_t bit_vector2 = 0;
	uint64_t nb_total_overlaps = 0; 
	uint64_t nb_kmers = 0;
	int actual_kmer_nb_overlaps = 0;

	//to avoid read file backwards -> save overlaps of precedent kmers with same k-1mer
	int precedent_nb_overlaps_pos0 = 1;  
	int precedent_nb_overlaps_pos1 = 0; 
	uint64_t precedent_bit_vector_kmer = -1;

	uint64_t overlaps_table[ALPH_SIZE+1];
	for (int i=0; i<ALPH_SIZE+1; ++i){
		overlaps_table[i] = 0;
	}

	uint64_t nb_vectors = 0;
	uint64_t nb_vec_m = 0;

	uint64_t bit_kmer1 = 0;
	//uint64_t bit_letter_kmer1 = 0;
	uint64_t bit_pos_kmer1 = 0;
	uint64_t bit_kmer2 = 0;
	uint64_t bit_letter_kmer2 = 0;
	uint64_t bit_pos_kmer2 = 0;
	bool same_kmers = true;
	bool end_of_file = false;
	int tab_encoutered_letters[ALPH_SIZE];
	for (int i=0; i<ALPH_SIZE; ++i){
		tab_encoutered_letters[i] = 0;
	}

	int letter_int;
	long cursor_save; //  cursor_save = ftell(ifp);  then  fseek(ifp, read1_cursor, SEEK_SET);
	while (fread(&bit_vector1, sizeof(uint64_t), 1, ifp) == 1){
		++nb_vectors;
		if (bit_vector1 != bit_vector1_old){
			++nb_kmers;

			bit_kmer1 = bit_vector1 >> 5;
			//bit_letter_kmer1 = ((bit_vector1<<59)>>60);
			bit_pos_kmer1 = ((bit_vector1<<63)>>63);

			cursor_save = ftell(ifp);

			same_kmers = true;
			end_of_file = false;
			while (same_kmers && !end_of_file){
				//if read occurs correctly
				if (fread(&bit_vector2, sizeof(uint64_t), 1, ifp) == 1){

					bit_kmer2 = bit_vector2 >> 5;
					bit_letter_kmer2 = ((bit_vector2<<59)>>60);
					bit_pos_kmer2 = ((bit_vector2<<63)>>63);

					if (bit_kmer1 == bit_kmer2){

						if (bit_pos_kmer1 != bit_pos_kmer2){
							letter_int = binary_decimal(bit_letter_kmer2);

							//if new overlap -> count it and save it (to not count it again)
							if (!tab_encoutered_letters[letter_int]){
								tab_encoutered_letters[letter_int] = 1;
								++actual_kmer_nb_overlaps;
								++nb_total_overlaps;

								//printf("bit_letter_kmer2 : %"PRIu64"\n", bit_letter_kmer2);

							}
						}


					}

					else{
						same_kmers = false;
					}
				}
				//else end of file
				else{
					end_of_file = true;
				}
			}

			//add the overlaps existing before the kmer (which have already been read)
			if (bit_kmer1 == precedent_bit_vector_kmer){

				if (bit_pos_kmer1 == 0){
					actual_kmer_nb_overlaps += precedent_nb_overlaps_pos1;
					++precedent_nb_overlaps_pos0;
					printf("...0...\n");
				}
				else{
					actual_kmer_nb_overlaps += precedent_nb_overlaps_pos0;
					++precedent_nb_overlaps_pos1;
					printf("...1...  p0:%d p1:%d\n", precedent_nb_overlaps_pos0, precedent_nb_overlaps_pos1);
				}
				printf("v1 %"PRIu64"     v2 %"PRIu64"    bk1 %"PRIu64"     bk2 %"PRIu64"    pbk1 %"PRIu64"\n",bit_vector1, bit_vector2, bit_kmer1, bit_kmer2, precedent_bit_vector_kmer);
			}

			else{
				precedent_bit_vector_kmer = bit_kmer1;
				precedent_nb_overlaps_pos0 = 1;
				precedent_nb_overlaps_pos1 = 0;
			}
/*
			printf("bit_kmer1 : %"PRIu64"\n", bit_kmer1);
			printf("bit_letter_kmer1 : %"PRIu64"\n", bit_letter_kmer1);
			printf("bit_pos_kmer1 : %"PRIu64"\n", bit_pos_kmer1);
			printf("actual_kmer_nb_overlaps : %d\n", actual_kmer_nb_overlaps);
			if (bit_pos_kmer1 == 0){
				printf("precedent_nb_overlaps_pos1 : %d\n\n", precedent_nb_overlaps_pos1);
			}	
			else{
				printf("precedent_nb_overlaps_pos0 : %d\n\n", precedent_nb_overlaps_pos0);
			}*/

			//save nb_overlaps
			++overlaps_table[actual_kmer_nb_overlaps];

			//reset tab saving letters, nb actual overlaps
			for (int i=0; i<ALPH_SIZE; ++i){
				tab_encoutered_letters[i] = 0;
			}
			actual_kmer_nb_overlaps = 0;

			fseek(ifp, cursor_save, SEEK_SET);

			//keep old vector
			bit_vector1_old = bit_vector1;
		}

		//bit_vector, (bit_vector>>5), ((bit_vector<<59)>>60), ((bit_vector<<63)>>63));
		//printf("vectors_read : %d\n", vectors_read);
		//printf("%"PRIu64"\n", bit_vector);
		if (nb_vectors == 1000000){
			nb_vectors = 0;
			++nb_vec_m;
			printf("%"PRIu64"m vectors reads\n", nb_vec_m);
		}
		printf("%"PRIu64"\n", nb_vectors);
	}

	//write results in file
	for (int i=0; i<ALPH_SIZE+1; ++i){
		fprintf(ofp, "nombre de kmers ayant %d chevauchements d'un côté : %"PRIu64".\n", i, overlaps_table[i]);
	}
	fprintf(ofp, "nombre total de chevauchements : %"PRIu64".\n", nb_total_overlaps);

}

int main(int argc, char const *argv[])
{
	
	if (argc != 3){
		usage();
		return EXIT_FAILURE;
	}

	FILE* ifp = fopen(argv[1], "r");
	FILE* ofp = fopen(argv[2], "w");

	count_overlaps(ifp, ofp);

	return 0;
}