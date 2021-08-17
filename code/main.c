#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vect.h"
#include "dcel_struct.h"
#include "dcel.h"
#include "watchtower.h"
#include "read_input.h"
#include "print_to_file.h"

#define MAXFILE 3
#define FILE_1 0
#define FILE_2 1
#define FILE_3 2

#define WATCH_IDX 0
#define VERT_IDX  1
#define SPLIT_IDX 2
#define EDGE_IDX  3
#define FACE_IDX  4


// done - get input
// done - implement doubly edge list
// done - store data2
// done - create a function to print geometry to make sure its working properly
// done - find total number of faces
// done - implement slicing
// find total number of faces and population in that area
// run tests

int up_not_in_face_logic(edge_linear_eq_t curr_eq, double watchtower_coord_x,
                         double halfedge_end_x, double residual);
int down_not_in_face_logic(edge_linear_eq_t curr_eq, double watchtower_coord_x,
                           double halfedge_end_x, double residual);
void locate_watchtower(watchtowers_organised_t **watchtowers_organised,
                       edge_linear_eq_t *linear_eq, watch_tower_t **watchtowers,
                       face_t *face, loc_t **vertex, int *len);
void print_final_output(FILE *f3,watchtowers_organised_t *watchtowers_organised,
						int len);
double calc_residual(edge_linear_eq_t linear_eq,loc_t watchtower_loc);
void create_edge_eqs(edge_linear_eq_t **linear_eq,edge_t *edge,
					 loc_t **vertex,int *len);
void organise_watchtowers(FILE *f3, watch_tower_t **watchtowers, face_t *face,
						  edge_t *edge, loc_t **vertex, int *len);
int main(int argc, char **argv) {
    //file pointers
    FILE *file[MAXFILE];
    //info storage
    watch_tower_t **watchtowers;
    loc_t **vertex;
    edge_t *edge;
    face_t *face;
    split_t *split;

    int i, len[5];
    for (i =0; i<5;i++){
        if(i<3){
            len[i]=10;
        }else{
            len[i]=0;
        }
    }

    for (i=0;i<MAXFILE;i++){
        file[i]=NULL;
    }

    if(argc!=MAXFILE+1){
        fprintf(stderr,"Invalid input\n");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i<argc; i++){
        if(i==3) {
            file[i-1]= fopen(argv[i],"w");
        }else {
            file[i-1]= fopen(argv[i],"r");
        }
        if(file[i-1]==NULL){
            fprintf(stderr,"Cannot open %s\n",argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    read_stdin(&split, len,file);
    //allocating pointers to structs in array
    allocate_structs(&watchtowers, &vertex, len);

    //reading files and storing mem
   len[WATCH_IDX] = read_first_file(file[FILE_1], &watchtowers,
                                     len[WATCH_IDX]);
    len[VERT_IDX] = read_second_file(file[FILE_2],&vertex,len[VERT_IDX]);

   // print_watchtowers(file[FILE_3],watchtowers,len[WATCH_IDX]);

  //print_vertex(file[FILE_3], vertex, len[VERT_IDX]);

 // printf("\n");
    make_init_poly(&edge, &face, len);
    if(len[SPLIT_IDX]>0){
    	do_split(&edge, face, split, &vertex, len);
    }
//    printf("len[VERT_IDX]:%d\n",len[VERT_IDX]);
//   // print_vertex(file[FILE_3], vertex, len[VERT_IDX]);
 traverse_full(file[FILE_3],edge, face,vertex, len);
   printf("\n\n");
   organise_watchtowers(file[FILE_3],watchtowers, face, edge, vertex, len);


    free(split);
    free_mem(watchtowers, vertex, len);
    free_poly(edge, len[EDGE_IDX]);
    free(face);
    fclose(file[2]);

    return 0;
}
double calc_residual(edge_linear_eq_t linear_eq,loc_t watchtower_loc){
    double y_predicted;
    //y = mx+c
    y_predicted = (linear_eq.m*watchtower_loc.x) +linear_eq.c;
    return watchtower_loc.y - y_predicted;
}

void create_edge_eqs(edge_linear_eq_t **linear_eq,edge_t *edge,
                     loc_t **vertex,int *len){
    loc_t *start,*end;
    *linear_eq = (edge_linear_eq_t *)
                 malloc(sizeof (edge_linear_eq_t)*len[EDGE_IDX]);
    assert(*linear_eq);
    for (int i =0; i <len[EDGE_IDX];i++){
        start = vertex[edge[i].halfedge->start_vertex_idx];
        end = vertex[edge[i].halfedge->end_vertex_idx];
        if(end->x==start->x){
            (*linear_eq)[i].m = 0;
            (*linear_eq)[i].c = 0;
            (*linear_eq)[i].is_vertical=1;
        }else{
            (*linear_eq)[i].m = (end->y-start->y)/(end->x-start->x);
            (*linear_eq)[i].c = end->y - ((*linear_eq)[i].m*end->x);
            (*linear_eq)[i].is_vertical=0;
//            printf("[create eqns:] m: (%lf-%lf)/(%lf-%lf) = %lf\n",
//				   end->y,start->y,end->x,start->x, (*linear_eq)[i].m );
        }
    }
}

void organise_watchtowers(FILE *f3, watch_tower_t **watchtowers, face_t *face,
                          edge_t *edge, loc_t **vertex, int *len){
    watchtowers_organised_t *watchtowers_organised;
    edge_linear_eq_t *linear_eq;
    watchtowers_organised = (watchtowers_organised_t *)
                   malloc(sizeof (watchtowers_organised_t)*len[FACE_IDX]);
    assert(watchtowers_organised);
    int i;
    for(i=0;i<len[FACE_IDX];i++){
        //one of the simplest split cases produces a minimum of 5 half_edges
        //so, I am allocating a little extra
        watchtowers_organised[i].total_pop = 0;
        watchtowers_organised[i].tot_len   = 10;
        watchtowers_organised[i].curr_len  = 0;
        watchtowers_organised[i].watchtowers = (watch_tower_t **) malloc(
                sizeof(watch_tower_t *)*watchtowers_organised[i].tot_len);
        assert(watchtowers_organised[i].watchtowers);
    }
    create_edge_eqs(&linear_eq, edge, vertex, len);
    locate_watchtower(&watchtowers_organised, linear_eq, watchtowers,
                      face, vertex, len);
    //print
    print_final_output(f3,watchtowers_organised,len[FACE_IDX]);
    for(i=0;i<len[FACE_IDX];i++){
    	free(watchtowers_organised[i].watchtowers) ;
    }
    free(linear_eq);
    free(watchtowers_organised);

}
void print_final_output(FILE *f3,watchtowers_organised_t *watchtowers_organised,
                        int len){
    watch_tower_t *watchtowers;
    int i,j;
    for(i =0; i<len;i++){
        fprintf(f3,"%d\n",i);
        for (j = 0;j<watchtowers_organised[i].curr_len;j++){
            watchtowers = watchtowers_organised[i].watchtowers[j];
            fprintf(f3,"Watchtower ID: %s, Postcode: %s, ",
                   watchtowers->id, watchtowers->postcode);
            fprintf(f3,"Population Served: %d, ", watchtowers->pop_served);
            fprintf(f3,"Watchtower Point of Contact Name: %s, ",
				   watchtowers->contact_name);
            fprintf(f3,"x: %10lf, y: %10lf\n", watchtowers->coords.x,
                   watchtowers->coords.y);
        }
    }
    for(i =0; i<len;i++){
        fprintf(f3,"Face %d population served: %d\n",i,
               watchtowers_organised[i].total_pop);
    }
}

void locate_watchtower(watchtowers_organised_t **watchtowers_organised,
                       edge_linear_eq_t *linear_eq, watch_tower_t **watchtowers,
                       face_t *face, loc_t **vertex, int *len){
    half_edge_t *start;
    half_edge_t *curr;
    edge_linear_eq_t curr_eq;
    loc_t *halfedge_origin;
    loc_t *halfedge_end;
    int total_edges, edges_matched, not_up_face, not_down_face,org_watch_len;

    int i,j; double residual;
    for (j =0;j<len[WATCH_IDX];j++){
        for (i =0; i<len[FACE_IDX];i++){
            start = face[i].halfedge->prev_half_edge;
            curr = face[i].halfedge;
            total_edges=edges_matched=0;
            while(curr!= start ){
//            	printf("current edge number: %d, face: %d\n",curr->edge_index,
//					   curr->face_index);
                halfedge_origin = vertex[curr->start_vertex_idx];
                halfedge_end = vertex[curr->end_vertex_idx];
                curr_eq = linear_eq[curr->edge_index];
                residual  = calc_residual(curr_eq, watchtowers[j]->coords);
//                printf("start(%10lf,%10lf), end(%10lf,%10lf)\n" ,
//					  halfedge_origin->x,
//					  halfedge_origin->y,halfedge_end->x,halfedge_end->y);
//                printf("(%10lf,%10lf), residual: %lf\n",
//					   watchtowers[j]->coords.x,
//					   watchtowers[j]->coords.y,residual);
                //not(in)_direction-of-half-edge_face
                not_up_face = up_not_in_face_logic(curr_eq, watchtowers[j]->coords.x,
                                                   halfedge_end->x, residual);
                not_down_face = down_not_in_face_logic(curr_eq, watchtowers[j]->coords.x,
                                                       halfedge_end->x,
                                                       residual);
//                printf("not up: %d, not_down: %d \n",not_up_face,
//						not_down_face);
                total_edges++;
                if(total_edges==1){
                    //need to do this once so loop captures all edges
                    start = start->next_half_edge;
                }
                //are we going up?
                if((halfedge_origin->y < halfedge_end->y) && not_up_face){
                    //okay we are going up and its not in current face

                    break;
                }else if((halfedge_origin->y > halfedge_end->y)&&
                not_down_face){
                    //looks like we are going down
                    //so not in current face
                    break;
                }else if(halfedge_origin->y == halfedge_end->y){
                    // we are just traveling horizontally
                    if((halfedge_origin->x < halfedge_end->x) &&
                    (watchtowers[j]->coords.y > halfedge_origin->y)){
                        //going to right but point is above
                        break;
                    }else if((halfedge_origin->x > halfedge_end->x)
                    &&(watchtowers[j]->coords.y < halfedge_origin->y)){
                        //going to left but point is below
                        break;
                    }
                }
                edges_matched++;
                curr = curr->next_half_edge;
            }
            if(total_edges == edges_matched){
                org_watch_len = (*watchtowers_organised)[i].curr_len;
                if( org_watch_len == (*watchtowers_organised)[i].tot_len){
                    //reallocate
                    (*watchtowers_organised)[i].tot_len *=2;
                    (*watchtowers_organised)[i].watchtowers = (watch_tower_t **)
                            realloc((*watchtowers_organised)[i].watchtowers,
                                    sizeof(sizeof(watch_tower_t *))
                                    *(*watchtowers_organised)[i].tot_len);
                    assert((*watchtowers_organised)[i].watchtowers);
                }
                //point is in this face, time to allocate it
                (*watchtowers_organised)[i].watchtowers[org_watch_len] =
                		watchtowers[j];
                (*watchtowers_organised)[i].total_pop+=watchtowers[j]->pop_served;
                (*watchtowers_organised)[i].curr_len++;
                break;
            }
        }
    }
}

int up_not_in_face_logic(edge_linear_eq_t curr_eq, double watchtower_coord_x,
                         double halfedge_end_x, double residual){
    if(curr_eq.is_vertical && (watchtower_coord_x < halfedge_end_x)){
        // in face would be watchtower_coord_x > halfedge_end_x
        return 1;
    }else if((curr_eq.m<0) && (residual<0)){
        //line goes like \
        //in face would be residual>=0
        return 1;
    }else if((curr_eq.m>0)&&(residual>0)){
        //line goes like /
        //in face would be residual<=0
        return 1;
    }
    //in face
    return 0;
}
int down_not_in_face_logic(edge_linear_eq_t curr_eq, double watchtower_coord_x,
                           double halfedge_end_x, double residual){
    if(curr_eq.is_vertical && (watchtower_coord_x> halfedge_end_x)){
        // in face would be watchtowers[j]->coords.x < halfedge_end.x
        return 1;
    }else if((curr_eq.m<0) && (residual>0)){
        /*line goes like
        //in face would be residual<=0*/
        return 1;
    }else if((curr_eq.m>0)&&(residual<0)){
        /*line goes like /
        in face would be residual>=0*/
        return 1;
    }
    return 0;
}