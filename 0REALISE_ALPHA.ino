#include "1allInOne.h"

//////////////////////////////////////Главный файл проекта. Здеcь будут подключатьcя библиотеки и раcпределятьcя задачи/////////////////


TaskHandle_t Task1;

// ;)

int32_t dataFromSend[WIGHT * 32] = {0}; // То что выводится на панель сейчас
int32_t BufferData[WIGHT * 32] = {0}; // То что должно вывестись на панель в итоге


void setup() {

//  Serial.begin(115200);

  pinMode(A_pin, OUTPUT);
  pinMode(B_pin, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(R_pin, OUTPUT);
  pinMode(nOE, OUTPUT);
  pinMode(2, OUTPUT);

  //  pinMode(I2C_SDA, OUTPUT);
  //  pinMode(I2C_SCL, OUTPUT);
  //  pinMode(termo_pin, OUTPUT);

  Wire.begin();
  if (!dsdt.begin()) dsdt.setBuildTime();
  dst.requestTemp();  // запрос следующего измерения

  load_data_bloks(); // Без комментариев
  load_fon_anims(); // Тут и так все ясно
  load_alpfaveet(); // Что происходит выгрузка данных из Энергонезависимой памяти

  xTaskCreatePinnedToCore(
    Task1code, /* Функция, cодержащая код задачи */
    "Task1", /* Название задачи */
    10000, /* Размер cтека в cловах */
    NULL, /* Параметр cоздаваемой задачи */
    0, /* Приоритет задачи */
    &Task1, /* Идентификатор задачи */
    0); /* Ядро, на котором будет выполнятьcя задача */



  BufferData[0] = 0b0000000000000000;
  BufferData[1] = 0b0000000000000000;
  BufferData[2] = 0b0001111100000000;
  BufferData[3] = 0b0010000010000000;
  BufferData[4] = 0b0100000001000000;
  BufferData[5] = 0b1000100000100000;
  BufferData[6] = 0b1000101100100000;
  BufferData[7] = 0b1000100000100000;
  BufferData[8] = 0b1000000000100000;
  BufferData[9] = 0b1001000100100000;
  BufferData[10] = 0b1001111100100000;
  BufferData[11] = 0b0100000001000000;
  BufferData[12] = 0b0010000010000000;
  BufferData[13] = 0b0001111100000000;
  BufferData[14] = 0b0000000000000000;
  BufferData[15] = 0b0000000000000000;
}

///////////////////Здеcь же будет реализован вызов функций по изменению cоcтояния панели/////////////////////
uint32_t timer;
byte blok = 0; // номер блока который выводим
byte f_anim;

void loop() {
  blok = 0;
  SendData(); // выводим содержимое буффера

  while (bloks > 0) { // Пока нет информации, которую нужно вывести, уходим в бесконечный цикл КОД: Ага, щас
    //  SendData(); // выводим содержимое буффера

    if (dsdt.tick() && time_tochki) {
      on_off_tochki = !on_off_tochki;
      setText(blok);
    }

    if (millis() - timer >= send_text[blok].text_time * 1000) {  // ищем разницу
//      timer = millis();                   // сброс таймера
      byte fa = send_text[blok].fon_anim;
      Datime dt(dsdt);
      bool my_station = 0;


      do { // Проверка на совпадение даты
        blok++;
        if (blok >= bloks) {
          blok = 0;
        }

        if (send_text[blok].day1_month1_day2) {

          if (send_text[blok].month2) {
            if (send_text[blok].day1_month1_day2 <= send_text[blok].month2) {
              if (((send_text[blok].day1_month1_day2 & 0b0000001111000000) >> 6) == dt.month) my_station = ((send_text[blok].day1_month1_day2 & 0b1111110000000000) >> 10) <= dt.day;
              else if (((send_text[blok].day1_month1_day2 & 0b0000001111000000) >> 6) < dt.month) my_station = 1;

              if (send_text[blok].month2 == dt.month) my_station &= (send_text[blok].day1_month1_day2 & 0b0000000000111111) >= dt.day;
              else if (send_text[blok].month2 < dt.month) my_station = 0;
            }
            else {
              if (send_text[blok].month2 == dt.month) my_station = (send_text[blok].day1_month1_day2 & 0b0000000000111111) <= dt.day;
              else if (send_text[blok].month2 < dt.month) my_station = 1;

              if (((send_text[blok].day1_month1_day2 & 0b0000001111000000) >> 6) == dt.month) my_station &= ((send_text[blok].day1_month1_day2 & 0b1111110000000000) >> 10) >= dt.day;
              else if (((send_text[blok].day1_month1_day2 & 0b0000001111000000) >> 6) < dt.month) my_station = 0;
            }
            my_station = !my_station;

          }
          else {
            my_station = ((send_text[blok].day1_month1_day2 & 0b0000011111110000) >> 4) == dt.day;
            my_station &= (send_text[blok].day1_month1_day2 & 0b0000000000001111) == dt.month;

          }
        }
        else my_station = 1;


      } while (!my_station); // конец проверки на совпадение даты

      time_tochki = 0;
      on_off_tochki = 0;
      setText(blok); // формируем окончательное изображение на пaнели

      if (send_text[blok].anim+1 && send_text[blok].anim_time) playAnim(blok);
      
//      if(send_text[blok].anim == 4 || send_text[blok].anim == 5){ // Колхоз lvl 1000
//        int dlin = 0;
//        byte txt_l = send_text[blok].leng;
//        char* text = new char[txt_l];
//        for (int i = 0; i < txt_l; i++) text[i] = send_text[blok].text[i];
//
//
//        for (int i = 0; i < WIGHT * 32; i++) dataFromSend[i] = 0;
//
//        for (int i = 0; i < send_text[blok].leng; i++) { // Замена переменных на их числовые значения
//          if (send_text[blok].text[i] == '_') { // Обработчик служебных блоков
//            if (send_text[blok].text[i + 1] == 'D' && send_text[blok].text[i + 2] == 'A' && send_text[blok].text[i + 3] == 'T' && send_text[blok].text[i + 4] == 'A') {
//              myGetData(text, txt_l, i);
//              i += 5;
//            }
//            if (send_text[blok].text[i + 1] == 'T' && send_text[blok].text[i + 2] == 'I' && send_text[blok].text[i + 3] == 'M' && send_text[blok].text[i + 4] == 'E') {
//              myGetTime(text, txt_l, i);
//              i += 5;
//            }
//            if (send_text[blok].text[i + 1] == 'T' && send_text[blok].text[i + 2] == 'E' && send_text[blok].text[i + 3] == 'M' && send_text[blok].text[i + 4] == 'P') {
//              myGetTemp(text, txt_l, i);
//              i += 5;
//            }
//          }
//        }
//        for (int i = 0; i < txt_l; i++) {
//          for (int j = 0; j < alph_length; j++) { // Перебираем алфавит в поисках нужной буквы
//            if (text[i] == alphaveet[j].bukva) {
//              dlin += alphaveet[j].lenght;
//              break;
//            }
//          }
//        }
//        delete[] text;
//
//        uint32_t anim_timer = 0;
//
//        for (int i = -dlin; i < WIGHT * 32 + 1, send_text[blok].anim == 5; i++) {
//          anim_timer = millis() + send_text[blok].anim_time * 1000 / max(dlin, WIGHT * 32);
//          while (millis() < anim_timer) {
//            for (int i = 0; i < WIGHT * 32; i++) BufferData[i] = 0;
//            setTextFrom(blok, i);
//            setBufferWhithoutAnim();
//            sendWithFonAnim(blok);
//          }
//        }
//
//        for (int i = WIGHT * 32; i >= -dlin, send_text[blok].anim == 4; i--) {
//          anim_timer = millis() + send_text[blok].anim_time * 1000 / max(dlin, WIGHT * 32);
//          while (millis() < anim_timer) {
//            for (int i = 0; i < WIGHT * 32; i++) BufferData[i] = 0;
//            setTextFrom(blok, i);
//            setBufferWhithoutAnim();
//            sendWithFonAnim(blok);
//          }
//        }
//        
//      }
      
      timer = millis();                   // сброс таймера

      if (fa != send_text[blok].fon_anim) timer_fanim = 0;
    }


    if (send_text[blok].text_time) {
      setBufferWhithoutAnim();
      sendWithFonAnim(blok);
    }

  }
}


// Структура EEPROM(Preferences):

// Пароль вай-фай
// Блоки текста
// Фоновые анимации
// Алфавит

/////////////////////////////////////////Сохранение/загрузка блоков текста/////////////////////////////

void save_data_bloks() {
  preferences.begin("text_bloks", false); // открываем прочтранство имен в режиме чтения-записи, дабы занести занчения
  preferences.clear();

  byte pass_l = 0;
  for (; password[pass_l] != '\0'; pass_l++);

  preferences.putBytes("WIFI_pass", password, pass_l);

  preferences.putUChar("bloks", bloks);

  for (int i = 0; i < bloks; i++) { // Начало записи блоков текста в память
    String namespase;
    char* buf;

    namespase = "length";
    namespase += i;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putUChar(buf, send_text[i].leng);

    namespase = "text";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putBytes(buf, send_text[i].text, send_text[i].leng);

    namespase = "text_time";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putFloat(buf, send_text[i].text_time);

    namespase = "anim";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putUChar(buf, send_text[i].anim);

    namespase = "anim_time";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, send_text[i].anim_time);

    namespase = "fon_anim";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putUChar(buf, send_text[i].fon_anim);

    namespase = "d1_m1_d2";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, send_text[i].day1_month1_day2);

    namespase = "month2";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putUChar(buf, send_text[i].month2);

    delete[] buf;
  }                           // Конец записи блоков текста
  preferences.end();

}



void load_data_bloks() {
  preferences.begin("text_bloks", true); // открываем прочтранство имен в режиме только для чтения, чтобы извлечь данные

  byte pass_l = 0;
  pass_l = preferences.getBytesLength("WIFI_pass");

  if (pass_l < 8) {
    password = new char[]{"password"};
  }
  else {
    if (password) delete[] password;
    password = new char[pass_l + 1];
    preferences.getBytes("WIFI_pass", password, pass_l);
    password[pass_l] = '\0';
  }

  bloks = preferences.getUChar("bloks");

  if (send_text) delete[] send_text;
  send_text = new bloks_of_text[bloks];


  for (int i = 0; i < bloks; i++) { // Начало чтения блоков текста в память
    String namespase;
    char* buf;

    namespase = "length";
    namespase += i;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    send_text[i].leng = preferences.getUChar(buf);

    send_text[i].text = new char[send_text[i].leng + 1];

    namespase = "text";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.getBytes(buf, send_text[i].text, send_text[i].leng);
    send_text[i].text[send_text[i].leng] = '\0';

    namespase = "text_time";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    send_text[i].text_time = preferences.getFloat(buf);

    namespase = "anim";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    send_text[i].anim = preferences.getUChar(buf);

    namespase = "anim_time";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    send_text[i].anim_time = preferences.getInt(buf);

    namespase = "fon_anim";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    send_text[i].fon_anim = preferences.getUChar(buf);

    namespase = "d1_m1_d2";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    send_text[i].day1_month1_day2 = preferences.getInt(buf);

    namespase = "month2";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    send_text[i].month2 = preferences.getUChar(buf);

    delete[] buf;
  }                           // Конец чтения блоков текста

  preferences.end();
}



/////////////////////////////////////////Сохранение/загрузка фоновых анимаций/////////////////////////////
void save_fon_anims() {
  preferences.begin("fon_anims", false); // открываем прочтранство имен в режиме чтения-записи, дабы занести занчения
  preferences.clear();

  preferences.putUChar("fon_anim_colich", fon_anim_colich);

  for (int i = 0; i < fon_anim_colich; i++) {
    String namespase;
    char* buf;

    byte faname_l = 0;
    for (; fon_anims[i].name_[faname_l] != '\0'; faname_l++);

    namespase = "name";
    namespase += i;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putBytes(buf, fon_anims[i].name_, faname_l);

    namespase = "simv";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putChar(buf, fon_anims[i].simv);

    namespase = "x_vect";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].x_vect);

    namespase = "y_vect";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].y_vect);

    namespase = "time_an";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putFloat(buf, fon_anims[i].time_an);

    namespase = "x_col0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].x_col[0]);

    namespase = "x_col1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].x_col[1]);

    namespase = "y_col0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].y_col[0]);

    namespase = "y_col1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].y_col[1]);

    namespase = "invers";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putBool(buf, fon_anims[i].invers);

    namespase = "x_nach0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].x_nach[0]);

    namespase = "x_nach1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].x_nach[1]);

    namespase = "y_nach0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].y_nach[0]);

    namespase = "y_nach1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].y_nach[1]);


    namespase = "plotn0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].plotn[0]);

    namespase = "plotn1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putInt(buf, fon_anims[i].plotn[1]);

    delete[] buf;
  }

  preferences.end();
}


void load_fon_anims() {
  preferences.begin("fon_anims", true); // открываем прочтранство имен в режиме только для чтения, чтобы извлечь данные

  fon_anim_colich = preferences.getUChar("fon_anim_colich");

  fon_anims = new fon_animations[fon_anim_colich];

  for (int i = 0; i < fon_anim_colich; i++) {
    String namespase;
    char* buf;

    namespase = "name";
    namespase += i;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    byte length_txt =  preferences.getBytesLength(buf);
    fon_anims[i].name_ = new char[length_txt + 1];
    preferences.getBytes(buf, fon_anims[i].name_, length_txt);
    fon_anims[i].name_[length_txt] = '\0';

    namespase = "simv";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].simv = preferences.getChar(buf);

    namespase = "x_vect";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].x_vect = preferences.getInt(buf);

    namespase = "y_vect";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].y_vect = preferences.getInt(buf);

    namespase = "time_an";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].time_an = preferences.getFloat(buf);

    namespase = "x_col0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].x_col[0] = preferences.getInt(buf);

    namespase = "x_col1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].x_col[1] = preferences.getInt(buf);

    namespase = "y_col0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].y_col[0] = preferences.getInt(buf);

    namespase = "y_col1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].y_col[1] = preferences.getInt(buf);

    namespase = "invers";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].invers = preferences.getBool(buf);

    namespase = "x_nach0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].x_nach[0] = preferences.getInt(buf);

    namespase = "x_nach1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].x_nach[1] = preferences.getInt(buf);

    namespase = "y_nach0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].y_nach[0] = preferences.getInt(buf);

    namespase = "y_nach1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].y_nach[1] = preferences.getInt(buf);


    namespase = "plotn0";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].plotn[0] = preferences.getInt(buf);

    namespase = "plotn1";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    fon_anims[i].plotn[1] = preferences.getInt(buf);

    delete[] buf;
  }
  preferences.end();
}

/////////////////////////////////////////Сохранение/загрузка алфавита/////////////////////////////

void save_alpfaveet() {
  preferences.begin("alpfaveet", false); // открываем прочтранство имен в режиме чтения-записи, дабы занести занчения
  preferences.clear();
  
  preferences.putUChar("alph_length", alph_length);

  for (int i = 0; i < alph_length; i++) {
    String namespase;
    char* buf;

    namespase = "bukva";
    namespase += i;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putChar(buf, alphaveet[i].bukva);

    namespase = "lenght";
    namespase += i;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    preferences.putUChar(buf, alphaveet[i].lenght);


    for (int j = 0; j < alphaveet[i].lenght; j++) {
      namespase = "bitMap";
      namespase += i;
      namespase += j;
      delete[] buf;
      buf = new char[namespase.length() + 1];
      namespase.toCharArray(buf, namespase.length() + 1);
      preferences.putInt(buf, alphaveet[i].bitMap[j]);
    }

    delete[] buf;
  }

  preferences.end();
}

void load_alpfaveet() {
  preferences.begin("alpfaveet", true); // открываем прочтранство имен в режиме только для чтения, чтобы извлечь данные

  alph_length = preferences.getUChar("alph_length");

  alphaveet = new bukovki_32[alph_length];

  for (int i = 0; i <  alph_length; i++) {

    String namespase;
    char* buf;



    namespase = "bukva";
    namespase += i;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    alphaveet[i].bukva = preferences.getChar(buf);

    namespase = "lenght";
    namespase += i;
    delete[] buf;
    buf = new char[namespase.length() + 1];
    namespase.toCharArray(buf, namespase.length() + 1);
    alphaveet[i].lenght = preferences.getUChar(buf);


    alphaveet[i].bitMap = new int32_t[alphaveet[i].lenght];


    for (int j = 0; j < alphaveet[i].lenght; j++) {
      namespase = "bitMap";
      namespase += i;
      namespase += j;
      delete[] buf;
      buf = new char[namespase.length() + 1];
      namespase.toCharArray(buf, namespase.length() + 1);
      alphaveet[i].bitMap[j] = preferences.getInt(buf);
    }

    delete[] buf;
  }

  preferences.end();
}
