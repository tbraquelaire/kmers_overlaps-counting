#include "merge_bin.h"

void usage(){
	printf("require an output\n");
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


void merge_bin(FILE* ofp){

	FILE* tmp_merge0;
	FILE* tmp_merge1;
	FILE* file_to_merge;

	int id_tmp_file = 0;
	int tmp_merge_id = 0; //tmp_merge file id
	char* tmp_file_name = "sort_tmp_file";
	//char* tmp_merge = "tmp_merge";
	char id[1024]; 
	char tmp_file[1024];

/*
	memset(tmp_file, '\0', strlen(tmp_file));
	memset(id, '\0', strlen(id));
	strcpy(tmp_file, tmp_file_name);
	sprintf(id,"%d",id_tmp_file);
	strcat(tmp_file, id);
*/	

	//open first tmp_sort_file
	//CHANGED to debug ? 
	while(!fopen(tmp_file, "r")){
		++id_tmp_file;
		memset(tmp_file, '\0', strlen(tmp_file));
		memset(id, '\0', strlen(id));
		strcpy(tmp_file, tmp_file_name);
		sprintf(id,"%d",id_tmp_file);
		strcat(tmp_file, id);
		printf("%s\n", tmp_file);
  	}

	file_to_merge = fopen(tmp_file, "r");

	//copy it on a tmp merged file
	tmp_merge0 = fopen("tmp_merge0", "w");
	copy_bin(tmp_merge0, file_to_merge);
	remove(tmp_file);
	

	//update ids and names of files
	++id_tmp_file;
	tmp_merge_id = (id_tmp_file)%2;

	// update next sort_tmp_file name
	memset(tmp_file, '\0', strlen(tmp_file));
	memset(id, '\0', strlen(id));
	strcpy(tmp_file, tmp_file_name);
	sprintf(id,"%d",id_tmp_file);
	strcat(tmp_file, id);
	while ((file_to_merge = fopen(tmp_file, "r")) != NULL){

		if (tmp_merge_id == 0){
			tmp_merge0 = fopen("tmp_merge0", "w");
			tmp_merge1 = fopen("tmp_merge1", "r");
			merge(tmp_merge1, file_to_merge, tmp_merge0);
			fclose(tmp_merge0);
			fclose(tmp_merge1);
		}
		else{
			tmp_merge0 = fopen("tmp_merge0", "r");
			tmp_merge1 = fopen("tmp_merge1", "w");
			merge(tmp_merge0, file_to_merge, tmp_merge1);
			fclose(tmp_merge0);
			fclose(tmp_merge1);
		}
		//delete sort_tmp_file
		remove(tmp_file);

		//update ids and names of files
		++id_tmp_file;
		tmp_merge_id = (id_tmp_file)%2;

		// update next sort_tmp_file name
		memset(tmp_file, '\0', strlen(tmp_file));
		memset(id, '\0', strlen(id));
		strcpy(tmp_file, tmp_file_name);
		sprintf(id,"%d",id_tmp_file);
		strcat(tmp_file, id);
	}

	//copy the last tmp_merge updated to the final dest file*
	//better to rename... just let the last merge file and user will have
	//to rename it...
	if (tmp_merge_id == 0){
		//tmp_merge1 = fopen("tmp_merge1", "r");
		//copy_bin(ofp, tmp_merge1);
		//fclose(tmp_merge1);
		remove("tmp_merge0");
	}
	else{
		//tmp_merge0 = fopen("tmp_merge0", "r");
		//copy_bin(ofp, tmp_merge0);
		//fclose(tmp_merge0);
		remove("tmp_merge1");
	}

	//remove("tmp_merge0");
	//remove("tmp_merge1");
	
}

int main(int argc, char const *argv[])
{
	
	if (argc != 2){
		usage();
		return EXIT_FAILURE;
	}

	FILE* ofp = fopen(argv[1], "w");

	merge_bin(ofp);

	fclose(ofp);

	return 0;
}