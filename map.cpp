#include "map.h"

#include "globals.h"
#include "graphics.h"
#define BUCKET_NUM 250
/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map map[2];
static int active_map;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
   return(Y*map_width() + X);
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned int map_hash(unsigned key){
    return(key%BUCKET_NUM);
}

void maps_init()
{
    HashTable* ht = createHashTable(map_hash, BUCKET_NUM);
    map[0].items = ht;
    map[0].w = 50;
    map[0].h = 50;
	
	HashTable* ht2 = createHashTable(map_hash, BUCKET_NUM);
	map[1].items = ht2;
	map[1].w = 50;
	map[1].h = 50;
}

Map* get_active_map()
{
    // There's only one map
    return &map[active_map];
}

Map* set_active_map(int m)
{
    active_map = m;
    return &map[active_map];
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P', 'N', 'M'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    return(get_active_map()->w);
}

int map_height()
{
    return(get_active_map()->h);
}

int map_area()
{
    return((get_active_map()->h)*(get_active_map()->w));
}

MapItem* get_north(int x, int y)
{
    if((y-1)<0){
    return NULL;
    }
    MapItem* north = (MapItem*)getItem(get_active_map()->items, XY_KEY(x, y-1));
    return(north);
}

MapItem* get_south(int x, int y)
{
    if((y+1)>49){
    return NULL;
    }
    MapItem* south = (MapItem*)getItem(get_active_map()->items, XY_KEY(x, y+1));
    return(south);
}

MapItem* get_east(int x, int y)
{
    if((x+1)>49){
        return NULL;
    }
    MapItem* east = (MapItem*)getItem(get_active_map()->items, XY_KEY(x+1, y));
    return(east);    
}

MapItem* get_west(int x, int y)
{
    if((x-1)<0){
    return NULL;
    }
    MapItem* west = (MapItem*)getItem(get_active_map()->items, XY_KEY(x-1, y));
    return(west);
}

MapItem* get_here(int x, int y)
{
    MapItem* this_square = (MapItem*)getItem(get_active_map()->items, XY_KEY(x,y));
    return(this_square);
}


void map_erase(int x, int y)
{
    deleteItem(get_active_map()->items, XY_KEY(x,y));
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_plant(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_pine_tree;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_NPC(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC;
    w1->draw = draw_NPC;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it 
}
void add_maze_portal(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = MAZE_PORTAL;
    w1->draw = draw_maze_portal;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it 
}