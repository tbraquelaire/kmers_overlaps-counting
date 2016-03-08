#include "count_overlaps.h"

#define ALPH_SIZE 8

void usage(){
	printf("require an ordered binary file\n");
}

void count_overlaps(FILE* ifp){

	//Initiate variables

	uint64_t bit_vector1 = 0;
	uint64_t bit_vector2 = 0;
	uint64_t new_bit_vector = 0;
	
	uint64_t kmer1 = 0;
	uint64_t kmer2 = 0;
	uint64_t pos1 = 0;
	uint64_t pos2 = 0;
	uint64_t letter1 = 0;
	uint64_t letter2 = 0;
	uint64_t last_letter = 0;
	//uint64_t count_overlaps1 = 0;
	//uint64_t count_overlaps2 = 0;

	uint64_t count0 = 0;
	uint64_t count1 = 0;

	//uint64_t mask_count_overlaps = pow(2, 5)-1;
	uint64_t mask_letter = (((uint64_t) 1 << 4)-1) << 5;
	uint64_t mask_pos = pow(2, 9);
	uint64_t mask_kmer = (((uint64_t) 1 << 54)-1) << 10;

	//printf("mask_count_overlaps : %"PRIu64"	mask_letter : %"PRIu64"	mask_pos : %"PRIu64" mask_kmer : %lx\n", 
	//	mask_count_overlaps, mask_letter, mask_pos, mask_kmer);

	bool read1 = true;
	bool read2 = true;

	long cursor_save_begining = ftell(ifp);
	long cursor_save_end;
	long cursor_precedent_position;

	//read first two kmers
	read1 = (fread(&bit_vector1, sizeof(uint64_t), 1, ifp) == 1);
	read2 = (fread(&bit_vector2, sizeof(uint64_t), 1, ifp) == 1); 

	printf("\n");

	//while  end of file is not reached
	while (read1 && read2){
	//	printf("bit_vector1 %"PRIu64"\n", bit_vector1);
		kmer1 = bit_vector1 & mask_kmer;
	//	printf("kmer1 %"PRIu64"\n", kmer1);
		pos1 = (bit_vector1 & mask_pos) != 0;
	//	printf("pos1 %"PRIu64"\n", pos1);
		letter1 = bit_vector1 & mask_letter;
	//	printf("letter1 %"PRIu64"\n", letter1);
	//	count_overlaps1 = bit_vector1 & mask_count_overlaps;
	//	printf("count_overlaps1 %"PRIu64"\n", count_overlaps1);	

	//	printf("bit_vector2 %"PRIu64"\n", bit_vector2);
		kmer2 = bit_vector2 & mask_kmer;
	//	printf("kmer2 %"PRIu64"\n", kmer2);
		pos2 = (bit_vector2 & mask_pos) != 0;
	//	printf("pos2 %"PRIu64"\n", pos2);
		letter2 = bit_vector2 & mask_letter;
	//	printf("letter2 %"PRIu64"\n", letter2);

		last_letter = letter1;

		//take into account first overlap
		if (kmer1 == kmer2){
			if (pos1 == 0){
				count0 = 1;
			}

			//count number of unique kmers with same end
			//result is in count0
			while (kmer1 == kmer2 && pos2 == 0 && read2){
				
				if (letter2 != last_letter){
					count0++;
					last_letter = letter2;
				}
				//TODO  : else suppress the int64 from the file
			//	printf("bit_vector2 %"PRIu64"\n", bit_vector2);
				read2 = (fread(&bit_vector2, sizeof(uint64_t), 1, ifp) == 1);
				kmer2 = bit_vector2 & mask_kmer;
			//	printf("kmer2 %"PRIu64"\n", kmer2);
				pos2 = (bit_vector2 & mask_pos) != 0;
			//	printf("pos2 %"PRIu64"\n", pos2);
				letter2 = bit_vector2 & mask_letter;
			//	printf("letter2 %"PRIu64"\n", letter2);
				
			}

			//count number of unique kmers with same start
			//and overlaping with the precedent read kmers
			if (kmer1 == kmer2 && pos2 == 1){
				//first count the last read
				count1 = 1;
				last_letter = letter2;
			}

			//then read the file until no more kmers overlaps with the 
			//precedent kmers

			//printf("bit_vector2 %"PRIu64"\n", bit_vector2);
			read2 = (fread(&bit_vector2, sizeof(uint64_t), 1, ifp) == 1);
			kmer2 = bit_vector2 & mask_kmer;
			//printf("kmer2 %"PRIu64"\n", kmer2);
			pos2 = (bit_vector2 & mask_pos) != 0;
			//printf("pos2 %"PRIu64"\n", pos2);
			letter2 = bit_vector2 & mask_letter;
			//printf("letter2 %"PRIu64"\n", letter2);

			while (kmer1 == kmer2 && pos2 == 1 && read2){

				if (letter2 != last_letter){
				count1++;
				last_letter = letter2;
				}

			//	printf("bit_vector2 %"PRIu64"\n", bit_vector2);
				read2 = (fread(&bit_vector2, sizeof(uint64_t), 1, ifp) == 1);
				kmer2 = bit_vector2 & mask_kmer;
			//	printf("kmer2 %"PRIu64"\n", kmer2);
				pos2 = (bit_vector2 & mask_pos) != 0;
			//	printf("pos2 %"PRIu64"\n", pos2);
				letter2 = bit_vector2 & mask_letter;
			//	printf("letter2 %"PRIu64"\n", letter2);
			}
		}
		cursor_save_end = ftell(ifp);
		
		//register the overlaps ont the bit-vectors's appropriate field
		//#1

		fseek(ifp, cursor_save_begining, SEEK_SET);
		
		//kmer2 = bit_vector2 & mask_kmer;
		//printf("kmer2 %"PRIu64"\n", kmer2);
		//pos2 = bit_vector2 & mask_pos;
		//printf("pos2 %"PRIu64"\n", pos2);
		//letter2 = bit_vector2 & mask_letter;
		//printf("letter2 %"PRIu64"\n", letter2);
		//count_overlaps2 = bit_vector2 & mask_count_overlaps;
		//printf("count_overlaps2 %"PRIu64"\n", count_overlaps2);		

		//TODO :
		//if letter 2 already encountered, may delete the intger...

		//printf("writing counts\n");
		cursor_precedent_position = ftell(ifp);
		read2 = (fread(&bit_vector2, sizeof(uint64_t), 1, ifp) == 1);
		kmer2 = (bit_vector2 & mask_kmer);
		while (kmer1 == kmer2 && read2){
			pos2 = (bit_vector2 & mask_pos) != 0;

			if (pos2 == 0){
				new_bit_vector = bit_vector2 | count1;
			}
			else{
				new_bit_vector = bit_vector2 | count0;
			}

			fseek(ifp, cursor_precedent_position, SEEK_SET);
			fwrite(&new_bit_vector, sizeof(uint64_t), 1, ifp);

			cursor_precedent_position = ftell(ifp);
			read2 = (fread(&bit_vector2, sizeof(uint64_t), 1, ifp) == 1);
			kmer2 = (bit_vector2 & mask_kmer);
		}


		//inititiate the variables for the next pool of kmers
		fseek(ifp, cursor_precedent_position, SEEK_SET);
		cursor_save_begining = ftell(ifp);
		read1 = (fread(&bit_vector1, sizeof(uint64_t), 1, ifp) == 1);
		read2 = (fread(&bit_vector2, sizeof(uint64_t), 1, ifp) == 1);
		//printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

		//end of #1

		count0 = 0;
		count1 = 0;

	}

	//end of file reached, save lasts counting
	// TODO remove duplication of code (same as #1)
	//register the overlaps ont the bit-vectors's appropriate field
	//count_overlaps1 = count1;
cursor_save_end = ftell(ifp);
		
		//register the overlaps ont the bit-vectors's appropriate field
		//#1

		fseek(ifp, cursor_save_begining, SEEK_SET);
		
		//kmer2 = bit_vector2 & mask_kmer;
		//printf("kmer2 %"PRIu64"\n", kmer2);
		//pos2 = bit_vector2 & mask_pos;
		//printf("pos2 %"PRIu64"\n", pos2);
		//letter2 = bit_vector2 & mask_letter;
		//printf("letter2 %"PRIu64"\n", letter2);
		//count_overlaps2 = bit_vector2 & mask_count_overlaps;
		//printf("count_overlaps2 %"PRIu64"\n", count_overlaps2);		

		//TODO :
		//if letter 2 already encountered, may delete the intger...

		//printf("writing counts\n");
		while ((cursor_precedent_position = ftell(ifp)) != cursor_save_end){
			read2 = (fread(&bit_vector2, sizeof(uint64_t), 1, ifp) == 1);

			if (pos2 == 0){
				new_bit_vector = bit_vector2 | count1;
			}
			else{
				new_bit_vector = bit_vector2 | count0;
			}

			fseek(ifp, cursor_precedent_position, SEEK_SET);
			fwrite(&new_bit_vector, sizeof(uint64_t), 1, ifp);
		}

}



int main(int argc, char const *argv[])
{
	if (argc != 2){
		usage();
		return EXIT_FAILURE;
	}

	FILE* ifp = fopen(argv[1], "r+");


	count_overlaps(ifp);

	return 0;
}