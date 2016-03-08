#include "search_kmer_overlaps.h"


#define ALPH_SIZE 4
#define A 0
#define C 1
#define G 2
#define T 3

//TODO
//compute string length, so don't have to give it in parameters
void usage(){
	printf("require a file (sorted and counted) and a kmer (chars) and its length (int)\n");
}

uint64_t translate_kmer_to_uint64(char* char_kmer, int length){

	int ch1;
	int ch2;
	uint64_t uint64_kmer = 0;
	int pos = 0;

	for (int i=0; i < length; ++i){
		ch1 = char_kmer[2*i];
		ch2 = char_kmer[(2*i)+1];

		if (ch1 == 'A' || ch1 == 'a'){
			ch1 = A;
		}
		if (ch1 == 'C' || ch1 == 'c'){
			ch1 = C;
		}
		if (ch1 == 'G' || ch1 == 'g'){
			ch1 = G;
		}
		if (ch1 == 'T' || ch1 == 't'){
			ch1 = T;
		}

		if (ch2 == 'A' || ch2 == 'a'){
			ch2 = A;
		}
		if (ch2 == 'C' || ch2 == 'c'){
			ch2 = C;
		}
		if (ch2 == 'G' || ch2 == 'g'){
			ch2 = G;
		}
		if (ch2 == 'T' || ch2 == 't'){
			ch2 = T;
		}

	//	if (power != 0){
		printf("ch1 : %d; ch2 : %d\n", ch1, ch2);
		printf("décalage ch1 : %"PRIu64"; décalage ch2 : %"PRIu64"\n", 
			(uint64_t) ch1 << ((4 * (length-pos-1))+2), (uint64_t) ch2 << 4 * (length-pos-1));
		uint64_kmer += (uint64_t) ch1 << ((4 * (length-pos-1))+2);
		uint64_kmer += (uint64_t) ch2 << 4 * (length-pos-1);
		++pos; 
		printf("pos : %d, kmer :%"PRIu64"\n", pos, uint64_kmer);
	//	}
	//	else{
	//		kmer += ch;
	//	}
	}

	return uint64_kmer;
}

int dicho_search(FILE* ifp, long bound_inf, long bound_sup, uint64_t kmer_searched, uint64_t pos_letter_searched){

	uint64_t bit_vector = 0;
	long ptr = ((bound_sup+bound_inf) >> 1);
	fseek(ifp, (ptr*sizeof(uint64_t)), SEEK_SET);
	fread(&bit_vector, sizeof(uint64_t), 1, ifp);

	uint64_t kmer = bit_vector >> 10;
	uint64_t pos_letter = (bit_vector << 54) >> 59;

	printf("kmer searched : %"PRIu64"\n", kmer_searched);
	printf("pos_letter searched : %"PRIu64"\n", pos_letter_searched);
	printf("searching kmer :\n");

	while (kmer != kmer_searched && bound_inf <= bound_sup){

		if (kmer < kmer_searched){
			bound_inf = ptr+1;
		}
		else{
			bound_sup = ptr-1;
		}

		ptr = ((bound_sup+bound_inf) >> 1);
		fseek(ifp, (ptr*sizeof(uint64_t)), SEEK_SET);
		fread(&bit_vector, sizeof(uint64_t), 1, ifp);
		kmer = bit_vector >> 10;
		pos_letter = (bit_vector << 54) >> 59;
		printf("ptr : %lu \n",ptr);

	}
	printf("last kmer :%"PRIu64"\n", kmer);
	printf("pos_letter :%"PRIu64"\n", pos_letter);

	printf("searching pos_letter :\n");

	//need to actualize ptr if we have to read the file at reverse
	//ptr = ftell(ifp);
	if (kmer == kmer_searched && pos_letter != pos_letter_searched){
		printf("ptr : %lu\n", ptr);
		if (pos_letter < pos_letter_searched && pos_letter != pos_letter_searched){
			printf("%s\n", "pos_letter < pos_letter_searched");
			while (kmer == kmer_searched && pos_letter != pos_letter_searched){
				printf("%s\n", "kmer == kmer_searched");
				if (fread(&bit_vector, sizeof(uint64_t), 1, ifp)){
					kmer = bit_vector >> 10;
					pos_letter = (bit_vector << 54) >> 59;
					printf("kmer : %"PRIu64" pos_letter : %"PRIu64" ptr : %"PRIu64"\n", 
						kmer, pos_letter, ptr);
				}
				else{
					break;
				}
			}
		}
		else{
			printf("%s\n", "pos_letter > pos_letter_searched");
			//fseek(ifp, ((ptr-1)*sizeof(uint64_t)), SEEK_SET);
			ptr -= 1;
			fseek(ifp, ((ptr)*sizeof(uint64_t)), SEEK_SET);
			while (kmer == kmer_searched && pos_letter != pos_letter_searched){
				printf("%s\n", "kmer == kmer_searched");
				if (fread(&bit_vector, sizeof(uint64_t), 1, ifp)){
					kmer = bit_vector >> 10;
					pos_letter = (bit_vector << 54) >> 59;
					ptr -= 1;
					printf("kmer : %"PRIu64" pos_letter : %"PRIu64" ptr : %"PRIu64"\n", 
						kmer, pos_letter, ptr);
					fseek(ifp, ((ptr/*-2*/)*sizeof(uint64_t)), SEEK_SET);
				}
				else{
					break;
				}
			}
		}
	}
	printf("last pos_letter :%"PRIu64"\n", pos_letter);

	if (kmer == kmer_searched && pos_letter == pos_letter_searched){
		int nb_overlaps = (int) bit_vector & ((int) ((1<<5)-1));
		return nb_overlaps;
	}

	else{
		return -1;
	}
	
}

int search_kmer_overlaps(FILE* ifp, uint64_t kmer, uint64_t letter, int pos){

	long bound_inf = 0;
	fseek(ifp, 0, SEEK_END);
	long bound_sup = ftell(ifp)/sizeof(uint64_t);

	uint64_t pos_letter = ((uint64_t) pos << 4) | letter;
	printf("POSLETTER %"PRIu64"\n", pos_letter);

	return dicho_search(ifp, bound_inf, bound_sup, kmer, pos_letter);

}

int main(int argc, char const *argv[])
{

	if (argc != 4){
		usage();
		return EXIT_FAILURE;
	}
	
	FILE* ifp = fopen(argv[1], "r");

	int length = atoi(argv[3]);
	char kmer[length+1];
	kmer[0] = '\0';
	strcpy(kmer, argv[2]);

	char kmer_left_part_char[length];
	char right_letter[2];
	strncpy(kmer_left_part_char, kmer, length-1);
	kmer_left_part_char[length-1] = '\0';
	strncpy(right_letter, &kmer[length-1], 1);
	right_letter[1] = '\0';

	char kmer_right_part_char[length];
	char left_letter[2];
	strncpy(kmer_right_part_char, &kmer[1], length-1);
	kmer_right_part_char[length-1] = '\0';
	strncpy(left_letter, kmer, 1);
	left_letter[1] = '\0';

	printf("kmer_left_part_char : %s\n", kmer_left_part_char);
	printf("kmer_right_part_char : %s\n", kmer_right_part_char);

	uint64_t kmer_left_part_uint64 = translate_kmer_to_uint64(kmer_left_part_char, length-1);
	uint64_t kmer_right_part_uint64 = translate_kmer_to_uint64(kmer_right_part_char, length-1);
	uint64_t right_letter_uint64 = translate_kmer_to_uint64(right_letter, 1);
	uint64_t left_letter_uint_64 = translate_kmer_to_uint64(left_letter, 1);

	printf("kmer_left_part_uint64 : %"PRIu64"\n", kmer_left_part_uint64);
	printf("kmer_right_part_uint64 : %"PRIu64"\n", kmer_right_part_uint64);

	printf("\nkmer left :\n");
	int nb_left_overlaps = search_kmer_overlaps(ifp, kmer_left_part_uint64, right_letter_uint64, 0);
	printf("\nkmer right :\n");
	int nb_right_overlaps = search_kmer_overlaps(ifp, kmer_right_part_uint64, left_letter_uint64, 1);

	printf("nombre de chavauchement à gauche  %d\n", nb_left_overlaps);
	printf("nombre de chavauchement à droite  %d\n", nb_right_overlaps);

	return EXIT_SUCCESS;
}