#include "naive_method.h"

#define KMER_SIZE 200

struct list{
	char kmer[KMER_SIZE+1];
	struct list* next;
};
typedef struct list * List;

void print_list(List l){
	printf("kmers : \n");
	while (l != NULL){
		printf("%s\n", l->kmer);
		l = l->next;
	}
}

List create_list(){
	List l = malloc(sizeof(struct list));
	l->kmer[0] = '\0';
	l->next = NULL;

	return l;
}

void destroy_list(List l){
	if (l->next != NULL){
		destroy_list(l->next);
	}
	free(l);
}

void add(struct list* l, char* kmer, int kmer_size){
	//printf("adding kmer : %s\n", kmer);
	if (l->kmer[0] == '\0'){
		strcpy(l->kmer, kmer);
		return;
	}
	List old_l = NULL;
	while ((l != NULL) && (strcmp(l->kmer, kmer) < 0)){
		old_l = l;
		l = l->next;
	}
	if ((l != NULL) && (strcmp(l->kmer, kmer) == 0)){
		return;
	}
	else{
		List new_l = create_list();
		if ((l != NULL) && (strcmp(l->kmer, kmer) > 0)){
			memset(new_l->kmer, '\0', kmer_size+1);
			strcpy(new_l->kmer, l->kmer);
			strcpy(l->kmer, kmer);
			new_l->next = l->next;
			l->next = new_l;
		}
		else{
			strcpy(new_l->kmer, kmer);
			if (old_l != NULL){
				old_l->next = new_l;
			}
			new_l->next = l;
		}
	}

}


void usage(){
	fprintf(stderr, "require an input (FASTA), kmer size, nb max of overlaps and an output\n");
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

struct list* create_kmer_list(FILE* ifp, int kmer_size, int ifp_nb_reads, int read_nb_lines){

	char kmer1[kmer_size+1];
	memset(&kmer1, '\0', kmer_size+1);
	char kmer2[kmer_size+1];
	memset(&kmer2, '\0', kmer_size+1);
	int result_kmer_size = 0; 
	char result_kmer[2*kmer_size+1];
	memset(result_kmer, '\0', 2*kmer_size+1);
	//uint64_t bit_vector;
	char ch;

	struct list* kmer_list = create_list();

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

	int nb_lines = 0;

	char ch1;
	char ch2;

	for (int reads_done=0; reads_done+2 <= ifp_nb_reads; reads_done+=2){
//		printf("entering while, bit_multiplex_kmer : %"PRIu64"\n", bit_multiplex_kmer);
		nb_lines = 0;

		result_kmer_size = 0;
		while((ch1 = fgetc(ifp)) !=  EOF && nb_lines < read_nb_lines ){
			//printf("ch1 : %c\n", ch1);

			if (ch1 == '\n'){
				++nb_lines;
				//printf("++cursor1\n");
				++read1_cursor;
				//printf("++cursor2\n");
				++read2_cursor;

			}
			/*build the kmer and :
			**if n encountered -> re-init kmer size to 0
			**else -> write kmer when completed
			*/
			else if (result_kmer_size < kmer_size){
//				printf("filling bit_kmer\n");
//				printf("ch1 : %c\n", ch1);

				if (ch1 == 'N' || ch1 == 'n'){
					result_kmer_size = 0;
					//printf("++cursor1\n");
					++read1_cursor;
					//				printf("++cursor2\n");
					++read2_cursor;
				}

				else{
					fseek(ifp, read2_cursor, SEEK_SET);
					ch2 = fgetc(ifp);
					//printf("ch2 : %c\n", ch2);
//					printf("ch2 : %c\n", ch2);
					//printf("++cursor1\n");
					++read1_cursor;
				//					printf("++cursor2\n");
					++read2_cursor;
					fseek(ifp, read1_cursor, SEEK_SET);

					if (ch2 == 'N' || ch2 == 'n'){
						result_kmer_size = 0;
					}

					else{
						result_kmer[2*result_kmer_size] = ch1;
						result_kmer[2*result_kmer_size+1] = ch2;
						
						++result_kmer_size;

//						printf("bit_multiplex_char : %"PRIu64"; bit_multiplex_kmer : %"PRIu64"\n",
//						bit_multiplex_char, bit_multiplex_kmer );

						//k-mer completed : write it 
						if (result_kmer_size == kmer_size){
//							
							/*
							char kmer_left_part[kmer_size*2-1];
							memset(kmer_left_part, '\0', kmer_size*2-1);
							strncpy(kmer_left_part, result_kmer, kmer_size*2-2);
							char kmer_right_part[kmer_size*2-1];
							memset(kmer_right_part, '\0', kmer_size*2-1);
							strncpy(kmer_right_part, &result_kmer[2], kmer_size*2-2);
							*/
							add(kmer_list, result_kmer, kmer_size);
							//printf("print_list :\n");
							//print_list(kmer_list);
							//printf("\n");
							/*
							add(kmer_list, kmer_right_part);
							printf("print_list :\n");
							print_list(kmer_list);
							printf("\n");*/
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
					//printf("ch2 : %c\n", ch2);
//					printf("ch2 : %c\n", ch2);
					//printf("++cursor1\n");
					++read1_cursor;
					//				printf("++cursor2\n");
					++read2_cursor;
					fseek(ifp, read1_cursor, SEEK_SET);

					if (ch2 != 'N' && ch2 != 'n'){
						char tmp_result_kmer[2*kmer_size+1];
						memset(tmp_result_kmer, '\0', 2*kmer_size+1);
						strncpy(tmp_result_kmer, &result_kmer[2], kmer_size*2-2);
						tmp_result_kmer[2*kmer_size-2] = ch1;
						tmp_result_kmer[2*kmer_size-1] = ch2;
						strcpy(result_kmer, tmp_result_kmer);
						
						//++result_kmer_size;

//						printf("bit_multiplex_char : %"PRIu64"; bit_multiplex_kmer : %"PRIu64"\n",
//						bit_multiplex_char, bit_multiplex_kmer );

						//k-mer completed : write it 
						if (result_kmer_size == kmer_size){
//							
							/*
							char kmer_left_part[kmer_size-1];
							memset(kmer_left_part, '\0', kmer_size*2-1);
							strncpy(kmer_left_part, result_kmer, kmer_size*2-2);
							char kmer_right_part[kmer_size-1];
							memset(kmer_right_part, '\0', kmer_size*2-1);
							strncpy(kmer_right_part, &result_kmer[2], kmer_size*2-2);
							*/
							add(kmer_list, result_kmer, kmer_size);
							//printf("print_list :\n");
							//print_list(kmer_list);
							//printf("\n");
							/*
							add(kmer_list, kmer_right_part);
							printf("print_list :\n");
							print_list(kmer_list);
							printf("\n");
							*/
						}

						else{

							result_kmer_size = 0;

						}
					}

					else{
						//printf("++cursor1\n");
						//++read1_cursor;
						//				printf("++cursor2\n");
						//++read2_cursor;
						result_kmer_size = 0;
					}

				}
				else{
					//printf("++cursor1\n");
					++read1_cursor;
					//				printf("++cursor2\n");
					++read2_cursor;
					result_kmer_size = 0;
				}
			}

		}

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
	return kmer_list;
}

void count_overlaps(List kmer_list, int kmer_size, int nb_max_overlaps, FILE* ofp){

	int nb_left_overlaps = 0;
	int nb_right_overlaps = 0;

	List head_kmer_list = kmer_list;
	List current_cursor = kmer_list;

	uint64_t* tab_nb_overlaps = malloc(sizeof(*tab_nb_overlaps)*(nb_max_overlaps+1));
	memset (tab_nb_overlaps, 0, (nb_max_overlaps+1)*(sizeof(*tab_nb_overlaps)));


	while(current_cursor != NULL){

		char kmer_left_part[kmer_size*2-1];
		memset(kmer_left_part, '\0', kmer_size*2-1);
		strncpy(kmer_left_part, current_cursor->kmer, kmer_size*2-2);
		char kmer_right_part[kmer_size*2-1];
		memset(kmer_right_part, '\0', kmer_size*2-1);
		strncpy(kmer_right_part, &current_cursor->kmer[2], kmer_size*2-2);

		//counting left overlaps
		kmer_list = head_kmer_list;
		while (kmer_list != NULL){

			if (strncmp(kmer_list->kmer, kmer_right_part, kmer_size*2-2) == 0){
				++nb_left_overlaps;
			}
			kmer_list = kmer_list->next;		
		}

		//counting right overlaps
		kmer_list = head_kmer_list;
		while (kmer_list != NULL){

			if (strncmp(&kmer_list->kmer[2], kmer_left_part, kmer_size*2-2) == 0){
				++nb_right_overlaps;
			}
			kmer_list = kmer_list->next;		
		}

		//printf("nb_right_overlaps : %d nb_left_overlaps : %d\n", nb_right_overlaps, nb_left_overlaps);
		++tab_nb_overlaps[nb_left_overlaps];
		++tab_nb_overlaps[nb_right_overlaps];

		nb_left_overlaps = 0;
		nb_right_overlaps = 0;

		current_cursor = current_cursor->next;
	}

	for (int actual_nb_overlaps = 0; actual_nb_overlaps <= nb_max_overlaps; ++actual_nb_overlaps){
		fprintf(ofp, "nb of kmers with %d overlaps : %"PRIu64"\n", 
			actual_nb_overlaps, 
			tab_nb_overlaps[actual_nb_overlaps]);
	}

	free(tab_nb_overlaps);

}


int main(int argc, char const *argv[])
{
	
	if (argc != 5){
		usage();
		return EXIT_FAILURE;
	}

	FILE* ifp = fopen(argv[1], "r");
	int kmer_size = atoi(argv[2]);

	if (kmer_size < 1){
		printf("kmer_size must be >= 1\n");
		return EXIT_FAILURE;
	}

	int nb_max_overlaps = atoi(argv[3]);

	FILE* ofp = fopen(argv[4], "w");

	int ifp_nb_reads = get_file_nb_reads(ifp);
	/* danger : var can change /!\/!\ */
	int read_nb_lines = 2;

	List kmer_list = create_kmer_list(ifp, kmer_size, ifp_nb_reads, read_nb_lines);

	count_overlaps(kmer_list, kmer_size, nb_max_overlaps, ofp);

	fclose(ofp);

	return EXIT_SUCCESS;
}