#include "create_bin.h"

//#define KMER_SIZE 5
#define ALPH_SIZE 4
#define A 0
#define C 1
#define G 2
#define T 3

//char kmer1[KMER_SIZE+1];
//char kmer2[KMER_SIZE+1];
void usage(){
	fprintf(stderr, "require an input (FASTA), kmer size and an output\n");
}

int get_file_nb_reads(FILE* file){
	fseek(file, 0, SEEK_SET);

	int nb_reads = 0;



	bool new_line = true;
	bool in_read = false;
	char ch;
	while ((ch = fgetc(file))!= EOF ){
		if(new_line){

			if (ch == '>'){
				in_read = false;
			}

			else{
				if(!in_read){
					nb_reads++;
					in_read = true;
				}

			}
			new_line = false;
		}
		if(ch == '\n'){
			new_line = true;
		}
	}

	fseek(file, 0, SEEK_SET);
	return nb_reads;
}

uint64_t multiplex(char ch1, char ch2){
	uint64_t res;
	int i1;
	int i2;
	
	if (ch1 == 'A' || ch1 =='a'){
		i1 = A;
	}
	else if (ch1 == 'C' || ch1 =='c'){
		i1 = C;
	}
	else if (ch1 == 'G' || ch1 =='g'){
		i1 = G;
	}
	else if (ch1 == 'T' || ch1 =='t'){
		i1 = T;
	}

	if (ch2 == 'A' || ch1 =='a'){
		i2 = A;
	}
	else if (ch2 == 'C' || ch1 =='c'){
		i2 = C;
	}
	else if (ch2 == 'G' || ch1 =='g'){
		i2 = G;
	}
	else if (ch2 == 'T' || ch1 =='t'){
		i2 = T;
	}

	res = i1 + (i2*ALPH_SIZE);
	return res;
}
/*
void fill_buffers(FILE* ifp, long read1_cursor, long read2_cursor, int kmer_size, int buffer_cursor){
	char ch1;
	char ch2;

	while((ch1 = fgetc(ifp)) != '\n' && (ch1 != EOF) && (buffer_cursor < kmer_size)){

		if (ch1 == 'N'){
			memset(&kmer1, '\0', kmer_size+1);
			memset(&kmer2, '\0', kmer_size+1);
			buffer_cursor = 0;
			++read1_cursor;
			++read2_cursor;
		}

		else{
			fseek(ifp, read2_cursor, SEEK_SET);
			ch2 = fgetc(ifp);
			++read1_cursor;
			++read2_cursor;
			fseek(ifp, read1_cursor, SEEK_SET);

			if (ch2 == 'N'){
				memset(&kmer1, '\0', kmer_size+1);
				memset(&kmer2, '\0', kmer_size+1);
				buffer_cursor = 0;
			}

			else{
				kmer1[buffer_cursor] = ch1;
				kmer2[buffer_cursor] = ch2;
				++buffer_cursor;
			}
		}

	}
}*/

/*create bin_vectors of 64 bits such as :
0-58th bits : k-1-mer
59-62th bits : truncated letter
63th bit : right or left part of kmer
bit_vector, (bit_vector>>5), ((bit_vector<<59)>>60), ((bit_vector<<63)>>63));
*/
void create_bin(FILE* ifp, FILE* ofp, int kmer_size, int ifp_nb_reads){

	char kmer1[kmer_size+1];
	memset(&kmer1, '\0', kmer_size+1);
	char kmer2[kmer_size+1];
	memset(&kmer2, '\0', kmer_size+1);
	uint64_t bit_multiplex_kmer;
	//uint64_t bit_vector;
	char ch;

	//get to first read
	while((ch = fgetc(ifp)) != '\n' && ch != EOF){
	}


	//place cursors ; evaluate gap between reads, header length
	long read1_cursor = ftell(ifp);
	long read2_cursor; //ftell, fseek
	int reads_gap = 0;
	int header_length = 0;
	bool in_header = false;
	bool searching_next_read = true;
	while((ch = fgetc(ifp)) != EOF && searching_next_read){
		if (in_header){

			if (ch == '\n'){
				read2_cursor = ftell(ifp);
				searching_next_read = false;
			}
			++header_length;
		}
		if (!in_header && (ch == '>')){
			in_header = true;
		}

		++reads_gap;
	}
	fseek(ifp, read1_cursor, SEEK_SET);
	printf("read gap : %d\n", reads_gap);
	printf("header length : %d\n", header_length);
	printf("read1 cursor : %lu\n", read1_cursor);
	printf("read2 cursor : %lu\n", read2_cursor);

	//initialize kmers buffers
//	int buffer_cursor = 0;
	//fill_buffers(ifp, read1_cursor, read2_cursor, kmer_size, buffer_cursor);
	char ch1;
	char ch2;
	/* REMOVE DUPLICATE*/
/*	while((ch1 = fgetc(ifp)) != '\n' && (ch1 != EOF) && (buffer_cursor < kmer_size)){

		if (ch1 == 'N' || ch1 == 'n'){
			memset(&kmer1, '\0', kmer_size+1);
			memset(&kmer2, '\0', kmer_size+1);
			buffer_cursor = 0;
			++read1_cursor;
			++read2_cursor;
		}

		else{
			fseek(ifp, read2_cursor, SEEK_SET);
			ch2 = fgetc(ifp);
			++read1_cursor;
			++read2_cursor;
			fseek(ifp, read1_cursor, SEEK_SET);

			if (ch2 == 'N' || ch2 == 'n'){
				memset(&kmer1, '\0', kmer_size+1);
				memset(&kmer2, '\0', kmer_size+1);
				buffer_cursor = 0;
			}

			else{
				kmer1[buffer_cursor] = ch1;
				kmer2[buffer_cursor] = ch2;
				++buffer_cursor;
			}
		}

	}
*/
	printf("%s\n", kmer1);
	printf("%s\n", kmer2);

	//multiplex first kmer
//	buffer_cursor = 0;
	bit_multiplex_kmer = 0;
	uint64_t bit_multiplex_char;
/*	while (buffer_cursor < kmer_size){
		bit_multiplex_char = multiplex(kmer1[buffer_cursor], kmer2[buffer_cursor]);
		bit_multiplex_kmer <<= 4;
		bit_multiplex_kmer = bit_multiplex_kmer | bit_multiplex_char;
		printf("ch1 : %c; ch2 : %c; bit_multiplex_char : %"PRIu64"; bit_multiplex_kmer : %"PRIu64"\n",
		kmer1[buffer_cursor], kmer2[buffer_cursor], bit_multiplex_char, bit_multiplex_kmer );

		++buffer_cursor;
	}
*/
	//write first kmer infos in output file
	uint64_t bit_vector = 0;
	uint64_t mask_first_letter = (((uint64_t) 1) << (4*(kmer_size)))-1;
	uint64_t mask_last_letter = (((uint64_t) 1) << 4)-1;
	uint64_t mask_end = (((uint64_t) 1) << (4*(kmer_size-1)))-1;
	uint64_t mask_multplex_kmer = (((uint64_t) 1) << (4*(kmer_size)))-1;

	uint64_t kmer_start = bit_multiplex_kmer >> 4;
	uint64_t kmer_end = bit_multiplex_kmer & mask_end;
	uint64_t first_letter = (bit_multiplex_kmer & mask_first_letter)  >> (4*(kmer_size-1));
	uint64_t last_letter = bit_multiplex_kmer & mask_last_letter;
/*
	printf("kmer-start : %"PRIu64"; kmer_end : %"PRIu64"; mask_end : %"PRIu64"\n", kmer_start, kmer_end, mask_end);
	printf("first_letter : %"PRIu64"; mask_first_letter : %"PRIu64"\n", first_letter, mask_first_letter);
	printf("last_letter : %"PRIu64"; mask_last_letter : %"PRIu64"\n", last_letter, mask_last_letter);


	bit_vector = (kmer_start << 5) | (last_letter << 1);
	fwrite(&bit_vector, sizeof(uint64_t), 1, ofp);
	printf("begin bitvector : %"PRIu64"; part1 : %"PRIu64"; part2 : %"PRIu64"; part3 : %"PRIu64"\n", 
		bit_vector, (bit_vector>>5), ((bit_vector<<59)>>60), ((bit_vector<<63)>>63));

	bit_vector = (kmer_end << 5) | (first_letter << 1) | 1;
	fwrite(&bit_vector, sizeof(uint64_t), 1, ofp);
	printf("end bitvector : %"PRIu64"; part1 : %"PRIu64"; part2 : %"PRIu64"; part3 : %"PRIu64"\n\n", 
		bit_vector, (bit_vector>>5), ((bit_vector<<59)>>60), ((bit_vector<<63)>>63));

*/
	//do the same until the end of the read
	int multiplex_kmer_size = 0;
	int read_nb_lines = 2;
	int nb_lines;

	for (int reads_done=0; reads_done+2 <= ifp_nb_reads; reads_done+=2){
//		printf("entering while, bit_multiplex_kmer : %"PRIu64"\n", bit_multiplex_kmer);
		nb_lines = 0;
		bit_multiplex_kmer = 0; //TODO essaie debug !!
		multiplex_kmer_size = 0;
		while((ch1 = fgetc(ifp)) !=  EOF && nb_lines < read_nb_lines ){

			if (ch1 == '\n'){
				++nb_lines;
				++read1_cursor;
				++read2_cursor;

			}
			/*build the kmer and :
			**if n encountered -> re-init kmer size to 0
			**else -> write kmer when completed
			*/
			else if (multiplex_kmer_size < kmer_size){
//				printf("filling bit_kmer\n");
//				printf("ch1 : %c\n", ch1);

				if (ch1 == 'N' || ch1 == 'n'){
					bit_multiplex_kmer = 0;
					multiplex_kmer_size = 0;
					++read1_cursor;
					++read2_cursor;
				}

				else{
					fseek(ifp, read2_cursor, SEEK_SET);
					ch2 = fgetc(ifp);
//					printf("ch2 : %c\n", ch2);
					++read1_cursor;
					++read2_cursor;
					fseek(ifp, read1_cursor, SEEK_SET);

					if (ch2 == 'N' || ch2 == 'n'){
						bit_multiplex_kmer = 0;
						multiplex_kmer_size = 0;
					}

					else{
						bit_multiplex_char = multiplex(ch1, ch2);
						bit_multiplex_kmer <<= 4;
						bit_multiplex_kmer = bit_multiplex_kmer | bit_multiplex_char;
						++multiplex_kmer_size;

//						printf("bit_multiplex_char : %"PRIu64"; bit_multiplex_kmer : %"PRIu64"\n",
//						bit_multiplex_char, bit_multiplex_kmer );

						//k-mer completed : write it 
						if (multiplex_kmer_size == kmer_size){
//							printf("\nBIT_MULTIPLEX_KMER JUST filled\n\n");
							kmer_start = bit_multiplex_kmer >> 4;
							kmer_end = bit_multiplex_kmer & mask_end;
							first_letter = (bit_multiplex_kmer & mask_first_letter)  >> (4*(kmer_size-1));
							last_letter = bit_multiplex_kmer & mask_last_letter;

//							printf("kmer-start : %"PRIu64"; kmer_end : %"PRIu64"; mask_end : %"PRIu64"\n", kmer_start, kmer_end, mask_end);
//							printf("first_letter : %"PRIu64"; mask_first_letter : %"PRIu64"\n", first_letter, mask_first_letter);
//							printf("last_letter : %"PRIu64"; mask_last_letter : %"PRIu64"\n", last_letter, mask_last_letter);

							bit_vector = (kmer_start << 10) | (last_letter << 5);
							fwrite(&bit_vector, sizeof(uint64_t), 1, ofp);
	//						printf("begin bitvector : %"PRIu64"; part1 : %"PRIu64"; part2 : %"PRIu64"; part3 : %"PRIu64"; part4 : %"PRIu64"\n", 
	//						bit_vector, (bit_vector>>10), ((bit_vector<<54)>>63), ((bit_vector<<55)>>60), ((bit_vector<<59)>>59));

							bit_vector = (kmer_end << 10) | 1 << 9 | (first_letter << 5);
							fwrite(&bit_vector, sizeof(uint64_t), 1, ofp);
	//						printf("begin bitvector : %"PRIu64"; part1 : %"PRIu64"; part2 : %"PRIu64"; part3 : %"PRIu64"; part4 : %"PRIu64"\n", 
	//						bit_vector, (bit_vector>>10), ((bit_vector<<54)>>63), ((bit_vector<<55)>>60), ((bit_vector<<59)>>59));

						}
					}
				}
			}

			/*
			**write k-mer until n encountered
			**when n is read, rebuild the kmer
			*/
			else{
//				printf("bit_kmer filled\n");
//				printf("ch1 : %c\n", ch1);

				if(ch1 != 'N' && ch1 != 'n'){
					fseek(ifp, read2_cursor, SEEK_SET);
					ch2 = fgetc(ifp);
//					printf("ch2 : %c\n", ch2);
					++read1_cursor;
					++read2_cursor;
					fseek(ifp, read1_cursor, SEEK_SET);

					if (ch2 != 'N' && ch2 != 'n'){
						bit_multiplex_char = multiplex(ch1, ch2);
						bit_multiplex_kmer <<= 4;
						bit_multiplex_kmer = (bit_multiplex_kmer | bit_multiplex_char) & mask_multplex_kmer;

//						printf("bit_multiplex_char : %"PRIu64"; bit_multiplex_kmer : %"PRIu64"\n",
//						bit_multiplex_char, bit_multiplex_kmer );

						kmer_start = bit_multiplex_kmer >> 4;
						kmer_end = bit_multiplex_kmer & mask_end;
						first_letter = (bit_multiplex_kmer & mask_first_letter)  >> (4*(kmer_size-1));
						last_letter = bit_multiplex_kmer & mask_last_letter;

//						printf("kmer-start : %"PRIu64"; kmer_end : %"PRIu64"; mask_end : %"PRIu64"\n", kmer_start, kmer_end, mask_end);
//						printf("first_letter : %"PRIu64"; mask_first_letter : %"PRIu64"\n", first_letter, mask_first_letter);
//						printf("last_letter : %"PRIu64"; mask_last_letter : %"PRIu64"\n", last_letter, mask_last_letter);

						bit_vector = (kmer_start << 10) | (last_letter << 5);
						fwrite(&bit_vector, sizeof(uint64_t), 1, ofp);
//						printf("begin bitvector : %"PRIu64"; part1 : %"PRIu64"; part2 : %"PRIu64"; part3 : %"PRIu64"; part4 : %"PRIu64"\n", 
//						bit_vector, (bit_vector>>10), ((bit_vector<<54)>>63), ((bit_vector<<55)>>60), ((bit_vector<<59)>>59));

						bit_vector = (kmer_end << 10) | 1 << 9 | (first_letter << 5);
						fwrite(&bit_vector, sizeof(uint64_t), 1, ofp);
//						printf("begin bitvector : %"PRIu64"; part1 : %"PRIu64"; part2 : %"PRIu64"; part3 : %"PRIu64"; part4 : %"PRIu64"\n", 
//						bit_vector, (bit_vector>>10), ((bit_vector<<54)>>63), ((bit_vector<<55)>>60), ((bit_vector<<59)>>59));

					}

					else{
//						printf("\nMUST FILL BIT_MULTIPLEX_KMER\n\n");
						bit_multiplex_kmer = 0;
						multiplex_kmer_size = 0;

					}
				}

				else{
//					printf("\nMUST FILL BIT_MULTIPLEX_KMER\n\n");
					++read1_cursor;
					++read2_cursor;
					bit_multiplex_kmer = 0;
					multiplex_kmer_size = 0;
				}
			}
		}
/*	printf("#####################################################\n");
	printf("#####################################################\n");
	printf("#####################################################\n");
	printf("#####################################################\n");
	printf("#####################################################\n");
	printf("#####################################################\n");
	printf("#####################################################\n");
	printf("#####################################################\n");
	printf("#####################################################\n");
	printf("#####################################################\n");
*/	

	//replace cursors
	nb_lines = 0;
//	printf("parcours : \n");
	while(nb_lines < read_nb_lines+2 && (ch = fgetc(ifp)) != EOF ){
//		printf("%c", ch);
		if (ch == '\n'){
			++nb_lines;
		}
	}

	read1_cursor = ftell(ifp);
	reads_gap = 0;
	header_length = 0;
	in_header = false;
	searching_next_read = true;
	while((ch = fgetc(ifp)) != EOF && searching_next_read){
		if (in_header){

			if (ch == '\n'){
				read2_cursor = ftell(ifp);
				searching_next_read = false;
			}
			++header_length;
		}
		if (!in_header && (ch == '>')){
			in_header = true;
		}

		++reads_gap;
	}
	fseek(ifp, read1_cursor, SEEK_SET);

	//	read1_cursor += header_length + reads_gap + 1;
	//	read2_cursor = read1_cursor + reads_gap;
	//	fseek(ifp, read1_cursor, SEEK_SET);

//		printf("%d reads done\n", reads_done);

//		printf("#####################################################\n");

	}	
}

int main(int argc, char const *argv[])
{
	
	if (argc != 4){
		usage();
		return EXIT_FAILURE;
	}

	FILE* ifp = fopen(argv[1],"r"); // read mode
	int kmer_size = atoi(argv[2]);
	FILE* ofp = fopen(argv[3], "w");

	int ifp_nb_reads = get_file_nb_reads(ifp);

	create_bin(ifp, ofp, kmer_size, ifp_nb_reads);

	return 0;
}