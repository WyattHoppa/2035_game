// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"

// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
int main ();
int halt_action = 0;
/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key;
} Player;

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
int get_action(GameInputs inputs)
{
    if((inputs.ax > -.2) && (inputs.ax < .2) && (inputs.ay <0) && (inputs.az>0))
        return GO_DOWN;
    if((inputs.ax > -.2) && (inputs.ax < .2) && (inputs.ay >0) && (inputs.az>0))
        return GO_UP;
    if((inputs.ax > 0) && (inputs.ay > -.2) && (inputs.ay <0.2) && (inputs.az>0))
        return GO_RIGHT;
    if((inputs.ax < 0) && (inputs.ax > -.2) && (inputs.ay <0.2) && (inputs.az>0))
        return GO_LEFT;
    if(!inputs.b1)
        return ACTION_BUTTON;
    return NO_ACTION;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 *
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
#define DRAW_NPC_SPEECH 3
#define Get_RID_of_SPEECH 4
#define SWITCH_TO_MAZE 5
int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;

    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case GO_UP:  
        if(Player.y-1 < 0)
            return NULL
        MapItem* north = get_north(Player.x, Player.y);
        if((north->walkable) && (!halt_action)){
            Player.y = Player.y-1;
            return FULL_DRAW;
            }   
        break;
        case GO_LEFT: 
        if(Player.x-1 < 0)
            return NULL
        MapItem* west = get_west(Player.x, Player.y);
        if((west->walkable) && (!halt_action)){
            Player.x = Player.x-1;
            return FULL_DRAW;
            } 
        break;
        case GO_DOWN: 
        if(Player.y+1 < 0)
            return NULL
        MapItem* south = get_south(Player.x, Player.y);
        if((south->walkable) && (!halt_action)){
            Player.y = Player.y+1;
            return FULL_DRAW;
            }   
        break;
        case GO_RIGHT: 
        if(Player.x+1 < 0)
            return NULL
        MapItem* east = get_east(Player.x, Player.y);
        if((east->walkable) && (!halt_action)){
            Player.y = Player.x+1;
            return FULL_DRAW;
            }  
        break;
        case ACTION_BUTTON: 
		if(halt_action){
			halt_action = 0;
			return Get_RID_of_SPEECH;
		}
		if((get_north(Player.x, Player.y)->type == NPC) || (get_south(Player.x, Player.y)->type == NPC) || (get_east(Player.x, Player.y)->type == NPC) || (get_west(Player.x, Player.y)->type == NPC)){
			halt_action = 1;
			return DRAW_NPC_SPEECH;
		}
		if((get_north(Player.x, Player.y)->type == MAZE_PORTAL) || (get_south(Player.x, Player.y)->type == MAZE_PORTAL) || (get_east(Player.x, Player.y)->type == MAZE_PORTAL) || (get_west(Player.x, Player.y)->type == MAZE_PORTAL)){
			return SWITCH_TO_MAZE;
		}
        break;
        case MENU_BUTTON: 
        break;
        default:        
        break;
    }
    return NO_RESULT;
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status
 * bars. Unless init is nonzero, this function will optimize drawing by only
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    // Draw game border first
    if(init) draw_border();

    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)

            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;

            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;

            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;

            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0) // Only draw the player on init
            {
                draw_player(u, v, Player.has_key);
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars
    draw_upper_status();
    draw_lower_status(Player.x, Player.y);
	if(init == DRAW_NPC_SPEECH)
		draw_npc_speech();
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion.
 */
void init_main_map()
{
    // "Random" plants
    Map* map = set_active_map(0);
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_plant(i % map_width(), i / map_width());
    }
    pc.printf("plants\r\n");

    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
	add_NPC(30,20);
	add_maze_portal(45, 5);
    pc.printf("Walls done!\r\n");

    print_map();
	
	
	Map* map2 = set_active_map(1);
	add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
	
}


/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");

    // Initialize the maps
    maps_init();
    init_main_map();
    // Initialize game state
    set_active_map(0);
    Player.x = Player.y = 5;

    // Initial drawing
    draw_game(true);

    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();

        // Actuall do the game update:
        // 1. Read inputs
        GameInputs input = read_inputs();
        // 2. Determine action (get_action)
        int action = get_action(input);
        // 3. Update game (update_game)
        int response = update_game(action);
        // 3b. Check for game over
        // 4. Draw frame (draw_game)
		if(response == DRAW_NPC_SPEECH){
			draw_game(DRAW_NPC_SPEECH);
		}else if(response == Get_RID_of_SPEECH){
			draw_game(FULL_DRAW);
		}else if(response == SWITCH_TO_MAZE){
			set_active_map(1);
			Player.x = Player.y = 5;
			draw_game(FULL_DRAW);
		}else{
			draw_game(false);
		}
			
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
}