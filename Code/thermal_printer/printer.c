#include <glob.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "config.h"

#include "utils.h"

#include "printer.h"

extern FONT current_font;
extern SerialCache ser_cache;

uint16_t STBx[] = {STB1_PIN, STB2_PIN, STB3_PIN, STB4_PIN, STB5_PIN, STB6_PIN};
uint8_t as;

static unsigned int printer_vps_time;
static uint8_t printer_vps_last_status;
static uint8_t printer_temp_check;

static char adc_file_path[128];

static unsigned int printer_last_pitch_time;
static uint8_t acc_time_idx;
static uint16_t acc_time[] = {5459,3459,2762,2314,2028,1828,1675,1553,1456,1374,1302,1242,1191,1144,1103,1065,1031,1000,970,940,910,880};
#define ACCMAX 22
void printer_send_data8(uint8_t w) {
  /*
  digitalWrite(SPI1_NSS_PIN, LOW); // manually take CSN low for SPI_1
  transmission SPI.transfer(w); //Send the HEX data 0x55 over SPI-1 port and
  store the received byte to the <data> variable.
  //SPI.transfer16(w);
  digitalWrite(SPI1_NSS_PIN, HIGH); // manually take CSN high between spi
  transmissions
  */
  wiringPiSPIDataRW(0, &w, 1);
}

void clear_printer_buffer() {
  uint8_t i = 0;

  for (i = 0; i < 48; i++)
    printer_send_data8(0x00);

  LATCH_ENABLE;
  delayus(1);
  LATCH_DISABLE;
  delayus(1);
}

uint8_t IsPaper() {
  uint8_t status;
  uint8_t tmp;

  if (millis() - printer_vps_time > 10) {
    ENABLE_PEM;
    if (ASK4PAPER == LOW) // * LOW is what we want**
    {
      status = IS_PAPER;
    } else {
      status = NO_PAPER;
      PRINTF("Error:NO PAPER\n");
      DISABLE_VH;
    }
    DISABLE_PEM;

    if (printer_temp_check > 20) {
      tmp = temperature();

      if (tmp >= HOT) {
        PRINTF("Printer too Hot\n");
        status |= HOT_PRINTER;
	DISABLE_VH;
      }

      printer_temp_check = 0;

    } else {
      printer_temp_check++;
    }

  } else {
    status = printer_vps_last_status;
  }

  printer_vps_last_status = status;
  printer_vps_time = millis();

  return status;
}

uint8_t header_init() {

  uint8_t pin[] = {THERMISTORPIN};

  uint8_t x;
  pinMode(LATCH_PIN, OUTPUT);

  for (x = 0; x < STB_NUMBER; x++) {
    pinMode(STBx[x], OUTPUT);
    digitalWrite(STBx[x], LOW);
  }

  LATCH_DISABLE;

  pinMode(VH_PIN, OUTPUT);
  digitalWrite(VH_PIN, LOW);

  pinMode(PEM_PIN, INPUT);
  // pinMode(PEM_CTL_PIN,OUTPUT);

  // adc.setChannels(pin, 1); //this is actually the pin you want to measure

  pinMode(THERMISTORPIN, INPUT); // 数字io没有 模拟接口。adc 读温度暂时不搞

  /*
  //SPI.begin(); //Initialize the SPI_1 port.
  SPI.setBitOrder(MSBFIRST); // Set the SPI_1 bit order
  SPI.setDataMode(SPI_MODE0); //Set the  SPI_1 data mode 0
  SPI.setClockDivider(SPI_CLOCK_DIV16);      // Slow speed (72 / 16 = 4.5 MHz
  SPI_1 speed) SPI.setDataSize(DATA_SIZE_8BIT); SPI.begin(); //Initialize the
  SPI_1 port.
  */
  if (!wiringPiSPISetup(0, 4500000)) {
    PRINTF("SPI init failed,exiting...\n");
  }

  /*
  pinMode(SPI1_NSS_PIN, OUTPUT);
  digitalWrite(SPI1_NSS_PIN,HIGH);
  */

  printer_vps_time = 0;
  printer_vps_last_status = NO_PAPER;
  printer_temp_check = 0;
  printer_last_pitch_time = 0;
  acc_time_idx  = 0;

  glob_file(ADC_FILE_PAT);
}

#if 1

uint8_t current_pos = 1;

uint8_t header_init1() {

  pinMode(PA_PIN, OUTPUT);
  pinMode(PNA_PIN, OUTPUT);
  pinMode(PB_PIN, OUTPUT);
  pinMode(PNB_PIN, OUTPUT);

  as = 0;

  return ASK4PAPER;
}

void motor_stepper_pos2(uint8_t position) // forward
{
  //  position = 9 - position;
  //  position = (position+1)/2;
  if(printer_last_pitch_time == 0) {
  	acc_time_idx = 0;
  }else {
      if( millis() - printer_last_pitch_time > 100 ) {
       
       if(READ_VH == LOW) {       
         acc_time_idx = 0; 
       }else{
         acc_time_idx ++;
         if(acc_time_idx > ACCMAX-1) {
           acc_time_idx = ACCMAX-1;
         }
       }
     } else {
       acc_time_idx ++;
       if(acc_time_idx > ACCMAX-1) {
         acc_time_idx = ACCMAX-1;
       }
     }
  }

  printer_last_pitch_time = millis();
  delayMicroseconds(acc_time[acc_time_idx]);
  switch (position) {
  case 0:
    digitalWrite(PA_PIN, LOW);
    digitalWrite(PNA_PIN, LOW);
    digitalWrite(PB_PIN, LOW);
    digitalWrite(PNB_PIN, LOW);
    break;
  case 1:
    digitalWrite(PA_PIN, HIGH);
    digitalWrite(PNA_PIN, LOW);
    digitalWrite(PB_PIN, LOW);
    digitalWrite(PNB_PIN, HIGH);
    break;
  case 2:
    digitalWrite(PA_PIN, HIGH);
    digitalWrite(PNA_PIN, LOW);
    digitalWrite(PB_PIN, HIGH);
    digitalWrite(PNB_PIN, LOW);
    break;
  case 3:
    digitalWrite(PA_PIN, LOW);
    digitalWrite(PNA_PIN, HIGH);
    digitalWrite(PB_PIN, HIGH);
    digitalWrite(PNB_PIN, LOW);
    break;
  case 4:
    digitalWrite(PA_PIN, LOW);
    digitalWrite(PNA_PIN, HIGH);
    digitalWrite(PB_PIN, LOW);
    digitalWrite(PNB_PIN, HIGH);
    break;
  }
}

uint8_t feed_pitch1(uint64_t lines, uint8_t forward_backward) {
  uint8_t pos = current_pos;
  uint8_t restor = ~forward_backward;

  restor &= 0x01;

  if (lines > 0) {
    /*
    MOTOR_ENABLE1;
    MOTOR_ENABLE2;
    */
    while (lines > 0) {
      motor_stepper_pos2(pos); /* 0.0625mm */

      if (pos >= 1 && pos <= 4)
        pos = pos + (1 - 2 * forward_backward); // adding or subtracting
      if (pos < 1 || pos > 4)
        pos = pos + (4 - 8 * restor); // restoring pos

      lines--;
    }
    /*
    MOTOR_DISABLE1;
    MOTOR_DISABLE2;
    */
  } else {
    return ERROR_FEED_PITCH;
  }
  current_pos = pos;
  return 0;
}

void print_dots_8bit_split(CONFIG *cfg, uint8_t *Array, uint8_t characters) {
  uint8_t i = 0, y = 0, MAX = MAXPIXELS;
  uint8_t blank;
  uint16_t pts;
  uint8_t temp[MAXPIXELS];
  uint8_t _array[MAXPIXELS];
  pts = 0;
  memcpy(_array, Array, MAXPIXELS);

  while ((i < characters) && (i < MAX)) {

    pts = pts + bits_number(Array[i]);

    if (pts > cfg->max_pts) {
      memset(temp, 0, MAXPIXELS);
      memcpy(temp, _array, i);
      print_dots_8bit(cfg, temp, characters, 0);
      pts = bits_number(_array[i]);
      memset(_array, 0, i);
    } else if (pts == cfg->max_pts) {
      memset(temp, 0, MAXPIXELS);
      memcpy(temp, _array, i + 1);
      print_dots_8bit(cfg, temp, characters, 0);
      pts = 0;
      memset(_array, 0, i + 1);
    }
    i++;
  }

  if (pts > 0) {
    print_dots_8bit(cfg, _array, characters, 0);
    pts = 0;
  }

  feed_pitch1(cfg->feed_pitch, cfg->orient);

  return;
}

void print_dots_8bit(CONFIG *cfg, uint8_t *Array, uint8_t characters,
                     uint8_t feed_num) {
  uint8_t i = 0, y = 0, MAX = MAXPIXELS;
  uint8_t blank;


  if (cfg->align == 0) {
    while ((i < characters) && (i < MAX)) {
      printer_send_data8(Array[i]);
      i++;
    }
    while (i < MAX) {
      printer_send_data8(0x00);
      i++;
    }
  } else if (cfg->align == 1) { // center
    blank = 0;
    blank = (MAX - characters) / 2;

    for (i = 0; i < blank; i++) {
      printer_send_data8(0x00);
    }
    for (i = 0; i < characters; i++) {
      printer_send_data8(Array[i]);
    }
    for (i = 0; i < (MAX - characters - blank); i++) {
      printer_send_data8(0x00);
    }
  } else if (cfg->align == 2) {
    blank = MAX - characters;
    for (i = 0; i < blank; i++) {
      printer_send_data8(0x00);
    }
    for (i = 0; i < characters; i++) {
      printer_send_data8(Array[i]);
    }
  }

  LATCH_ENABLE;
  delayus(1);
  LATCH_DISABLE;
  delayMicroseconds(1);

  i = 0;

  while (y < STB_NUMBER) {

    while (i < 10) {

      digitalWrite(STBx[y], HIGH);
      delayus(HEAT_TIME + cfg->density * 46);
      digitalWrite(STBx[y], LOW);
      delayus(14);
      i++;
    }

    y++;
  }

  feed_pitch1(feed_num, cfg->orient);


  return;
}

uint16_t read_adc(char *adc_file) {
  long ret;
  char c[16];
  FILE *fptr;
  if ((fptr = fopen(adc_file, "r")) == NULL) {
    printf("Error! ADC File cannot be opened\n");
    // Program exits if the file pointer returns NULL.
    return 0;
  }
  fscanf(fptr, "%[^\n]", c);
  // printf("Data from the file:\n%s", c);
  fclose(fptr);

  ret = strtol(c, NULL, 10);
  // printf("the number ret %d\n",ret);

  return (uint16_t)ret;
}

uint16_t temperature() {

  double Rthermistor = 0, TempThermistor = 0;
  uint16_t ADCSamples = 0;
  int Sample = 1;
  uint16_t ADCConvertedValue;

  while (Sample <= NumSamples) {
    // ADCSamples += analogRead(THERMISTORPIN); //stm32
    ADCSamples += read_adc(adc_file_path);
    Sample++;
  }
  // Thermistor Resistance at x Kelvin
  ADCConvertedValue = (double)ADCSamples / NumSamples;
  Rthermistor = ((double)ADCResolution / ADCConvertedValue) - 1;
  Rthermistor = (double)SeriesResistor / Rthermistor;
  // Thermistor temperature in Kelvin
  TempThermistor = Rthermistor / RthNominal;
  TempThermistor = log(TempThermistor);
  TempThermistor /= BCoefficent;
  TempThermistor += (1 / (TempNominal + 273.15));
  TempThermistor = 1 / TempThermistor;

  return (uint16_t)(TempThermistor - 273.15);

  // return  (uint16_t)(0);
}

int glob_file(char *av) {

  glob_t globlist;

  if (glob(av, GLOB_PERIOD | GLOB_NOSORT, NULL, &globlist) == GLOB_NOSPACE ||
      glob(av, GLOB_PERIOD | GLOB_NOSORT, NULL, &globlist) == GLOB_NOMATCH)
    return -1;
  if (glob(av, GLOB_PERIOD | GLOB_NOSORT, NULL, &globlist) == GLOB_ABORTED)
    return 1;

  if (globlist.gl_pathc > 0) {
    strcpy(adc_file_path, globlist.gl_pathv[0]);
  }
  return 0;
}

#endif
uint16_t get_serial_cache_font_width(CONFIG *cfg) {

  int i;
  uint8_t *ch;
  uint32_t codename;
  int w;
  w = 0;
  i = 0;
  while (i < ser_cache.idx) {
    ch = (uint8_t *)&ser_cache.data[i];
    codename = utf8_to_utf32(ch);
    FT_UInt gi = FT_Get_Char_Index(cfg->face, codename);
    FT_Load_Glyph(cfg->face, gi, FT_LOAD_NO_BITMAP);
    w += cfg->face->glyph->metrics.horiAdvance / 64;
    i++;
  }
  return w + cfg->font->width;
}

// print with freetype font dots glyph
uint8_t print_lines_ft(CONFIG *cfg,int lines,int bf) {

  uint8_t i, j, k;
  int8_t w;
  uint8_t dot_line_data[MAXPIXELS];
  uint8_t dot_line_idx = 0;
  uint8_t dot_line_bitsidx = 0;

  uint8_t lastidx, lastw, lastj;
  uint8_t row, row_cnt;
  uint16_t line_bits;

  int8_t left = ser_cache.idx;
  uint8_t rv;
 
  if(cfg == NULL && lines > 0) {
	ENABLE_VH;
	feed_pitch1(lines,bf);
	DISABLE_VH;
	return 0;
  }
  line_bits = cfg->margin.width;
  dot_line_idx = line_bits / 8;
  dot_line_bitsidx = line_bits % 8;

  lastidx = 0;
  lastw = 0;
  lastj = 0;

  uint32_t codename;
  uint8_t *ch;
  //printf("left = %d\n", left);
  int line_height = (cfg->face->size->metrics.ascender -
                     cfg->face->size->metrics.descender) >>
                    6;
  int baseline_height =
      abs(cfg->face->descender) * current_font.height / cfg->face->units_per_EM;
  int dpx = 64;
  FT_Matrix matrix;
  ENABLE_VH;
  while (left > 0) {
    i = lastidx;
    row_cnt = 0;
    row = 0;
    while (row < line_height) {
      line_bits = cfg->margin.width;
      dot_line_idx = line_bits / 8;
      dot_line_bitsidx = line_bits % 8;
      memset(dot_line_data, 0, MAXPIXELS);
      // line by line bitmap dots to print
      i = lastidx;

      while (i < ser_cache.idx) {
        ch = (uint8_t *)&ser_cache.data[i];
        codename = utf8_to_utf32(ch);

        matrix.xx = (FT_Fixed)(cos(((double)cfg->degree / 360) * 3.14159 * 2) *
                               0x10000L);
        matrix.xy = (FT_Fixed)(-sin(((double)cfg->degree / 360) * 3.14159 * 2) *
                               0x10000L);
        matrix.yx = (FT_Fixed)(sin(((double)cfg->degree / 360) * 3.14159 * 2) *
                               0x10000L);
        matrix.yy = (FT_Fixed)(cos(((double)cfg->degree / 360) * 3.14159 * 2) *
                               0x10000L);
        FT_Set_Transform(cfg->face, &matrix, NULL);

        FT_UInt gi = FT_Get_Char_Index(cfg->face, codename);
        FT_Load_Glyph(cfg->face, gi, FT_LOAD_DEFAULT);
        int y_off = line_height - baseline_height -
                    cfg->face->glyph->metrics.horiBearingY / dpx;
        int glyph_width = cfg->face->glyph->metrics.width / dpx;
        int glyph_height = cfg->face->glyph->metrics.height / dpx;
        int advance = cfg->face->glyph->metrics.horiAdvance / dpx;

        int x_off = (advance - glyph_width) / 2;

        int bitmap_rows = cfg->face->glyph->bitmap.rows;
        int bitmap_width = cfg->face->glyph->bitmap.width;
        // FT_Render_Glyph(cfg->face->glyph, FT_RENDER_MODE_NORMAL);
        FT_Render_Glyph(cfg->face->glyph, FT_RENDER_MODE_MONO); // disable AA

        j = 0;
        w = 0;
        if (lastj != 0) {
          j = lastj;
        }
        if (lastw != 0) {
          w = lastw;
        }
        while (w < advance) {
          // if(w > 0 && (w%8) == 0) j++;
          if (dot_line_bitsidx > 7) {
            dot_line_idx++;
            dot_line_bitsidx = 0;
          }

          // unsigned char p = cfg->face->glyph->bitmap.buffer[row *
          // cfg->face->glyph->bitmap.pitch + w];
          unsigned char p = 0;
          int pitch = abs(cfg->face->glyph->bitmap.pitch);

          if (w >= x_off && row >= y_off) {
            row_cnt = row - y_off;
            if (row_cnt < bitmap_rows) {
              // p =
              // (cfg->face->glyph->bitmap.buffer[row_cnt*cfg->face->glyph->bitmap.pitch+j]
              // >> (7-w%8)) & 1;//disable AA
              j = (w - x_off) / 8;
              p = cfg->face->glyph->bitmap.buffer[row_cnt * pitch + j];
              p = p & (128 >> ((w - x_off) & 7));
            }
          }

          if (p) {
            //printf("#");
            dot_line_data[dot_line_idx] |= 1 << (7 - dot_line_bitsidx);
          } else {
            //printf("0");
          }

          dot_line_bitsidx++;
          w++;
          line_bits++;
          if (line_bits >= MAX_DOTS)
            break;
        }
        // word gap
        k = 0;
        while (k < cfg->wordgap) {
          if (dot_line_bitsidx > 7) {
            dot_line_idx++;
            dot_line_bitsidx = 0;
          }
          k++;
          dot_line_bitsidx++;
          line_bits++;
          if (line_bits >= MAX_DOTS)
            break;
        }

        if (line_bits < MAX_DOTS) {
          i++;
        }

        if (line_bits >= MAX_DOTS || i >= ser_cache.idx) {

          if (row == (line_height - 1)) { // last of the row loop
            if (w >= advance) {
              lastidx = i + 1;
              lastw = 0;
              lastj = 0;
            } else {
              lastidx = i;
              lastw = w;
              lastj = j;
            }
          }

          break;
        }
      }
      rv = IsPaper();
      if (rv == IS_PAPER) {
        // DEBUG("dot_line_idx",dot_line_idx);
        // DEBUG("dot_line_bits",dot_line_bitsidx);
        print_dots_8bit_split(cfg, dot_line_data, dot_line_idx + 1);
      }
      row++;
      //printf("\n");
    }
    left = left - lastidx;
    row = 0;
    /*
    if(cfg->line_space > cfg->font->height){
      feed_pitch1(cfg->line_space - cfg->font->height,cfg->orient);
    }
    */
  }
  DISABLE_VH;

}

uint8_t print_lines8(CONFIG *cfg,int lines,int backforward) {
   

  if( cfg == NULL ){
	if(lines > 0) {
	  ENABLE_VH;
	  feed_pitch1(lines,backforward);
	  DISABLE_VH;
	}

        return 0;
   }
  
  if (cfg->font->mode == FONT_MODE_1 && cfg->face!=NULL) {
    
    return print_lines_ft(cfg,0,0);
  }
  uint8_t i, j, k;
  int8_t w;
  uint8_t *data;
  uint8_t row, pad;
  uint16_t addr;

  uint16_t line_bits;

  uint8_t dot_line_data[MAXPIXELS];
  uint8_t dot_line_idx = 0;
  uint8_t dot_line_bitsidx = 0;

  uint8_t lastidx, lastw, lastj;
  int8_t left;
  uint8_t rv;

  pad = current_font.width % BITS8;

  if (pad > 0) {
    pad = 1;
  }

  i = 0;
  i = current_font.width / BITS8;

  pad = i + pad;

  row = 0;
  rv = IsPaper();

  data = (uint8_t *)malloc(sizeof(uint8_t) * (pad + 1));
  i = 0;

  line_bits = cfg->margin.width;

  dot_line_idx = line_bits / 8;
  dot_line_bitsidx = line_bits % 8;
  left = ser_cache.idx;
  lastidx = 0;
  lastw = 0;
  lastj = 0;

  // DEBUG("left",left);
  ENABLE_VH;
  while (left > 0) {
    i = lastidx;
    while (row < current_font.height) {

      line_bits = cfg->margin.width;
      dot_line_idx = line_bits / 8;
      dot_line_bitsidx = line_bits % 8;
      memset(dot_line_data, 0, MAXPIXELS);
      i = lastidx;
      // DEBUG("i",i)
      // DEBUG("ser_cache.idx",ser_cache.idx)
      while (i < ser_cache.idx) {
        addr = pad * (uint8_t)ser_cache.data[i] * current_font.height;
        for (j = 0; j < pad; j++) {
          data[j] = current_font.data[addr + row * pad + j];
        }
        j = 0;
        w = 0;
        if (lastj != 0) {
          j = lastj;
        }
        if (lastw != 0) {
          w = lastw;
        }

        while (w < current_font.width) {
          if (w > 0 && (w % 8) == 0)
            j++;
          if (dot_line_bitsidx > 7) {
            dot_line_idx++;
            dot_line_bitsidx = 0;
          }

          k = (data[j] >> (7 - (w % 8))) & 1;
          // Serial.print(data[j],HEX);
          if (k > 0) {
            dot_line_data[dot_line_idx] |= 1 << (7 - dot_line_bitsidx);
            // Serial.print("1");
          }

          dot_line_bitsidx++;
          w++;
          line_bits++;
          if (line_bits >= MAX_DOTS)
            break;
        }

        /// word gap
        k = 0;
        while (k < cfg->wordgap) {
          if (dot_line_bitsidx > 7) {
            dot_line_idx++;
            dot_line_bitsidx = 0;
          }

          k++;
          dot_line_bitsidx++;
          line_bits++;
          if (line_bits >= MAX_DOTS)
            break;
        }

        if (line_bits < MAX_DOTS) {
          i++;
        }

        if (line_bits >= MAX_DOTS || i >= ser_cache.idx) {

          if (row == (current_font.height - 1)) { // last of the row loop
            if (w >= current_font.width) {
              lastidx = i + 1;
              lastw = 0;
              lastj = 0;
            } else {
              lastidx = i;
              lastw = w;
              lastj = j;
            }
          }

          break;
        }
      }
      rv = IsPaper();
      if (rv == IS_PAPER) {
        // DEBUG("dot_line_idx",dot_line_idx);
        // DEBUG("dot_line_bits",dot_line_bitsidx);
        print_dots_8bit_split(cfg, dot_line_data, dot_line_idx + 1);
      }
      row++;
    }
    left = left - lastidx;
    row = 0;

    if (cfg->line_space > cfg->font->height) {
      feed_pitch1(cfg->line_space - cfg->font->height, cfg->orient);
    }
  }

  // Serial.println("print ever");

  free(data);
  DISABLE_VH;
  return rv;
}

uint8_t print_image8(CONFIG *cfg) {

  uint16_t height;
  uint16_t x, y, addr;

  uint8_t rv;
  uint8_t LinePixels[MAXPIXELS];

  uint8_t maxchars = PRINTER_BITS / 8;
  height = cfg->img->num / cfg->img->width;
  y = 0;
  addr = 0;

  rv = IsPaper();
  ENABLE_VH;
  while (y < height) {
    x = 0;
    while (x < cfg->img->width) {
      addr = x + y * cfg->img->width;

      if (cfg->img->revert_bits > 0) // LSB
        LinePixels[x] = invert_bit(cfg->img->cache[addr]);
      else
        LinePixels[x] = cfg->img->cache[addr];

      x++;
    }
    rv = IsPaper();
    if (rv == IS_PAPER) {
      print_dots_8bit_split(cfg, LinePixels, x);
    }

    // feed_pitch1(FEED_PITCH,cfg->orient);
    y++;
  }
  // feed_pitch1(cfg->feed_pitch,cfg->orient);
  cfg->img->need_print = 0;

  cfg->img->num = 0;
  cfg->img->idx = 0;
  cfg->img->width = 0;
  DISABLE_VH;

  return rv;
}

void print_cut_line(CONFIG *cfg) {
  uint8_t bs, i;

  bs = PRINTER_BITS / cfg->font->width;
  bs -= 1;

  reset_cmd();

  for (i = 0; i < bs; i++) {
    if (i % 2 == 0) {
      parse_serial_stream(cfg, '=');
    } else {
      parse_serial_stream(cfg, '-');
    }
  }
  parse_serial_stream(cfg, ASCII_FF);
}
