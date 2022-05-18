#include "table.h"

// OK = 0; OF = -1; UN = -2; DB = -3
#ifndef _file

void new_table(Table** table) {
	*table = (Table*)calloc(1, sizeof(Table));
	if (*table == NULL)
		return;
	(*table)->ks = NULL;
}

int insert(Table* table, char* key, char* info) {
	KS* ptr = table->ks;
	while (ptr != NULL) {
		if (strcmp(ptr->key, key) == 0)
			return -3;
		ptr = ptr->next;
	}
	ptr = (KS*)calloc(1, sizeof(KS));
	if (ptr == NULL)
		return -1;
	ptr->item = (Item*)calloc(1, sizeof(Item));
	if (ptr->item == NULL)
		return -1;
	ptr->key = key;
	ptr->item->info = info;
	ptr->next = table->ks;
	table->ks = ptr;
	return 0;
}

int copy_str(char** destination, char* source) {
	*destination = (char*)calloc(strlen(source) + 1, sizeof(char));
	if (*destination == NULL)
		return -1;
	for (int i = 0; i < strlen(source); i++)
		(*destination)[i] = source[i];
	return 0;
}

int search_element(Table* table, char* key, char** info_data) {
	KS* ptr = table->ks;
	while (ptr != NULL) {
		if (strcmp(ptr->key, key) == 0) {
			copy_str(info_data, ptr->item->info);
			return 0;
		}
	}
	return -2;
}

int delete_element(Table* table, char* key) {
	KS* ptr = table->ks;
	while ((ptr != NULL) && (ptr->next != NULL || ptr == table->ks)) {
		if (ptr->next != NULL)
			if (strcmp(ptr->next->key, key) == 0) {
				free(ptr->next->key);
				free(ptr->next->item->info);
				free(ptr->next->item);
				KS* ptr2 = ptr->next;
				ptr->next = ptr->next->next;
				free(ptr2);
				return 0;
			}
		if (ptr == table->ks)
			if (strcmp(ptr->key, key) == 0) {
				free(ptr->key);
				free(ptr->item->info);
				free(ptr->item);
				table->ks = ptr->next;
				free(ptr);
				return 0;
			}
		ptr = ptr->next;
	}
	return -2;
}

int search_in_range(Table* table, char* key1, char* key2, Table** table_data) {
	KS* ptr = table->ks;
	new_table(table_data);
	int flag = 1;
	while (ptr != NULL) {
		if (strcmp(ptr->key, key1) > 0 && strcmp(ptr->key, key2) < 0) {
			char* key_data = NULL, * info_data = NULL;
			copy_str(&key_data, ptr->key);
			copy_str(&info_data, ptr->item->info);
			insert(*table_data, key_data, info_data);
			flag = 0;
		}
		ptr = ptr->next;
	}
	if (flag) {
		return -2;
	}
	return 0;
}

void print_table(Table* table) {
	KS* ptr = table->ks;
	if (ptr != NULL) {
		printf("key info\n");
		while (ptr != NULL) {
			printf("%s %s\n", ptr->key, ptr->item->info);
			ptr = ptr->next;
		}
	}
	else
		printf("Table is empty\n");
}

void free_table(Table* table) {
	KS* ptr = table->ks;
	while (ptr != NULL) {
		if (ptr->key != NULL)
			free(ptr->key);
		if (ptr->item != NULL) {
			if (ptr->item->info != NULL)
				free(ptr->item->info);
			free(ptr->item);
		}
		KS* next = ptr->next;
		free(ptr);
		ptr = next;
	}
	free(table);
}
#endif
