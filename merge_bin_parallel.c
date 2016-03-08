#include "merge_bin.h"

void usage(){
	printf("require an output and the number of files to merge\n");
}

uint64_t index_min_tab(uint64_t* tab, int tab_size, bool* ended_file_tab){
	//TODO : find better way to have max_uint64
	uint64_t min = (((uint64_t)1<<63)-1) + ((uint64_t)1<<63);
	//printf("max_uint64 : %"PRIu64"\n", min);
	int index = 0;

	for (int i = 0; i < tab_size; ++i){
		if ((tab[i] < min) && !ended_file_tab[i]){
			min = tab[i];
			index = i;
		}
	}

	return index;
}

void get_file_name_id(int current_id, char* file_dest, char* tmp_file_name){

	char id[1024];

	memset(file_dest, '\0', strlen(file_dest));
	memset(id, '\0', strlen(id));
	strcpy(file_dest, tmp_file_name);
	sprintf(id,"%d",current_id);
	strcat(file_dest, id);

}

int merge_bin(FILE* ofp, int nb_files){

	char* file_to_merge = "sort_tmp_file";
	int current_id = 0;
	char current_file[1024];
	memset(current_file, '\0', 1024);

	FILE** file_tab = malloc(sizeof(FILE*)* nb_files);
	uint64_t *tab = malloc(sizeof(uint64_t)* nb_files);
	bool *ended_file_tab = malloc(sizeof(bool) * nb_files);
	memset (ended_file_tab, 0, nb_files);

	uint64_t last_inserted_value;

	bool end_of_files = false;

	//open the files ans init table values
	for (current_id=0; current_id<nb_files; ++current_id){
		get_file_name_id(current_id, current_file, file_to_merge);
		file_tab[current_id] = fopen(current_file, "r");
		if (fread(&tab[current_id], sizeof(uint64_t), 1, file_tab[current_id]) == 0){

		//verify no reading error
		if (ferror(file_tab[current_id])){
			fprintf(stderr, "error while reading %s%d\n", file_to_merge, current_id);
			return EXIT_FAILURE;
		}

			ended_file_tab[current_id] = true;

			// delete the file
			get_file_name_id(current_id, current_file, file_to_merge);
			printf("deleting %s\n", current_file);
			if (remove(current_file) == -1){
				fprintf(stderr, "error while deleting %s%d\n", file_to_merge, current_id);
				return EXIT_FAILURE;
			}
		}
	}

	//initialization
	int index_min = index_min_tab(tab, nb_files, ended_file_tab);
	fwrite(&tab[index_min], sizeof(uint64_t), 1, ofp);
	last_inserted_value = tab[index_min];

	//read the next uint64 of the file that had the previous min value
	current_id = index_min;
	//get_file_name_id(current_id, current_file, file_to_merge);
	if (fread(&tab[current_id], sizeof(uint64_t), 1, file_tab[current_id]) == 0){

		//verify no reading error
		if (ferror(file_tab[current_id])){
			fprintf(stderr, "error while reading %s%d\n", file_to_merge, current_id);
			return EXIT_FAILURE;
		}

		ended_file_tab[current_id] = true;

		// delete the file
		get_file_name_id(current_id, current_file, file_to_merge);
		printf("deleting %s\n", current_file);
		if (remove(current_file) == -1){
			fprintf(stderr, "error while deleting %s%d\n", file_to_merge, current_id);
			return EXIT_FAILURE;
		}
		//actualize tab of ended files
		bool all_files_ended = true;
		int file_index = 0;
		while (all_files_ended && (file_index < nb_files)){
			all_files_ended = ended_file_tab[file_index];
			++file_index;
		}
		end_of_files = all_files_ended;
	}

	while (!end_of_files){
		//get the min index
		int index_min = index_min_tab(tab, nb_files, ended_file_tab);

		if(last_inserted_value < tab[index_min]){
			fwrite(&tab[index_min], sizeof(uint64_t), 1, ofp);
			last_inserted_value = tab[index_min];
			//printf("index_min : %d, min : %"PRIu64"\n", index_min, tab[index_min]);
		}

		//read the next uint64 of the file that had the previous min value
		current_id = index_min;
		//get_file_name_id(current_id, current_file, file_to_merge);
		if (fread(&tab[current_id], sizeof(uint64_t), 1, file_tab[current_id]) == 0){

			//verify no reading error
			if (ferror(file_tab[current_id])){
				fprintf(stderr, "error while reading %s%d\n", file_to_merge, current_id);
				return EXIT_FAILURE;
			}

			ended_file_tab[current_id] = true;

			// delete the file
			get_file_name_id(current_id, current_file, file_to_merge);
			printf("deleting %s\n", current_file);
			if (remove(current_file) == -1){
				fprintf(stderr, "error while deleting %s%d\n", file_to_merge, current_id);
				return EXIT_FAILURE;
			}

			//actualize tab of ended files
			bool all_files_ended = true;
			int file_index = 0;
			while (all_files_ended && (file_index < nb_files)){
				all_files_ended = ended_file_tab[file_index];
				++file_index;
			}
			end_of_files = all_files_ended;
		}
	}




	free(tab);
	return EXIT_SUCCESS;
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

	return EXIT_SUCCESS;
}