//
// Created by mukul on 14/08/2021.
//

#ifndef CODE_VECT_H
#define CODE_VECT_H

typedef  struct {
    double x;
    double y;
}loc_t;   //use as vertex

typedef struct {
    int edge_start;
    int edge_end;
}split_t;

typedef struct{
    double m;
    double c;
    int is_vertical;
}edge_linear_eq_t;

#endif //CODE_VECT_H
