#include <iostream> // используется в основном для отправки ошибок в консоль
#include <cstdlib>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

void must_init(bool initialization, const char* module) {
  if (initialization) {
    return;
  }
  std::cout << module << " init error" << std::endl;
  std::exit(1);
}

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

int main() {
  srand(0);
  must_init(al_init(), "Allegro");
  must_init(al_install_keyboard(), "Keyboard");
  must_init(al_init_image_addon(), "Image addon");

  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
  must_init(timer, "Timer");
  
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  must_init(queue, "Queue");

  ALLEGRO_DISPLAY* disp = al_create_display(480, 480);
  must_init(disp, "Display");
  
  ALLEGRO_FONT* font = al_create_builtin_font();
  must_init(font, "Font");
  
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

  bool done = false;
  bool redraw = true;
  ALLEGRO_EVENT event;

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
      b->invis_type = static_cast<uint16_t>((rand() % 2) * 9);
      new_cell_x += 20;
    }
    new_cell_x = 80;
    new_cell_y += 20;
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
    
    switch (event.type) { // ALLEGRO_EVENT_KEY_DOWN - нажатие любой клавиши пользователем
      case ALLEGRO_EVENT_TIMER:
        redraw = true;
        break;

      // case ALLEGRO_EVENT_KEY_DOWN:
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
              al_draw_bitmap(cCONC, b->x, b->y, 0);
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