#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <string.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>


#include "config.h"

#include "utils.h"

#include "printer.h"

extern FONT current_font;
extern SerialCache ser_cache;

uint16_t STBx[] = {STB1_PIN,STB2_PIN,STB3_PIN,STB4_PIN,STB5_PIN,STB6_PIN};
uint8_t as;

static unsigned int printer_vps_time;
static uint8_t printer_vps_last_status;

void printer_send_data8(uint8_t w)
{
  /*
  digitalWrite(SPI1_NSS_PIN, LOW); // manually take CSN low for SPI_1 transmission
  SPI.transfer(w); //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  //SPI.transfer16(w);
  digitalWrite(SPI1_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
  */
  wiringPiSPIDataRW (0, &w, 1);
  
}


void clear_printer_buffer()
{
  uint8_t i= 0;
  
   for(i=0;i<48;i++)
     printer_send_data8(0x00);
  
  LATCH_ENABLE;
  delayus(1);
  LATCH_DISABLE;
  delayus(1);
}


uint8_t IsPaper()
{
  uint8_t status;
  uint8_t tmp;
  
  if(  millis() - printer_vps_time > 10) {
    ENABLE_PEM;
    if(ASK4PAPER==HIGH) // * temporary set,LOW is what we want**
    {status = IS_PAPER;}
    else
    {status = NO_PAPER;printf("Error:NO PAPER\n");}
    DISABLE_PEM;
  }else {
    status = printer_vps_last_status;
  }
  printer_vps_last_status = status;
  
  tmp = temperature();
  if (tmp >= HOT){
    printf("Printer too Hot\n");
    status |= HOT_PRINTER;
  }
  
  printer_vps_time = millis();
  
  return status;
}


uint8_t header_init() {
  
  uint8_t pin[] = {THERMISTORPIN};
  
  uint8_t x;
  pinMode(LATCH_PIN,OUTPUT);

  for(x=0; x < STB_NUMBER; x++){
    pinMode(STBx[x],OUTPUT);
    digitalWrite(STBx[x],LOW);
  }
  
  LATCH_DISABLE;

  pinMode(VH_PIN,OUTPUT);
  digitalWrite(VH_PIN,LOW);

  pinMode(PEM_PIN,INPUT);
  //pinMode(PEM_CTL_PIN,OUTPUT);

  
  //adc.setChannels(pin, 1); //this is actually the pin you want to measure
  
  //pinMode(THERMISTORPIN,INPUT_ANALOG); // 数字io没有 模拟接口。adc 读温度暂时不搞 
  
  /*
  //SPI.begin(); //Initialize the SPI_1 port.
  SPI.setBitOrder(MSBFIRST); // Set the SPI_1 bit order
  SPI.setDataMode(SPI_MODE0); //Set the  SPI_1 data mode 0
  SPI.setClockDivider(SPI_CLOCK_DIV16);      // Slow speed (72 / 16 = 4.5 MHz SPI_1 speed)
  SPI.setDataSize(DATA_SIZE_8BIT);
  SPI.begin(); //Initialize the SPI_1 port.
  */
  if (!wiringPiSPISetup (0,  4500000 )) {
	  printf("SPI init failed,exiting...\n");
  }
  
  /*
  pinMode(SPI1_NSS_PIN, OUTPUT);  
  digitalWrite(SPI1_NSS_PIN,HIGH);
  */
  
  printer_vps_time = 0;
  printer_vps_last_status = IS_PAPER;
}


#if 1

uint8_t current_pos = 1; 

uint8_t header_init1() {
  
  pinMode(PA_PIN,OUTPUT);
  pinMode(PNA_PIN,OUTPUT);
  pinMode(PB_PIN,OUTPUT);
  pinMode(PNB_PIN,OUTPUT);

  as = 0;

  return ASK4PAPER;
}

void motor_stepper_pos2(uint8_t position)//forward
{
//  position = 9 - position;
//  position = (position+1)/2;
  delayMicroseconds(6700);
  switch(position){
    case 0:
      digitalWrite(PA_PIN,LOW);
      digitalWrite(PNA_PIN,LOW);
      digitalWrite(PB_PIN,LOW);
      digitalWrite(PNB_PIN,LOW);
      break;   
    case 1:
      digitalWrite(PA_PIN,HIGH);
      digitalWrite(PNA_PIN,LOW);
      digitalWrite(PB_PIN,LOW);
      digitalWrite(PNB_PIN,HIGH);
      break;
    case 2:
      digitalWrite(PA_PIN,HIGH);
      digitalWrite(PNA_PIN,LOW);
      digitalWrite(PB_PIN,HIGH);
      digitalWrite(PNB_PIN,LOW);
    break;
    case 3:
      digitalWrite(PA_PIN,LOW);
      digitalWrite(PNA_PIN,HIGH);
      digitalWrite(PB_PIN,HIGH);
      digitalWrite(PNB_PIN,LOW);
    break;
    case 4:
      digitalWrite(PA_PIN,LOW);
      digitalWrite(PNA_PIN,HIGH);
      digitalWrite(PB_PIN,LOW);
      digitalWrite(PNB_PIN,HIGH);
    break;

  }
}

uint8_t feed_pitch1(uint64_t lines, uint8_t forward_backward)
{
  uint8_t pos = current_pos;
  uint8_t restor =  ~forward_backward;
  
  restor &= 0x01;
  
  if(lines>0)
  {
    MOTOR_ENABLE1;
    MOTOR_ENABLE2;
    ENABLE_VH;
    while(lines>0)
    {
      motor_stepper_pos2(pos);     /* 0.0625mm */
      
      if(pos >= 1 && pos <= 4)
        pos = pos + (1 - 2*forward_backward); // adding or subtracting
      if(pos < 1 || pos > 4)
        pos = pos + (4 - 8*restor); // restoring pos

      lines--;
    }
    MOTOR_DISABLE1;
    MOTOR_DISABLE2;
    DISABLE_VH;
    
  }
  else
  {
    return ERROR_FEED_PITCH;
  }
  current_pos = pos;
  return 0;

}

void print_dots_8bit_split(CONFIG*cfg,uint8_t *Array, uint8_t characters) 
{
  uint8_t i=0,y=0, MAX=48;
  uint8_t blank;
  uint16_t pts;
  uint8_t temp[48];
  uint8_t _array[48];
  pts = 0;
  memcpy(_array,Array,48);
  
  while( (i< characters) && (i < MAX)) {

    pts = pts + bits_number(Array[i]);
    
    if(pts > MAX_PRINT_PTS) {
      memset(temp,0,48);
      memcpy(temp,_array,i);
      print_dots_8bit(cfg,temp,characters,0);
      pts = bits_number(_array[i]);
      memset(_array,0,i);
    }else if(pts==MAX_PRINT_PTS) {
      memset(temp,0,48);
      memcpy(temp,_array,i+1);      
      print_dots_8bit(cfg,temp,characters,0);
      pts=0;
      memset(_array,0,i+1);
    }
    i++;
  }

  if(pts >0){
    print_dots_8bit(cfg,_array,characters,0);
    pts = 0;
  }

  feed_pitch1(cfg->feed_pitch,cfg->orient);

  return;
}

void print_dots_8bit(CONFIG*cfg,uint8_t *Array, uint8_t characters,uint8_t feed_num) 
{
  uint8_t i=0,y=0, MAX=48;
  uint8_t blank;
  
      ENABLE_VH;

      if(cfg->align == 0) {
        while((i<characters) && (i < MAX))
        {
          printer_send_data8(Array[i]);
          i++;
        }  
        while( i < MAX)
        {
          printer_send_data8(0x00);
          i++;
        }
      }else if(cfg->align==1){// center
         blank = 0;
         blank = (MAX-characters)/2;
         
         for(i=0;i<blank;i++){
          printer_send_data8(0x00);
         }
         for(i=0;i<characters;i++){
          printer_send_data8(Array[i]);
         }
         for(i=0;i<(MAX-characters-blank);i++){
          printer_send_data8(0x00);
         }
      }else if(cfg->align==2){
        blank = MAX-characters;
        for(i=0;i<blank;i++){
          printer_send_data8(0x00);
        }
        for(i=0;i<characters;i++){
          printer_send_data8(Array[i]);
        }      
      }
      
      LATCH_ENABLE;
      delayus(1);
      LATCH_DISABLE;
      delayMicroseconds(1);
      
      i =0;
      
      while(y<STB_NUMBER)
      {
        
          while(i <10)
          {

                 
            digitalWrite(STBx[y],HIGH); 
            delayus(HEAT_TIME+cfg->density*46);
            digitalWrite(STBx[y],LOW);
            delayus(14);
            i++;
          }
         
          y++;
      }


    feed_pitch1(feed_num,cfg->orient);

      
    DISABLE_VH;

    return;
}

uint16_t temperature() {
  
  /*
  double Rthermistor = 0, TempThermistor = 0;
  uint16 ADCSamples=0;
  int Sample = 1;
  uint16_t ADCConvertedValue;

 
  while(Sample<=NumSamples)
  {
      ADCSamples += analogRead(THERMISTORPIN);
      Sample++;
  }
  //Thermistor Resistance at x Kelvin
  ADCConvertedValue = (double)ADCSamples/NumSamples;
  Rthermistor = ( (double)ADCResolution/ ADCConvertedValue) - 1;
  Rthermistor =  (double)SeriesResistor/Rthermistor;
  //Thermistor temperature in Kelvin
  TempThermistor =  Rthermistor / RthNominal ;
  TempThermistor =  log(TempThermistor);
  TempThermistor /= BCoefficent;
  TempThermistor +=  (1/(TempNominal + 273.15));
  TempThermistor = 1/TempThermistor;

  return  (uint16_t)(TempThermistor - 273.15);
  */
  
  return  (uint16_t)(0);
}


#endif


void print_lines8(CONFIG*cfg) {
  uint8_t i,j,k;
  int8_t w;
  uint8_t *data;
  uint8_t row,pad;
  uint16_t addr;
  
  uint16_t line_bits;
  
  uint8_t dot_line_data[MAXPIXELS];
  uint8_t dot_line_idx=0;
  uint8_t dot_line_bitsidx=0;

  uint8_t lastidx,lastw,lastj;
  int8_t left;
  pad = current_font.width %BITS8;
  
  if(pad > 0){
    pad = 1;
  }

  i = 0;
  i = current_font.width/BITS8;
  
  pad = i+pad;
  
  row = 0;
  
  data = (uint8_t*)malloc(sizeof(uint8_t)*(pad+1));
  i=0;

  
  line_bits=cfg->margin.width;
   
  dot_line_idx = line_bits/8;
  dot_line_bitsidx = line_bits%8;
  left = ser_cache.idx;
  lastidx=0;
  lastw=0;
  lastj=0;

  //DEBUG("left",left);  
  while(left>0){
    i = lastidx;
    while(row<current_font.height){
    
      line_bits=cfg->margin.width;
      dot_line_idx = line_bits/8;
      dot_line_bitsidx = line_bits%8;
      memset(dot_line_data,0,MAXPIXELS);
      i = lastidx;
      //DEBUG("i",i)
      //DEBUG("ser_cache.idx",ser_cache.idx)
      while( i <ser_cache.idx){
        addr = pad*ser_cache.data[i]*current_font.height;
        for(j=0;j<pad;j++){
          data[j] = current_font.data[addr+row*pad+j];
        }
        j=0; w=0;
        if(lastj !=0){j= lastj;}
        if(lastw !=0) { w = lastw;}
        
          while(w < current_font.width){
            if(w > 0 && ( w%8) == 0)j++;
            if(dot_line_bitsidx > 7){
              dot_line_idx++;
              dot_line_bitsidx=0;
            }
          
            k = (data[j] >> (7-(w%8))) &1;
            //Serial.print(data[j],HEX);
            if( k > 0){
              dot_line_data[dot_line_idx] |= 1 << (7-dot_line_bitsidx);
              //Serial.print("1");
            }
          
            dot_line_bitsidx++;
            w++;
            line_bits++;
            if(line_bits >= MAX_DOTS)break;
          }

          ///word gap
          k=0;
          while( k < cfg->wordgap ){
            if(dot_line_bitsidx > 7){
              dot_line_idx++;
              dot_line_bitsidx=0;
            }

            k++;
            dot_line_bitsidx++;
            line_bits++;
            if(line_bits >= MAX_DOTS)break;
          }
         
        if(line_bits < MAX_DOTS){
          i++;
        }
        
        if(line_bits >= MAX_DOTS || i >=ser_cache.idx){
          
          if(row == (current_font.height-1)) {// last of the row loop         
            if(w >= current_font.width){
              lastidx = i+1;
              lastw =0;
              lastj =0;
            }else {              
              lastidx = i;
              lastw = w;
              lastj = j;
            }
          }
          
          break;
        }
      }
      
      if(IsPaper()== IS_PAPER){
        //DEBUG("dot_line_idx",dot_line_idx);
        //DEBUG("dot_line_bits",dot_line_bitsidx);
        print_dots_8bit_split(cfg,dot_line_data,dot_line_idx+1);
      }        
      row++;
    }
    left = left - lastidx;
    row = 0;
    
    if(cfg->line_space > cfg->font->height){
      feed_pitch1(cfg->line_space - cfg->font->height,cfg->orient);
    }
    
  }
  
  //Serial.println("print ever");

  free(data);
 
}


void print_image8(CONFIG*cfg){

  uint16_t height;
  uint16_t x,y,addr;
  
  uint8_t LinePixels[MAXPIXELS];

  uint8_t maxchars= PRINTER_BITS/8;
  height  = cfg->img->num / cfg->img->width;
  y=0;
  addr = 0;
  
  while(y < height )
  {
    x=0;
    while( x < cfg->img->width )
    {
      addr  = x+y*cfg->img->width;

     
      if(cfg->img->revert_bits > 0)//LSB
        LinePixels[x] = invert_bit(cfg->img->cache[addr]);
      else
        LinePixels[x] = cfg->img->cache[addr];
      
      x++;
    }
    
    if(IsPaper()== IS_PAPER) print_dots_8bit_split(cfg,LinePixels,x);
    
    //feed_pitch1(FEED_PITCH,BACKWARD);
    y++;
  }
  //feed_pitch1(cfg->feed_pitch,cfg->orient);
  cfg->img->need_print= 0;
  
  cfg->img->num = 0;
  cfg->img->idx = 0;
  cfg->img->width = 0;
  
}

void print_cut_line(CONFIG*cfg){
  uint8_t bs,i;

  bs= PRINTER_BITS/ cfg->font->width;
  bs-=1;
 
  reset_cmd();
  
  for(i=0;i<bs;i++){
    if(i%2==0){
      parse_serial_stream(cfg,'=');
    }else{
      parse_serial_stream(cfg,'-');
    }
  }
  parse_serial_stream(cfg,ASCII_FF);
  
}
