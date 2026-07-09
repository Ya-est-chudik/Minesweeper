#include <iostream> // используется в основном для отправки ошибок в консоль
#include <cstdlib>
#include <cstring>
#include <random>
#include <chrono>
#include <thread> 

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
// g++ Minesweeper.cpp -o Minesweeper.exe -lallegro -lallegro_image -lallegro_font - команда сборки программы
#define KEY_SEEN 1
#define KEY_DOWN 2

enum CELL_TYPE {
  CT_WOANY,
  CT_WONE,
  CT_WTWO,
  CT_WTHREE,
  CT_WFOUR,
  CT_WFIVE,
  CT_WSIX,
  CT_WSEVEN,
  CT_WEIGHT,
  CT_MINE,
  CT_CONV,
  CT_CONC,
  CT_FLAG,
};

struct CELL { // Клетка сапера
  float x;
  float y;
  uint16_t vis_type;
  uint16_t invis_type;
};

uint16_t count_mines(CELL** playground, int16_t i, int16_t j) {
  uint16_t mine_count = 0;
    
  for (int16_t di = -1; di <= 1; di++) {               // Вайбкокод
    for (int16_t dj = -1; dj <= 1; dj++) {             // Вайбкокод
      if (di == 0 && dj == 0) {                         // Вайбкокод
        continue;                                       // Вайбкокод
      }                                                 // Вайбкокод
      int16_t ni = i + di;                             // Вайбкокод
      int16_t nj = j + dj;                             // Вайбкокод
          
      if (ni >= 0 && ni < 16 && nj >= 0 && nj < 16) {   // Вайбкокод
        if (playground[ni][nj].invis_type == CT_MINE) { // Вайбкокод
          mine_count++;                                 // Вайбкокод
        }
      }
    }   
  }
  return mine_count;
}

void first_click_opening(CELL** playground, int16_t i, int16_t j) {
  playground[i][j].vis_type = playground[i][j].invis_type;
  if (playground[i][j].invis_type != CT_WOANY) {
    return;
  }
  for (int16_t di = -1; di < 2; di++) {
    for (int16_t dj = -1; dj < 2; dj++) {
      if (di == 0 && dj == 0) {
        continue;
      }
      int16_t ni = i + di;
      int16_t nj = j + dj;
      if (ni > -1 && ni < 16 && nj > -1 && nj < 16) {
        if (playground[ni][nj].vis_type == CT_CONC || playground[ni][nj].vis_type == CT_CONV) {
          first_click_opening(playground, ni, nj);
        }
      }
    }
  }
}

void must_init(bool initialization, const char* module) {
  if (initialization) {
    return;
  }
  std::cout << module << " init error" << std::endl;
  std::exit(1);
}

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, 15);
  
  must_init(al_init(), "Allegro");
  must_init(al_install_keyboard(), "Keyboard");
  must_init(al_init_image_addon(), "Image addon");
  must_init(al_install_mouse(), "mouse");

  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
  must_init(timer, "Timer");
  
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "Queue");

  ALLEGRO_DISPLAY* disp = al_create_display(480, 480);
  must_init(disp, "Display");
  
  ALLEGRO_FONT* font = al_create_builtin_font();
  must_init(font, "Font");
  
  ALLEGRO_MOUSE_STATE msestate;
  ALLEGRO_KEYBOARD_STATE kbdstate;
  
  ALLEGRO_BITMAP* bg = al_load_bitmap("images/cells/background.png");
  ALLEGRO_BITMAP* cCONV = al_load_bitmap("images/cells/not_oppened_cell(convex).png");
  ALLEGRO_BITMAP* cCONC = al_load_bitmap("images/cells/not_oppened_cell(concave).png");
  ALLEGRO_BITMAP* cMINE = al_load_bitmap("images/cells/mine_on_cell.png");
  ALLEGRO_BITMAP* cFLAG_RU = al_load_bitmap("images/cells/flagru_on_cell.png");
  ALLEGRO_BITMAP* cWOANY = al_load_bitmap("images/cells/empty_cell.png");
  ALLEGRO_BITMAP* cWONE = al_load_bitmap("images/cells/one_on_cell.png");
  ALLEGRO_BITMAP* cWTWO = al_load_bitmap("images/cells/two_on_cell.png");
  ALLEGRO_BITMAP* cWTHREE = al_load_bitmap("images/cells/three_on_cell.png");
  ALLEGRO_BITMAP* cWFOUR = al_load_bitmap("images/cells/four_on_cell.png");
  ALLEGRO_BITMAP* cWFIVE = al_load_bitmap("images/cells/five_on_cell.png");
  ALLEGRO_BITMAP* cWSIX = al_load_bitmap("images/cells/six_on_cell.png");
  ALLEGRO_BITMAP* cWSEVEN = al_load_bitmap("images/cells/seven_on_cell.png");
  ALLEGRO_BITMAP* cWEIGHT = al_load_bitmap("images/cells/eight_on_cell.png");

  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));
  al_register_event_source(queue, al_get_mouse_event_source());

  bool done = false;
  bool redraw = true;
  ALLEGRO_EVENT event;
  
  bool first_click = true;
  
  float mouse_x = 0;
  float mouse_y = 0;
  
  bool wpressed_1 = 0;
  bool wpressed_2 = 0;

  uint16_t pg_x_size = 16;
  uint16_t pg_y_size = 16; // В будущем для настройки размеров поля
  
  uint16_t new_cell_x = 80;
  uint16_t new_cell_y = 80;
  
  CELL** playground = new CELL*[16];
  for (uint16_t i = 0; i < 16; ++i) {
    playground[i] = new CELL[16];
  }
  
  for (uint16_t i = 0; i < 16; ++i) {
    for (uint16_t j = 0; j < 16; ++j) {
      CELL *b = &playground[i][j];
      b->x = new_cell_x;
      b->y = new_cell_y;
      b->vis_type = CT_CONC;
      b->invis_type = CT_WOANY;
      new_cell_x += 20;
    }
    new_cell_x = 80;
    new_cell_y += 20;
  }
  
  for (uint32_t i = 0; i < 55; ++i) {
    uint32_t xm = dist(gen);
    uint32_t ym = dist(gen);
    if (playground[xm][ym].invis_type == CT_MINE) {
      i--;
      continue;
    }
    playground[xm][ym].invis_type = CT_MINE;
  }
  
  for (uint16_t i = 0; i < 16; ++i) {
    for (uint16_t j = 0; j < 16; ++j) {
      CELL* b = &playground[i][j];
    
      if (b->invis_type == CT_MINE) {
        continue;
      }
        
      uint16_t mine_count = count_mines(playground, i, j);
        
      switch (mine_count) {
        case 1:  
          b->invis_type = CT_WONE; 
          break;
        case 2:  
          b->invis_type = CT_WTWO; 
          break;
        case 3:  
          b->invis_type = CT_WTHREE; 
          break;
        case 4:  
          b->invis_type = CT_WFOUR; 
          break;
        case 5:  
          b->invis_type = CT_WFIVE; 
          break;
        case 6:  
          b->invis_type = CT_WSIX; 
          break;
        case 7:  
          b->invis_type = CT_WSEVEN; 
          break;
        case 8:  
          b->invis_type = CT_WEIGHT; 
          break;
      }
    }
  }
  
  for (uint16_t i = 0; i < 16; ++i) {
    for (uint16_t j = 0; j < 16; ++j) {
      CELL *b = &playground[i][j];
      std::cout << b->invis_type << " ";
    }
    std::cout << std::endl;
  }
  
  al_start_timer(timer);
  
  while(true) {
    al_wait_for_event(queue, &event);
    al_get_mouse_state(&msestate);
    al_get_keyboard_state(&kbdstate);
    
    switch (event.type) {
      case ALLEGRO_EVENT_TIMER:
        if(al_key_down(&kbdstate, ALLEGRO_KEY_ESCAPE)) {
          done = true;
        }
        
        for (uint16_t i = 0; i < 16; i++) {
          for (uint16_t j = 0; j < 16; j++) {
            CELL* b = &playground[i][j];
            bool mouse_inside = mouse_x > b->x && mouse_x < b->x + 20 && mouse_y > b->y && mouse_y < b->y + 20;
            
            if (mouse_inside && b->vis_type == CT_CONC) {
              b->vis_type = CT_CONV;
            } else if (!mouse_inside && b->vis_type == CT_CONV) {
              b->vis_type = CT_CONC;
            }
          }
        }
  
        redraw = true;
        break;
        
      case ALLEGRO_EVENT_MOUSE_AXES:
        mouse_x = event.mouse.x;
        mouse_y = event.mouse.y;
        break;
        
      case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        for (uint16_t i = 0; i < 16; i++) {
          for (uint16_t j = 0; j < 16; j++) {
            CELL* b = &playground[i][j];
            bool mouse_inside = mouse_x > b->x && mouse_x < b->x + 20 && mouse_y > b->y && mouse_y < b->y + 20;
            
            if (event.mouse.button == 1 && mouse_inside && (b->vis_type == CT_CONV || b->vis_type == CT_CONC)) {
              if (first_click && b->invis_type == CT_WOANY) {
                first_click_opening(playground, i, j);
              }
              b->vis_type = b->invis_type;
            }
            
            if (event.mouse.button == 2 && mouse_inside && (b->vis_type == CT_CONV || b->vis_type == CT_CONC)) {
              b->vis_type = CT_FLAG;
            } else if (event.mouse.button == 2 && mouse_inside && b->vis_type == CT_FLAG) {
              b->vis_type = CT_CONC;
            }
            
          }
        }
        
        break;
    
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        done = true;
        break;
    }
    
    if (done) {
      break;
    }

    if (redraw && al_is_event_queue_empty(queue)) {
      al_clear_to_color(al_map_rgb(255, 255, 255));
      al_draw_bitmap(bg, 0, 0, 0);

      for (uint16_t i = 0; i < 16; i++) {
        for (uint16_t j = 0; j < 16; j++) {
          CELL* b = &playground[i][j];
          switch(b->vis_type) {
            case CT_CONC:
              al_draw_bitmap(cCONV, b->x, b->y, 0);
              break;
            case CT_CONV:
              al_draw_bitmap(cCONC, b->x, b->y, 0);
              break;
            case CT_WOANY:
              al_draw_bitmap(cWOANY, b->x, b->y, 0);
              break;
            case CT_WONE:
              al_draw_bitmap(cWONE, b->x, b->y, 0);
              break;
            case CT_WTWO:
              al_draw_bitmap(cWTWO, b->x, b->y, 0);
              break;
            case CT_WTHREE:
              al_draw_bitmap(cWTHREE, b->x, b->y, 0);
              break;
            case CT_WFOUR:
              al_draw_bitmap(cWFOUR, b->x, b->y, 0);
              break;
            case CT_WFIVE:
              al_draw_bitmap(cWFIVE, b->x, b->y, 0);
              break;
            case CT_WSIX:
              al_draw_bitmap(cWSIX, b->x, b->y, 0);
              break;
            case CT_WSEVEN:
              al_draw_bitmap(cWSEVEN, b->x, b->y, 0);
              break;
            case CT_WEIGHT:
              al_draw_bitmap(cWEIGHT, b->x, b->y, 0);
              break;
            case CT_MINE:
              al_draw_bitmap(cMINE, b->x, b->y, 0);
              break;
            case CT_FLAG:
              al_draw_bitmap(cFLAG_RU, b->x, b->y, 0);
              break;
          }
        }
      }
      al_flip_display();

      redraw = false;
    }
  }
  
  al_destroy_bitmap(cCONV);
  al_destroy_bitmap(cCONC);
  al_destroy_bitmap(cMINE);
  al_destroy_bitmap(cFLAG_RU);
  al_destroy_bitmap(cWOANY);
  al_destroy_bitmap(cWONE);
  al_destroy_bitmap(cWTWO);
  al_destroy_bitmap(cWTHREE);
  al_destroy_bitmap(cWFOUR);
  al_destroy_bitmap(cWFIVE);
  al_destroy_bitmap(cWSIX);
  al_destroy_bitmap(cWSEVEN);
  al_destroy_bitmap(cWEIGHT);

  
  al_destroy_font(font);
  al_destroy_display(disp);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);
  
  
  for (uint16_t i = 0; i < 16; ++i) {
    delete[] playground[i];
  }
  delete[] playground;

  return 0;
}