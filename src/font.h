#ifndef __FONT_H__
#define __FONT_H__

#include "font_resources.h"
#include "common.h"

const Glyph_t*
font_find_glyph(const Font_t* font, char ch);

Extents_t
font_text_extents(const Font_t* font, const char* str);

#endif
