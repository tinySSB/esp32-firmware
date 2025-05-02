// ui-t5s3pro.cpp

#include "ui-t5s3pro.h"

#if defined(TINYSSB_BOARD_T5S3PRO)

#include "lib/tinySSBlib.h"

#include "hardware.h"
#include "lib/cmd.h"

#include "mbedtls/base64.h"
#include "ArduinoNvs.h"

extern void set_backlight(int lvl);
extern void set_backlight_delta(int d);
extern void get_battery_status(int *isCharging, int *batteryLevel, bool *vbusIn);

extern char mute_io;
extern void (*home_button_fct)(void);

// ---------------------------------------------------------------------------

static bool mk_signature(unsigned char *sig, unsigned char *data, int len)
{
  UI_T5s3pro_Class *watch = (UI_T5s3pro_Class*) theUI;
  if (!watch->myid_valid)
    return false;
  crypto_sign_ed25519_detached(sig, NULL, data, len, watch->myid_sk);
  return true;
}

// ---------------------------------------------------------------------------

void home_button(void)
{
    lv_obj_invalidate( lv_scr_act() );
    lv_refr_now(NULL);
    // lv_task_handler();
}

// ---------------------------------------------------------------------------
void graffiti_set_pixel(int x, int y);
void graffiti_clear();

UIClass::UIClass()
{
  Serial.println("# init of UIClass()");
  home_button_fct = home_button;
}

void UI_T5s3pro_Class::spinner(bool show)
{
    /*
  if (show)
    lv_obj_clear_flag(spin, LV_OBJ_FLAG_HIDDEN);
  else
    lv_obj_add_flag(spin, LV_OBJ_FLAG_HIDDEN);
  lv_task_handler();
    */
}

static lv_obj_t *status_label;

void UI_T5s3pro_Class::loop()
{
    static uint64_t next_status_update;
    static char old_status[20];

    if (millis() > next_status_update) {
        next_status_update = millis() + 10000;

        int isCharging, batteryLevel;
        get_battery_status(&isCharging, &batteryLevel, NULL);

        // /*
        char buf[20];
        buf[0] = '\0';
        if (isCharging)
            strcpy(buf, LV_SYMBOL_CHARGE);
        if (batteryLevel > 98)
            strcpy(buf+strlen(buf), LV_SYMBOL_BATTERY_FULL);
        else if (batteryLevel >= 75)
            strcpy(buf+strlen(buf), LV_SYMBOL_BATTERY_3);
        else if (batteryLevel >= 50)
            strcpy(buf+strlen(buf), LV_SYMBOL_BATTERY_2);
        else if (batteryLevel >= 25)
            strcpy(buf+strlen(buf), LV_SYMBOL_BATTERY_1);
        else
            strcpy(buf+strlen(buf), LV_SYMBOL_BATTERY_EMPTY);
        if (strcmp(old_status, buf)) {
            strcpy(old_status, buf);
            lv_label_set_text(status_label, buf);
        }
        // */
    }

    lv_task_handler();
}

// ---------------------------------------------------------------------------

extern UIClass *theUI;

void UI_T5s3pro_Class::refresh()
{
//  lv_obj_invalidate( lv_scr_act() );
//  lv_task_handler();
}

// ---------------------------------------------------------------------------

#include "../assets/t5s3pro/tinySSB_logo_240x132.h"

#define MAX_TEXT_LEN 64

static uint8_t pageId = 0;
static lv_obj_t *graffiti; // canvas
static char graffiti_text[MAX_TEXT_LEN+1];

static lv_style_t bgTransparent;
static lv_style_t tile_label_style;
static lv_style_t tile_title_style;
static lv_style_t tile_text_style;
static lv_style_t graffiti_text_style;
static lv_style_t post_text_style;
static lv_style_t peers_title_style;
static lv_style_t peers_text_style;
static lv_style_t config_text_style;
static lv_style_t bg_grey_style;
static lv_style_t bg_green_style;
static lv_style_t bg_red_style;
static lv_style_t menu_title_style;
static lv_style_t menu_item_style;

// static lv_obj_t *status_label;
static lv_obj_t *tileview;
static lv_obj_t *tile_chat;
static lv_obj_t *tile_chat_list;
static lv_obj_t *tile_chat_graffiti;
static lv_obj_t *tile_peers_list;
static lv_obj_t *tile_config_list;
static lv_obj_t *tile_about;

static lv_obj_t *logo_img;

// graffiti objects (buttons):
static lv_obj_t *g_abort;
static lv_obj_t *g_text; // label
static lv_obj_t *g_ok;
static lv_obj_t *g_left;
static lv_obj_t *g_right;
static lv_obj_t *g_del;

// ---------------------------------------------------------------------------

static void scroll_begin_event(lv_event_t* e)
{
    // Disable the scroll animations
    if (lv_event_get_code(e) == LV_EVENT_SCROLL_BEGIN) {
        lv_anim_t* a = (lv_anim_t *) lv_event_get_param(e);
        if (a)
            a->time = 0;
    }
}

static void btn_event_cb(lv_event_t *e)
{
    // Serial.printf("button %d\r\n", lv_event_get_code(e));
    //
    // if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) {
    //     lv_event_stop_processing(e);
    // }
}

/*

Set an event function for the container and in LV_EVENT_PRESSING

lv_indev_t * indev= lv_indev_create();
lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);

lv_indev_set_read_cb(indev, read_cb);

in event handler:
    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t p;
    lv_indev_get_point(indev, &p);

*/

lv_timer_t *tim = NULL;
short _x[1000];
short _y[1000];
short _sid[100];
struct gest_s *gest;
int pnt_cnt;
int sid = 0;

void cb_timer(lv_timer_t *t)
{
  // Serial.println("# cb_timer for gesture capturing");
  mute_io = false;

  if (pnt_cnt == 1) { // at least 2 points needed
    _x[1] = _x[0];
    _y[1] = _y[0];
    _sid[1] = _sid[0];
    pnt_cnt++;
  }

  /*
  struct gest_s *g = qd_import(_x, _y, _sid, pnt_cnt);
  char X = qd_classify(g);
  Serial.printf("# retrieved gesture for '%c' (%d)\r\n", X, X);
  qd_free(g);
  int pos = strlen(graffiti_text);
  if (pos < sizeof(graffiti_text)-2) {
    graffiti_text[pos] = X;
    graffiti_text[pos+1] = '\0';
    lv_label_set_text(g_text, graffiti_text);
  }
  graffiti_clear();
  */

  pnt_cnt = 0;
  sid = 0;
  tim = NULL;
}

static void cb_g_draw(lv_event_t *e)
{
  lv_obj_t *target = lv_event_get_current_target(e);
  if (target != tile_chat_graffiti)
    return;
  int c = lv_event_get_code(e);
  lv_indev_t * indev = lv_indev_get_act();
  lv_point_t p;
  lv_indev_get_point(indev, &p);

  // printf("pressing %d,%d, code %d\n", p.x, p.y, c);

  graffiti_set_pixel(p.x, p.y);

  _x[pnt_cnt] = p.x;
  _y[pnt_cnt] = 240 - p.y;
  _sid[pnt_cnt] = sid;
  pnt_cnt++;
  if (c == 2) { // start
    mute_io = true;
    if (tim) {
      lv_timer_del(tim);
      tim = NULL;
    }
  } else if (c == 8) { // stop
    tim = lv_timer_create(cb_timer, 500, NULL);
    lv_timer_set_repeat_count(tim, 1);
    if (pnt_cnt == 1 || _sid[pnt_cnt-2] != sid) { // at least 2 pts per seg
      _x[pnt_cnt] = p.x;
      _y[pnt_cnt] = 240 - p.y;
      _sid[pnt_cnt] = sid;
      pnt_cnt++;
    }
    sid++;
  }
}

static void cb_g_btn(lv_event_t * e)
{
  lv_obj_t *target = lv_event_get_current_target(e);
  if (target == g_abort)
    lv_obj_add_flag(tile_chat_graffiti, LV_OBJ_FLAG_HIDDEN);
  else if (target == g_del) {
    if (strlen(graffiti_text) > 0) {
      graffiti_text[strlen(graffiti_text)-1] = '\0';
      lv_label_set_text(g_text, graffiti_text);
    }
  } else if (target == g_left) {
    strcpy(graffiti_text, "no ");
    lv_label_set_text(g_text, graffiti_text);
  } else if (target == g_right) {
    strcpy(graffiti_text, "yes ");
    lv_label_set_text(g_text, graffiti_text);
  } else if (target == g_ok) {
    // Serial.printf("ok <%s><%s>\r\n", graffiti_text, lv_label_get_text(g_text));
    if (strlen(graffiti_text) > 0) {
      ReplicaClass *r = theRepo->fid2replica(((UI_T5s3pro_Class*) theUI)->myid_pk);
      theTAV->publish_text(r, ((UI_T5s3pro_Class*) theUI)->my_signing_fct,
                           graffiti_text);
      graffiti_text[0] = '\0';
      lv_label_set_text(g_text, "");
    }
    lv_obj_add_flag(tile_chat_graffiti, LV_OBJ_FLAG_HIDDEN);
  }
}

void graffiti_clear()
{
    lv_color_t c;
    c.full = 0;
    lv_canvas_fill_bg(graffiti, c, LV_OPA_COVER);
}

void graffiti_set_pixel(int x, int y)
{
    lv_color_t c;
    c.full = 1;
    lv_canvas_set_px_color(graffiti, x-1, y-1, c);
    lv_canvas_set_px_color(graffiti, x+0, y-1, c);
    lv_canvas_set_px_color(graffiti, x+1, y-1, c);
    lv_canvas_set_px_color(graffiti, x-1, y+0, c);
    lv_canvas_set_px_color(graffiti, x+0, y+0, c);
    lv_canvas_set_px_color(graffiti, x+1, y+0, c);
    lv_canvas_set_px_color(graffiti, x-1, y+1, c);
    lv_canvas_set_px_color(graffiti, x+0, y+1, c);
    lv_canvas_set_px_color(graffiti, x+1, y+1, c);
}

lv_obj_t* mk_graffiti(lv_obj_t *tile)
{
    lv_style_init(&graffiti_text_style);
    // lv_style_set_text_decor(&peers_title_style, LV_TEXT_DECOR_UNDERLINE);
    lv_style_set_bg_color(&graffiti_text_style, lv_color_hex(0xc0c0c0));
    lv_style_init(&graffiti_text_style);
    lv_style_set_bg_color(&graffiti_text_style, lv_color_hex(0xe0e0e0));

    lv_obj_t *g = lv_obj_create(tile), *tmp;
    lv_obj_set_style_pad_all(g, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(g, 0, LV_PART_MAIN);
    lv_obj_set_size(g, 540, 960);
    lv_obj_set_pos(g, 0, 0);
    lv_obj_add_flag(g, LV_OBJ_FLAG_HIDDEN);

    lv_obj_clear_flag(g, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_clear_flag(g, LV_OBJ_FLAG_SCROLL_CHAIN_VER);

    graffiti = lv_canvas_create(g);
    lv_obj_set_style_pad_all(graffiti, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(graffiti, 0, LV_PART_MAIN);
    lv_obj_clear_flag(graffiti, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_set_pos(graffiti, 0, 0);
    lv_obj_set_size(graffiti, 540, 960);
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(240-2,240-2)];
    lv_canvas_set_buffer(graffiti, cbuf, 540-2, 960-2, LV_IMG_CF_INDEXED_1BIT);
    lv_canvas_set_palette(graffiti, 0, lv_color_hex(0xd0d0d0));
    lv_canvas_set_palette(graffiti, 1, lv_color_hex(0xff4040));

    lv_color_t c;
    c.full = 0;
    lv_canvas_fill_bg(graffiti, c, LV_OPA_COVER);
 
    g_abort = lv_btn_create(g);     lv_obj_set_size(g_abort, 40,  40);
    g_ok    = lv_btn_create(g);     lv_obj_set_size(g_ok,    50,  40);
    g_left  = lv_btn_create(g);     lv_obj_set_size(g_left,  40,  50);
    g_right = lv_btn_create(g);     lv_obj_set_size(g_right, 40,  50);
    g_del   = lv_btn_create(g);     lv_obj_set_size(g_del,   40,  75);
    tmp     = lv_obj_create(g);     lv_obj_set_size(tmp,    134,  40);
    g_text  = lv_label_create(tmp); lv_obj_set_size(g_text, 125,  30);

    lv_obj_add_style(g_abort, &bg_red_style, LV_PART_MAIN);
    lv_obj_add_style(g_ok,    &bg_green_style, LV_PART_MAIN);
    lv_obj_add_style(tmp,     &bg_grey_style, LV_PART_MAIN);
    lv_obj_add_style(g_text,  &bg_grey_style, LV_PART_MAIN);
    lv_obj_add_style(g_text,  &graffiti_text_style, LV_PART_MAIN);
    lv_obj_set_style_text_align(g_text, LV_TEXT_ALIGN_RIGHT, 0);
    // lv_label_set_long_mode(g_text, LV_LABEL_LONG_SCROLL);
    lv_label_set_long_mode(g_text, LV_LABEL_LONG_CLIP);

    lv_obj_set_pos(g_left,    2,   2);
    lv_obj_set_pos(g_del,     2,  58);
    lv_obj_set_pos(g_right,   2, 138);
    lv_obj_set_pos(g_abort,   2, 194);
    lv_obj_set_pos(tmp,      46, 194);
    lv_obj_set_pos(g_text,    5,  10);
    lv_obj_set_pos(g_ok,    184, 194);

    lv_label_set_text(g_text, "");
    lv_obj_t *label;
    label = lv_label_create(g_abort); lv_label_set_text(label, "X");
    label = lv_label_create(g_ok);    lv_label_set_text(label, "OK");
    label = lv_label_create(g_left);  lv_label_set_text(label, "<");
    label = lv_label_create(g_right); lv_label_set_text(label, ">");
    label = lv_label_create(g_del);   lv_label_set_text(label, "D\nE\nL");

    lv_obj_add_event_cb(g_abort, cb_g_btn, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(g_ok,    cb_g_btn, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(g_left,  cb_g_btn, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(g_right, cb_g_btn, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(g_del,   cb_g_btn, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(g, cb_g_draw, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(g, cb_g_draw, LV_EVENT_RELEASED, NULL);

    return g;
}

void cb_new_post(lv_event_t *e)
{
  lv_obj_clear_flag(tile_chat_graffiti, LV_OBJ_FLAG_HIDDEN);
  graffiti_text[0] = '\0';
}

// ---------------------------------------------------------------------------

void tile_select_cb(lv_event_t* e)
{
    lv_obj_t *lbl = lv_event_get_current_target(e);
    Serial.printf("labelbox %d, event %d\r\n",
                  lv_obj_get_index(lbl), lv_event_get_code(e));
}

void mk_tile_selectbox(lv_obj_t *tile, int ndx)
{
    char *lbls[] = { "chat", "peers", "config", "about" };

    lv_obj_t *labelbox = lv_obj_create(tile);
    lv_obj_set_size(labelbox, 540, 150);
    lv_obj_set_pos(labelbox, 0, 960-150);

    for (int i = 0; i < 4; i++) {
        // lv_obj_t *btn = lv_btn_create(labelbox);
        // lv_obj_set_size(btn, 540 / 4, 150);
        // lv_obj_set_pos(btn, i * 540 / 4, 0);
        lv_obj_t *lbl = lv_label_create(labelbox);
        lv_obj_set_size(lbl, 540 / 4, 150);
        lv_obj_set_pos(lbl, i * 540 / 4, 0);
        lv_label_set_text(lbl, lbls[i]);
        lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
        if (i == ndx)
            lv_obj_add_style(lbl, &bg_grey_style, LV_PART_MAIN);
        else
            lv_obj_add_style(lbl, &bgTransparent, LV_PART_MAIN);
        // lv_obj_add_event_cb(lbl, tile_select_cb, LV_EVENT_CLICKED, NULL);
    }
    lv_obj_add_event_cb(labelbox, tile_select_cb, LV_EVENT_CLICKED, NULL);
}

// ---------------------------------------------------------------------------

void mk_tile_chat(lv_obj_t *tile)
{
    // lv_obj_add_event_cb(tile, scroll_begin_event, LV_EVENT_SCROLL_BEGIN, NULL);
    lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLL_CHAIN_VER);


    mk_tile_selectbox(tile, 0);
    tile = lv_obj_create(tile);
    lv_obj_set_size(tile, 540, 960-150);
    lv_obj_set_pos(tile, 0, 0);

  lv_obj_t *label;

  lv_obj_set_style_pad_all(tile, 5, LV_PART_MAIN);
  lv_obj_set_layout(tile, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);

  label = lv_label_create(tile);
  lv_obj_add_style(label, &tile_title_style, LV_PART_MAIN);
  lv_label_set_text(label, "---- Chat ---");

  lv_obj_t *btn = lv_btn_create(tile);
  lv_obj_add_style(btn, &bg_grey_style, LV_PART_MAIN);
  lv_obj_add_event_cb(btn, cb_new_post, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn);
  lv_label_set_text(label, "<new post>");
  lv_obj_set_style_pad_all(label, 10, LV_PART_MAIN);

  tile_chat_list = lv_obj_create(tile);
  lv_obj_set_size(tile_chat_list, LV_PCT(100), LV_PCT(100));
  lv_obj_add_flag(tile_chat_list, LV_OBJ_FLAG_HIDDEN); // while initializing
  lv_obj_set_style_pad_all(tile_chat_list, 10, LV_PART_MAIN);
  lv_obj_add_style(tile_chat_list, &bgTransparent, LV_PART_MAIN);
  lv_obj_clear_flag(tile_chat_list, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
  lv_obj_clear_flag(tile_chat_list, LV_OBJ_FLAG_SCROLL_CHAIN_VER);

  tile_chat_graffiti = mk_graffiti(tile);
  
  lv_style_init(&post_text_style);
  lv_style_set_text_color(&post_text_style, lv_color_black());
  lv_style_set_bg_opa(&post_text_style, LV_OPA_COVER);
  lv_style_set_pad_all(&post_text_style, 10);
  lv_style_set_text_align(&post_text_style, LV_TEXT_ALIGN_LEFT);

  lv_obj_set_layout(tile_chat_list, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(tile_chat_list, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(tile_chat_list, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(tile_chat_list, 10, LV_PART_MAIN);

}
// ---------------------------------------------------------------------------

static void menu_cb(lv_event_t * e)
{
    lv_obj_t *dropdown = lv_event_get_target(e);
    char buf[64];
    lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));
    Serial.printf("'%s' is selected\r\n", buf);

    if (!strcmp(LV_SYMBOL_PLUS, buf))
        set_backlight_delta(1);
    if (!strcmp(LV_SYMBOL_MINUS, buf))
        set_backlight_delta(-1);
    if (!strcmp(LV_SYMBOL_POWER, buf)) {
        bool vbusIn;
        get_battery_status(NULL, NULL, &vbusIn);
        if (vbusIn) {
            lv_obj_t *mb = lv_msgbox_create(NULL, "\n\n\nSHUTDOWN\n\n\n",
                                            "please disconnect power first",
                                            NULL, true);
            // lv_obj_set_size(mb, 3*540/4, 960/4);
            lv_obj_add_style(mb, &bg_grey_style, LV_PART_MAIN);
            lv_obj_center(mb);
            lv_task_handler();
        } else
            hw_shutdown();
    }
}

void mk_common_menu(lv_obj_t *scr)
{
    lv_obj_t *dropdown = lv_dropdown_create(scr);
    lv_obj_align(dropdown, LV_ALIGN_TOP_RIGHT, -5, 0);
    lv_dropdown_set_options(dropdown,
                            LV_SYMBOL_SETTINGS "\n"
                            LV_SYMBOL_PLUS     "\n"
                            LV_SYMBOL_MINUS    "\n"
                            LV_SYMBOL_POWER);
    lv_dropdown_set_text(dropdown, ""); // LV_SYMBOL_LIST);

    lv_dropdown_set_dir(dropdown, LV_DIR_LEFT);
    lv_dropdown_set_symbol(dropdown, LV_SYMBOL_LIST); // LV_SYMBOL_LEFT);
    
    lv_style_init(&menu_title_style);
    lv_style_set_bg_color(&menu_title_style, lv_color_black());
    lv_style_set_bg_opa(&menu_title_style, LV_OPA_COVER);
    lv_style_set_text_font(&menu_title_style, &lv_font_montserrat_36);
    lv_style_set_text_color(&menu_title_style, lv_color_white());
    lv_style_set_text_line_space(&menu_title_style, 35);
    lv_style_set_pad_all(&menu_title_style, 10);
    lv_obj_add_style(dropdown, &menu_title_style, LV_PART_MAIN);
    lv_obj_add_style(lv_dropdown_get_list(dropdown),
                     &menu_title_style, LV_PART_MAIN);

    lv_dropdown_set_selected_highlight(dropdown, false);

    lv_obj_add_event_cb(dropdown, menu_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

void mk_common_status(lv_obj_t *scr)
{
    status_label = lv_label_create(scr);

    lv_obj_align(status_label, LV_ALIGN_TOP_RIGHT, -5, 0);
    lv_label_set_text(status_label, ""); // LV_SYMBOL_CHARGE LV_SYMBOL_BATTERY_3);
    lv_obj_add_style(status_label, &menu_title_style, LV_PART_MAIN);
}

// ---------------------------------------------------------------------------
extern void update_peers();
extern void update_config();

lv_obj_t* append_peer(lv_obj_t *flex, char *nm, char *rssi, char *age)
{
  lv_obj_t *ln = lv_obj_create(flex);
  lv_obj_set_size(ln, 540, 40);
  lv_obj_set_style_pad_all(ln, 0, 0);
  lv_obj_add_style(ln, &peers_text_style, LV_PART_MAIN);

  lv_obj_t *label = lv_label_create(ln);
  lv_obj_set_width(label, 290);
  lv_label_set_text(label, nm);
  lv_obj_set_pos(label, 10, 5);

  label = lv_label_create(ln);
  lv_obj_set_width(label, 100);
  lv_label_set_text(label, rssi);
  lv_obj_set_pos(label, 320, 5);

  label = lv_label_create(ln);
  lv_obj_set_width(label, 100);
  lv_label_set_text(label, age);
  lv_obj_set_pos(label, 430, 5);

  return ln;
}

void update_peers(void)
{
    if (pageId != 1)
        return;

    while (lv_obj_get_child_cnt(tile_peers_list) > 4)
        lv_obj_del(lv_obj_get_child(tile_peers_list, 4));

    long now = millis();
    char buf1[10], buf2[10];
    for (int i = 0; i < MAX_HEARD_PEERS; i++) {
        struct peer_s *p = thePeers->heard_peers + i;
        if (p->id[0] == '\0')
            break;
        sprintf(buf1, "  %d", int(p->rssi));
        sprintf(buf2, "  %d", (now - p->when) / 1000);
        append_peer(tile_peers_list, p->id, buf1, buf2);
    }
}

void mk_tile_peers(lv_obj_t *tile)
{
    mk_tile_selectbox(tile, 1);

    extern char ssid[];

    lv_obj_set_style_pad_all(tile, 5, 0);
    lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_clear_flag(tile, LV_OBJ_FLAG_SCROLL_CHAIN_VER);

    lv_style_init(&peers_title_style);
    // lv_style_set_text_decor(&peers_title_style, LV_TEXT_DECOR_UNDERLINE);
    lv_style_set_bg_color(&peers_title_style, lv_color_hex(0xc0c0c0));
    lv_style_init(&peers_text_style);
    lv_style_set_bg_color(&peers_text_style, lv_color_hex(0xe0e0e0));

    lv_obj_set_layout(tile, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_START,
    //                       LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *label = lv_label_create(tile);
    lv_obj_add_style(label, &tile_title_style, LV_PART_MAIN);
    lv_label_set_text(label, "--- LoRa Peers ---");

    label = lv_label_create(tile);
    lv_obj_add_style(label, &tile_text_style, LV_PART_MAIN);
    lv_label_set_text(label, ssid);

    label = lv_label_create(tile);
    // lv_obj_add_style(label, &tile_text_style, LV_PART_MAIN);
    lv_label_set_text(label, "");

    lv_obj_t *title = append_peer(tile, "| Peer", "| RSSI", "| Age");
    lv_obj_add_style(title, &peers_title_style, LV_PART_MAIN);
}

// ---------------------------------------------------------------------------

lv_obj_t* append_kv(lv_obj_t *flex, char *key, char *val)
{
  lv_obj_t *ln = lv_obj_create(flex);
  lv_obj_set_size(ln, 540, 40);
  lv_obj_set_style_pad_all(ln, 0, 0);
  lv_obj_add_style(ln, &tile_text_style, LV_PART_MAIN);
  lv_obj_set_style_border_width(ln, 0, LV_PART_MAIN);

  lv_obj_t *label = lv_label_create(ln);
  lv_obj_set_width(label, 180);
  lv_obj_set_pos(label, 5, 5);
  lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
  lv_label_set_text(label, key);

  label = lv_label_create(ln);
  lv_obj_set_width(label, 320);
  lv_obj_set_pos(label, 190, 5);
  lv_obj_add_style(label, &config_text_style, LV_PART_MAIN);
  lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
  lv_label_set_text(label, val);

  return ln;
}

void update_config(void)
{
    if (pageId != 2)
        return;

    while (lv_obj_get_child_cnt(tile_config_list) > 1)
        lv_obj_del(lv_obj_get_child(tile_config_list, 1));

    extern char ssid[];
    char buf[50];

    append_kv(tile_config_list, "this node", ssid);
    append_kv(tile_config_list, "compiled", utc_compile_time);

    append_kv(tile_config_list, "LoRa plan", the_lora_config->plan);
    int f = the_lora_config->fr / 10000;
    sprintf(buf, "%d.%02d MHz", f/100, f%100);
    append_kv(tile_config_list, "LoRa freq", buf);
    sprintf(buf, "%dkHz, %d",
            (int)(the_lora_config->bw/1000), the_lora_config->sf);
    append_kv(tile_config_list, "LoRa BW,SF", buf);

    sprintf(buf, "%d/%d (%d)", theGOset->goset_len,
            theGOset->largest_claim_span, theRepo->rplca_cnt);
    append_kv(tile_config_list, "# feeds", buf);
    sprintf(buf, "%d", theRepo->entry_cnt);
    append_kv(tile_config_list, "# entries", buf);
    sprintf(buf, "%d", theRepo->chunk_cnt);
    append_kv(tile_config_list, "# chunks", buf);

    int total = MyFS.totalBytes();
    int avail = total - MyFS.usedBytes();
    sprintf(buf, "%2d%% (%d MB)", avail / (total/100), avail/1024/1024);
    append_kv(tile_config_list, "avail. mem", buf);
}

void mk_tile_config(lv_obj_t *tile)
{
    mk_tile_selectbox(tile, 2);

    lv_obj_set_style_pad_all(tile, 5, LV_PART_MAIN);

    lv_style_init(&config_text_style);
    lv_style_set_text_color(&config_text_style, lv_color_hex(0xD51B10));
    lv_style_set_bg_opa(&config_text_style, LV_OPA_COVER);
    // lv_style_set_bg_opa(&config_text_style, LV_OPA_TRANSP);
    lv_style_set_text_line_space(&config_text_style, 30);

    lv_obj_set_layout(tile, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_START,
    //                       LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(tile, 8, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(tile);
    lv_obj_add_style(label, &tile_title_style, LV_PART_MAIN);
    lv_label_set_text(label, "--- Config --- ");
}

// ---------------------------------------------------------------------------

void mk_tile_about(lv_obj_t *tile)
{
    mk_tile_selectbox(tile, 3);

    lv_obj_set_style_pad_all(tile, 5, LV_PART_MAIN);

    lv_obj_set_layout(tile, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_START,
    //                       LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t *label = lv_label_create(tile);
    lv_obj_add_style(label, &tile_title_style, LV_PART_MAIN);
    lv_label_set_text(label, "--- Welcome ---");
    lv_obj_set_style_pad_bottom(label, 25, LV_PART_MAIN);

    // label = lv_label_create(tile);
    // lv_label_set_text(label, "_");

    label = lv_label_create(tile);
    lv_label_set_text(label,
                      "tinySSB is a proud descendant\n"
                      "of Secure Scuttlebutt. It runs over\n"
                      "Bluetooth Low Energy and LoRA."
        );
    lv_obj_add_style(label, &tile_text_style, LV_PART_MAIN);
    lv_obj_add_style(label, &bg_grey_style, LV_PART_MAIN);
    lv_obj_set_style_pad_all(label, 15, LV_PART_MAIN);

    label = lv_label_create(tile);
    char buf[100];
    sprintf(buf, "compiled %s", utc_compile_time);
    lv_label_set_text(label, buf);
    lv_obj_add_style(label, &tile_text_style, LV_PART_MAIN);
    lv_obj_add_style(label, &bg_grey_style, LV_PART_MAIN);
    lv_obj_set_style_pad_all(label, 15, LV_PART_MAIN);

    label = lv_label_create(tile);
    lv_label_set_text(label, " ");

    label = lv_label_create(tile);
    lv_label_set_text(label, "https://github.com/tinySSB ");
    lv_obj_add_style(label, &tile_text_style, LV_PART_MAIN);
    lv_obj_add_style(label, &bg_grey_style, LV_PART_MAIN);
    lv_obj_set_style_pad_left(label, 15, LV_PART_MAIN);

    label = lv_label_create(tile);
    lv_label_set_text(label, "https://scuttlebutt.nz ");
    lv_obj_add_style(label, &tile_text_style, LV_PART_MAIN);
    lv_obj_add_style(label, &bg_grey_style, LV_PART_MAIN);
    lv_obj_set_style_pad_left(label, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(label, 25, LV_PART_MAIN);

    logo_img = lv_img_create(tile);
    lv_img_set_src(logo_img, &tinySSB_logo_240x132);
    lv_obj_set_style_pad_all(logo_img, 15, LV_PART_MAIN);
}

// ---------------------------------------------------------------------------

void tileview_change_cb(lv_event_t *e)
{
    // lv_obj_t *tv = lv_event_get_target(e);
    lv_obj_t *t = lv_tileview_get_tile_act(tileview);
    pageId = lv_obj_get_index(t);

    /*
    lv_event_code_t c = lv_event_get_code(e);
    Serial.print("# tile CB -- code=");
    Serial.print(c);
    uint32_t count =  lv_obj_get_child_cnt(tileview);
    Serial.print(" count=");
    Serial.print(count);
    */
    Serial.printf("# tile CB -- pageId=%d\r\n", pageId);
    // lv_scr_act() );
    // lv_obj_invalidate( lv_tileview_get_tile_btns(tileview) );

    /*
    lv_obj_invalidate( lv_scr_act() );
    lv_refr_now(NULL);
    */
}


UI_T5s3pro_Class::UI_T5s3pro_Class()
{
    Serial.println("# init of UI_T5s3pro_Class()");

    myid_valid = false;
    my_signing_fct = mk_signature;
    if (!NVS.begin("tinySSB"))
      Serial.println("#   NVS.begin() failed");
    else {
      Serial.println("#   NVS.begin() succeeded");
      unsigned char sk[64], pk[32];
      if (!ed25519_get_keypair("ed25519-seed", pk, sk)) {
        Serial.println("#     no seed found, generating one");
        if (ed25519_new_seed("ed25519-seed")) {
          Serial.println("#    new_seed OK");
          if (!ed25519_get_keypair("ed25519-seed", pk, sk)) {
            Serial.println("#    reading pk and sk worked now");
            myid_valid = true;
          } else
            Serial.println("#    reading pk and sk still failed");
        } else {
          Serial.println("#     new_seed failed, formatting NVS now");
          NVS.format();
        }
      } else {
        Serial.println("#     reading pk and sk worked");
        memcpy(myid_pk, pk, sizeof(pk));
        memcpy(myid_sk, sk, sizeof(sk));
        myid_valid = true;
      }
      if (myid_valid) {
        unsigned char buf[100];
        unsigned int len;
        mbedtls_base64_encode(buf, sizeof(buf), &len, myid_pk, sizeof(myid_pk));
        Serial.printf("#   public key is @%s.ed25519\r\n", buf);
        Serial.printf("#   public key is %s\r\n", to_hex(myid_pk, 32));
      }
    }

    lv_obj_t *label = 0;

    lv_style_init(&bg_style);
    lv_style_set_bg_color(&bg_style, lv_color_white());
    lv_style_set_text_color(&bg_style, lv_color_hex(0xff4040));

    scr = lv_scr_act();

    // styles
    lv_style_init(&bgTransparent);
    lv_style_set_bg_opa(&bgTransparent, LV_OPA_TRANSP);

    lv_style_init(&bg_grey_style);
    lv_style_set_pad_all(&bg_grey_style, 0);
    lv_style_set_bg_color(&bg_grey_style, lv_color_hex(0xe0e0e0));
    lv_style_set_border_width(&bg_grey_style, 0);
    lv_style_set_bg_opa(&bg_grey_style, LV_OPA_COVER);

    lv_style_init(&bg_green_style);
    lv_style_set_bg_color(&bg_green_style, lv_color_hex(0x02bb02));

    lv_style_init(&bg_red_style);
    lv_style_set_bg_color(&bg_red_style, lv_color_hex(0xff2020));

    lv_style_init(&tile_label_style);
    // lv_style_set_text_color(&tile_label_style, lv_color_white());
    lv_style_set_text_color(&tile_label_style, lv_color_black());
    lv_style_set_text_font(&tile_label_style, &lv_font_montserrat_28);
    lv_style_set_bg_opa(&tile_label_style, LV_OPA_TRANSP);

    lv_style_init(&tile_title_style);
    lv_style_set_text_color(&tile_title_style, lv_color_black());
    lv_style_set_text_font(&tile_title_style, &lv_font_montserrat_36);
    lv_style_set_pad_all(&tile_title_style, 10);
    lv_style_set_pad_bottom(&tile_title_style, 20);
    lv_style_set_bg_opa(&tile_title_style, LV_OPA_TRANSP);

    lv_style_init(&tile_text_style);
    lv_style_set_text_color(&tile_text_style, lv_color_black());
    lv_style_set_text_font(&tile_text_style, &lv_font_montserrat_28);
    lv_style_set_bg_opa(&tile_text_style, LV_OPA_TRANSP);

    // tiles
    tileview = lv_tileview_create(scr);
    lv_obj_clear_flag(tileview, LV_OBJ_FLAG_SCROLL_CHAIN_HOR |
                                LV_OBJ_FLAG_SCROLL_CHAIN_HOR );
    lv_obj_clear_flag(tileview, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    // lv_obj_add_event_cb(tileview,
    //                     scroll_begin_event, LV_EVENT_SCROLL_BEGIN, NULL);
    // lv_obj_remove_style(tileview, NULL, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    // lv_obj_clear_flag(tileview, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(tileview, &bgTransparent, LV_PART_MAIN);
    lv_obj_add_event_cb(tileview, tileview_change_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // chat
    lv_obj_t *t1_0 = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_HOR); // "chat"
    tile_chat = t1_0;
    mk_tile_chat(t1_0);

    // peers (name/RSSI/age)
    lv_obj_t *t2_0 = lv_tileview_add_tile(tileview, 0, 1, LV_DIR_HOR); // "peers"
    tile_peers_list = t2_0;
    mk_tile_peers(t2_0);

    // config
    lv_obj_t *t3_0 = lv_tileview_add_tile(tileview, 0, 2, LV_DIR_HOR); // "conf"
    tile_config_list = t3_0;
    mk_tile_config(t3_0);

    // about
    lv_obj_t *t4_0 = lv_tileview_add_tile(tileview, 0, 3, LV_DIR_HOR); // "about"
    tile_about = t4_0;
    mk_tile_about(t4_0);

    /*
    // tile buttons
    lv_obj_t *btns = lv_tileview_get_tile_btns(tileview);
    // lv_obj_remove_style(btns, NULL, LV_STATE_ANY);
    // LV_STATE_CHECKED, LV_STATE_FOCUSED, LV_STATE_FOCUS_KEY,
    // LV_STATE_PRESSED
    lv_obj_add_style(btns, &bg_grey_style, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_add_style(btns, &tile_label_style, LV_PART_ITEMS |
                     LV_STATE_DEFAULT | LV_STATE_CHECKED | LV_STATE_FOCUSED |
                     LV_STATE_FOCUS_KEY | LV_STATE_PRESSED);
    //
    lv_obj_set_style_outline_width(btns, 0, LV_PART_ITEMS |
                                   LV_STATE_FOCUS_KEY | LV_STATE_FOCUSED |
                                   LV_STATE_DEFAULT | LV_STATE_CHECKED |
                                   LV_STATE_PRESSED);
    //
    lv_obj_remove_style(btns, NULL, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
    lv_obj_remove_style(btns, NULL, LV_PART_ANY | LV_STYLE_TRANSITION);
    lv_obj_add_event_cb(btns, btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_remove_style(tileview, NULL, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
    lv_obj_remove_style(tileview, NULL, LV_PART_ANY | LV_STYLE_TRANSITION);
    lv_obj_remove_style(scr, NULL, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
    lv_obj_remove_style(scr, NULL, LV_PART_ANY | LV_STYLE_TRANSITION);
    */
        
    mk_common_menu(scr);
    mk_common_status(scr);
    
    static lv_timer_t *updateTimer = lv_timer_create([](lv_timer_t *timer)
      {
        update_peers();
        update_config();
      }, 10000, NULL
    );

    // lv_tileview_set_act(tileview, 3, LV_ANIM_OFF); // about
    lv_obj_set_tile(tileview, tile_about, LV_ANIM_OFF);
    lv_task_handler();
}

// ---------------------------------------------------------------------------

void UI_T5s3pro_Class::add_new_post(unsigned char *fid, char *txt, int t, int pos)
{
    Serial.printf("#   post %2d: \"%s\" %d\r\n", pos, txt, t);
    lv_obj_t *box;

    char buf[100];
    unsigned int len;
    mbedtls_base64_encode((unsigned char*)buf+1, sizeof(buf)-1, &len, fid, 32);
    buf[0] = '@';
    buf[7] = '\0';
    // strcpy(buf, "@ABCDE");

    if (post_cnt < 20) { // append
        box = lv_obj_create(tile_chat_list);
        lv_obj_set_size(box, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_layout(box, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(box, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_border_width(box, 2, LV_PART_MAIN);
        lv_obj_t *label = lv_label_create(box);
        lv_obj_set_width(label, LV_PCT(100));
        lv_obj_add_style(label, &post_text_style, LV_PART_MAIN);
        lv_label_set_text(label, buf);
        label = lv_label_create(box);
        lv_obj_set_width(label, LV_PCT(100));
        lv_obj_add_style(label, &post_text_style, LV_PART_MAIN);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_label_set_text(label, txt);
        post_cnt++;
        if (pos >= (post_cnt-1))
            return;
        // and now move to the right pos, see below
    } else { // overwrite first entry, before moving
        box = lv_obj_get_child(tile_chat_list, 1);
        lv_obj_t *label = lv_obj_get_child(box, 0);
        lv_label_set_text(label, buf);
        label = lv_obj_get_child(box, 1);
        lv_label_set_text(label, txt);
        pos--;
        if (pos <= 0)
            return;
    }
    lv_obj_move_to_index(box, pos + 1);
}

void UI_T5s3pro_Class::boot_ended()
{
    lv_obj_clear_flag(tile_chat_list, LV_OBJ_FLAG_HIDDEN); // after initializing
    // lv_tileview_set_act(tileview, 0, LV_ANIM_OFF);
    lv_obj_set_tile(tileview, tile_chat, LV_ANIM_OFF);
    lv_task_handler();
}

#endif // TINYSSB_BOARD_T5S3PRO

// eof
