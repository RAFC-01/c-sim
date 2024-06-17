#define SDL_MAIN_HANDLED

#include "headers.h"

#include "defs.c"
#include "gfx.c"
#include "fonts.c"
#include "render_batcher.c"
#include "personalities.c"
#include "seed.c"
#include "console.c"
#include "pause_menu.c"

#define VA_ARGS(...) , ##__VA_ARGS__  // For variadic macros

// FIXME: This can only produce numbers up to RAND_MAX which is at least 32767
int random_int_between(int min, int max) {
  return min + (rand() % (max - min));
}

GFX_TEXTURE_ID terrains[1000][1000];

RenderBatcher render_batcher = {0};
int game_is_still_running = 1;

bool entity_has_personality(int entity_index, Personality personality) {
  return game_context.personalities[entity_index][personality] > 0;
}

Vec2 vec2_world_to_screen(Vec2 point) {
  Vec2 translated_point;
  translated_point.x = (point.x - render_context.camera.current.x) * render_context.camera.zoom + render_context.window_w * 0.5f;
  translated_point.y = (point.y - render_context.camera.current.y) * render_context.camera.zoom + render_context.window_h * 0.5f;
  return translated_point;
}

Vec2 vec2_screen_to_world(Vec2 point) {
  Vec2 translated_point;
  translated_point.x = (point.x - render_context.window_w * 0.5f) / render_context.camera.zoom + render_context.camera.current.x;
  translated_point.y = (point.y - render_context.window_h * 0.5f) / render_context.camera.zoom + render_context.camera.current.y;
  return translated_point;
}

FRect frect_world_to_screen(FRect rect) {
  FRect translated_frect;
  translated_frect.position = vec2_world_to_screen(rect.position);
  translated_frect.size = vec2_world_to_screen(rect.size);
  return translated_frect;
}

FRect frect_screen_to_world(FRect rect) {
  FRect translated_frect;
  translated_frect.position = vec2_screen_to_world(rect.position);
  translated_frect.size = vec2_screen_to_world(rect.size);
  return translated_frect;
}

void load_fonts() {
  init_japanese_character_sets(HIRAGANA_BIT | KATAKANA_BIT);

  init_latin_character_sets(BASIC_LATIN_BIT | LATIN_ONE_SUPPLEMENT_BIT);

  FontLoadParams font_parameters = {0};
  font_parameters.size = 24;
  font_parameters.renderer = render_context.renderer;
  font_parameters.character_sets = BASIC_LATIN_BIT | LATIN_ONE_SUPPLEMENT_BIT;
  font_parameters.outline_size = 1;

  render_context.fonts[0] = load_font("assets/OpenSans-Regular.ttf", font_parameters);
  font_parameters.size = 32;
  render_context.fonts[1] = load_font("assets/OpenSans-Regular.ttf", font_parameters);
  font_parameters.size = 64;
  render_context.fonts[2] = load_font("assets/OpenSans-Regular.ttf", font_parameters);
}

void create_tree() {
  float entity_width = 100.0f;
  int texture_id = 8;
  game_context.textures[game_context.entity_count] = (TextureComponent){.texture_id = texture_id, .size = {.x = entity_width}};

  float scale = entity_width / render_context.texture_atlas.size[texture_id].x;
  game_context.textures[game_context.entity_count].size.y = (float)(render_context.texture_atlas.size[texture_id].y * scale);

  game_context.health_current[game_context.entity_count] = 1000;
  game_context.health_max[game_context.entity_count] = 1000;

  strcpy(game_context.names[game_context.entity_count], "tree");  // FIXME: Use the safe version strcpy_s. PRs welcome

  game_context.selected[game_context.entity_count] = false;
  game_context.hovered[game_context.entity_count] = false;
  game_context.positions[game_context.entity_count] = (PositionComponent){
      .current_position =
          {
              .x = (float)random_int_between(-400, 400) * 100,
              .y = (float)random_int_between(-400, 400) * 100,
          },
  };
  game_context.positions[game_context.entity_count].previous_position = game_context.positions[game_context.entity_count].current_position;

  game_context.entity_count++;
}

void create_human(char *name) {
  float entity_width = 100.0f;
  int texture_id = random_int_between(0, 7);
  game_context.textures[game_context.entity_count] = (TextureComponent){.texture_id = texture_id, .size = {.x = entity_width}};

  float scale = entity_width / render_context.texture_atlas.size[texture_id].x;
  game_context.textures[game_context.entity_count].size.y = (float)(render_context.texture_atlas.size[texture_id].y * scale);

  game_context.health_current[game_context.entity_count] = random_int_between(10, 100);
  game_context.health_max[game_context.entity_count] = 100;

  strcpy(game_context.names[game_context.entity_count], name);  // FIXME: Use the safe version strcpy_s. PRs welcome

  game_context.selected[game_context.entity_count] = false;
  game_context.hovered[game_context.entity_count] = false;
  game_context.positions[game_context.entity_count] = (PositionComponent){
      .current_position =
          {
              .x = (float)random_int_between(-1000, 1000),
              .y = (float)random_int_between(-1000, 1000),
          },
  };
  game_context.positions[game_context.entity_count].previous_position = game_context.positions[game_context.entity_count].current_position;
  game_context.speeds[game_context.entity_count] = (SpeedComponent){
      .current_direction.x = (((float)(rand() % 400) - 200) / 100),
      .current_direction.y = (((float)(rand() % 400) - 200) / 100),
      .current_velocity = (float)random_int_between(40, 55),
  };

  int random_amount_of_personalities = random_int_between(5, 10);
  for (int i = 0; i < random_amount_of_personalities; i++) {
    int personality = random_int_between(0, Personality_Count);
    game_context.personalities[game_context.entity_count][personality] = random_int_between(0, 100);
  }

  game_context.entity_count++;
}

void create_entities() {
  char entity_names[][32] = {
      "pushqrdx",
      "Athano",
      "AshenHobs",
      "adrian_learns",
      "RVerite",
      "Orshy",
      "ruggs888",
      "Xent12",
      "nuke_bird",
      "kasper_573",
      "SturdyPose",
      "coffee_lava",
      "goudacheeseburgers",
      "ikiwixz",
      "NixAurvandil",
      "smilingbig",
      "tk_dev",
      "realSuperku",
      "Hoby2000",
      "CuteMath",
      "forodor",
      "Azenris",
      "collector_of_stuff",
      "EvanMMO",
      "thechaosbean",
      "Lutf1sk",
      "BauBas9883",
      "physbuzz",
      "rizoma0x00",
      "Tkap1",
      "GavinsAwfulStream",
      "Resist_0",
      "b1k4sh",
      "nhancodes",
      "qcircuit1",
      "fruloo",
      "programmer_jeff",
      "BluePinStudio",
      "Pierito95RsNg",
      "jumpylionnn",
      "Aruseus",
      "lastmiles",
      "soulfoam",
      "AQtun81",
      "jess_forrealz",
      "RAFi18",
      "Delvoid",
      "Lolboy_30",
      "VevenVelour",
      "Kisamius",
      "tobias_bms",
      "spectral_ray1",
      "Toasty",  // AKA CarbonCollins
      "Roilisi",
      "MickyMaven",
      "Katsuida",
      "YogiEisbar",
      "WaryOfDairy",
      "BauBas9883",
      "Kataemoi",
      "AgentulSRI",
      "Pushtoy",
      "Neron0010",
      "exodus_uk",
      "Coopert1n0",
      "mantra4aa",
      "Keikzz",
      "sreetunks",
      "noisycat3",
      "ca2software",
      "GyrosGeier",
      "GloriousSir",
      "kuviman",
      "nigelwithrow",
      "pgorley",
      "Kasie_SoftThorn",
      "tapir2342",
      "Protonmat",
      "davexmachina_",
      "seek1337",
      "godmode0071",
      "cakez77",
      "TravisVroman",
      "Deharma",
      "Rogue_Wolf_Dev",
      "Tuhkamjuhkam",
      "lolDayzo",
      "retromaximusplays",
      "nickely",
      "MaGetzUb",
      "capuche_man",
      "MrElmida",
      "Zanarias",
      "dasraizer",
  };

  for (int name_index = 0; name_index < array_count(entity_names); name_index++) {
    create_human(entity_names[name_index]);
  }

  for (int i = 0; i < 9999; i++) {
    create_tree();
  }
}

FRect get_camera_rect() {
  FRect camera_rect = {
      .position =
          {
              .x = -300.0f,
              .y = -300.0f,
          },
      .size =
          {
              .x = (float)render_context.window_w,
              .y = (float)render_context.window_h,
          },
  };

  return camera_rect;
}

FRect get_entity_render_rect(int entity_id) {
  FRect texture_rect = {
      .position =
          {.x = game_context.positions[entity_id].current_position.x * (float)physics_context.alpha +
                game_context.positions[entity_id].previous_position.x * (float)(1.0 - physics_context.alpha),
           .y = game_context.positions[entity_id].current_position.y * (float)physics_context.alpha +
                game_context.positions[entity_id].previous_position.y * (float)(1.0 - physics_context.alpha)}
  };
  texture_rect.size.x = texture_rect.position.x + game_context.textures[entity_id].size.x;
  texture_rect.size.y = texture_rect.position.y + game_context.textures[entity_id].size.y;

  return texture_rect;
}

void draw_entity_name_batched(int entity_id, RenderBatcher *batcher) {
  Font *font = &render_context.fonts[0];
  RGBA color = (RGBA){1, 1, 1, 1};
  FRect entity_render_rect = get_entity_render_rect(entity_id);
  FRect entity_screen_rect = frect_world_to_screen(entity_render_rect);

  float y = (entity_screen_rect.position.y - (45.0f));

  if (game_context.hovered[entity_id]) {
    y -= 10.0f;  // move the text up a little when using the bigger font
    color = (RGBA){1, 1, 0, 1};
    font = &render_context.fonts[1];
  }

  Vec2 text_size = get_text_size(game_context.names[entity_id], font, false, true);

  float diff = ((entity_screen_rect.size.x - entity_screen_rect.position.x) / 2) - (text_size.x / 2);
  float x = entity_screen_rect.position.x + diff;

  draw_text_outlined_utf8_batched(game_context.names[entity_id], (Vec2){x, y}, color, (RGBA){0, 0, 0, 1}, font, batcher);
}

void draw_debug_text(int index, char *str, ...) {
  char text_buffer[128];
  va_list args;
  va_start(args, str);
  int chars_written = vsnprintf(text_buffer, sizeof(text_buffer), str, args);
  assert(chars_written > 0);
  va_end(args);

  draw_text_outlined_utf8(text_buffer, (Vec2){10.0f, (32.0f * index)}, (RGBA){0, 1, 0, 1}, (RGBA){0, 0, 0, 1}, &render_context.fonts[0]);
}

FRect get_selection_rect() {
  FRect rect =
      {.position =
           {
               .x = min(mouse_state.position.x, render_context.selection.position.x),
               .y = min(mouse_state.position.y, render_context.selection.position.y),
           },
       .size = {
           .x = fabsf(mouse_state.position.x - render_context.selection.position.x),
           .y = fabsf(mouse_state.position.y - render_context.selection.position.y),
       }};

  rect.size.x = rect.position.x + rect.size.x;
  rect.size.y = rect.position.y + rect.size.y;

  return rect;
}

void render_debug_info() {
  int index = 0;
  draw_debug_text(index++, "fps: %.2f", render_context.fps);
  draw_debug_text(index++, "mouse state: %d, button: %d, clicks: %d", mouse_state.state, mouse_state.button, mouse_state.clicks);
  draw_debug_text(index++, "prev mouse state: %d", mouse_state.prev_state);
  draw_debug_text(index++, "camera zoom: %.1f", render_context.camera.target_zoom);
  draw_debug_text(index++, "game speed: %.1f", physics_context.simulation_speed);
  draw_debug_text(
      index++, "camera: current x,y: %.2f,%.2f target x,y: %.2f,%.2f", render_context.camera.current.x, render_context.camera.current.y,
      render_context.camera.target.x, render_context.camera.target.y
  );
  FRect selection_rect = get_selection_rect();
  draw_debug_text(
      index++, "selection: current x,y: %.2f,%.2f target x,y: %.2f,%.2f", selection_rect.position.x, selection_rect.position.y,
      render_context.selection.target.x, render_context.selection.target.y
  );
}

void draw_selection_box() {
  if (!mouse_primary_pressed(mouse_state) || game_context.in_pause_menu) {
    return;
  }

  FRect selection_rect = get_selection_rect();

  gfx_draw_frect(&selection_rect, &(RGBA){1, 1, 1, 1});
}

void draw_personalities(int entity_id, FRect around) {
  // TODO: This should probably be sorted on creation, so we don't need to do it on every render
  int sorted_personality_ids[16] = {0};
  int length = 0;

  for (int personality_i = 0; personality_i < Personality_Count; personality_i++) {
    if (entity_has_personality(entity_id, personality_i)) {
      int score = game_context.personalities[entity_id][personality_i];

      if (length == 0) {
        sorted_personality_ids[0] = personality_i;
        length++;
        continue;
      }

      for (int i = 0; i < length; i++) {
        if (score > game_context.personalities[entity_id][sorted_personality_ids[i]]) {
          memmove(sorted_personality_ids + i + 1, sorted_personality_ids + i, (length - i) * sizeof(int));
          sorted_personality_ids[i] = personality_i;
          length++;
          break;
        }

        if (i == length - 1) {
          sorted_personality_ids[length] = personality_i;
          length++;
          break;
        }
      }
    }
  }

  char text_buffer[128];
  for (int index = 0; index < length; index++) {
    sprintf(
        text_buffer, "%d: %s", game_context.personalities[entity_id][sorted_personality_ids[index]],
        Personality__Strings[sorted_personality_ids[index]]
    );
    draw_text_outlined_utf8(
        text_buffer, (Vec2){around.position.x, (around.size.y + 10.0f + (32.0f * index))}, (RGBA){1, 1, 1, 1}, (RGBA){0, 0, 0, 1},
        &render_context.fonts[0]
    );
  }
}

float Spring__update(Spring *spring, float target) {
  spring->target = target;
  spring->velocity += (target - spring->current) * spring->acceleration;
  spring->velocity *= spring->friction;
  return spring->current += spring->velocity;
}

void draw_health_bar(int entity_id, FRect entity_rect) {
  const float y = (entity_rect.position.y - 15.0f * min(render_context.camera.zoom, 1.0f));
  const float h = (10.0f * min(render_context.camera.zoom, 1.0f));

  FRect total_health_rect = {
      .position.x = entity_rect.position.x,
      .position.y = y,
      .size.x = entity_rect.size.x,
      .size.y = y + h,
  };
  gfx_draw_frect_filled(&total_health_rect, &(RGBA){0, 0, 0, 1});

  float size_x = frect_width(&entity_rect);
  float health_width = (game_context.health_max[entity_id] - game_context.health_current[entity_id]) * size_x / game_context.health_max[entity_id];
  FRect current_health_rect = {
      .position.x = entity_rect.position.x,
      .position.y = y,
      .size.x = entity_rect.size.x - health_width,
      .size.y = y + h,
  };
  gfx_draw_frect_filled(&current_health_rect, &(RGBA){1, 0, 0, 1});
}

void draw_border(FRect around, float gap_width, float border_width) {
  FRect borders[4];

  //         1
  //   |-----------|
  //   |           |
  // 0 |           | 2
  //   |           |
  //   |-----------|
  //         3
  for (int i = 0; i < 4; i++) {
    borders[i] = around;

    float width = frect_width(&around);
    float height = frect_height(&around);

    if (i == 0) {  // Left (0)
      borders[i].position.x += -(gap_width + border_width);
      borders[i].position.y -= gap_width + border_width;
      borders[i].size.x = borders[i].position.x + border_width;
      borders[i].size.y = borders[i].position.y + height + (gap_width + border_width) * 2;
    } else if (i == 1) {  // Top (1)
      borders[i].position.x -= gap_width + border_width;
      borders[i].position.y += -(gap_width + border_width);
      borders[i].size.x = borders[i].position.x + width + (gap_width + border_width) * 2;
      borders[i].size.y = borders[i].position.y + border_width;
    } else if (i == 2) {  // right (2)
      borders[i].position.x = around.position.x + width + gap_width;
      borders[i].position.y -= gap_width + border_width;
      borders[i].size.x = borders[i].position.x + border_width;
      borders[i].size.y = borders[i].position.y + height + (gap_width + border_width) * 2;
    } else {  // bottom (3)
      borders[i].position.x -= gap_width + border_width;
      borders[i].position.y = around.position.y + height + gap_width;
      borders[i].size.x = borders[i].position.x + width + (gap_width + border_width) * 2;
      borders[i].size.y = borders[i].position.y + border_width;
    }

    gfx_draw_frect_filled(&borders[i], &(RGBA){1, 1, 1, 1});
  }
}

void move_entity(int entity_id) {
  game_context.positions[entity_id].previous_position = game_context.positions[entity_id].current_position;
  game_context.speeds[entity_id].previous_direction = game_context.speeds[entity_id].current_direction;
  game_context.speeds[entity_id].previous_velocity = game_context.speeds[entity_id].current_velocity;
  game_context.positions[entity_id].current_position.x += game_context.speeds[entity_id].current_direction.x *
                                                          game_context.speeds[entity_id].current_velocity *
                                                          (float)(physics_context.delta_time * physics_context.simulation_speed);
  game_context.positions[entity_id].current_position.y += game_context.speeds[entity_id].current_direction.y *
                                                          game_context.speeds[entity_id].current_velocity *
                                                          (float)(physics_context.delta_time * physics_context.simulation_speed);
}

void render_entity_batched(int entity_id, RenderBatcher *batcher) {
  FRect entity_render_rect = get_entity_render_rect(entity_id);
  FRect entity_screen_rect = frect_world_to_screen(entity_render_rect);
  FRect entity_shadow_rect = entity_screen_rect;

  float entity_shadow_height = entity_shadow_rect.size.y - entity_shadow_rect.position.y;
  entity_shadow_rect.position.y += (entity_shadow_height * 0.8f);
  entity_shadow_rect.size.y += (entity_shadow_height * 0.2f);

  render_batcher_copy_texture_quad(
      batcher, render_context.texture_atlas.textures[GFX_TEXTURE_SHADOW], &(RGBA){1, 1, 1, 0.25}, &entity_shadow_rect, NULL
  );

  render_batcher_copy_texture_quad(
      batcher, render_context.texture_atlas.textures[game_context.textures[entity_id].texture_id], &(RGBA){1, 1, 1, 1}, &entity_screen_rect, NULL
  );

  if (render_context.camera.zoom > 0.5f) {
    draw_health_bar(entity_id, entity_screen_rect);
  }

  // FIXME: Make this faster using the render batcher
  if (game_context.selected[entity_id]) {
    draw_border(entity_screen_rect, 5.0f, 4.0f);

    // Draw the personalities list
    draw_personalities(entity_id, entity_screen_rect);
  }
}

void draw_grid() {
  gfx_set_blend_mode_blend();
  float grid_size = 256.0f;
  float window_w = (float)render_context.window_w;
  float window_h = (float)render_context.window_h;

  FRect grid = {
      .position.x = (0 - render_context.camera.current.x) * render_context.camera.zoom + render_context.window_w / 2,
      .position.y = (0 - render_context.camera.current.y) * render_context.camera.zoom + render_context.window_h / 2,
      .size.x = grid_size * render_context.camera.zoom,
      .size.y = grid_size * render_context.camera.zoom,
  };

  float x_start = grid.position.x - floorf(grid.position.x / grid.size.x) * grid.size.x;
  for (float x = x_start; x < window_w; x += grid.size.x) {
    gfx_draw_line(x, 0, x, window_h, &(RGBA){0, 0, 0, 0.15f});
  }

  float y_start = grid.position.y - floorf(grid.position.y / grid.size.y) * grid.size.y;
  for (float y = y_start; y < window_h; y += grid.size.y) {
    gfx_draw_line(0, y, window_w, y, &(RGBA){0, 0, 0, 0.15f});
  }

  gfx_set_blend_mode_none();
}

void generate_grass_textures() {
  // GFX_TEXTURE_ID assigned_textures[16][16];
  // int column = 0;
  // int row = 0;

  for (int y = 0; y < 1000; y++) {
    for (int x = 0; x < 1000; x++) {
      // Now get the next possible texture_id based on the current one. Loop over the textures.
      int previous_right_grass_type = (LONG_GRASS_RIGHT | OVERGROWN_GRASS_RIGHT | SHORT_GRASS_RIGHT);
      if (x > 0) {
        // We're on the top column, we don't care about what was above us.
        previous_right_grass_type = grass_textures[terrains[y][x - 1]] & (LONG_GRASS_RIGHT | OVERGROWN_GRASS_RIGHT | SHORT_GRASS_RIGHT);
      }

      int previous_bottom_grass_type = (LONG_GRASS_BOTTOM | OVERGROWN_GRASS_BOTTOM | SHORT_GRASS_BOTTOM);
      if (y > 0) {
        // We're no longer on the top, we need to consider what was above.
        previous_bottom_grass_type = grass_textures[terrains[y - 1][x]] & (LONG_GRASS_BOTTOM | OVERGROWN_GRASS_BOTTOM | SHORT_GRASS_BOTTOM);
      }

      int possible_textures[64] = {0};
      int possible_textures_count = 0;
      for (int texture_id = GFX_TEXTURE_GRASS_LONG_CENTER; texture_id <= GFX_TEXTURE_GRASS_SHORT_OVERGROWN_TOP; texture_id++) {
        // { return tile.type & rightType != 0 && tile.type & topType != 0;});
        if ((grass_textures[texture_id] & (previous_right_grass_type << 2)) != 0 &&
            (grass_textures[texture_id] & (previous_bottom_grass_type >> 2)) != 0) {
          possible_textures[possible_textures_count] = texture_id;
          possible_textures_count++;
        }
      }

      int chosen_texture_id = possible_textures[random_int_between(0, possible_textures_count)];
      terrains[y][x] = chosen_texture_id;
    }
  }
}

void draw_terrain(RenderBatcher *batcher) {
  gfx_set_blend_mode_blend();
  float grid_size = 256.0f * render_context.camera.zoom;

  // FRect terrain = {
  //     .position.x = (0 + render_context.camera.current.x),
  //     .position.y = (0 + render_context.camera.current.y),
  //     .size.x = grid_size,
  //     .size.y = grid_size,
  // };

  // float x_start = max(0, floorf((terrain.position.x - (render_context.window_w / 2)) / grid_size));
  // float y_start = max(0, floorf((terrain.position.y - (render_context.window_h / 2)) / grid_size));

  // float x_start = (grid.position.x - floorf(grid.position.x / grid.size.x) * grid.size.x) - grid.size.x;
  // float y_start = (grid.position.y - floorf(grid.position.y / grid.size.y) * grid.size.y) - grid.size.y;

  RGBA color = {1, 1, 1, 1};
  // GFX_TEXTURE_ID grass_texture_id = GFX_TEXTURE_GRASS_LONG_CENTER;

  // Vertical array length is window_h / grid.size.y
  // Horizontal array length is window_w / grid.size.x

  for (float y = 0; y < (render_context.window_h / grid_size); y += 1) {
    for (float x = 0; x < (render_context.window_w / grid_size); x += 1) {
      Vec2 grid_screen_position = (Vec2){
          .x = (x + render_context.camera.current.x),
          .y = (y + render_context.camera.current.y),
      };

      render_batcher_copy_texture_quad(
          batcher, render_context.texture_atlas.textures[terrains[(int)grid_screen_position.y][(int)grid_screen_position.x]], &color,
          &(FRect){
              .position.x = grid_screen_position.x,
              .position.y = grid_screen_position.y,
              .size.x = grid_screen_position.x + grid_size,
              .size.y = grid_screen_position.y + grid_size,
          },
          NULL
      );
    }
  }
  gfx_set_blend_mode_none();
}

void mouse_control_camera() {
  if (mouse_state.button == SDL_BUTTON_RIGHT && mouse_state.state == SDL_PRESSED) {
    if (mouse_state.prev_position.x != mouse_state.position.x || mouse_state.prev_position.y != mouse_state.position.y) {
      float delta_x = mouse_state.position.x - mouse_state.prev_position.x;
      float delta_y = mouse_state.position.y - mouse_state.prev_position.y;
      mouse_state.prev_position.x = mouse_state.position.x;
      mouse_state.prev_position.y = mouse_state.position.y;

      render_context.camera.target.x -= delta_x / render_context.camera.zoom;
      render_context.camera.target.y -= delta_y / render_context.camera.zoom;
    }
  }
}

void deselect_all_entities() {
  loop(game_context.entity_count, entity_id) {
    game_context.selected[entity_id] = false;
  }
}

// Camera movement and selection rect movement
void keyboard_control_camera() {
  float camera_keyboard_movement_speed = 15.0f;
  if (render_context.keyboard_state[SDL_GetScancodeFromKey(SDLK_w)]) {
    deselect_all_entities();
    render_context.camera.target.y -= camera_keyboard_movement_speed / render_context.camera.zoom;
    render_context.selection.target.y += camera_keyboard_movement_speed;
  }
  if (render_context.keyboard_state[SDL_GetScancodeFromKey(SDLK_s)]) {
    deselect_all_entities();
    render_context.camera.target.y += camera_keyboard_movement_speed / render_context.camera.zoom;
    render_context.selection.target.y -= camera_keyboard_movement_speed;
  }
  if (render_context.keyboard_state[SDL_GetScancodeFromKey(SDLK_a)]) {
    deselect_all_entities();
    render_context.camera.target.x -= camera_keyboard_movement_speed / render_context.camera.zoom;
    render_context.selection.target.x += camera_keyboard_movement_speed;
  }
  if (render_context.keyboard_state[SDL_GetScancodeFromKey(SDLK_d)]) {
    deselect_all_entities();
    render_context.camera.target.x += camera_keyboard_movement_speed / render_context.camera.zoom;
    render_context.selection.target.x -= camera_keyboard_movement_speed;
  }
}

int get_entity_to_follow() {
  int result = INVALID_ENTITY;
  int selected_count = 0;
  loop(game_context.entity_count, entity_id) {
    if (game_context.selected[entity_id]) {
      selected_count += 1;
      result = entity_id;
    }
  }
  return selected_count == 1 ? result : INVALID_ENTITY;
}

// Set selected on any entity within the selection_rect
void select_entities_within_selection_rect() {
  loop(game_context.entity_count, entity_id) {
    FRect entity_render_rect = get_entity_render_rect(entity_id);
    FRect entity_screen_rect = frect_world_to_screen(entity_render_rect);

    Vec2 point_top_left = {
        .x = entity_screen_rect.position.x,
        .y = entity_screen_rect.position.y,
    };
    Vec2 point_bottom_right = {
        .x = entity_screen_rect.size.x,
        .y = entity_screen_rect.size.y,
    };

    FRect selection_rect = get_selection_rect();

    if (selection_rect.size.x > 3.0f) {
      if (gfx_frect_contains_point(&selection_rect, &point_top_left) && gfx_frect_contains_point(&selection_rect, &point_bottom_right)) {
        game_context.selected[entity_id] = true;
      } else {
        if (!render_context.keyboard_state[SDL_GetScancodeFromKey(SDLK_LSHIFT)]) {
          game_context.selected[entity_id] = false;
        }
      }
    }
  }
}

bool is_entity_under_mouse(int entity_id) {
  FRect entity_render_rect = get_entity_render_rect(entity_id);
  FRect rect = frect_world_to_screen(entity_render_rect);

  return gfx_frect_contains_point(&rect, &mouse_state.position);
}

void update() {
  // Spring the camera zoom
  render_context.camera.zoom = Spring__update(&render_context.camera.zoom_spring, render_context.camera.target_zoom);

  // Spring the console position
  console.y = Spring__update(&console.y_spring, console.target_y);

  mouse_control_camera(&mouse_state);

  if (game_context.in_pause_menu) {
    return;
  }

  if (!console_is_open()) {
    keyboard_control_camera();
  }

  if (physics_context.simulation_speed > 0.0) {
    loop(game_context.entity_count, entity_id) {
      move_entity(entity_id);
    }
  }

  // Spring the selection box
  render_context.selection.position.x = Spring__update(&render_context.selection.spring_x, render_context.selection.target.x);
  render_context.selection.position.y = Spring__update(&render_context.selection.spring_y, render_context.selection.target.y);
}

void handle_input() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
      mouse_state.prev_state = mouse_state.state;
      mouse_state.state = event.button.state;
      mouse_state.button = event.button.button;
      mouse_state.clicks = event.button.clicks;
      if (mouse_state.prev_state != SDL_PRESSED) {
        // Set selection target to the current mouse position
        render_context.selection.target.x = mouse_state.position.x;
        render_context.selection.target.y = mouse_state.position.y;
        // Reset selection spring so it doesn't spring between the old selection and the new one
        render_context.selection.spring_x.current = render_context.selection.target.x;
        render_context.selection.spring_y.current = render_context.selection.target.y;
      }
    }
    if (event.type == SDL_MOUSEMOTION) {
      mouse_state.prev_state = mouse_state.state;
      mouse_state.prev_position.x = mouse_state.position.x;
      mouse_state.prev_position.y = mouse_state.position.y;
      mouse_state.position.x = (float)event.motion.x;
      mouse_state.position.y = (float)event.motion.y;
    }
    if (event.type == SDL_QUIT) {
      game_context.game_is_still_running = 0;
      return;
    }
    if (console_is_open()) {
      console_handle_input(&event);
      return;
    }
    if (game_context.in_pause_menu) {
      pause_menu_handle_input(&event);
      return;
    }
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          bool entity_had_selection = false;

          // 1. If anything is selected, then deselect it and break.
          reverse_loop(game_context.entity_count, entity_id) {
            if (game_context.selected[entity_id]) {
              entity_had_selection = true;
              game_context.selected[entity_id] = false;
            }
          }

          if (entity_had_selection) {
            break;
          }

          // 2. If nothing was deselected, then open the pause menu.
          game_context.in_pause_menu = true;
          pause_menu.current_screen = PAUSE_MENU_MAIN;

          if (physics_context.simulation_speed > 0) {
            physics_context.prev_simulation_speed = physics_context.simulation_speed;
            physics_context.simulation_speed = 0;
          }

          break;
        case SDLK_UP:
          physics_context.simulation_speed += 0.5;
          physics_context.simulation_speed = min(physics_context.simulation_speed, 10.0);
          break;
        case SDLK_DOWN:
          physics_context.simulation_speed -= 0.5;
          physics_context.simulation_speed = max(physics_context.simulation_speed, 0.0);
          break;
        case SDLK_SPACE:
          if (physics_context.prev_simulation_speed > 0) {
            physics_context.simulation_speed = physics_context.prev_simulation_speed;
            physics_context.prev_simulation_speed = 0;
          } else {
            physics_context.prev_simulation_speed = physics_context.simulation_speed;
            physics_context.simulation_speed = 0;
          }
          break;
        case SDLK_TAB: {
          console_open();
          break;
        }
        default:
          break;
      }
    }
    if (event.type == SDL_MOUSEWHEEL) {
      if (event.wheel.y > 0) {
        // zoom in
        render_context.camera.target_zoom = min(render_context.camera.target_zoom + 0.1f, 2.0f);
      } else if (event.wheel.y < 0) {
        // zoom out
        render_context.camera.target_zoom = max(render_context.camera.target_zoom - 0.1f, 0.1f);
      }
    }

    // Two loops needed so we can have a case where multiple entities can be hovered over, but only one can be selected
    reverse_loop(game_context.entity_count, entity_id) {
      game_context.hovered[entity_id] = is_entity_under_mouse(entity_id);
    }

    reverse_loop(game_context.entity_count, entity_id) {
      if (is_entity_under_mouse(entity_id)) {
        if (mouse_state.button == SDL_BUTTON_LEFT && mouse_state.state == SDL_PRESSED && mouse_state.prev_state == SDL_RELEASED) {
          game_context.selected[entity_id] = !game_context.selected[entity_id];
          break;
        }
      }
    }
  }
}

void render() {
  gfx_clear_screen();

  draw_terrain(&render_batcher);

  flush_render_batcher(&render_batcher);

  draw_grid();

  FRect camera_rect = get_camera_rect();
  FRect translated_rect = frect_screen_to_world(camera_rect);

  loop(game_context.entity_count, entity_id) {
    FRect entity_render_rect = get_entity_render_rect(entity_id);
    if (gfx_frect_intersects_frect(&entity_render_rect, &translated_rect)) {
      render_entity_batched(entity_id, &render_batcher);
    }
  }

  if (render_context.camera.zoom > 0.5f) {
    loop(game_context.entity_count, entity_id) {
      FRect entity_render_rect = get_entity_render_rect(entity_id);
      if (gfx_frect_intersects_frect(&entity_render_rect, &translated_rect)) {
        draw_entity_name_batched(entity_id, &render_batcher);
      }
    }
  }

  flush_render_batcher(&render_batcher);

  if (mouse_primary_pressed(mouse_state)) {
    // Draw the selection box
    draw_selection_box();
  }

  render_debug_info(&mouse_state);

  pause_menu_draw();

  console_draw();

  gfx_render_present();
}

void update_timer(Timer *timer, double frame_time) {
  timer->accumulated += frame_time;
  if (timer->accumulated >= timer->interval) {
    timer->accumulated -= timer->interval;
  }
}

int main(int argc, char *args[]) {
  srand(create_seed("FOLLOWING_IS_KIND_OF_HARD"));

  int gfx_init_result = gfx_init();
  if (gfx_init_result == 1) {
    return EXIT_FAILURE;
  }

  render_context.background_color = (SDL_Color){35, 127, 178, 255};
  render_context.camera = (Camera){
      .zoom = 1.0f,
      .target_zoom = 1.0f,
      .pan_spring_x =
          {
              .target = 1.0f,
              .current = 1.0f,
              .velocity = 0.0f,
              .acceleration = 2.0f,
              .friction = 0.05f,
          },
      .pan_spring_y =
          {
              .target = 1.0f,
              .current = 1.0f,
              .velocity = 0.0f,
              .acceleration = 2.0f,
              .friction = 0.05f,
          },
      .zoom_spring =
          {
              .target = 1.0f,
              .current = 1.0f,
              .velocity = 0.0f,
              .acceleration = 0.4f,
              .friction = 0.1f,
          },
  };
  render_context.selection =
      (Selection){
          .spring_x =
              {
                  .target = 1.0f,
                  .current = 1.0f,
                  .velocity = 0.0f,
                  .acceleration = 0.5f,
                  .friction = 0.1f,
              },
          .spring_y =
              {
                  .target = 1.0f,
                  .current = 1.0f,
                  .velocity = 0.0f,
                  .acceleration = 0.5f,
                  .friction = 0.1f,
              },
      },
  console.y_spring = (Spring){
      .target = 0.0f,
      .current = 0.0f,
      .velocity = 0.1f,
      .acceleration = 0.5f,
      .friction = 0.1f,
  };
  game_context.game_is_still_running = 1;
  physics_context = (PhysicsContext){.delta_time = 0.01, .simulation_speed = 1.0};
  render_batcher = new_render_batcher(100000, render_context.renderer);
  render_context.timer[0] = (Timer){.interval = 100};  // Second timer
  render_context.timer[1] = (Timer){.interval = 60000};  // Minute timer

  gfx_load_textures();

  load_fonts();

  create_entities();
  generate_grass_textures();

  u32 start_ticks = SDL_GetTicks();
  int frame_count = 0;

  // Here we keep the frame_time within a reasonable bound. If a frame_time exceeds 250ms, we "give up" and drop simulation frames
  // This is necessary as if our frame_time were to become too large, we would effectively lock ourselves in an update cycle
  // and the simulation would fall completely out of sync with the physics being rendered
  float max_frame_time_threshold = 0.25;
  double accumulator = 0.0;
  double current_time = SDL_GetTicks64() / 1000.0;

  while (game_context.game_is_still_running) {
    double new_time = SDL_GetTicks64() / 1000.0;
    double frame_time = new_time - current_time;

    update_timer(&render_context.timer[0], frame_time);

    frame_count++;
    if (SDL_GetTicks() - start_ticks >= 1000) {
      render_context.fps = (float)frame_count;
      frame_count = 0;
      start_ticks = SDL_GetTicks();
    }

    gfx_get_window_size(&render_context.window_w, &render_context.window_h);
    render_context.keyboard_state = SDL_GetKeyboardState(NULL);

    if (frame_time > max_frame_time_threshold) {
      frame_time = max_frame_time_threshold;
    }

    current_time = new_time;
    accumulator += frame_time;

    handle_input();

    while (accumulator >= physics_context.delta_time) {
      update();
      accumulator -= physics_context.delta_time;
    }

    physics_context.alpha = min(accumulator / physics_context.delta_time, 1.0);

    // Set the alpha to 1.0 so that rendering is consistent when the simulation speed is 0.
    if (physics_context.simulation_speed == 0) {
      physics_context.alpha = 1.0;
    }

    // bool spring_camera = true;
    Vec2 camera_spring_distance = {
        .x = fabsf(render_context.camera.target.x - render_context.camera.current.x),
        .y = fabsf(render_context.camera.target.y - render_context.camera.current.y),
    };

    int entity_to_follow = get_entity_to_follow();

    if (mouse_primary_pressed(mouse_state)) {
      select_entities_within_selection_rect();
    } else {
      if (entity_to_follow != INVALID_ENTITY) {
        FRect entity_render_rect = get_entity_render_rect(entity_to_follow);
        render_context.camera.target.x = entity_render_rect.position.x + ((entity_render_rect.size.x - entity_render_rect.position.x) / 2);
        render_context.camera.target.y = entity_render_rect.position.y + ((entity_render_rect.size.y - entity_render_rect.position.y) / 2);

        if (camera_spring_distance.x < 0.5f && camera_spring_distance.y < 0.5f) {
          render_context.camera.current.x = entity_render_rect.position.x + ((entity_render_rect.size.x - entity_render_rect.position.x) / 2);
          render_context.camera.current.y = entity_render_rect.position.y + ((entity_render_rect.size.y - entity_render_rect.position.y) / 2);
        }
      }
    }

    if (camera_spring_distance.x > 0.5f || camera_spring_distance.y > 0.5f) {
      // Spring the camera position
      render_context.camera.current.x = Spring__update(&render_context.camera.pan_spring_x, render_context.camera.target.x);
      render_context.camera.current.y = Spring__update(&render_context.camera.pan_spring_y, render_context.camera.target.y);
    }

    render();
  }

  gfx_destroy();

  return EXIT_SUCCESS;
}
