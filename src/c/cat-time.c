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
static bool welcome_message_displayed = false;

static void update_time() {
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    int hour = tick_time->tm_hour;
    int min = tick_time->tm_min;

    // Display this time on the TextLayer
    text_layer_set_text(s_time_layer, hour_to_string(hour%12, min));
    text_layer_set_text(s_part_time_layer, part_time_to_string(hour%12, min));

    if (min == 0 || !welcome_message_displayed)  {
        text_layer_set_text(s_welcome_layer, welcome_message(hour));
        if (!welcome_message_displayed) welcome_message_displayed = true;
    }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();

    // Get the weather every 30 minutes
    if (tick_time->tm_min % 30 == 0) {
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
    }
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
        GRect(0, PBL_IF_ROUND_ELSE(0, 0), bounds.size.w, 30));


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
    text_layer_set_text(s_weather_layer, "n");

    text_layer_set_background_color(s_welcome_layer, GColorScreaminGreen);
    text_layer_set_text_color(s_welcome_layer, GColorBlack);
    text_layer_set_font(s_welcome_layer, s_part_time_font);
    text_layer_set_text_alignment(s_welcome_layer, GTextAlignmentCenter);

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

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    static char temperature_buffer[8];
    static char weather_buffer[8];

    Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
    Tuple *weather_tuple = dict_find(iterator, MESSAGE_KEY_WEATHER);

    if (temp_tuple && weather_tuple) {
        snprintf(
            temperature_buffer,
            sizeof(temperature_buffer), "%dC", (int)temp_tuple->value->int32);
        snprintf(
            weather_buffer,
            sizeof(weather_buffer), "%s", weather_tuple->value->cstring);

        text_layer_set_text(s_weather_layer, weather_buffer);
    }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void prv_init(void) {
    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = prv_window_load,
        .unload = prv_window_unload,
    });

    // Register callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    // Open AppMessage
    const int inbox_size = 128;
    const int outbox_size = 128;
    app_message_open(inbox_size, outbox_size);

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
