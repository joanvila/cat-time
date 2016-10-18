#include <pebble.h>
#include "time-strings.h"

static Window *s_window;
static GFont s_time_font;
static GFont s_part_time_font;
static GFont s_weather_font;

static TextLayer *s_time_layer;
static TextLayer *s_part_time_layer;

static TextLayer *s_weather_layer;

static TextLayer *s_welcome_layer;

/*
static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
    text_layer_set_text(s_time_layer, "Select");
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
    text_layer_set_text(s_time_layer, "Up");
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
    text_layer_set_text(s_time_layer, "Down");
}

static void prv_click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}
*/
static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    int hour = tick_time->tm_hour % 12;
    int min = tick_time->tm_min;

    // Display this time on the TextLayer
    text_layer_set_text(s_time_layer, hour_to_string(hour, min));
    text_layer_set_text(s_part_time_layer, part_time_to_string(hour, min));
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void prv_window_load(Window *window) {
    // Get information about the window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_time_font = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_TIME_BOLD_24));
    s_part_time_font = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_TIME_BOLD_20));
    s_weather_font = fonts_load_custom_font(
        resource_get_handle(RESOURCE_ID_FONT_WEATHER_44));

    // Create the text layer with specific bounds
    s_time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(133, 133), bounds.size.w, 35));

    s_part_time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(107, 107), bounds.size.w, 30));

    s_weather_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(40, 40), bounds.size.w, 50));

    s_welcome_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(10, 10), bounds.size.w, 30));


    // Layout improvements
    text_layer_set_background_color(s_time_layer, GColorScreaminGreen);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_font(s_time_layer, s_time_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

    text_layer_set_background_color(s_part_time_layer, GColorScreaminGreen);
    text_layer_set_text_color(s_part_time_layer, GColorBlack);
    text_layer_set_font(s_part_time_layer, s_part_time_font);
    text_layer_set_text_alignment(s_part_time_layer, GTextAlignmentCenter);

    text_layer_set_text_color(s_weather_layer, GColorBlack);
    text_layer_set_font(s_weather_layer, s_weather_font);
    text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
    text_layer_set_text(s_weather_layer, "p");

    text_layer_set_background_color(s_part_time_layer, GColorScreaminGreen);
    text_layer_set_text_color(s_welcome_layer, GColorBlack);
    text_layer_set_font(s_welcome_layer, s_part_time_font);
    text_layer_set_text_alignment(s_welcome_layer, GTextAlignmentCenter);
    text_layer_set_text(s_welcome_layer, "bon dia, joan");


    // Add it as a child of the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_part_time_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_welcome_layer));
}

static void prv_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_part_time_layer);
    text_layer_destroy(s_weather_layer);
    text_layer_destroy(s_welcome_layer);

    fonts_unload_custom_font(s_time_font);
    fonts_unload_custom_font(s_part_time_font);
    fonts_unload_custom_font(s_weather_font);
}

static void prv_init(void) {
    s_window = window_create();
    //window_set_click_config_provider(s_window, prv_click_config_provider);
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = prv_window_load,
        .unload = prv_window_unload,
    });

    const bool animated = true;
    window_stack_push(s_window, animated);

    update_time();

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void prv_deinit(void) {
    window_destroy(s_window);
}

int main(void) {
    prv_init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

    app_event_loop();
    prv_deinit();
}
