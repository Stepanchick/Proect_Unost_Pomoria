#pragma once

#define HEIGTH 2
#define WIGHT 3

#include <WiFi.h>
#include<WiFiClient.h>
#include<WiFiServer.h>
#include <Arduino.h>

#include <GyverDS3231.h>
GyverDS3231 dsdt;

#include <GyverDS18.h>
#define termo_pin 27
GyverDS18Single dst(termo_pin);

#include <Preferences.h>
Preferences preferences;

#define nOE 32
#define A_pin 19
#define B_pin 17
#define CLK 26
#define SCLK 16
#define R_pin 33

//#define I2C_SDA 21
//#define I2C_SCL 22
//#define termo_pin 27


//  A_pin - оранжевый
//  B_pin - желтый
//  CLK - черный
//  SCLK - белый
//  R_pin - серый
//  nOE - коричневый

void Task1code( void * parameter);
char* password = new char[]{"password"};

void SendData();
void setText(int blok_number);
void setTextFrom(int blok_number, int nach);
void setBufferWhithoutAnim();
void playAnim(byte blok);

int myGetTemp(char* &text, byte &txt_l, int t);

void load_data_bloks();
void save_data_bloks();
void load_fon_anims();
void save_fon_anims();
void load_alpfaveet();
void save_alpfaveet();

void sendWithFonAnim(byte blok);

void add_simv(WiFiClient &client);

void mainSait(WiFiClient &client);
void add_b_site(WiFiClient &client);

bool time_tochki = 0;
bool on_off_tochki;


///////////////////////Фоновые анимации///////////////////
byte fon_anim_colich;


class fon_animations {
  public:
    char* name_;
    char simv;
    int x_vect;
    int y_vect;
    float time_an;
    
    int x_col[2];
    int y_col[2];
    bool invers;
//    byte notprozr;

    int x_nach[2];
    int y_nach[2];
    float plotn[2];
    
//    ~fon_animations() {
//      delete[] name_;
//    }
};

fon_animations* fon_anims;

void del_anims(int num){
  fon_animations* t = new fon_animations[fon_anim_colich - 1];

  for(int i = 0; i < num; i++) t[i] = fon_anims[i];
  for(int i = num+1; i < fon_anim_colich; i++) t[i-1] = fon_anims[i];
  delete[] fon_anims[num].name_;

  fon_anim_colich--;

  delete[] fon_anims;
  fon_anims = t;
}

void add_fon_anim(char n[]) {
  fon_animations* t = new fon_animations[fon_anim_colich + 1];
  for (int i = 0; i < fon_anim_colich; i++) {
    t[i] = fon_anims[i];
  }
  t[fon_anim_colich].name_ = n;
  delete[] fon_anims;
  fon_anims = t;
  fon_anim_colich++;
};

///////////////////////Блоки текcта/////////////////////

class bloks_of_text {
  public:
    char* text;
    byte leng = 0;
    
    float text_time;
    byte anim;
    int anim_time = 10;
    byte fon_anim;
//    int text_t;
    
    int day1_month1_day2;
    byte month2;
    
    bloks_of_text() {}
    ~bloks_of_text() {
//      if(leng) delete[] text;
    }
};

bloks_of_text* send_text;
byte bloks = 0;


void add_text_blok(bloks_of_text &bl) {
  bloks_of_text* t = new bloks_of_text[bloks + 1];
  for (int i = 0; i < bloks; i++) {
    t[i] = send_text[i];
  }
  t[bloks] = bl;
  if(bl.leng) t[bloks].text = new char[bl.leng+1];
  for (int j = 0; j < bl.leng; j++) {
    t[bloks].text[j] = bl.text[j];
  }
  t[bloks].text[bl.leng] = '\0';

  if (bloks) {
    delete[] send_text; // Чтобы ядро не cвихнулоcь нужно удалять маccив только когда там что-то еcть
  }
  send_text = t;
  bloks++;
}

void delete_text_bloks(int num) {
  bloks_of_text* t = new bloks_of_text[num];
  for (int i = 0; i < num; i++) {
    t[i] = send_text[i];
  }
  for (int i = num; i < bloks; i++) {
    if(send_text[i].leng) delete[] send_text[i].text; // Не забываем отчищать память...
  }
  delete[] send_text;
  send_text = t;
  bloks = num;
}


class bukovki_32 {
  public:
    char bukva;
    byte lenght;
    int32_t *bitMap;
//    ~bukovki_32(){delete[] bitMap;}
};

int alph_length = 0;
bukovki_32 *alphaveet;


class partikle{
public:
  float x; // координата х
  float y; // координата у
  
  int Vx; // скорость по оси х
  int Vy; // скорость по оси у

  int x_col[2]; // колебания частицы по x
  int y_col[2]; // колебания частицы по y

  uint32_t timer; // персональный таймер
//  byte pwm; // переменная для создания шим сигнал для регулирования яркости символов
  fon_animations* predok; // ссылка на настройки остальные анимации, незачем здесь память тратить

  partikle* next = nullptr; // будет очередь
};

partikle pt1;
byte part_length = 0; // Переменная, котрую я был вынужден вытащить сюды
uint32_t timer_fanim;


#include "3.1MainSite.h"
#include "3.2AddBykovkaSite.h"
#include "3.3AddFonAnimationSite.h"
#include "3.4Data_Time.h"
