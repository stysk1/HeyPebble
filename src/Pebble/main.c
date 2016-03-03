#include <pebble.h>

#define KEY_BUTTON    0
#define KEY_DATA      0
#define KEY_VIBRATE   1

#define BUTTON_UP     0
#define BUTTON_SELECT 1
#define BUTTON_DOWN   2

static Window *s_main_window;
static TextLayer *s_text_layer;
static DictationSession *s_dictation_session;

static const uint32_t DICTATION_KEY = 0xabbababe;
static char s_last_text[256];

static char s_buffer[256];

/******************************* AppMessage ***********************************/

static void send(int key, int message) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  dict_write_int(iter, key, &message, sizeof(int), true);

  app_message_outbox_send();
}

static void inbox_received_handler(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while(t != NULL) {
    // Process this pair's key
    switch(t->key) {
      case KEY_DATA:
        snprintf(s_buffer, sizeof(s_buffer), "'%s'", t->value->cstring);
        text_layer_set_text(s_text_layer, s_buffer);
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_INFO, "Unknown key: %d", (int)t->key);
        break;
    }

    // Get next pair, if any
    t = dict_read_next(iterator);
  }
}

static void inbox_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_handler(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_handler(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

/******************************* Dictation API ********************************/

static void send_dictation_appmessage(char* transcription)
{
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_cstring(iter, DICTATION_KEY, transcription);

  // Send the message!
  app_message_outbox_send();
  
}

static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, 
                                       char *transcription, void *context) {
  if(status == DictationSessionStatusSuccess) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Dictation succeeded!");

    // Display the dictated text
    snprintf(s_last_text, sizeof(s_last_text), "Transcription:\n\n%s", transcription);
    text_layer_set_text(s_text_layer, s_last_text);
    
    send_dictation_appmessage(transcription);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Dictation failed with status: %d!", (int)status);

    // Display the reason for any error
    static char s_failed_buff[128];
    snprintf(s_failed_buff, sizeof(s_failed_buff), "Transcription failed.\n\nError ID:\n%d", (int)status);
    text_layer_set_text(s_text_layer, s_failed_buff);
  }
}

/******************************* button events **********************************/


// Single click handler for BUTTON_ID_SELECT
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Log every time we process the select button
  APP_LOG(APP_LOG_LEVEL_INFO, "Received select click event!");

  // Start voice dictation UI
  dictation_session_start(s_dictation_session);
}


// Map button events to function pointers
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}



/******************************* main_window **********************************/


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create main TextLayer
  s_text_layer = text_layer_create(GRect(5, 5, bounds.size.w - 10, 100));
  text_layer_set_text(s_text_layer, "Press Select to get input!");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  
#ifdef PBL_ROUND
  text_layer_enable_screen_text_flow_and_paging(s_text_layer, 5);
#endif
}

static void main_window_unload(Window *window) {
  // Destroy main TextLayer
  text_layer_destroy(s_text_layer);
}

static void init(void) {
  // Register callbacks
  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_handler);
  app_message_register_outbox_failed(outbox_failed_handler);
  app_message_register_outbox_sent(outbox_sent_handler);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Create main Window
  s_main_window = window_create();
  
  // Set event handler for window load and unload
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
  // Set the main window's click handler
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_stack_push(s_main_window, true);
  
  
  // Create new dictation session
  #ifdef PBL_MICROPHONE
  APP_LOG(APP_LOG_LEVEL_INFO, "MIC AVAILABLE!");
  s_dictation_session = dictation_session_create(sizeof(s_last_text), dictation_session_callback, NULL);
  #else
  APP_LOG(APP_LOG_LEVEL_INFO, "NO MIC!");
  #endif
  
}

static void deinit(void){
  dictation_session_destroy(s_dictation_session);
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}