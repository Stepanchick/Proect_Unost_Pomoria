////////////////Главный файл c функциями для обновления панели////////////
#include "1allInOne.h"
#include <Random16.h>



/*  Гайд по управлению матрицей:

   1. по SPI Выдаем данные, которые необходимо загрузить в матрицу (сначало это, чтобы матрица поменялась моментально независимо от размеров и количестава загружаемых данных)
   2. nOE Переводим в 0 тем самым получаем доступ к изменению состояния светодиодов, по факту перейдя в режим редактирования панели
   3. ножками A и B Выбираем обновляемую группу светодиодов (ряд)
   4. Подтверждаем выбор подачей короткого импульса на SCLK
   5. Устанавливаем логическую единице на nOE чтобы сохранить настройки и выйти из режима редактирования
   ВАЖНО: одномоментно может гореть только один ряд светодиодов

   P.S. там стоит инвентор(логическое не) поэтому так
*/


void SendData() {

  // 1 3 5 7  9 11 13 15
  // 2 4 6 8 10 12 14 16 <- Пока перевернутые панели

  // 0b0001000100010001 0001000100010001
  // верх                            низ


  for (byte i = 0; i < 4; i++) { // выбор ряда
    for (int t = HEIGTH; t > 0; t--) {
      for (int j = 0; j < WIGHT; j++) { // выбираем пару ПАНЕЛЕЙ по оси х.
        for (int a = 0; a < 4; a++) { //выбор сегмента в пределах одной пнаели по ширине
          for (int b = 0; b < 4; b++) { //выбор сегмента в пределах одной пнаели по высоте
            for (int c = a * 8; c < a * 8 + 8; c ++) {
              if (t <= HEIGTH / 2) {
                digitalWrite(R_pin, !(BufferData[j * 32 + c] & (1 << t * 16 - b * 4 - i - 1)));
              }
              else {
                digitalWrite(R_pin, !(BufferData[WIGHT * 32 - j * 32 - 1 - c] & (1 << t * 16 - 16 + b * 4 + i)));
              }
              digitalWrite(CLK, 1);
              digitalWrite(CLK, 0);

            }
          }
        }
      }
    }

    digitalWrite(nOE, 0);
    digitalWrite(A_pin, !(i & 1));
    digitalWrite(B_pin, !(i > 1));
    digitalWrite(SCLK, 1);
    digitalWrite(SCLK, 0);
    digitalWrite(nOE, 1);
  }

}

float temp_f;


void setTextFrom(int blok_number, int nach) { // Ход конем
  //  send_text[blok_number].text // Текст, который нужно написать

  byte txt_l = send_text[blok_number].leng;
  char* text = new char[txt_l];
  for (int i = 0; i < txt_l; i++) text[i] = send_text[blok_number].text[i];


  for (int i = 0; i < WIGHT * 32; i++) dataFromSend[i] = 0;

  for (int i = 0; i < txt_l; i++) { // Замена переменных на их числовые значения
    if (text[i] == '_') { // Обработчик служебных блоков
      if (text[i + 1] == 'D' && text[i + 2] == 'A' && text[i + 3] == 'T' && text[i + 4] == 'A') {
        i += myGetData(text, txt_l, i);
      }
      if (text[i + 1] == 'T' && text[i + 2] == 'I' && text[i + 3] == 'M' && text[i + 4] == 'E') {
        i += myGetTime(text, txt_l, i);
      }
      if (text[i + 1] == 'T' && text[i + 2] == 'E' && text[i + 3] == 'M' && text[i + 4] == 'P') {
        i += myGetTemp(text, txt_l, i);
      }
    }
  }


  int dlinna_texta_pikseli_and_nachalo_texta = nach;
  for (int i = 0; i < txt_l; i++) {

    for (int j = 0; j < alph_length; j++) { // Перебираем алфавит в поисках нужной буквы
      if (text[i] == alphaveet[j].bukva) {
        // Далее - цикл занесения необходимой буквы в буфер

        for (int a = 0; a < alphaveet[j].lenght; a++) {

          if (dlinna_texta_pikseli_and_nachalo_texta < 0 || dlinna_texta_pikseli_and_nachalo_texta >= WIGHT * 32) {
            dlinna_texta_pikseli_and_nachalo_texta++;
            continue;
          }
          dataFromSend[dlinna_texta_pikseli_and_nachalo_texta] = alphaveet[j].bitMap[a];
          dlinna_texta_pikseli_and_nachalo_texta++;
        }
        continue;
      }
    }
  }

  delete[] text;
}


void setText(int blok_number) { // Заполнение нового текстового блока
  //  send_text[blok_number].text // Текст, который нужно написать

  byte txt_l = send_text[blok_number].leng;
  char* text = new char[txt_l];
  for (int i = 0; i < txt_l; i++) text[i] = send_text[blok_number].text[i];


  for (int i = 0; i < WIGHT * 32; i++) dataFromSend[i] = 0;

  for (int i = 0; i < txt_l; i++) { // Замена переменных на их числовые значения
    if (text[i] == '_') { // Обработчик служебных блоков
      if (text[i + 1] == 'D' && text[i + 2] == 'A' && text[i + 3] == 'T' && text[i + 4] == 'A') {
        i += myGetData(text, txt_l, i);
      }
      if (text[i + 1] == 'T' && text[i + 2] == 'I' && text[i + 3] == 'M' && text[i + 4] == 'E') {
        i += myGetTime(text, txt_l, i);
      }
      if (text[i + 1] == 'T' && text[i + 2] == 'E' && text[i + 3] == 'M' && text[i + 4] == 'P') {
        
        if (dst.ready()) {         // измерения готовы по таймеру
          if (dst.readTemp()) {  // если чтение успешно
            temp_f = dst.getTemp();
            dst.requestTemp();  // запрос следующего измерения
          }
        }
        
        i += myGetTemp(text, txt_l, i);
      }
    }
  }

  int dlinna_texta_pikseli_and_nachalo_texta = 0;
  for (int i = 0; i < txt_l; i++) {
    for (int j = 0; j < alph_length; j++) { // Перебираем алфавит в поисках нужной буквы
      if (text[i] == alphaveet[j].bukva) {
        dlinna_texta_pikseli_and_nachalo_texta += alphaveet[j].lenght;
        break;
      }
    }
  }
  // Отлично, здесь у нас будет информация о том, сколько пикселей занимает запись
  // и массив символов где необходимые переменные заменены на цифры


  dlinna_texta_pikseli_and_nachalo_texta = WIGHT * 16 - dlinna_texta_pikseli_and_nachalo_texta / 2; // Теперь у нас есть координаты начала теста

  for (int i = 0; i < txt_l; i++) {

    for (int j = 0; j < alph_length; j++) { // Перебираем алфавит в поисках нужной буквы
      if (text[i] == alphaveet[j].bukva) {
        // Далее - цикл занесения необходимой буквы в буфер

        for (int a = 0; a < alphaveet[j].lenght; a++) {

          if (dlinna_texta_pikseli_and_nachalo_texta < 0 || dlinna_texta_pikseli_and_nachalo_texta >= WIGHT * 32) {
            dlinna_texta_pikseli_and_nachalo_texta++;
            continue;
          }
          dataFromSend[dlinna_texta_pikseli_and_nachalo_texta] = alphaveet[j].bitMap[a];
          dlinna_texta_pikseli_and_nachalo_texta++;
        }
        continue;
      }
    }
  }

  delete[] text;
}


void setBufferWhithoutAnim() { // Заполнение буфера
  for (int i = 0; i < WIGHT * 32; i++)  BufferData[i] = dataFromSend[i];
}

int myGetTemp(char* &text, byte &txt_l, int t) {
  String temp;
  char* ttext = text;
  //  if (dst.ready()) {         // измерения готовы по таймеру
  //    if (dst.readTemp()) {  // если чтение успешно
  //      temp_f = dst.getTemp();
  //      dst.requestTemp();  // запрос следующего измерения
  //    }
  //  }

  temp = String(temp_f);

  text = new char[txt_l - 5 + temp.length() - 1];

  for (int i = 0; i < t; i++) {
    text[i] = ttext[i];
  }
  for (int i = t; i < t + temp.length() - 1; i++) {
    text[i] = temp.charAt(i - t);
  }
  for (int i = t + temp.length() - 1; i < txt_l - 5 + temp.length() - 1; i++) {
    text[i] = ttext[i + 5 - temp.length() + 1];
  }

  txt_l = txt_l - 5 + temp.length() - 1;
  delete[] ttext;
  //  ttext = text;

  //   for (int i = 0; i < txt_l; i++) {
  //    Serial.print(text[i]);
  //  }
  //  Serial.println();
  //  dst.requestTemp();  // запрос следующего измерения
  return temp.length() - 1;
}


int myGetData(char* &text, byte &txt_l, int t) {
  Datime dt(dsdt);

  char* bufer = new char[11];
  dt.dateToChar(bufer);

  char* ttext = text;

  text = new char[txt_l - 5 + 10];

  for (int i = 0; i < t; i++) {
    text[i] = ttext[i];
  }
  for (int i = t; i < t + 10; i++) {
    text[i] = bufer[i - t];
  }
  for (int i = t + 10; i < txt_l - 5 + 10; i++) {
    text[i] = ttext[i + 5 - 10];
  }

  txt_l = txt_l - 5 + 10;
  delete[] ttext;
  delete[] bufer;

  return 10;
}

int myGetTime(char* &text, byte &txt_l, int t) {
  Datime dt(dsdt);
  time_tochki = 1;
  char* bufer = new char[9];
  dt.timeToChar(bufer);

  char* ttext = text;

  text = new char[txt_l - 5 + 5];

  for (int i = 0; i < t; i++) {
    text[i] = ttext[i];
  }

  for (int i = t; i < t + 2; i++) {
    text[i] = bufer[i - t];
  }
  if (on_off_tochki) {
    text[t + 2] = ':';
  }
  else {
    text[t + 2] = ' ';
  }

  for (int i = t + 3; i < t + 3 + 2; i++) {
    text[i] = bufer[i - t];
  }

  for (int i = t + 5; i < txt_l - 5 + 5; i++) {
    text[i] = ttext[i + 5 - 5];
  }

  txt_l = txt_l - 5 + 5;
  delete[] ttext;
  delete[] bufer;

  return 5;
}

// ------------------------------Обработка фоновых анимаций--------------------------------

/*
    char* name_;
    char simv;
    int x_vect;
    int y_vect;
    int time_an;

    int x_col[2];
    int y_col[2];
    bool invers;
    byte notprozr;

    int x_nach[2];
    int y_nach[2];
    int plotn;
*/

//class partikle{
//public:
//  float x; // координата х
//  float y; // координата у
//
//  int Vx; // скорость по оси х
//  int Vy; // скорость по оси у
//
//  int x_col[2]; // колебания частицы по x
//  int y_col[2]; // колебания частицы по y
//
//  uint32_t timer; // персональный таймер
//  byte pwm; // переменная для создания шим сигнал для регулирования яркости символов
//  fon_animations* predok; // ссылка на настройки остальные анимации, незачем здесь память тратить
//
//  partikle* next = nullptr; // будет очередь
////  partikle* back = nullptr;
//};
//
//
//
//partikle pt1;




void sendWithFonAnim(byte blok) {
  //  setBuffer(); // обновляем буфер?

  Random16 rnd;

  partikle* pt = &pt1;

  if (part_length != 0) {
    if (millis() > pt->next->timer + pt->next->predok->time_an * 1000) {
      if (part_length == 1) {
        delete pt->next;
        part_length--;
        SendData(); // выводим содержимое буффера
        return; // и валим от греха подальше
      }
      else {
        partikle* pt2;
        pt2 = pt->next->next;
        delete pt->next;
        pt->next = pt2;
        part_length--;
      }
    }

    pt = pt->next;
    int s = -1;

    //  Serial.println(part_length);


    for (int j = 0; j < alph_length; j++) if (pt->predok->simv == alphaveet[j].bukva) {
        s = j;  // краткая форма записи алгоритма по поиску символа
        j = alph_length;
      }

    for (byte i = 0; i < part_length; i++) {

      if ( !(pt->y > HEIGTH * 16 * 2  || pt->y < -HEIGTH * 16 * 2  || pt->x > WIGHT * 32 + 64 * 2 ||  pt->x < -64 * 2)) { // во избежание переполнения
        pt->x += (float)pt->Vx / 1000; // перво-наперво надо посчитать физику частицы
        pt->y += (float)pt->Vy / 1000;
      }

      // Дальше прописываем отрисовку



      if (s >= 0 && pt->x + alphaveet[s].lenght > 0 && s < alph_length) { // && pt->y+32 > 0 && pt->y < HEIGTH*16 && pt->x < WIGHT*32){ // Символ длжен существовать и быть в пределах видимой зоны
        for (byte a = 0; a < alphaveet[s].lenght; a++) {
          if (a + pt->x >= 0 && a + pt->x < WIGHT * 32) {
            if (pt->y < 0) {
              if (a + (int)pt->x >= 0 && a + (int)pt->x < WIGHT * 32) BufferData[a + (int)pt->x] = pt->predok->invers ? BufferData[a + (int)pt->x] ^ (alphaveet[s].bitMap[a] << (int)pt->y * (-1)) : BufferData[a + (int)pt->x] | (alphaveet[s].bitMap[a] << (int)pt->y * (-1));
            }
            else {
              if (a + (int)pt->x >= 0 && a + (int)pt->x < WIGHT * 32) BufferData[a + (int)pt->x]  = pt->predok->invers ?  BufferData[a + (int)pt->x] ^ (alphaveet[s].bitMap[a] >> (int)pt->y) : BufferData[a + (int)pt->x] | (alphaveet[s].bitMap[a] >> (int)pt->y);
            }
          }
        }
      }

      //    pt->pwm++;
      //    if(pt->pwm > 100) pt->pwm = 0;

      // Здеся проверка и удаление уже отработавших частиц
      if (i < part_length - 1) {

        if (millis() > pt->next->timer + pt->next->predok->time_an * 1000) {
          if (i < part_length - 2) {
            partikle* pt2;
            pt2 = pt->next->next;
            delete pt->next;
            pt->next = pt2;
          }
          else {
            delete pt->next;
            part_length--;
            SendData(); // выводим содержимое буффера
            return; // и валим от греха подальше
          }
          part_length--;

        }


        if (pt->predok->simv != pt->next->predok->simv) for (int j = 0; j < alph_length; j++) if (pt->next->predok->simv == alphaveet[j].bukva) {
              s = j;
              j = alph_length;
            }
        pt = pt->next;
      }

    }

    //   part_length = newlength;

    // Тута добавление новых частиц по таймеру
  }

  if (send_text[blok].fon_anim == byte(-1) || send_text[blok].fon_anim >= fon_anim_colich ) {
    SendData();
    return;
  }

  if (part_length < 150 && millis() > timer_fanim) {
    timer_fanim = millis();
    timer_fanim += rnd.get((int)(fon_anims[send_text[blok].fon_anim].plotn[0] * 1000), (int)(fon_anims[send_text[blok].fon_anim].plotn[1] * 1000));

    rnd.setSeed(timer_fanim * rnd.get());

    pt->next = new partikle;
    pt = pt->next;

    pt->x_col[0] = fon_anims[send_text[blok].fon_anim].x_col[0];
    pt->x_col[1] = fon_anims[send_text[blok].fon_anim].x_col[1];

    pt->y_col[0] = fon_anims[send_text[blok].fon_anim].y_col[0];
    pt->y_col[1] = fon_anims[send_text[blok].fon_anim].y_col[1];



    pt->Vx = fon_anims[send_text[blok].fon_anim].x_vect + rnd.get(pt->x_col[0], pt->x_col[1] + 1);
    pt->Vy = fon_anims[send_text[blok].fon_anim].y_vect + rnd.get(pt->y_col[0], pt->y_col[1] + 1);

    float x = (float)rnd.get(fon_anims[send_text[blok].fon_anim].x_nach[0], fon_anims[send_text[blok].fon_anim].x_nach[1]);
    float y = (float)rnd.get(fon_anims[send_text[blok].fon_anim].y_nach[0], fon_anims[send_text[blok].fon_anim].y_nach[1]);


    pt->x = x;
    pt->y = y;



    pt->timer = millis();

    //    pt->pwm = fon_anims[send_text[blok].fon_anim].notprozr;
    pt->predok = &fon_anims[send_text[blok].fon_anim];

    part_length++;
  }

  SendData(); // выводим содержимое буффера
}
