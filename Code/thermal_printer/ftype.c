#include "ftype.h"

#include "utf8-utils.h"

static const UTF32 utf32_space[2] = {' ', 0};

bool init_ft(const char *ttf_file, FT_Face *face, FT_Library *ft,
             int req_size_w, int req_size_h, char **error) {

  bool ret = false;
  if (FT_Init_FreeType(ft) == 0) {
    if (FT_New_Face(*ft, ttf_file, 0, face) == 0) {

      if (FT_Set_Pixel_Sizes(*face, req_size_w, req_size_h) == 0) {
        ret = true;
      } else {
        if (error)
          *error = strdup("Can't set font size");
      }

    } else {
      if (error)
        *error = strdup("Can't load TTF file");
    }

  } else {
    if (error)
      *error = strdup("Can't init freetype library");
  }

  return ret;
}

bool change_ft_size(FT_Face face, int req_size_w, int req_size_h) {
  bool ret = false;
  if(face == NULL) return ret;

  if (FT_Set_Pixel_Sizes(face, req_size_w, req_size_h) == 0) {
    ret = true;
  } else {
    printf("Can't set font size");
  }

  return ret;
}
/*===========================================================================
  done_ft
  Clean up after we've finished wih the FreeType librar
  =========================================================================*/
void done_ft(FT_Library ft) { FT_Done_FreeType(ft); }

/*===========================================================================

  face_get_line_spacing

  Get the nominal line spacing, that is, the distance between glyph
  baselines for vertically-adjacent rows of text. This is "nominal" because,
  in "real" typesetting, we'd need to add extra room for accents, etc.

  =========================================================================*/
int face_get_line_spacing(FT_Face face) {
  return face->size->metrics.height / 64;
  // There are other possibilities the give subtly different results:
  // return (face->bbox.yMax - face->bbox.yMin)  / 64;
  // return face->height / 64;
}

int get_slice_len(const char lb) {

  if ((lb & 0x80) == 0)
    return 1;
  else if ((lb & 0xE0) == 0xC0)
    return 2;
  else if ((lb & 0xF0) == 0xE0)
    return 3;
  else if ((lb & 0xF8) == 0xF0)
    return 4;
  return 1;
}

UTF32 *cjk_utf8_to_utf32(const char *word) {
  assert(word != NULL);
  int l = strlen(word);
  int u8l = utf8_strlen(word);

  char buf[5];

  UTF32 *ret = malloc((u8l + 1) * sizeof(UTF32));
  int i = 0, j = 0;
  int bskip = 1;

  while (i < l) {

    bskip = get_slice_len(word[i]);
    strncpy(buf, &word[i], bskip);
    if (bskip > 1) {
      ret[j] = (UTF32)utf8_to_utf32(buf);
    } else {
      ret[j] = (UTF32)buf[0];
    }

    j++;
    i += bskip;
  }

  ret[u8l] = 0;
  return ret;
}
