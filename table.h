#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _file

typedef struct Item {
#ifndef _file
	char* info; // строка, которая является информацией
#else
	int info;
#endif
}Item;


typedef struct KS {
#ifndef _file
	char* key; // строка, которая является ключом 
#else
	int key;
#endif
	Item* item; // указатель на информацию
	struct KS* next; // указатель на следующий элемент таблицы 
}KS;

typedef struct Table {
	KS* ks; // указатель на пространство ключей
#ifdef _file
	char* file_name;
#endif
}Table;

#ifndef _file
void new_table(Table** table);
#else
void new_table(Table** table, char* file_name);
#endif

int insert(Table* table, char* key, char* item);

int search_element(Table* table, char* key, char** info_data);

int search_in_range(Table* table, char* key1, char* key2, Table** table_data);

int delete_element(Table* table, char* key);

void print_table(Table* table);

void free_table(Table* table);