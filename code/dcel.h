//
// Created by mukul on 14/08/2021.
//

#ifndef CODE_DCEL_H
#define CODE_DCEL_H


void make_init_poly(edge_t **edge, face_t **face, int *len);
void free_poly(edge_t *edge, int len);
void traverse_init_poly(edge_t *edge);
void traverse_full(FILE *f3,edge_t *edge, face_t *face,loc_t **vertex,int *len);
void insert_edge(int vert_num, edge_t *edge, int edge_num,int is_last);
void do_split(edge_t **edge, face_t *face,split_t *split, loc_t ***vertex,int *len);
void print_split(split_t *split,int len);
int get_face_num(edge_t edge);
void allocate_face_num(half_edge_t *half_edge);
loc_t* find_midpoint(edge_t *edge, int edge_num,loc_t **vertex);
void make_a_split_edge(edge_t *edge,split_t split, face_t *face,
					   int edge_num, int vert_num, int *current_face_num );

#endif //CODE_DCEL_H