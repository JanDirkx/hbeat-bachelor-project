/******************************************************************************
Copyright (C) 2014 by Nibbles

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <obs-module.h>
#include <util/platform.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <sys/stat.h>
#include "lsl_plugin.h"
#include "obs-convenience.h"

float offsets[16] = {-2.0f, 0.0f, 0.0f, -2.0f, 2.0f,  0.0f, 2.0f,  0.0f,
		     0.0f,  2.0f, 0.0f, 2.0f,  -2.0f, 0.0f, -2.0f, 0.0f};

extern uint32_t texbuf_w, texbuf_h;

void draw_outlines(struct lsl_plugin*srcdata)
{
	// Horrible (hopefully temporary) solution for outlines.
	uint32_t *tmp;

	struct gs_vb_data *vdata = gs_vertexbuffer_get_data(srcdata->vbuf);

	if (!srcdata->text)
		return;

	tmp = vdata->colors;
	vdata->colors = srcdata->colorbuf;

	gs_matrix_push();
	for (int32_t i = 0; i < 8; i++) {
		gs_matrix_translate3f(offsets[i * 2], offsets[(i * 2) + 1],
				      0.0f);
		draw_uv_vbuffer(srcdata->vbuf, srcdata->tex,
				srcdata->draw_effect,
				(uint32_t)wcslen(srcdata->text) * 6);
	}
	gs_matrix_identity();
	gs_matrix_pop();

	vdata->colors = tmp;
}

void draw_drop_shadow(struct lsl_plugin*srcdata)
{
	// Horrible (hopefully temporary) solution for drop shadow.
	uint32_t *tmp;

	struct gs_vb_data *vdata = gs_vertexbuffer_get_data(srcdata->vbuf);

	if (!srcdata->text)
		return;

	tmp = vdata->colors;
	vdata->colors = srcdata->colorbuf;

	gs_matrix_push();
	gs_matrix_translate3f(4.0f, 4.0f, 0.0f);
	draw_uv_vbuffer(srcdata->vbuf, srcdata->tex, srcdata->draw_effect,
			(uint32_t)wcslen(srcdata->text) * 6);
	gs_matrix_identity();
	gs_matrix_pop();

	vdata->colors = tmp;
}

void set_up_vertex_buffer(struct lsl_plugin*srcdata)
{
	FT_UInt glyph_index = 0;
	uint32_t x = 0, space_pos = 0, word_width = 0;
	size_t len;

	if (!srcdata->text)
		return;

	srcdata->cx = get_ft2_text_width(srcdata->text, srcdata);
	srcdata->cy = srcdata->max_h;

	obs_enter_graphics();
	if (srcdata->vbuf != NULL) {
		gs_vertbuffer_t *tmpvbuf = srcdata->vbuf;
		srcdata->vbuf = NULL;
		gs_vertexbuffer_destroy(tmpvbuf);
	}

	if (*srcdata->text == 0) {
		obs_leave_graphics();
		return;
	}

	srcdata->vbuf =
		create_uv_vbuffer((uint32_t)wcslen(srcdata->text) * 6, true);

	fill_vertex_buffer(srcdata);
	obs_leave_graphics();
}

void fill_vertex_buffer(struct lsl_plugin*srcdata)
{
	struct gs_vb_data *vdata = gs_vertexbuffer_get_data(srcdata->vbuf);
	if (vdata == NULL || !srcdata->text)
		return;

	struct vec2 *tvarray = (struct vec2 *)vdata->tvarray[0].array;
	uint32_t *col = (uint32_t *)vdata->colors;

	FT_UInt glyph_index = 0;

	uint32_t dx = 0, dy = srcdata->max_h, max_y = dy;
	uint32_t cur_glyph = 0;
	uint32_t offset = 0;
	size_t len = wcslen(srcdata->text);

	if (srcdata->outline_text) {
		offset = 2;
		dx = offset;
	}

	if (srcdata->colorbuf != NULL) {
		bfree(srcdata->colorbuf);
		srcdata->colorbuf = NULL;
	}
	srcdata->colorbuf = (uint32_t*)bzalloc(sizeof(uint32_t) * wcslen(srcdata->text) * 6);
	for (size_t i = 0; i < len * 6; i++) {
		srcdata->colorbuf[i] = 0xFF000000;
	}

	for (size_t i = 0; i < len; i++) {
	add_linebreak:;
		if (srcdata->text[i] != L'\n')
			goto draw_glyph;
		dx = offset;
		i++;
		dy += srcdata->max_h + 4;
		if (i == wcslen(srcdata->text)){
				//goto skip_glyph;
		}else{

		if (srcdata->text[i] == L'\n')
			goto add_linebreak;
	draw_glyph:;
		// Skip filthy dual byte Windows line breaks
		if (srcdata->text[i] == L'\r'){
		//goto skip_glyph;
	}else{

		glyph_index =
			FT_Get_Char_Index(srcdata->font_face, srcdata->text[i]);
		if (src_glyph == NULL){

		}else{


			//goto skip_glyph;

		set_v3_rect(vdata->points + (cur_glyph * 6),
			    (float)dx + (float)src_glyph->xoff,
			    (float)dy - (float)src_glyph->yoff,
			    (float)src_glyph->w, (float)src_glyph->h);
		set_v2_uv(tvarray + (cur_glyph * 6), src_glyph->u, src_glyph->v,
			  src_glyph->u2, src_glyph->v2);
		set_rect_colors2(col + (cur_glyph * 6), srcdata->color[0],
				 srcdata->color[1]);
		dx += src_glyph->xadv;
		if (dy - (float)src_glyph->yoff + src_glyph->h > max_y)
			max_y = dy - src_glyph->yoff + src_glyph->h;
		cur_glyph++;
	}
}
}
	//skip_glyph:;
	}

	srcdata->cy = max_y;
}

void cache_standard_glyphs(struct lsl_plugin*srcdata)
{
	for (uint32_t i = 0; i < num_cache_slots; i++) {
		if (srcdata->cacheglyphs[i] != NULL) {
			bfree(srcdata->cacheglyphs[i]);
			srcdata->cacheglyphs[i] = NULL;
		}
	}

	srcdata->texbuf_x = 0;
	srcdata->texbuf_y = 0;

	cache_glyphs(srcdata, L"abcdefghijklmnopqrstuvwxyz"
			      L"ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
			      L"!@#$%^&*()-_=+,<.:>/?\\|[]{}`~ \'\"\0");
}

FT_Render_Mode get_render_mode(struct lsl_plugin*srcdata)
{
	return srcdata->antialiasing ? FT_RENDER_MODE_NORMAL
				     : FT_RENDER_MODE_MONO;
}

void load_glyph(struct lsl_plugin*srcdata, const FT_UInt glyph_index,
		const FT_Render_Mode render_mode)
{
	const FT_Int32 load_mode = render_mode == FT_RENDER_MODE_MONO
					   ? FT_LOAD_TARGET_MONO
					   : FT_LOAD_DEFAULT;
	FT_Load_Glyph(srcdata->font_face, glyph_index, load_mode);
}

struct glyph_info *init_glyph(FT_GlyphSlot slot, const uint32_t dx,
			      const uint32_t dy, const uint32_t g_w,
			      const uint32_t g_h)
{
	struct glyph_info *glyph = (glyph_info*)bzalloc(sizeof(struct glyph_info));
	glyph->u = (float)dx / (float)texbuf_w;
	glyph->u2 = (float)(dx + g_w) / (float)texbuf_w;
	glyph->v = (float)dy / (float)texbuf_h;
	glyph->v2 = (float)(dy + g_h) / (float)texbuf_h;
	glyph->w = g_w;
	glyph->h = g_h;
	glyph->yoff = slot->bitmap_top;
	glyph->xoff = slot->bitmap_left;
	glyph->xadv = slot->advance.x >> 6;

	return glyph;
}

uint8_t get_pixel_value(const unsigned char *buf_row,
			FT_Render_Mode render_mode, const uint32_t x)
{
	if (render_mode == FT_RENDER_MODE_NORMAL) {
		return buf_row[x];
	}

	const uint32_t byte_index = x / 8;
	const uint8_t bit_index = x % 8;
	const bool pixel_set = (buf_row[byte_index] >> (7 - bit_index)) & 1;
	return pixel_set ? 255 : 0;
}

void rasterize(struct lsl_plugin*srcdata, FT_GlyphSlot slot,
	       const FT_Render_Mode render_mode, const uint32_t dx,
	       const uint32_t dy)
{
	/**
	 * The pitch's absolute value is the number of bytes taken by one bitmap
	 * row, including padding.
	 *
	 * Source: https://www.freetype.org/freetype2/docs/reference/ft2-basic_types.html
	 */
	const int pitch = abs(slot->bitmap.pitch);

	for (uint32_t y = 0; y < slot->bitmap.rows; y++) {
		const uint32_t row_start = y * pitch;
		const uint32_t row = (dy + y) * texbuf_w;

		for (uint32_t x = 0; x < slot->bitmap.width; x++) {
			const uint32_t row_pixel_position = dx + x;
			const uint8_t pixel_value =
				get_pixel_value(&slot->bitmap.buffer[row_start],
						render_mode, x);
			srcdata->texbuf[row_pixel_position + row] = pixel_value;
		}
	}
}

void cache_glyphs(struct lsl_plugin*srcdata, wchar_t *cache_glyphs)
{
	if (!srcdata->font_face || !cache_glyphs)
		return;

	FT_GlyphSlot slot = srcdata->font_face->glyph;

	uint32_t dx = srcdata->texbuf_x;
	uint32_t dy = srcdata->texbuf_y;

	int32_t cached_glyphs = 0;
	const size_t len = wcslen(cache_glyphs);

	const FT_Render_Mode render_mode = get_render_mode(srcdata);
	for (size_t i = 0; i < len; i++) {
		const FT_UInt glyph_index =
			FT_Get_Char_Index(srcdata->font_face, cache_glyphs[i]);
		if (src_glyph != NULL) {
			continue;
		}

		load_glyph(srcdata, glyph_index, render_mode);
		FT_Render_Glyph(slot, render_mode);

		const uint32_t g_w = slot->bitmap.width;
		const uint32_t g_h = slot->bitmap.rows;

		if (srcdata->max_h < g_h) {
			srcdata->max_h = g_h;
		}

		if (dx + g_w >= texbuf_w) {
			dx = 0;
			dy += srcdata->max_h + 1;
		}

		if (dy + g_h >= texbuf_h) {
			blog(LOG_WARNING,
			     "Out of space trying to render glyphs");
			break;
		}

		src_glyph = init_glyph(slot, dx, dy, g_w, g_h);
		rasterize(srcdata, slot, render_mode, dx, dy);

		dx += (g_w + 1);
		if (dx >= texbuf_w) {
			dx = 0;
			dy += srcdata->max_h;
		}

		cached_glyphs++;
	}
	srcdata->texbuf_x = dx;
	srcdata->texbuf_y = dy;

	if (cached_glyphs > 0) {

		obs_enter_graphics();

		if (srcdata->tex != NULL) {
			gs_texture_t *tmp_texture = srcdata->tex;
			srcdata->tex = NULL;
			gs_texture_destroy(tmp_texture);
		}

		srcdata->tex = gs_texture_create(
			texbuf_w, texbuf_h, GS_A8, 1,
			(const uint8_t **)&srcdata->texbuf, 0);

		obs_leave_graphics();
	}

}


static void remove_cr(wchar_t *source)
{
	int j = 0;
	for (int i = 0; source[i] != '\0'; ++i) {
		if (source[i] != L'\r') {
			source[j++] = source[i];
		}
	}
	source[j] = '\0';
}

uint32_t get_ft2_text_width(wchar_t *text, struct lsl_plugin*srcdata)
{
	if (!text) {
		return 0;
	}

	FT_GlyphSlot slot = srcdata->font_face->glyph;
	uint32_t w = 0, max_w = 0;
	const size_t len = wcslen(text);
	for (size_t i = 0; i < len; i++) {
		const FT_UInt glyph_index =
			FT_Get_Char_Index(srcdata->font_face, text[i]);

		load_glyph(srcdata, glyph_index, get_render_mode(srcdata));

		if (text[i] == L'\n')
			w = 0;
		else {
			w += slot->advance.x >> 6;
			if (w > max_w)
				max_w = w;
		}
	}

	return max_w;
}
