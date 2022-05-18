#include "table.h"
#include <vld.h>

char* getstr() {
	char c, * str = (char*)calloc(1, sizeof(char));
	int returned_value = scanf("%c", &c), size = 0;
	while (c == '\n' || c == ' ') {
		returned_value = scanf("%c", &c);
		if (returned_value < 0) {
			free(str);
			return NULL;
		}
	}
	while (returned_value >= 0 && c != ' ' && c != '\n') {
		size++;
		str = (char*)realloc(str, (size + 2) * sizeof(char));
		str[size - 1] = c;
		returned_value = scanf("%c", &c);
		if (returned_value < 0) {
			free(str);
			return NULL;
		}
	}
	str[size] = '\0';
	return str;
}

int do_variant(Table* table, int variant) {
	char* key, * info;
	Table* table_data;
	switch (variant) {
	case 1:
		printf("Enter key: ");
		key = getstr();
		if (!key)
			return -1;
		printf("Enter info: ");
		info = getstr();
		if (!info)
			return -1;
		switch (insert(table, key, info)) {
		case -1: 
			printf("Overflow\n");
#ifndef _file
			free(key);
			free(info);
#endif
			break;
		case -3:
			printf("Duplicate was found\n");
#ifndef _file
			free(key);
			free(info);
#endif
			break;
		case 0:
			printf("Item was successfully added\n");
			break;
		}
#ifdef _file
		free(key);
		free(info);
#endif
		return 0;
	case 2:
		printf("Enter key: ");
		key = getstr();
		if (!key)
			return -1;
		if (!search_element(table, key, &info)) {
			printf("Item was successfully found\nkey: %s\ninfo: %s\n", key, info);
			free(info);
		}
		else
			printf("Nothing was found\n");
		free(key);
		return 0;
	case 3:
		printf("Enter key: ");
		key = getstr();
		if (!key)
			return -1;
		if (!delete_element(table, key))
			printf("Item was successfully deleted\n");
		else
			printf("Nothing was found\n");
		free(key);
		return 0;
	case 4:
		printf("Enter key1: ");
		key = getstr();
		if (!key)
			return -1;
		printf("Enter key2: ");
		info = getstr();
		if (!info)
			return -1;
		if (!search_in_range(table, key, info, &table_data))
			print_table(table_data);
		else
			printf("Nothing was found\n");
		free_table(table_data);
		free(key);
		free(info);
#ifdef _file
		system("del searchresult.bin");
#endif
		return 0;
	case 5:
		print_table(table);
		return 0;
	}
	return -1;
}

void print_menu() {
	printf("0. Exit\n");
	printf("1. Add element\n");
	printf("2. Search element by key\n");
	printf("3. Delete element by key\n");
	printf("4. Search elements in range\n");
	printf("5. Print table\n");
	printf("Enter number of position: ");
}

int get_variant(int* value, int count) {
	int returned_value;
	do {
		returned_value = scanf("%d", value);
		if (*value == 0 || returned_value < 0)
			return -1;
		if (*value < 1 || *value > count || returned_value == 0) {
			printf("Input error, try agin: ");
			scanf("%*c");
		}
	} while (*value < 1 || *value > count || returned_value == 0);
	return 0;
}

int main() {
	int returned_value, flag = 1, variant;
	Table* table;
#ifdef _file
	printf("Enter path to the file: ");
	char* file_name = getstr();
	FILE* file = fopen(file_name, "rb");
	if (file == NULL)
		printf("There is no such file, create\n");
	else {
		printf("Open file\n");
		fclose(file);
	}
	new_table(&table, file_name);
#else
	new_table(&table);
#endif
	do {
		print_menu();
		returned_value = get_variant(&variant, 5);
		if (!returned_value) {
			if (do_variant(table, variant))
				flag = 0;
		}
		else flag = 0;

	} while (flag);
	free_table(table);
	return 0;
}