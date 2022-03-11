#ifndef FTYPE_H
#define FTYPE_H
// clang-format off
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <freetype2/ft2build.h>

#include <freetype/freetype.h>
// clang-format on
#ifndef UTF8
typedef unsigned char UTF8;
#endif

#ifndef UTF32
typedef int32_t UTF32;
#endif

bool init_ft(const char *ttf_file, FT_Face *face, FT_Library *ft,
             int req_size_w, int req_size_h, char **error);

bool change_ft_size(FT_Face face, int req_size_w, int req_size_h);

int face_get_line_spacing(FT_Face face);

int get_slice_len(const char lb);

#endif
