#include "table.h"

#ifdef _file

void new_table(Table** table, char* file_name) {
	*table = (Table*)calloc(1, sizeof(Table));
	(*table)->file_name = file_name;
	(*table)->ks = NULL;
	int flag = 0;
	FILE* file = fopen(file_name, "rb");
	if (file == NULL) {
		file = fopen(file_name, "wb+");
		flag = 1;
	}
	if (!flag) {
		/*int len, returned_value;
		returned_value = fread(&len, sizeof(int), 1, file);
		fseek(file, 0, SEEK_SET);
		while (returned_value > 0) {
			KS* ptr = (KS*)calloc(1, sizeof(KS));
			ptr->item = (Item*)calloc(1, sizeof(Item));
			ptr->key = ftell(file);
			returned_value = fread(&len, sizeof(int), 1, file);
			if (returned_value <= 0) {
				free(ptr->item);
				free(ptr);
				break;
			}
			fseek(file, len, SEEK_CUR);
			ptr->item->info = ftell(file);
			returned_value = fread(&len, sizeof(int), 1, file);
			if (returned_value <= 0) {
				free(ptr->item);
				free(ptr);
				break;
			}
			fseek(file, len, SEEK_CUR);
			ptr->next = (*table)->ks;
			(*table)->ks = ptr;
		}*/
		int len, returned_value, ptr;
		returned_value = fread(&ptr, sizeof(int), 1, file);
		while (returned_value > 0 && ptr != 0) {
			fseek(file, ptr, SEEK_SET);
			KS* new_ks = (KS*)calloc(1, sizeof(KS));
			new_ks->item = (Item*)calloc(1, sizeof(Item));
			new_ks->key = ptr;
			returned_value = fread(&len, sizeof(int), 1, file);
			if (returned_value <= 0) {
				free(new_ks->item);
				free(new_ks);
				break;
			}
			fseek(file, len, SEEK_CUR);
			new_ks->item->info = ftell(file);
			returned_value = fread(&len, sizeof(int), 1, file);
			if (returned_value <= 0) {
				free(new_ks->item);
				free(new_ks);
				break;
			}
			fseek(file, len, SEEK_CUR);
			returned_value = fread(&ptr, sizeof(int), 1, file);
			KS* ks_ptr = (*table)->ks;
			if (ks_ptr != NULL) {
				while (ks_ptr->next != NULL)
					ks_ptr = ks_ptr->next;
				ks_ptr->next = new_ks;
			}
			else
				(*table)->ks = new_ks;
		}
	}
	else {
		int ptr = 0;
		fwrite(&ptr, sizeof(int), 1, file);
	}
	fclose(file);
}

int insert(Table* table, char* key, char* item) {
	KS* ptr = table->ks;
	FILE* file = fopen(table->file_name, "rb+");
	if (file == NULL)
		return -2;
	while (ptr != NULL) {
		fseek(file, ptr->key, SEEK_SET);
		int len;
		char* key_file;
		fread(&len, sizeof(int), 1, file);
		key_file = (char*)calloc(len + 1, sizeof(char));
		fread(key_file, sizeof(char), len, file);
		key_file[len] = '\0';
		if (strcmp(key_file, key) == 0) {
			free(key_file);
			fclose(file);
			return -3;
		}
		free(key_file);
		ptr = ptr->next;
	}
	int offset;
	fseek(file, 0, SEEK_SET);
	fread(&offset, sizeof(int), 1, file);
	fseek(file, 0, SEEK_END);
	int len = strlen(key);
	fwrite(&len, 1, sizeof(int), file);
	fseek(file, -4, SEEK_CUR);
	ptr = (KS*)calloc(1, sizeof(KS));
	ptr->item = (Item*)calloc(1, sizeof(Item));
	ptr->key = ftell(file);
	fseek(file, 4, SEEK_CUR);
	fwrite(key, sizeof(char), strlen(key), file);
	ptr->item->info = ftell(file);
	len = strlen(item);
	fwrite(&len, 1, sizeof(int), file);
	fwrite(item, sizeof(char), strlen(item), file);
	fwrite(&offset, sizeof(int), 1, file);
	fseek(file, 0, SEEK_SET);
	fwrite(&ptr->key, sizeof(int), 1, file);
	ptr->next = table->ks;
	table->ks = ptr;
	fclose(file);
	return 0;
}

int search_element(Table* table, char* key, char** info_data) {
	KS* ptr = table->ks;
	FILE* file = fopen(table->file_name, "rb+");
	if (file == NULL)
		return -2;
	while (ptr != NULL) {
		fseek(file, ptr->key, SEEK_SET);
		int len;
		char* key_file;
		fread(&len, sizeof(int), 1, file);
		key_file = (char*)calloc(len + 1, sizeof(char));
		fread(key_file, sizeof(char), len, file);
		key_file[len] = '\0';
		if (strcmp(key_file, key) == 0) {
			fread(&len, sizeof(int), 1, file);
			char* info_file = (char*)calloc(len + 1, sizeof(char));
			fread(info_file, sizeof(char), len, file);
			info_file[len] = '\0';
			*info_data = info_file;
			free(key_file);
			return 0;
		}
		free(key_file);
		ptr = ptr->next;
	}
	return -2;
}

int delete_element(Table* table, char* key) {
	FILE* file = fopen(table->file_name, "rb+");
	if (file == NULL)
		return -2;
	KS* ptr = table->ks;
	while ((ptr != NULL) && (ptr->next != NULL || ptr == table->ks)) {
		if (ptr->next != NULL) {
			fseek(file, ptr->next->key, SEEK_SET);
			int len;
			fread(&len, sizeof(int), 1, file);
			char* key_file = (char*)calloc(len + 1, sizeof(char));
			fread(key_file, sizeof(char), len, file);
			key_file[len] = '\0';
			if (strcmp(key_file, key) == 0) {
				free(key_file);
				fseek(file, ptr->item->info, SEEK_SET);
				fread(&len, sizeof(int), 1, file);
				fseek(file, len, SEEK_CUR);
				if (ptr->next->next != NULL)
					fwrite(&ptr->next->next->key, sizeof(int), 1, file);
				else {
					int buffer = 0;
					fwrite(&buffer, sizeof(int), 1, file);
				}
				KS* ptr_next = ptr->next->next;
				free(ptr->next->item);
				free(ptr->next);
				ptr->next = ptr_next;
				fclose(file);
				return 0;
			}
			free(key_file);
		}
		if (ptr == table->ks) {
			fseek(file, ptr->key, SEEK_SET);
			int len;
			fread(&len, sizeof(int), 1, file);
			char* key_file = (char*)calloc(len + 1, sizeof(char));
			fread(key_file, sizeof(char), len, file);
			key_file[len] = '\0';
			if (strcmp(key_file, key) == 0) {
				free(key_file);
				fread(&len, sizeof(int), 1, file);
				fseek(file, len, SEEK_CUR);
				int ptr_next;
				fread(&ptr_next, sizeof(int), 1, file);
				fseek(file, 0, SEEK_SET);
				fwrite(&ptr_next, sizeof(int), 1, file);
				KS* ptr_next_ = ptr->next;
				free(ptr->item);
				free(ptr);
				table->ks = ptr_next_;
				fclose(file);
				return 0;
			}
			free(key_file);
		}
		ptr = ptr->next;
	}
	fclose(file);
	return -2;
}

int search_in_range(Table* table, char* key1, char* key2, Table** table_data) {
	char* table_name_for_search = (char*)calloc(17, sizeof(char*));
	int flag = 0;
	strcat(table_name_for_search, "searchresult.bin");
	new_table(table_data, table_name_for_search);
	KS* ptr = table->ks;
	FILE* file = fopen(table->file_name, "rb+");
	if (file == NULL)
		return -2;
	while (ptr != NULL) {
		fseek(file, ptr->key, SEEK_SET);
		int len;
		char* key_file;
		fread(&len, sizeof(int), 1, file);
		key_file = (char*)calloc(len + 1, sizeof(char));
		fread(key_file, sizeof(char), len, file);
		key_file[len] = '\0';
		if (strcmp(key_file, key1) > 0 && strcmp(key_file, key2) < 0) {
			fread(&len, sizeof(int), 1, file);
			char* info_file = (char*)calloc(len + 1, sizeof(char));
			fread(info_file, sizeof(char), len, file);
			info_file[len] = '\0';
			insert(*table_data, key_file, info_file);
			flag = 1;
			free(info_file);
		}
		free(key_file);
		ptr = ptr->next;
	}
	fclose(file);
	if (!flag)
		return -2;
	return 0;
}

void print_table(Table* table) {
	KS* ptr = table->ks;
	FILE* file = fopen(table->file_name, "rb+");
	if (file == NULL)
		return;
	if (ptr != NULL)
		printf("key info\n");
	else
		printf("Table is empty\n");
	while (ptr != NULL) {
		fseek(file, ptr->key, SEEK_SET);
		int len;
		char* key_file, * info_file;
		fread(&len, sizeof(int), 1, file);
		key_file = (char*)calloc(len + 1, sizeof(char));
		fread(key_file, sizeof(char), len, file);
		key_file[len] = '\0';
		fread(&len, sizeof(int), 1, file);
		info_file = (char*)calloc(len + 1, sizeof(char));
		fread(info_file, sizeof(char), len, file);
		info_file[len] = '\0';
		printf("%s %s\n", key_file, info_file);
		ptr = ptr->next;
		free(key_file);
		free(info_file);
	}
	fclose(file);
}

void free_table(Table* table) {
	KS* ptr = table->ks;
	while (ptr != NULL) {
		if (ptr->item != NULL)
			free(ptr->item);
		KS* next = ptr->next;
		free(ptr);
		ptr = next;
	}
	free(table->file_name);
	free(table);
}

#endif
