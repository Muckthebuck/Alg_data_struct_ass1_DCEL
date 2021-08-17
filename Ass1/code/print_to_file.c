//
// Created by mukul on 14/08/2021.
//
#include <stdio.h>
#include "watchtower.h"
#include "print_to_file.h"

void print_watchtowers(FILE *f3,watch_tower_t **watchtowers, int len){
    for(int j=0;j<len;j++){
        fprintf(f3,"[main]%i: %s,%s,%i,%s,%0.14lf,%0.15lf\n",j+1,
                watchtowers[j]->id,watchtowers[j]->postcode,
                watchtowers[j]->pop_served,watchtowers[j]->contact_name,
                watchtowers[j]->coords.x,watchtowers[j]->coords.y);
    }
}
void print_vertex(FILE *f3,loc_t **vertex,int len){
    for (int i=0;i<len;i++){
        printf("%0.6lf %0.6lf\n",vertex[i]->x,vertex[i]->y);
    }
}
