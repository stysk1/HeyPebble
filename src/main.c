#include <pebble.h>

Window *window;
TextLayer *text_layer;
static DictationSession *s_dictation_session;
static char s_last_text[512];

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, 
                                       char *transcription, void *context) {
  // Print the results of a transcription attempt                                     
  APP_LOG(APP_LOG_LEVEL_INFO, "Dictation status: %d", (int)status);
}

void init() {
  window = window_create();
  text_layer = text_layer_create(GRect(0, 0, 144, 40));
  text_layer_set_text(text_layer, "Hello, Pebble!");
  layer_add_child(window_get_root_layer(window), 
                    text_layer_get_layer(text_layer));
  window_stack_push(window, true);
}

void deinit() {
  text_layer_destroy(text_layer);
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
  return 0;
}