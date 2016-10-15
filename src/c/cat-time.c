#include <pebble.h>

static Window *s_window;
static TextLayer *s_time_layer;

static const char *textHours[] = {
    "una",
    "dues",
    "tres",
    "quatre",
    "cinc",
    "sis",
    "set",
    "vuit",
    "nou",
    "deu",
    "onze",
    "dotze"
};

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

char* concat(const char *s1, const char *s2) {
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 2);  //+2 for the zero-terminator and space between
    memcpy(result, s1, len1);
    memcpy(result + len1, " ", 1);
    memcpy(result + len1 + 1, s2, len2 + 1);  //+1 to copy the null-terminator
    return result;
}

static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    int hour = tick_time->tm_hour % 12;
    const char *textHour = textHours[hour - 1];

    const char *hourPrefix = "les";
    if (hour == 1) {
        hourPrefix = "la";
    }

    // Display this time on the TextLayer
    text_layer_set_text(s_time_layer, concat(hourPrefix, textHour));
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void prv_window_load(Window *window) {
    // Get information about the window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Create the text layer with specific bounds
    s_time_layer = text_layer_create(
        GRect(0, PBL_IF_ROUND_ELSE(112, 120), bounds.size.w, 50));

    // Layout improvements
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

    // Add it as a child of the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void prv_window_unload(Window *window) {
    text_layer_destroy(s_time_layer);
}

static void prv_init(void) {
    s_window = window_create();
    window_set_click_config_provider(s_window, prv_click_config_provider);
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
