#include <pebble.h>
#include "HelloWorld.h"

Window *window;
TextLayer *text_layer;

void init(char* str){
  // Create the Window
  window = window_create();

  // Push to the stack, animated
  window_stack_push(window, true);
  
  // Create the TextLayer, for display at (0, 0),
  // and with a size of 144 x 40
  text_layer = text_layer_create(GRect(0, 0, 144, 40));
  
  // Set the text that the TextLayer will display
  //int end = strlen(str);
  //for(int i=0;i<end;i++){
    text_layer_set_text(text_layer, str);
  //}
  
  // Add as child layer to be included in rendering
  layer_add_child(window_get_root_layer(window), 
                  text_layer_get_layer(text_layer));
  
  vibes_short_pulse();
  
}

void deinit(){
  // Destroy the Window
  window_destroy(window);
  // Destroy the TextLayer
  text_layer_destroy(text_layer);
}

int main(){
  char string[] = "Hello Pebble BOI!";
  init(&string[0]);
  app_event_loop();
  deinit();
  return 0;
}