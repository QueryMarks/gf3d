#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_config_def.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"
#include "gfc_string.h"
#include "gfc_actions.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"

#include "model.h"
#include "gf3d_mesh.h"

#include "window.h"

#include "entity.h"
#include "element.h"
#include "player.h"
#include "collider.h"

#include "enemy.h"
#include "rpg_battler.h"

#include "puzzle.h"

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

void parse_arguments(int argc,char *argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}

void make_platform(GFC_Vector3D position, GFC_Vector3D scale)
{
    Entity* test_ground_ent = entity_new();
    test_ground_ent->position = position;
    test_ground_ent->solid = 1;
    test_ground_ent->model = model_load("models/cube.model");
    GFC_Box ground_box = gfc_box(0, 0, 0, 2, 2, 2);
    //GFC_Vector3D ground_scale = gfc_vector3d(40, 40, 1);
    test_ground_ent->scale = scale;
    //test_ground_ent->rotation.z = 1;
    //test_ground_ent->rotation.y = 1;

    GFC_Vector3D ground_box_scaled_vector = gfc_vector3d_multiply(gfc_vector3d(ground_box.w, ground_box.h, ground_box.d), scale);
    GFC_Box ground_box_scaled = gfc_box(
        ground_box.x,
        ground_box.y,
        ground_box.z,
        ground_box_scaled_vector.x,
        ground_box_scaled_vector.y,
        ground_box_scaled_vector.z);

    Collider* ground_collider = collider_new(ground_box_scaled, false, test_ground_ent);
    test_ground_ent->collider = ground_collider;
    ground_collider->entity = test_ground_ent;
    collider_reposition(test_ground_ent->collider);
    ground_collider = test_ground_ent->collider;
}


int main(int argc,char *argv[])
{
    //local variables
    Sprite *bg;
    Model* agumon;
    Entity* agumon_ent;

    GFC_Matrix4 mat;
    GFC_Matrix4* view;
    GFC_Vector3D offset = { 0,-15,0 };
    //initializtion    
    parse_arguments(argc,argv);
    init_logger("gf3d.log",0);
    slog("gf3d begin");
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);
    slog("gfc initiated");
    //gf3d init

    gf3d_vgraphics_init("config/setup.cfg");
    slog("vgraphics initiated");
    gf2d_font_init("config/font.cfg");
    slog("font initiated");
    gf2d_actor_init(1000);
    slog("actor initiated");
    entity_init(256);

    collider_manager_init(1024);

    rpg_battle_manager_init();
    slog("gf3d initiated");
    //init audio here too please
    
    //game init

    gfc_matrix4_identity(mat);
    view = gf3d_vgraphics_get_view_matrix();
    slog("got gf3d matrix");

    srand(SDL_GetTicks());
    slog_sync();
    bg = gf2d_sprite_load_image("images/bg_flat.png");
    gf2d_mouse_load("actors/mouse.actor");
    window_manager_init(128);

    
    //agumon_ent->model = model_load("models/dino/dino.model");
    //agumon_ent->rotation.z = 90;
    
    agumon_ent = player_spawn(gfc_vector3d(0, 0, 0));
    agumon_ent->colorMod = GFC_COLOR_MAGENTA;

    make_platform(
        gfc_vector3d(0, 0, -1),
        gfc_vector3d(40, 40, 1)
    );

    make_platform(
        gfc_vector3d(70, 5, -1),
        gfc_vector3d(20, 20, 1)
    );


    
    //test enemy
    Entity* test_enemy = enemy_entity_new(gfc_vector3d(5,5,1));
 

    enemy_entity_new(gfc_vector3d(10, 20, 1));

    enemy_entity_new(gfc_vector3d(-20, -20, 1));

    enemy_entity_new(gfc_vector3d(90, -30, 1));
    enemy_entity_new(gfc_vector3d(100, 40, 1));

    Entity* door = wood_door_entity_new(gfc_vector3d(-5, -3, 4));
    wood_door_entity_new(gfc_vector3d(-10, -3, 4));

    grate_door_entity_new(gfc_vector3d(4, -3, 4));


    //commented out because it takes forever to load
    Entity* awesome_purple_agumon = awesome_purple_agumon_entity_new(gfc_vector3d(70, 10, 1));



    /*
    Entity* test_ground_ent2 = entity_new();
    test_ground_ent2->position = gfc_vector3d(-1, -1, 1);
    test_ground_ent2->solid = 1;
    test_ground_ent2->model = model_load("models/cube.model");
    GFC_Box ground_box2 = gfc_box(0, 0, 0, 2, 2, 2);
    Collider* ground_collider2 = collider_new(ground_box2, false, test_ground_ent2);
    test_ground_ent2->collider = ground_collider2;
    test_ground_ent2->scale = gfc_vector3d(2, 2, 2);
    */
    

    GFC_Vector3D cam_pos;
    GFC_Vector3D cam_target = gfc_vector3d(0, 0, 0);
    slog("making a new awesome window");
    //Window *window = window_new();
    //Element* window_text = text_new(window, gfc_vector2d(0, 0), gfc_vector2d(500, 300), "text");
    
    slog("game loop about to start");
    
    // main game loop    
    while(!_done)
    {
        //slog("we are in game loop");
        gfc_input_update();
        gf2d_mouse_update();
        //slog("just before font update...?");
        gf2d_font_update();
        //camera updaes
        //slog("right before graphics render start");
        gf3d_vgraphics_render_start();
        //slog("after graphics render start");
                //2D draws
                //gf2d_sprite_draw_image(bg,gfc_vector2d(0,0));
                if (rpg_battle_get_active())
                {
                    cam_target = gfc_vector3d(agumon_ent->position.x + 2, agumon_ent->position.y + 2, agumon_ent->position.z);
                    gfc_vector3d_add(cam_pos, agumon_ent->position, gfc_vector3d(0, -7, 3));
                }
                else
                {
                    cam_target = agumon_ent->position;
                    gfc_vector3d_add(cam_pos, agumon_ent->position, gfc_vector3d(0, -20, 10));
                }
                
                gfc_matrix4_view(*view, cam_pos, cam_target, gfc_vector3d(0, 0, 1));
                //slog("at window stuff");
                window_manager_think_all();
                //slog("after window think");
                window_manager_update_all();
                //slog("after window update");
                window_manager_draw_all();
                //slog("after window draw");
                collider_manager_check_collisions();
                entity_system_think();
                entity_system_update();
                entity_system_postupdate();
                entity_system_draw();

                if (rpg_battle_get_active())
                {
                    rpg_battle_think();
                    rpg_battle_update();
                    rpg_battle_draw();
                }
                

                //gf2d_font_draw_line_tag("ALT+F4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));
                
                

                //gf2d_font_draw_line_tag("SO COOL", FT_H1, GFC_COLOR_WHITE, gfc_vector2d(10, 200));
        gf3d_vgraphics_render_end();


        gf2d_mouse_draw();
        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    exit(0);
    slog_sync();
    return 0;
}

void parse_arguments(int argc,char *argv[])
{
    int a;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }    
}

void game_frame_delay()
{
    Uint32 diff;
    static Uint32 now;
    static Uint32 then;
    then = now;
    slog_sync();// make sure logs get written when we have time to write it
    now = SDL_GetTicks();
    diff = (now - then);
    if (diff < frame_delay)
    {
        SDL_Delay(frame_delay - diff);
    }
    fps = 1000.0/MAX(SDL_GetTicks() - then,0.001);
//     slog("fps: %f",fps);
}
/*eol@eof*/
