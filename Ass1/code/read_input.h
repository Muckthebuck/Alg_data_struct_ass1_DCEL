//
// Created by mukul on 14/08/2021.
//

#ifndef CODE_READ_INPUT_H
#define CODE_READ_INPUT_H

//allocate memory and free memory
void allocate_structs(watch_tower_t ***watchtowers, loc_t ***vertex ,int *len);
void free_mem(watch_tower_t **watchtowers, loc_t **vertex, const int *len);
//read files
int  read_first_file(FILE *f1, watch_tower_t ***watchtowers,int len);
int  read_second_file(FILE *f2, loc_t ***vertex, int len);
void read_stdin(split_t **split, int *len, FILE *file[]);
#endif //CODE_READ_INPUT_H
