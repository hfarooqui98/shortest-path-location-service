/*     Project 2 sollution for Foundations of Algorithm; Semester 2, 2017
   Written by: Hummad Farooqui       Last Edited: Thursday 15 October 2017 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* 999 indicates a paths is blocked and can not be taken */
#define  BLOCKED 999
/* For any given coordinate the maximum dierections that can be traveled in */
#define  MAX_DIRECTIONS 4
/* At least 2 taxi loactions will be provided (specified in lecture) */
#define  MIN_TAXICOUNT 2
/* Doubling for ideal efficiency of memory handling */
#define  DOUBLE 2
/* For the purpose of indicating uninitialised coordinates x,y  */
#define  UNDEFINED_X -99
#define  UNDEFINED_Y '$'
/* Used to set path cost for the target coordinate and taxi index location */
#define  START_POINT 0
/* ASCII value of 'a' to subtract from the ascii value of the y coordinate to
   find int equivilent on grid */
#define  CHARY_TO_INTY 97
/*************** Structure for travel times for each coordinate ***************/
typedef struct {
        int east;
        int north;
        int west;
        int south;
} traveltime_t;
/*************** Structure for a point location on the grid *******************/
typedef struct {
        int x;
        char y;
} point_t;
/******** Structure for complete coordinate on grid with travel times *********/
typedef struct {
        point_t location;
        traveltime_t travel_time;
} gridpoint_t;
/****** Structure for coordinate's shortest distance and parent path **********/
typedef struct {
        point_t location;
        point_t from;
        int total_cost;
} pathcost_t;
/*************************** Function Prototypes ******************************/

void calculate_costs(gridpoint_t *grid, point_t *taxilocs, int xmax, int ymax,
                     pathcost_t *all_pathcost, int taxi_count);

void print_map(gridpoint_t *grid, point_t *taxilocs, pathcost_t *all_pathcost,
               int xmax, int ymax, int taxi_count);

void print_path(point_t dest,point_t origin,pathcost_t *all_pathcost,int xmax);

int update_costs(gridpoint_t *grid,pathcost_t *all_pathcost,int grid_location,
                 int xmax,int ymax);

int comp_neighbour(int cost_toloc, int centre_x, int centre_y,pathcost_t *loc);

int to_index(int x, int y, int xmax);

int yatoi(char y);

char yitoa(int y);

point_t *populate_grid(gridpoint_t *grid,int xmax,int ymax,point_t *taxilocs,
                       int *taxi_count_return);

/******************************* Main function ********************************/

int
main(int argc, char *argv[]){
        /* assigning the range and domain of the grid and calcuating
           total intersections */
        int xmax;
        scanf("%d",&xmax);
        int ymax;
        scanf("%d",&ymax);
        int grid_size = xmax * ymax;
        /* memory allocation for the city array */
        gridpoint_t* grid = malloc(grid_size * sizeof(*grid));
        assert(grid != NULL);
        /* memory allocation for the taxi list */
        point_t* taxilocs = malloc(MIN_TAXICOUNT * sizeof(*taxilocs));
        assert(taxilocs != NULL);
        /* populating city array and taxi list and re-assigning taxilocs pointer
           in case the size needed to increase */
        int taxi_count_return = 0;
        taxilocs = populate_grid(grid, xmax, ymax, taxilocs,
                                 &taxi_count_return);
        /* Initialising and updating costs array for travel time information for
           each coordinate on city grid */
        /* Memory allocation for array of path costs */
        pathcost_t *all_pathcost = malloc(grid_size * sizeof(*all_pathcost));
        assert(all_pathcost != NULL);
        calculate_costs(grid, taxilocs, xmax, ymax, all_pathcost,
                        taxi_count_return);
        /* New line to adhere to expected output format */
        printf("\n");
        /* calling function to print output for stage 3 */
        print_map(grid,taxilocs,all_pathcost,xmax,ymax,taxi_count_return);
        printf("\n\n");


        /* Freeing memory allocated */
        free(taxilocs);
        taxilocs = NULL;
        free(all_pathcost);
        all_pathcost = NULL;
        free(grid);
        grid = NULL;
        return 0;
}
/******************** Stage 1: Function to read the data **********************/

point_t*
populate_grid(gridpoint_t *grid, int xmax, int ymax, point_t *taxilocs,
              int *taxi_count_return) {
        /* looping through input and populating city coordinate information */
        int count, grid_size = xmax * ymax;
        int x, east, north, west, south;
        int blocked_count = 0, total_paths = grid_size * MAX_DIRECTIONS;
        int total_pathcost = 0;
        char y;
        /* Inserting coordinates into city array */
        gridpoint_t new_cord;
        /* Array to assist with storing and iterating through grid costs in all
           4 directions */
        int temp_costlist[MAX_DIRECTIONS], i;
        /* Scanning input, inserting coordinate in respective index */
        for(count = 0; count < grid_size; count++) {
                scanf("%d%c %d %d %d %d", &x,&y,&east,&north,
                      &west,&south);
                /* assigning values to memebers of new_cord to be added to grid
                   as well as temp_costlist */
                new_cord.location.x = x;
                new_cord.location.y = y;
                new_cord.travel_time.east = east;
                temp_costlist[0] = east;
                new_cord.travel_time.north = north;
                temp_costlist[1] = north;
                new_cord.travel_time.west = west;
                temp_costlist[2] = west;
                new_cord.travel_time.south = south;
                temp_costlist[3] = south;
                /* adding total path cost and blocked path count to be printed
                   as part of stage 1 output */
                for(i = 0; i < MAX_DIRECTIONS; i++) {
                        int check_cost = temp_costlist[i];
                        if(check_cost != BLOCKED) {
                                total_pathcost += check_cost;
                        }
                        else{
                                blocked_count++;
                        }
                }
                grid[count] = new_cord;
        }
        /* Storing Taxi locations */
        int taxi_count = 0;
        int taxilocs_size = MIN_TAXICOUNT;
        point_t new_taxi;
        while(scanf("%d%c", &x,&y) == 2) {
                /* If the array is large enough to store the taxi loaction */
                if(taxi_count <= taxilocs_size) {
                        new_taxi.x = x;
                        new_taxi.y = y;
                        taxilocs[taxi_count] = new_taxi;
                }
                /* Doubling array size and then adding taxi location if the
                   array no longer has space for 1 more taxi location */
                else{
                        taxilocs_size = DOUBLE * taxilocs_size;
                        taxilocs = realloc(taxilocs, taxilocs_size
                                           * sizeof(*taxilocs));
                        assert(taxilocs != NULL);
                        new_taxi.x = x;
                        new_taxi.y = y;
                        taxilocs[taxi_count] = new_taxi;
                }
                taxi_count++;
        }
        /******* printing expected output from stage 1 of assignment **********/
        printf("S1: grid is %d x %d, and has %d intersections\n", xmax,
               ymax, grid_size);
        printf("S1: of %d possibilities, %d of them cannot be used\n",
               total_paths, blocked_count);
        printf("S1: total cost of remaining possibilities is %d seconds\n",
               total_pathcost);
        printf("S1: %d grid locations supplied, first one is %d%c, last one "
               "is %d%c\n\n", taxi_count, taxilocs[0].x, taxilocs[0].y,
               taxilocs[taxi_count-1].x, taxilocs[taxi_count-1].y);
        /**********************************************************************/
        *taxi_count_return = taxi_count;
        return taxilocs;
}
/****************** Stage 2: Path cost calculation function *******************/

void
calculate_costs(gridpoint_t *grid, point_t *taxilocs, int xmax, int ymax,
                pathcost_t *all_pathcost, int taxi_count) {
        int grid_size = xmax * ymax;
        /* Finding path calculation source: target (first taxi location) */
        point_t target;
        target.x = taxilocs[START_POINT].x;
        target.y = taxilocs[START_POINT].y;
        /* Initialising array of path costs for each coordinate on grid */
        int count, add_x;
        char add_y;
        for(count = 0; count < grid_size; count++) {
                add_x = grid[count].location.x;
                add_y = grid[count].location.y;
                all_pathcost[count].location.x = add_x;
                all_pathcost[count].location.y = add_y;
                /* if the coordinate being created is the target
                   coordinate as well */
                if(add_x == target.x && add_y == target.y) {
                        all_pathcost[count].total_cost = START_POINT;
                        all_pathcost[count].from.x = add_x;
                        all_pathcost[count].from.y = add_y;
                }
                else {
                        all_pathcost[count].total_cost = BLOCKED;
                        all_pathcost[count].from.x = UNDEFINED_X;
                        all_pathcost[count].from.y = UNDEFINED_Y;
                }
        }
        /* Iterating through grid locations setting change to 1 to enter loop
           initially and Continue looping through all_pathcost unitll no change
           is made */
        int change = 1, grid_location;
        while(change) {
                /* Reverting change to 0 to check if any changes are made */
                change = 0;
                /* going through each coordinate to check shortest distance */
                for(grid_location = 0; grid_location < grid_size;
                    grid_location++) {
                        if(update_costs(grid, all_pathcost, grid_location,
                                        xmax, ymax)) {
                                change = 1;
                        }
                }
        }
/* Itterating through remaining taxi loactions as destinations and printing
   path */
        int i;
        for(i = 1; i < taxi_count; i++) {
                point_t dest = taxilocs[i];
                point_t origin = taxilocs[START_POINT];
                print_path(dest,origin,all_pathcost,xmax);
        }
}

/********************* Stage 3: Grid map printing function ********************/
/******************************** INCOMPLETE **********************************/
void
print_map(gridpoint_t *grid, point_t *taxilocs, pathcost_t * all_pathcost,
          int xmax, int ymax, int taxi_count) {
        int grid_size = xmax * ymax;
        /* Reseting array of path costs */
        int count, add_x;
        char add_y;
        for(count = 0; count < grid_size; count++) {
                add_x = grid[count].location.x;
                add_y = grid[count].location.y;
                all_pathcost[count].location.x = add_x;
                all_pathcost[count].location.y = add_y;
                all_pathcost[count].total_cost = BLOCKED;
                all_pathcost[count].from.x = UNDEFINED_X;
                all_pathcost[count].from.y = UNDEFINED_Y;
        }
        /* Updating path costs to set all taxi locations to cost 0 */
        int i;
        for(i = 0; i < taxi_count; i++) {
                int update_x = taxilocs[i].x, update_y = yatoi(taxilocs[i].y);
                all_pathcost[to_index(update_x,update_y,xmax)].total_cost = 0;
                all_pathcost[to_index(update_x,update_y,xmax)].from.x =
                        update_x;
                all_pathcost[to_index(update_x,update_y,xmax)].from.y =
                        yitoa(update_y);
        }
        /* Iterating through grid locations setting change to 1 to enter loop
           initially and Continue looping through all_pathcost unitll no change
           is made */
        int change = 1, grid_location;
        while(change) {
                /* Reverting change to 0 to check if any changes are made */
                change = 0;
                /* going through each coordinate to check shortest distance */
                for(grid_location = 0; grid_location < grid_size;
                    grid_location++) {
                        if(update_costs(grid, all_pathcost, grid_location,
                                        xmax, ymax)) {
                                change = 1;
                        }
                }
        }
        /* creating array to hold print information for the array */
        int costs[grid_size];
        int from_north[grid_size];
        int from_south[grid_size];
        int from_east[grid_size];
        int from_west[grid_size];
        int checked;
        /* Iterating through the cost array to gather printing information */
        for(i=0; i<grid_size; i++) {
                checked = 0;
                pathcost_t gathering_target = all_pathcost[i];
                costs[i] = gathering_target.total_cost;
                if((gathering_target.location.y) + 1 == gathering_target.from.y)
                {
                        from_north[i] = 1;
                }
                else{
                        from_north[i] = 0;
                }
                if((gathering_target.location.y - 1) == gathering_target.from.y)
                {
                        from_south[i] = 1;
                }
                else{
                        from_south[i] = 0;
                }
                if((gathering_target.location.x + 1) == gathering_target.from.x)
                {
                        from_east[i] = 1;
                }
                else{
                        from_east[i] = 0;
                }
                if((gathering_target.location.x - 1) == gathering_target.from.x)
                {
                        from_west[i] = 1;
                }
                else{
                        from_west[i] = 0;
                }
        }
        /* Printing out map */
        /* x-axis value output */
        printf("S3:");
        for(i=0; i<xmax; i++) {
                printf("        %d",i);
        }
        printf("\n");
        /* x-axis graphical line */
        printf("S3:   +----+");
        for(i=1; i<xmax; i++) {
                printf("--------+");
        }
        printf("\n");
        /* printing remaining information */
        /* for every y coordinate there are 2 gap lines except the last coord */
        int y_pos, coord_count = 0,x_pos;
        for(y_pos = 0; y_pos < ymax; y_pos++) {
                printf("S3: %c |    ",yitoa(y_pos));
                for(x_pos =0; x_pos < xmax; x_pos++) {
                        /* if not at the left most section of map */
                        if(x_pos != 0) {
                                /* coordinate to be printed came from west */
                                if(from_west[coord_count]) {
                                }
                                else{
                                }
                                /* printing coordinate */
                                coord_count++;
                        }
                        /* if not at the right most section of map */
                        if(x_pos != (xmax-1)) {
                                /* coordinate to be printed came from east */
                                if(from_east[coord_count]) {
                                        coord_count++;
                                }
                                else if(x_pos == 0) {
                                        coord_count++;
                                }
                                else{
                                }
                        }
                }
                /* if not at the first line print the buffer for north and
                   south directions */
                if(y_pos != (ymax-1)) {
                        for(i=0; i<2; i++) {
                                printf("\nS3:   |    ");
                        }
                }
                printf("\n");
        }
}
/******* Function to check shortest distance to neighbouring locations ********/

int
update_costs(gridpoint_t *grid, pathcost_t *all_pathcost,
             int grid_location, int xmax, int ymax){
        /* Extracting information for the target coordinate */
        gridpoint_t target_gridloc = grid[grid_location];
        int target_x = target_gridloc.location.x;
        int target_y = yatoi(target_gridloc.location.y);
        int target_cost = all_pathcost[grid_location].total_cost;
        /* Variable to indicate a change has been made */
        int change = 0;
        /* Skip neighbour checking if the cost at target itself is BLOCKED */
        if(target_cost < BLOCKED) {
                /* All information pertaining to coordinate north of target */
                int north_x = target_x, north_y = target_y - 1,
                    cost_tonorth = target_gridloc.travel_time.north +
                                   target_cost;
                /* All information pertaining to coordinate south of target */
                int south_x = target_x, south_y = target_y + 1,
                    cost_tosouth = target_gridloc.travel_time.south +
                                   target_cost;
                /* All information pertaining to coordinate east of target */
                int east_x = target_x + 1, east_y = target_y,
                    cost_toeast = target_gridloc.travel_time.east + target_cost;
                /* All information pertaining to coordinate west of target */
                int west_x = target_x - 1, west_y = target_y,
                    cost_towest = target_gridloc.travel_time.west + target_cost;
                /* if cealing not exceeded compare costs and re-assign as
                   necessary */
                if(north_y > -1 && (cost_tonorth < BLOCKED)) {
                        pathcost_t north = all_pathcost[
                                to_index(north_x,north_y,xmax)];
                        if(comp_neighbour(cost_tonorth,target_x,target_y,
                                          &north)) {
                                change = 1;
                        }
                        all_pathcost[to_index(north_x,north_y,xmax)] = north;
                }
                /* if bottom not exceeded compare costs and re-assign as
                   necessary */
                if(south_y < ymax && (cost_tosouth < BLOCKED)) {
                        pathcost_t south = all_pathcost[
                                to_index(south_x,south_y,xmax)];
                        if(comp_neighbour(cost_tosouth,target_x,target_y,
                                          &south)) {
                                change = 1;
                        }
                        all_pathcost[to_index(south_x,south_y,xmax)] = south;
                }
                /* if right limit not exceeded compare costs and re-assign as
                   necessary */
                if(east_x < xmax && (cost_toeast < BLOCKED)) {
                        pathcost_t east = all_pathcost[
                                to_index(east_x,east_y,xmax)];
                        if(comp_neighbour(cost_toeast,target_x,target_y,
                                          &east)) {
                                change = 1;
                        }
                        all_pathcost[to_index(east_x,east_y,xmax)] = east;
                }
                /* if the left limit not exceeded compare costs and re-assign as
                   necessary */
                if(west_x > -1 && (cost_towest < BLOCKED)) {
                        pathcost_t west = all_pathcost[
                                to_index(west_x,west_y,xmax)];
                        if(comp_neighbour(cost_towest,target_x,target_y,
                                          &west)) {
                                change = 1;
                        }
                        all_pathcost[to_index(west_x,west_y,xmax)] = west;
                }
        }
        return change;
}
/*** Compares the cost of neighbouring coordinates relative to costs already ***
************** assigned to them, updating if cheaper cost found ***************/

int
comp_neighbour(int cost_toloc, int target_x, int target_y, pathcost_t *loc){
        int change = 0;
        /* if the cost from current position is cheaper */
        if(cost_toloc < loc->total_cost) {
                loc->total_cost = cost_toloc;
                loc->from.x = target_x;
                loc->from.y = yitoa(target_y);
                change = 1;
        }
        /* if the cost is equal sorting lexicographically */
        else if(cost_toloc == loc->total_cost) {
                /* If new x coordinate is smaller than currently source */
                if(target_x < loc->from.x) {
                        loc->from.x = target_x;
                        loc->from.y = yitoa(target_y);
                        change = 1;
                }
                /* New x coordinate is equal to currently assigned source and
                   y coordinate is smaller than that of the current source */
                else if((target_x == loc->from.x) &&
                        (target_y< yatoi(loc->from.y))) {
                        loc->from.x = target_x;
                        loc->from.y = yitoa(target_y);
                        change = 1;
                }
        }
        return change;
}

/*** Funtion to navigate and store path to destination from target location ***/

void
print_path(point_t dest, point_t origin, pathcost_t *all_pathcost, int xmax){
        /* Memory allocation for path array */
        int path_size = MAX_DIRECTIONS;
        pathcost_t *path = malloc( path_size * sizeof(*path));
        assert(path != NULL);
        /* Initialising coordinate to compare with target coordinate */
        point_t check;
        check.x = dest.x;
        check.y = dest.y;
        /* Following path untill origin is reached and populating path array */
        int points_added = 0;
        pathcost_t add;
        while(!((check.x == origin.x) && (check.y == origin.y))) {
                /* If the array is large enough to add another point in path */
                if(points_added < path_size) {
                        add = all_pathcost[to_index(check.x,
                                                    yatoi(check.y),xmax)];
                        path[points_added] = add;
                        check.x = add.from.x;
                        check.y = add.from.y;
                        points_added++;
                }
                /* If the array is not large enough realloc */
                else {
                        path_size = DOUBLE * path_size;
                        path = realloc(path, path_size * sizeof(*path));
                        assert(path != NULL);
                        add = all_pathcost[to_index(check.x,
                                                    yatoi(check.y),xmax)];
                        path[points_added] = add;
                        check.x = add.from.x;
                        check.y = add.from.y;
                        points_added++;
                }
        }
        /* Printing path from origin to destination */
        int count;
        pathcost_t print_point;
        printf("S2: start at grid %d%c, cost of 0\n",origin.x,origin.y);
        for(count = (points_added-1); count >= 0; count--) {
                print_point.location.x = path[count].location.x;
                print_point.location.y = path[count].location.y;
                print_point.total_cost = path[count].total_cost;
                printf("S2:      then  to %d%c, cost of %d\n",
                       print_point.location.x,print_point.location.y,
                       print_point.total_cost);
        }
        /* Freeing allocated memory */
        free(path);
        path = NULL;
}

/**** Function to convert input x,y coordinates to index position in array ****/

int
to_index(int x, int y, int xmax){
        return ((y * xmax) + x);
}

/** Funcitons to convert Y coordinate to integer index location or viceversa **/

int yatoi(char y){
        return y-CHARY_TO_INTY;
}
char yitoa(int y){
        return y+CHARY_TO_INTY;
}

/*********************** ALGORITHMS ARE FUN!!! ********************************/
