//
// Created by mukul on 14/08/2021.
//
//
// Created by mukul on 14/08/2021.
//
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vect.h"
#include "watchtower.h"
#include "read_input.h"


#define BUFF_LEN 512
#define WATCH_IDX 0
#define VERT_IDX  1
#define SPLIT_IDX 2

#define MAXFILE 3


void allocate_structs(watch_tower_t ***watchtowers, loc_t ***vertex, int *len){
	int j;
	*watchtowers = (watch_tower_t**)
			malloc(sizeof(watch_tower_t*) * len[WATCH_IDX]);
	assert(*watchtowers);
	/* allocating vertex memory*/
	*vertex = (loc_t**) malloc(sizeof (loc_t*) * len[VERT_IDX]);
	assert(*vertex);

	for(j=0; j<len[WATCH_IDX];j++){
		//file 1
		(*watchtowers)[j]= (watch_tower_t*) malloc(sizeof(watch_tower_t));
		assert((*watchtowers)[j]);
		(*watchtowers)[j]->contact_name=NULL;
		(*watchtowers)[j]->id=NULL;
		(*watchtowers)[j]->postcode=NULL;
		//file 2
		(*vertex)[j] = (loc_t*) malloc(sizeof (loc_t));
		assert((*vertex)[j]);
	}
}

void free_mem(watch_tower_t **watchtowers, loc_t **vertex, const int *len){
	int j;
	for (j =0; j<len[WATCH_IDX];j++){
		free((char*)watchtowers[j]->id);
		free((char*)watchtowers[j]->postcode);
		free((char*)watchtowers[j]->contact_name);
		free(watchtowers[j]);
	}
	for(j=0;j<len[VERT_IDX];j++){
		free(vertex[j]);
	}
	free(watchtowers);
	free(vertex);
}


int  read_first_file(FILE *f1, watch_tower_t ***watchtowers, int len) {
	char *buf = (char*) malloc(sizeof(char)*(BUFF_LEN+1));
	char *tmp;
	assert(buf);
	int i = 0,j,is_first=1;

	while (fgets(buf, BUFF_LEN+1, f1) != NULL){
		if(is_first){
			//ignore first line
			is_first=0;
			continue;
		}
		if(i == len){
			//not enough memory need to realloc
			len=2*len;
			*watchtowers = (watch_tower_t**)
					realloc(*watchtowers,len * sizeof(watch_tower_t*));
			assert(*watchtowers);
			//need to fill array with pointers to structs
			for(j=i; j<len;j++){
				(*watchtowers)[j] = (watch_tower_t*) malloc(sizeof(watch_tower_t));
				assert((*watchtowers)[j]);
				(*watchtowers)[j]->contact_name=NULL;
				(*watchtowers)[j]->id=NULL;
				(*watchtowers)[j]->postcode=NULL;
			}
		}
		//so there is memory do the usual
		if ((strlen(buf)>0) && (buf[strlen(buf) - 1] == '\n')){
			buf[strlen(buf) - 1] = '\0';// replacing new line with \0
		}
		//parsing data and storing in structs
		// Watchtower ID,Postcode,Population Served,
		// Watchtower Point of Contact Name,x,y
		tmp = strtok(buf, ",");
		(*watchtowers)[i]->id = (char*)malloc(sizeof(char)*(strlen(tmp)+1));
		strcpy((*watchtowers)[i]->id,tmp);         //Watchtower ID

		tmp = strtok(NULL, ",");
		(*watchtowers)[i]->postcode = (char*)malloc(sizeof(char)*(strlen(tmp)+1));
		strcpy((*watchtowers)[i]->postcode,tmp);    // Postcode

		tmp = strtok(NULL, ",");
		(*watchtowers)[i]->pop_served =atoi(tmp);      // population served

		tmp = strtok(NULL, ",");
		(*watchtowers)[i]->contact_name = (char*)malloc(sizeof(char)*(strlen(tmp)+1));
		strcpy((*watchtowers)[i]->contact_name,tmp);  // Watchtower POC Name

		tmp = strtok(NULL, ",");
		(*watchtowers)[i]->coords.x = atof(tmp);       // x and y coords
		tmp = strtok(NULL, ",");
		(*watchtowers)[i]->coords.y = atof(tmp);

		i++;
	}

	for (j=i; j<len;j++){
		free((char*)(*watchtowers)[j]->id);
		free((char*)(*watchtowers)[j]->postcode);
		free((char*)(*watchtowers)[j]->contact_name);
		free((*watchtowers)[j]);
	}
	free(buf);
	fclose(f1);
	return i;
}

int  read_second_file(FILE *f2, loc_t ***vertex, int len){
	int i=0,j;
	while(fscanf(f2, "%lf %lf", &((*vertex)[i]->x), &((*vertex)[i]->y))!=EOF){
		if(++i==len){
			len*=2;
			*vertex = (loc_t**) realloc(*vertex, len* sizeof(loc_t*));
			assert(*vertex);
			//need to fill array with pointers to structs
			for(j=i; j<len;j++){
				(*vertex)[j] = (loc_t *) malloc(sizeof(loc_t));
				assert((*vertex)[j]);
			}
		}
	}
	for(int j=i; j<len;j++){
		free((*vertex)[j]);
	}
	fclose(f2);
	return i;
}

void read_stdin(split_t **split, int *len, FILE *file[]) {
	int i=0;
	*split = (split_t*) malloc(sizeof (split_t*) * len[SPLIT_IDX]);
	assert(*split);
	//read that data from std input
	while (scanf("%d %d",&((*split)[i].edge_start),&((*split)[i].edge_end))==2){
		i++;
		if (i==len[SPLIT_IDX]){
			len[SPLIT_IDX]*=2;
			*split = (split_t*) realloc(*split,sizeof (split_t)*len[SPLIT_IDX]);
		}
	}
	len[SPLIT_IDX]=i;
}

