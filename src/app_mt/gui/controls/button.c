
#include "button.h"
#include "gui.h"
#include "gfx.h"

#include <stdlib.h>
#include <string.h>


#define BTN_FIRST_REPEAT_DELAY MS2ST(500)
#define BTN_REPEAT_DELAY       MS2ST(100)


typedef struct {
  bool is_down;
  const Image_t* icon;
  color_t up_icon_color;
  color_t up_btn_color;
  color_t down_icon_color;
  color_t down_btn_color;
  color_t disabled_icon_color;
  color_t disabled_btn_color;
  systime_t next_event_time;
  char* text;
  const font_t* font;

  button_event_handler_t evt_handler;
} button_t;


static void button_touch(touch_event_t* event);
static void button_paint(paint_event_t* event);
static void button_enable(enable_event_t* event);
static void button_destroy(widget_t* w);


static const widget_class_t button_widget_class = {
    .on_touch   = button_touch,
    .on_paint   = button_paint,
    .on_enable  = button_enable,
    .on_destroy = button_destroy,
};

widget_t*
button_create(widget_t* parent, rect_t rect, const Image_t* icon, color_t icon_color, color_t btn_color,
    button_event_handler_t evt_handler)
{
  button_t* b = calloc(1, sizeof(button_t));

  b->icon = icon;
  b->up_icon_color = icon_color;
  b->up_btn_color = btn_color;
  b->down_icon_color = icon_color;
  b->down_btn_color = DARK_GRAY;
  b->disabled_icon_color = icon_color;
  b->disabled_btn_color = DARK_GRAY;

  b->evt_handler = evt_handler;

  widget_t* w = widget_create(parent, &button_widget_class, b, rect);
  widget_set_background(w, btn_color);

  return w;
}

void
button_set_icon(widget_t* w, const Image_t* icon)
{
  button_t* b = widget_get_instance_data(w);
  if (b->icon != icon) {
    b->icon = icon;
    widget_invalidate(w);
  }
}

void
button_set_up_bg_color(widget_t* w, color_t color)
{
  button_t* b = widget_get_instance_data(w);
  if (b->up_btn_color != color) {
    b->up_btn_color = color;
    if (!b->is_down && widget_is_enabled(w))
      widget_set_background(w, color);
    widget_invalidate(w);
  }
}

void
button_set_up_icon_color(widget_t* w, color_t color)
{
  button_t* b = widget_get_instance_data(w);
  if (b->up_icon_color != color) {
    b->up_icon_color = color;
    if (!b->is_down && widget_is_enabled(w))
      widget_invalidate(w);
  }
}

void
button_set_down_bg_color(widget_t* w, color_t color)
{
  button_t* b = widget_get_instance_data(w);
  if (b->down_btn_color != color) {
    b->down_btn_color = color;
    if (b->is_down && widget_is_enabled(w))
      widget_set_background(w, color);
    widget_invalidate(w);
  }
}

void
button_set_down_icon_color(widget_t* w, color_t color)
{
  button_t* b = widget_get_instance_data(w);
  if (b->down_icon_color != color) {
    b->down_icon_color = color;
    if (b->is_down && widget_is_enabled(w))
      widget_invalidate(w);
  }
}

void
button_set_disabled_bg_color(widget_t* w, color_t color)
{
  button_t* b = widget_get_instance_data(w);
  if (b->disabled_btn_color != color) {
    b->disabled_btn_color = color;
    if (!widget_is_enabled(w))
      widget_set_background(w, color);
    widget_invalidate(w);
  }
}

void
button_set_disabled_icon_color(widget_t* w, color_t color)
{
  button_t* b = widget_get_instance_data(w);
  if (b->disabled_icon_color != color) {
    b->disabled_icon_color = color;
    if (!widget_is_enabled(w))
      widget_invalidate(w);
  }
}

void
button_set_text(widget_t* w, const char* text)
{
  button_t* b = widget_get_instance_data(w);
  if (b->text == NULL) {
    if (text != NULL) {
      b->text = strdup(text);
      widget_invalidate(w);
    }
  }
  else {
    if (text != NULL) {
      if (strcmp(text, b->text) != 0) {
        b->text = strdup(text);
        widget_invalidate(w);
      }
    }
    else {
      b->text = NULL;
      widget_invalidate(w);
    }
  }
}

const char*
button_get_text(widget_t* w)
{
  button_t* b = widget_get_instance_data(w);
  return b->text;
}

void
button_set_font(widget_t* w, const font_t* font)
{
  button_t* b = widget_get_instance_data(w);
  if (b->font != font) {
    b->font = font;
    widget_invalidate(w);
  }
}

static void
button_destroy(widget_t* w)
{
  button_t* b = widget_get_instance_data(w);

  if (b->text != NULL)
    free(b->text);

  free(b);
}

static void
button_touch(touch_event_t* event)
{
  if (!widget_is_enabled(event->widget))
    return;

  button_t* b = widget_get_instance_data(event->widget);

  button_event_t be = {
      .widget = event->widget,
      .pos = event->pos,
  };

  if (event->id == EVT_TOUCH_DOWN) {
    if (!b->is_down) {
      b->is_down = true;
      gui_acquire_touch_capture(event->widget);
      widget_set_background(event->widget, b->down_btn_color);
      widget_invalidate(event->widget);

      if (b->evt_handler) {
        be.id = EVT_BUTTON_DOWN;
        b->evt_handler(&be);
      }
      b->next_event_time = chTimeNow() + BTN_FIRST_REPEAT_DELAY;
    }
    else if (chTimeNow() > b->next_event_time) {
      if (b->evt_handler) {
        be.id = EVT_BUTTON_REPEAT;
        b->evt_handler(&be);
      }
      b->next_event_time = chTimeNow() + BTN_REPEAT_DELAY;
    }
  }
  else {
    if (b->is_down) {
      b->is_down = false;
      widget_set_background(event->widget, b->up_btn_color);
      gui_release_touch_capture();
      widget_invalidate(event->widget);

      if (b->evt_handler) {
        be.id = EVT_BUTTON_UP;
        b->evt_handler(&be);
      }

      if (rect_inside(widget_get_rect(event->widget), event->pos)) {
        if (b->evt_handler) {
          be.id = EVT_BUTTON_CLICK;
          b->evt_handler(&be);
        }
      }
    }
  }
}

static void
button_paint(paint_event_t* event)
{
  button_t* b = widget_get_instance_data(event->widget);

  rect_t rect = widget_get_rect(event->widget);
  point_t center = rect_center(rect);

  if (widget_is_enabled(event->widget)) {
    if (b->is_down)
      gfx_set_fg_color(b->down_icon_color);
    else
      gfx_set_fg_color(b->up_icon_color);
  }
  else {
    gfx_set_fg_color(b->disabled_icon_color);
  }

  /* draw icon */
  if (b->icon != NULL) {
    gfx_draw_bitmap(
        center.x - (b->icon->width / 2),
        center.y - (b->icon->height / 2),
        b->icon);
  }

  if (b->text != NULL && b->font != NULL) {
    Extents_t x = font_text_extents(b->font, b->text);
    gfx_set_font(b->font);
    gfx_draw_str(b->text, -1,
        center.x - (x.width / 2),
        center.y - (x.height / 2));
  }
}

static void
button_enable(enable_event_t* event)
{
  button_t* b = widget_get_instance_data(event->widget);

  if (event->enabled) {
    widget_set_background(event->widget, b->up_btn_color);
  }
  else {
    b->is_down = false;
    widget_set_background(event->widget, b->disabled_btn_color);
  }
}
