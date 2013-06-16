
#include "gui_output.h"
#include "gfx.h"
#include "gui/button.h"
#include "gui/label.h"
#include "gui/icon.h"
#include "gui.h"
#include "temp_control.h"

typedef struct {
  widget_t* widget;
  widget_t* back_button;

  widget_t* function_button;
  widget_t* function_icon;
  widget_t* function_header_label;
  widget_t* function_desc_label;

  widget_t* trigger_button;
  widget_t* trigger_icon;
  widget_t* trigger_header_label;
  widget_t* trigger_desc_label;

  output_function_t function;
  output_trigger_t trigger;
} output_screen_t;


static void output_settings_screen_destroy(widget_t* w);
static void back_button_clicked(click_event_t* event);
static void function_button_clicked(click_event_t* event);
static void trigger_button_clicked(click_event_t* event);
static void select_function(output_screen_t* s, output_function_t function);
static void select_trigger(output_screen_t* s, output_trigger_t trigger);


widget_class_t output_settings_widget_class = {
    .on_destroy = output_settings_screen_destroy
};


widget_t*
output_settings_screen_create()
{
  output_screen_t* s = calloc(1, sizeof(output_screen_t));
  s->widget = widget_create(NULL, &output_settings_widget_class, s, display_rect);
  widget_set_background(s->widget, BLACK, FALSE);

  rect_t rect = {
      .x = 15,
      .y = 15,
      .width = 56,
      .height = 56,
  };
  s->back_button = button_create(s->widget, rect, NULL, img_left, BLACK, back_button_clicked);

  rect.x = 85;
  rect.y = 26;
  rect.width = 220;
  label_create(s->widget, rect, "Output 1 Setup", font_opensans_22, WHITE);

  rect.x = 10;
  rect.y = 95;
  rect.width = 300;
  rect.height = 66;
  s->function_button = button_create(s->widget, rect, NULL, NULL, BLACK, function_button_clicked);

  rect.y = 165;
  s->trigger_button = button_create(s->widget, rect, NULL, NULL, BLACK, trigger_button_clicked);

  rect.x = 5;
  rect.y = 5;
  rect.width = 56;
  rect.height = 56;
  s->function_icon = icon_create(s->function_button, rect, img_flame, ORANGE);
  s->trigger_icon = icon_create(s->trigger_button, rect, img_temp_38, PURPLE);

  rect.x = 70;
  rect.y = 5;
  rect.width = 200;
  s->function_header_label = label_create(s->function_button, rect, NULL, font_opensans_16, WHITE);
  s->trigger_header_label = label_create(s->trigger_button, rect, NULL, font_opensans_16, WHITE);

  rect.y = 30;
  s->function_desc_label = label_create(s->function_button, rect, NULL, font_opensans_8, WHITE);
  s->trigger_desc_label = label_create(s->trigger_button, rect, NULL, font_opensans_8, WHITE);

  select_function(s, OUTPUT_FUNC_HEATING);
  select_trigger(s, OUTPUT_TRIG_PROBE1);

  return s->widget;
}

static void
output_settings_screen_destroy(widget_t* w)
{
  output_screen_t* s = widget_get_instance_data(w);
  free(s);
}

static void
back_button_clicked(click_event_t* event)
{
  (void)event;

  gui_pop_screen();
}

static void
function_button_clicked(click_event_t* event)
{
  widget_t* screen = widget_get_parent(event->widget);
  output_screen_t* s = widget_get_instance_data(screen);

  switch (s->function) {
  case OUTPUT_FUNC_HEATING:
    select_function(s, OUTPUT_FUNC_COOLING);
    break;

  case OUTPUT_FUNC_COOLING:
    select_function(s, OUTPUT_FUNC_MANUAL);
    break;

  case OUTPUT_FUNC_MANUAL:
    select_function(s, OUTPUT_FUNC_HEATING);
    break;

  default:
    break;
  }
}

static void
select_function(output_screen_t* s, output_function_t function)
{
  s->function = function;

  char* header;
  char* desc;
  const Image_t* img;
  uint16_t color;

  switch (function) {
  case OUTPUT_FUNC_HEATING:
    header = "Function: Heating";
    desc = "The output will turn on when the temp is below the setpoint.";
    img = img_flame;
    color = ORANGE;
    widget_show(s->trigger_button);
    break;

  case OUTPUT_FUNC_COOLING:
    header = "Function: Cooling";
    desc = "The output will turn on when the temp is above the setpoint.";
    img = img_snowflake;
    color = CYAN;
    widget_show(s->trigger_button);
    break;

  case OUTPUT_FUNC_MANUAL:
    header = "Function: Manual";
    desc = "The output will turn on when commanded.";
    img = img_flame;
    color = MAGENTA;
    widget_hide(s->trigger_button);
    break;

  default:
    return;
  }

  label_set_text(s->function_header_label, header);
  label_set_text(s->function_desc_label, desc);
  icon_set_image(s->function_icon, img);
  widget_set_background(s->function_icon, color, FALSE);
}

static void
trigger_button_clicked(click_event_t* event)
{
  widget_t* screen = widget_get_parent(event->widget);
  output_screen_t* s = widget_get_instance_data(screen);

  if (s->trigger == OUTPUT_TRIG_PROBE1) {
    select_trigger(s, OUTPUT_TRIG_PROBE2);
  }
  else {
    select_trigger(s, OUTPUT_TRIG_PROBE1);
  }
}

static void
select_trigger(output_screen_t* s, output_trigger_t trigger)
{
  s->trigger = trigger;

  char* header;
  char* desc;
  uint16_t color;

  switch (trigger) {
  case OUTPUT_TRIG_PROBE1:
    header = "Trigger: Probe 1";
    desc = "The temperature will be read from Probe 1.";
    color = AMBER;
    break;

  case OUTPUT_TRIG_PROBE2:
    header = "Trigger: Probe 2";
    desc = "The temperature will be read from Probe 2.";
    color = PURPLE;
    break;

  default:
    return;
  }

  label_set_text(s->trigger_header_label, header);
  label_set_text(s->trigger_desc_label, desc);
  widget_set_background(s->trigger_icon, color, FALSE);
}