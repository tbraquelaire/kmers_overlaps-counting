#include "merge_bin_tree.h"

void usage(){
	printf("require an output and the number of files to merge\n");
}

/*
** return the tree's length
*/
int get_tree_length(int x){
	int res = 0;
	while (x > 1){
		x >>= 1;
		++res;
	}
	return res;
}

/*
** copy f2 in f1
*/
void copy_bin(FILE* f1, FILE* f2){

	uint64_t bit_vector;
	while (fread(&bit_vector, sizeof(uint64_t), 1, f2)){
		fwrite(&bit_vector, sizeof(uint64_t), 1, f1);
	}
}

/*
** write the name of the next file to merge int file_dest, following the current id
*/
void get_next_file_name_id(int current_id, char* file_dest, char* tmp_file_name){

	char id[1024];

	memset(file_dest, '\0', strlen(file_dest));
	memset(id, '\0', strlen(id));
	strcpy(file_dest, tmp_file_name);
	sprintf(id,"%d",current_id);
	strcat(file_dest, id);

}

// merge f1 with f2, result is in des

void merge(FILE* f1, FILE* f2, FILE* dest){
	uint64_t bit_vector1;
	uint64_t bit_vector2;

	//initialize variables and read begning of files
	bool end_of_f1 = (fread(&bit_vector1, sizeof(uint64_t), 1, f1)==0);
	bool end_of_f2 = (fread(&bit_vector2, sizeof(uint64_t), 1, f2)==0);

	uint64_t last_value_added;
	
	//while end of a file not reached
	while (!end_of_f1 && !end_of_f2){
		
		//write the more little bit vector
		if (bit_vector1 <= bit_vector2){
			if(bit_vector1 != last_value_added){
				fwrite(&bit_vector1, sizeof(uint64_t), 1, dest);
				last_value_added = bit_vector1;
			}
			//read next bit vector and check if end of file reached
			if (fread(&bit_vector1, sizeof(uint64_t), 1, f1)==0){
				end_of_f1 = true;
			}
		}

		else{
			if(bit_vector2 != last_value_added){
				fwrite(&bit_vector2, sizeof(uint64_t), 1, dest);
				last_value_added = bit_vector2;
			}
			if (fread(&bit_vector2, sizeof(uint64_t), 1, f2)==0){
				end_of_f2 = true;
			}
		}
		
	}

	while (!end_of_f1){
		if(bit_vector1 != last_value_added){
				fwrite(&bit_vector1, sizeof(uint64_t), 1, dest);
				last_value_added = bit_vector1;
			}
		if (fread(&bit_vector1, sizeof(uint64_t), 1, f1)==0){
				end_of_f1 = true;
			}
	}

	while (!end_of_f2){
		if(bit_vector2 != last_value_added){
				fwrite(&bit_vector2, sizeof(uint64_t), 1, dest);
				last_value_added = bit_vector2;
			}
		if (fread(&bit_vector2, sizeof(uint64_t), 1, f2)==0){
				end_of_f2 = true;
			}
	}
	
}

void merge_rec(){

}


void merge_bin(FILE* ofp, int nb_files){

	FILE* file_dest_merge;
	FILE* file_to_merge1;
	FILE* file_to_merge2;

	int tree_length = get_tree_length(nb_files);
	printf("tree_length : %d\n", tree_length);

	int tmp_file_to_merge_id = 0;
	int tmp_merged_file_id = 0; //tmp_merge file id
	char* tmp_merged_file_name = "tmp_merged_file";
	char* tmp_file_name = "sort_tmp_file";
	//char* tmp_merge = "tmp_merge";
	//char id[1024]; 
	char merged_file_name[1024];
	char tmp_file1[1024];
	char tmp_file2[1024];

/*
	memset(tmp_file, '\0', strlen(tmp_file));
	memset(id, '\0', strlen(id));
	strcpy(tmp_file, tmp_file_name);
	sprintf(id,"%d",id_tmp_file);
	strcat(tmp_file, id);
*/	

	//open first tmp_sort_file
	/*while(!fopen(tmp_file, "r")){
		++id_tmp_file;
		memset(tmp_file, '\0', strlen(tmp_file));
		memset(id, '\0', strlen(id));
		strcpy(tmp_file, tmp_file_name);
		sprintf(id,"%d",id_tmp_file);
		strcat(tmp_file, id);
		printf("%s\n", tmp_file);
  	}*/

	//get files to merge in tmp_file1 and tmp_file2
	++tmp_file_to_merge_id;
  	get_next_file_name_id(tmp_file_to_merge_id, tmp_file1, tmp_file_name);
  	++tmp_file_to_merge_id;
	get_next_file_name_id(tmp_file_to_merge_id, tmp_file2, tmp_file_name);


	//copy it on a tmp merged file
	/*
	tmp_merge0 = fopen("tmp_merge0", "w");
	copy_bin(tmp_merge0, file_to_merge);
	remove(tmp_file);
	*/

	//update ids and names of files

	// update next sort_tmp_file name
	/*memset(tmp_file, '\0', strlen(tmp_file));
	memset(id, '\0', strlen(id));
	strcpy(tmp_file, tmp_file_name);
	sprintf(id,"%d",id_tmp_file);
	strcat(tmp_file, id);*/

	for (int i = 0; i < tree_length; ++i){
		printf("TREE LEVEL : %d\n\n", i);

		while ( ((file_to_merge1 = fopen(tmp_file1, "r")) != NULL) && 
			((file_to_merge2 = fopen(tmp_file2, "r")) != NULL) ){


			file_dest_merge = fopen(tmp_merged_file_name, "w");

			printf("merging %s with %s into :\n", tmp_file1, tmp_file2);
			merge(file_to_merge1, file_to_merge2, file_dest_merge);

			//delete merged files
			remove(tmp_file1);
			remove(tmp_file2);

			//rename file_dest_merge
			++tmp_merged_file_id;
			get_next_file_name_id(tmp_merged_file_id, merged_file_name, tmp_file_name);
			printf("%s\n\n", merged_file_name);
			rename(tmp_merged_file_name, merged_file_name);

			//get next files to merge in tmp_file1 and tmp_file2
			++tmp_file_to_merge_id;
		  	get_next_file_name_id(tmp_file_to_merge_id, tmp_file1, tmp_file_name);
		  	++tmp_file_to_merge_id;
			get_next_file_name_id(tmp_file_to_merge_id, tmp_file2, tmp_file_name);

		}

		//verify if there is a lonely file remaining
		//if there is, upper it in the tree

		if (file_to_merge1 != NULL){
			printf("some tmp_file (%s) feels lonely :(\n", tmp_file1);
			++tmp_merged_file_id;
			get_next_file_name_id(tmp_merged_file_id, merged_file_name, tmp_file_name);
			printf("his new name is %s\n\n", merged_file_name);
			rename(tmp_file1, merged_file_name);
		}

		//reinit ids

		tmp_file_to_merge_id = 0;
		tmp_merged_file_id = 0;

		++tmp_file_to_merge_id;
	  	get_next_file_name_id(tmp_file_to_merge_id, tmp_file1, tmp_file_name);
	  	++tmp_file_to_merge_id;
		get_next_file_name_id(tmp_file_to_merge_id, tmp_file2, tmp_file_name);

	}

}

int main(int argc, char const *argv[])
{
	
	if (argc != 3){
		usage();
		return EXIT_FAILURE;
	}

	FILE* ofp = fopen(argv[1], "w");
	int nb_files = atoi(argv[2]);

	if (nb_files < 1){
		printf("nb of files have to be at least of 2\n");
		return 1;
	}

	merge_bin(ofp, nb_files);

	fclose(ofp);

	return 0;
}