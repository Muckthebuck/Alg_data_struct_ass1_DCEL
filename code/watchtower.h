//
// Created by mukul on 14/08/2021.
//

#ifndef CODE_WATCHTOWER_H
#define CODE_WATCHTOWER_H
#include "vect.h"
typedef struct {
    char *id;
    char *postcode;
    char *contact_name;
    int   pop_served;
    loc_t   coords;
}watch_tower_t;

typedef struct {
    int total_pop;
    int tot_len;
    int curr_len;
    watch_tower_t **watchtowers;
}watchtowers_organised_t;

#endif //CODE_WATCHTOWER_H
