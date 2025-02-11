/////////////////Главный файл анимаций//////////////
#include "1allInOne.h"

void playAnim(byte blok) { // Заполнение буфера
  //  for (int i = 0; i < WIGHT * 32; i++)  BufferData[i] = dataFromSend[i];
  uint32_t anim_timer = 0;
  switch (send_text[blok].anim) {
    case 0: { // Выплывание справа
        for (int i = WIGHT * 32; i >= 0; i--) {
          anim_timer = millis() + send_text[blok].anim_time * 1000 / (WIGHT * 32);
          while (millis() < anim_timer) {
            for (int j = 0; j < WIGHT * 32; j++) {
              if (j >= i) BufferData[j] = dataFromSend[j - i];
              else BufferData[j] = 0;
            }
            if (dsdt.tick() && time_tochki) {
              on_off_tochki = !on_off_tochki;
              setText(blok);
            }
            sendWithFonAnim(blok);
          }
        }
      } break;

    case 1: { // Выплывание слева
        for (int i = 0; i < WIGHT * 32; i++) {
          anim_timer = millis() + send_text[blok].anim_time * 1000 / (WIGHT * 32);
          while (millis() < anim_timer) {
            for (int j = 0; j < WIGHT * 32; j++) {
              if (j < i) BufferData[j] = dataFromSend[WIGHT * 32 - (i - j)];
              else BufferData[j] = 0;
            }
            if (dsdt.tick() && time_tochki) {
              on_off_tochki = !on_off_tochki;
              setText(blok);
            }
            sendWithFonAnim(blok);
          }
        }
      } break;
    case 2: { // Падение
        for (int i = HEIGTH * 16 - 1; i >= 0; i--) {
          anim_timer = millis() + send_text[blok].anim_time * 1000 / (HEIGTH * 16);
          while (millis() < anim_timer) {
            for (int j = WIGHT * 32 - 1; j >= 0; j--) {
              BufferData[j] = dataFromSend[j] >> i;
            }
            if (dsdt.tick() && time_tochki) {
              on_off_tochki = !on_off_tochki;
              setText(blok);
            }
            sendWithFonAnim(blok);
          }
        }
      } break;
    case 3: { // Всплытие
        for (int i = HEIGTH * 16 - 1; i >= 0; i--) {
          anim_timer = millis() + send_text[blok].anim_time * 1000 / (HEIGTH * 16);
          while (millis() < anim_timer) {
            for (int j = 0; j < WIGHT * 32; j++) {
              BufferData[j] = dataFromSend[j] << i;
            }
            if (dsdt.tick() && time_tochki) {
              on_off_tochki = !on_off_tochki;
              setText(blok);
            }
            sendWithFonAnim(blok);
          }
        }
      } break;
    case 4: { // Проплывание справа
        int dlin = 0;
        byte txt_l = send_text[blok].leng;
        char* text = new char[txt_l];
        for (int i = 0; i < txt_l; i++) text[i] = send_text[blok].text[i];


        for (int i = 0; i < WIGHT * 32; i++) dataFromSend[i] = 0;

        for (int i = 0; i < send_text[blok].leng; i++) { // Замена переменных на их числовые значения
          if (send_text[blok].text[i] == '_') { // Обработчик служебных блоков
            if (send_text[blok].text[i + 1] == 'D' && send_text[blok].text[i + 2] == 'A' && send_text[blok].text[i + 3] == 'T' && send_text[blok].text[i + 4] == 'A') {
              i += myGetData(text, txt_l, i);
            }
            if (send_text[blok].text[i + 1] == 'T' && send_text[blok].text[i + 2] == 'I' && send_text[blok].text[i + 3] == 'M' && send_text[blok].text[i + 4] == 'E') {
              i += myGetTime(text, txt_l, i);
            }
            if (send_text[blok].text[i + 1] == 'T' && send_text[blok].text[i + 2] == 'E' && send_text[blok].text[i + 3] == 'M' && send_text[blok].text[i + 4] == 'P') {
              
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
        for (int i = 0; i < txt_l; i++) {
          for (int j = 0; j < alph_length; j++) { // Перебираем алфавит в поисках нужной буквы
            if (text[i] == alphaveet[j].bukva) {
              dlin += alphaveet[j].lenght;
              break;
            }
          }
        }
        delete[] text;

        for (int i = WIGHT * 32; i >= -dlin; i--) {
          anim_timer = millis() + send_text[blok].anim_time * 1000 / max(dlin, WIGHT * 32);
          while (millis() < anim_timer) {
            for (int i = 0; i < WIGHT * 32; i++) BufferData[i] = 0;
            setTextFrom(blok, i);
            setBufferWhithoutAnim();
            sendWithFonAnim(blok);
          }
        }
      } break;

    case 5: { // Проплывание слева
        int dlin = 0;
        byte txt_l = send_text[blok].leng;
        char* text = new char[txt_l];
        for (int i = 0; i < txt_l; i++) text[i] = send_text[blok].text[i];


        for (int i = 0; i < WIGHT * 32; i++) dataFromSend[i] = 0;

        for (int i = 0; i < send_text[blok].leng; i++) { // Замена переменных на их числовые значения
          if (send_text[blok].text[i] == '_') { // Обработчик служебных блоков
            if (send_text[blok].text[i + 1] == 'D' && send_text[blok].text[i + 2] == 'A' && send_text[blok].text[i + 3] == 'T' && send_text[blok].text[i + 4] == 'A') {
              i += myGetData(text, txt_l, i);
            }
            if (send_text[blok].text[i + 1] == 'T' && send_text[blok].text[i + 2] == 'I' && send_text[blok].text[i + 3] == 'M' && send_text[blok].text[i + 4] == 'E') {
              i += myGetTime(text, txt_l, i);
            }
            if (send_text[blok].text[i + 1] == 'T' && send_text[blok].text[i + 2] == 'E' && send_text[blok].text[i + 3] == 'M' && send_text[blok].text[i + 4] == 'P') {
              
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
        for (int i = 0; i < txt_l; i++) {
          for (int j = 0; j < alph_length; j++) { // Перебираем алфавит в поисках нужной буквы
            if (text[i] == alphaveet[j].bukva) {
              dlin += alphaveet[j].lenght;
              break;
            }
          }
        }
        delete[] text;

        for (int i = -dlin; i < WIGHT * 32 + 1; i++) {
          anim_timer = millis() + send_text[blok].anim_time * 1000 / max(dlin, WIGHT * 32);
          while (millis() < anim_timer) {
            for (int i = 0; i < WIGHT * 32; i++) BufferData[i] = 0;
            setTextFrom(blok, i);
            setBufferWhithoutAnim();
            sendWithFonAnim(blok);
          }
        }
      } break;
    case 6: { // Шторка справа
        for (int i = WIGHT * 32; i >= 0; i--) {
          anim_timer = millis() + send_text[blok].anim_time * 1000 / (WIGHT * 32);
          while (millis() < anim_timer) {
            for (int j = 0; j < WIGHT * 32; j++) {
              if (j >= i) BufferData[j] = dataFromSend[j];
              else BufferData[j] = 0;
            }
            if (dsdt.tick() && time_tochki) {
              on_off_tochki = !on_off_tochki;
              setText(blok);
            }
            sendWithFonAnim(blok);
          }
        }
      } break;

    case 7: { // Шторка слева
        for (int i = 0; i < WIGHT * 32; i++) {
          anim_timer = millis() + send_text[blok].anim_time * 1000 / (WIGHT * 32);
          if (i == 0) {
            timer = millis();
            while (millis()  <= send_text[blok].text_time * 1000 + timer) {
              if (dsdt.tick() && time_tochki) {
                on_off_tochki = !on_off_tochki;
                setText(blok);
              }
              sendWithFonAnim(blok);
            }
          }
          while (millis() < anim_timer) {
            for (int j = 0; j < WIGHT * 32; j++) {
              if (j < i) BufferData[j] = dataFromSend[j];
              else BufferData[j] = 0;
            }
            if (dsdt.tick() && time_tochki) {
              on_off_tochki = !on_off_tochki;
              setText(blok);
            }
            sendWithFonAnim(blok);
          }
        }
      } break;
    case 8: { // Падение
        for (int i = HEIGTH * 16; i >= 0; i--) {
          anim_timer = millis() + send_text[blok].anim_time * 1000 / (HEIGTH * 16);
          while (millis() < anim_timer) {
            for (int j = WIGHT * 32 - 1; j >= 0; j--) {
              BufferData[j] = dataFromSend[j] >> i;
            }
            if (dsdt.tick() && time_tochki) {
              on_off_tochki = !on_off_tochki;
              setText(blok);
            }
            sendWithFonAnim(blok);
          }
        }
      } break;
  }
}
