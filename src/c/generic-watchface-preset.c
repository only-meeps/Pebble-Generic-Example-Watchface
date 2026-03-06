#include <pebble.h>
static Window *s_window;
static TextLayer *s_time_text_layer;
static TextLayer *s_date_text_layer;
static char s_buffer[32];
static char s_date_buffer[16];
static GFont s_clock_font;
static GFont s_date_font;

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
#if defined(PBL_PLATFORM_EMERY)
  s_clock_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  s_time_text_layer =
      text_layer_create(GRect(0, bounds.size.h - 75, bounds.size.w, 40));
  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
#elif defined(PBL_PLATFORM_CHALK)
  s_clock_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  s_time_text_layer =
      text_layer_create(GRect(0, bounds.size.h - 60, bounds.size.w, 40));
  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
#else
  s_clock_font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  s_time_text_layer =
      text_layer_create(GRect(0, bounds.size.h - 60, bounds.size.w, 40));
  s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
#endif

  s_date_text_layer =
      text_layer_create(GRect(0, bounds.size.h - 30, bounds.size.w, 30));
  text_layer_set_font(s_time_text_layer, s_clock_font);
  text_layer_set_font(s_date_text_layer, s_date_font);
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_time_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_time_text_layer, GColorClear);
  text_layer_set_background_color(s_date_text_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_time_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_text_layer));
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_time_text_layer);
  text_layer_destroy(s_date_text_layer);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  strftime(s_date_buffer, sizeof(s_date_buffer), "%b %d", tick_time);
  strftime(s_buffer, sizeof(s_buffer),
           clock_is_24h_style() ? "%a %H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_time_text_layer, s_buffer);
  text_layer_set_text(s_date_text_layer, s_date_buffer);
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void prv_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = prv_window_load,
                                           .unload = prv_window_unload,
                                       });

  const bool animated = true;
  window_stack_push(s_window, animated);
  update_time();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void prv_deinit(void) {

  window_destroy(s_window);
  fonts_unload_custom_font(s_clock_font);
  fonts_unload_custom_font(s_date_font);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
