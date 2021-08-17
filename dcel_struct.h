//
// Created by mukul on 15/08/2021.
//

#ifndef CODE_DCEL_STRUCT_H
#define CODE_DCEL_STRUCT_H

typedef struct half_edge half_edge_t;
struct half_edge{
    int start_vertex_idx;
    int end_vertex_idx;

    half_edge_t *next_half_edge;
    half_edge_t *prev_half_edge;
    half_edge_t *twin_half_edge;

    int face_index;
    int edge_index;
};

typedef struct {
    half_edge_t *halfedge;
}edge_t;

typedef struct {
    half_edge_t *halfedge;
}face_t;


#endif //CODE_DCEL_STRUCT_H
