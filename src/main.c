#include <pebble.h>
	
#define KEY_GET_TRENDS 99
#define MAX_TRENDS 10
	
static char *trendsArray[MAX_TRENDS];
	
Window *my_window;
ScrollLayer *scrollLayer;

typedef struct {
	TextLayer *title;
	TextLayer *description;
} Card;

Card new_card(){
	Card card;
	card.title = text_layer_create(GRect(10, 10, 144, 150));
    card.description = text_layer_create(GRect(0, 80, 144, 150));
	return card;
}

Card c;

static void send(int key, int value){
	DictionaryIterator *iterator;
	app_message_outbox_begin(&iterator);
	dict_write_int(iterator, key, &value, sizeof(int), true);
	
	app_message_outbox_send();
}

void appMessageReciever(DictionaryIterator *iterator, void *context){
	Tuple *t = dict_read_first(iterator);
	
	while (t != NULL){
		int key = t->key;
		trendsArray[key] = malloc(sizeof(t->value->cstring));
		snprintf(trendsArray[key], sizeof(t->value->cstring), "%s", t->value->cstring);
		
		APP_LOG(APP_LOG_LEVEL_INFO, "Got: %s", trendsArray[0]);
		//text_layer_set_text(c.title, trendsArray[0]);
		
		send(key+1, 0);
		
		t = dict_read_next(iterator);
	}
}

void appMessageOutBoxSent(DictionaryIterator *iterator, void *context){
		APP_LOG(APP_LOG_LEVEL_INFO, "App Message Outbox Sent.");
}

void appBoxFailed(DictionaryIterator *iterator, AppMessageResult reason, void *context){
	APP_LOG(APP_LOG_LEVEL_INFO, "App Message Failed. %d", (int) reason);
    if (reason == APP_MSG_SEND_TIMEOUT){
        APP_LOG(APP_LOG_LEVEL_INFO, "FOUND");
    }
}

void main_window_unload(Window *window){
	text_layer_destroy(c.title);
	text_layer_destroy(c.description);
}
	
void main_window_load(Window *window){
	window_set_background_color(window, GColorBlack);
	
	c = new_card();
	
	text_layer_set_text_color(c.title, GColorWhite);
	
	text_layer_set_font(c.title, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_BOLD_24)));
	
	text_layer_set_background_color(c.title, GColorClear);
    
    
	text_layer_set_text(c.title, "This is the long title of the current trending topic.");
	
	int titleHeight = (text_layer_get_content_size(c.title)).h;
	
	c.description = text_layer_create(GRect(0, titleHeight+30, 144, 150));
	text_layer_set_font(c.description, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_BOLD_20)));
	text_layer_set_text(c.description, "This is the description of the current trending topic. This is the description of the current trending topic.");
	
    
	int descHeight = (text_layer_get_content_size(c.description)).h;

	scroll_layer_add_child(scrollLayer, (Layer*) c.title);
	scroll_layer_add_child(scrollLayer, (Layer*) c.description);
	
	APP_LOG(APP_LOG_LEVEL_INFO, "height= %d", titleHeight + descHeight + 60);
	
	scroll_layer_set_content_size(scrollLayer, GSize(144, titleHeight + descHeight + 60));
	
	layer_add_child(window_get_root_layer(window), (Layer*) scrollLayer);
	
	send(KEY_GET_TRENDS, 0);
}

void handle_init(void) {
  my_window = window_create();
  scrollLayer = scroll_layer_create(GRect(0,0,144,152));
  scroll_layer_set_click_config_onto_window(scrollLayer, my_window);
	
  window_set_window_handlers(my_window, (WindowHandlers){
	 .load = main_window_load,
  });

  app_message_register_inbox_received(appMessageReciever);
  app_message_register_outbox_failed(appBoxFailed);
  app_message_register_outbox_sent(appMessageOutBoxSent);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
  window_stack_push(my_window, true);
  
}

void handle_deinit(void) {
  app_message_deregister_callbacks();
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}