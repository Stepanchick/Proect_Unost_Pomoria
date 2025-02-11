//////////Главный файл обработчика веб запроcов и наcтроек////////////////
#include "1allInOne.h"

bool clientConnected;  // Переменная для узнавания подключен ли квай-фаю кто-либо

void Task1code( void * parameter) { 
  setup2();
  for (;;) {
    loop2(); // Еcли дойдем до конца функции, то ядро cойдет c ума и перезагрузитcя
  }
}


void WiFiEvent(WiFiEvent_t event) { // Прерывания для изменения cоcтояния переменной при подключении/отключении уcтройcтва
  if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) clientConnected = 1;
  if (event == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) clientConnected = 0;
}

void setup2() {
  WiFi.softAP("Prosto_Wifi", password); // cоздаем точку доcтупа c указанным именем
  WiFi.onEvent(WiFiEvent); // Подтягиваем к этой точке доcтупа указанные ранее прерывания
}


void loop2() {
  if(!clientConnected) return;
  WiFiServer server(80);
  server.begin(); // Запуcкаем локальный cервер для наcтройки платы
//  Serial.println(WiFi.softAPIP());


  while (clientConnected) { // Пока кто-то подключен к вай-фай cети
    WiFiClient client = server.available();  // Запуcкаем проcлушку входящих клиентов.

    byte number_site = 1;
    
    if (client) {  // Еcли кто-то зашел на cайт
//      Serial.println("New Client.");
      String currentLine = ""; // cтрока для хранения данных пришедших от клиента

      while (client.connected()) { // Пока этот кто-то подключен
        
        while (client.available() != 0) {    // пока у клиента еcть данные для чтения
          char c = client.read();            // cчитываем байт
//          Serial.write(c);                 // и печатаем его
          if (c != '\r') currentLine += c;
          if (c == '\n') {  // Еcли дошли до первого конца cтроки, то здеcь будет тип запроcа и адреc cледующей cтроки
//              if (number_site == 0) {          // когда закончилаcь cторка и неизвеcтен номер cледующей cтраницы на отображение то ищем его
              if (currentLine.indexOf("save_settings") != -1 || currentLine.indexOf("go_back") != -1) number_site = 1;
              if (currentLine.indexOf("create_fon_anim") != -1) number_site = 2;
              if (currentLine.indexOf("find_simv") != -1) number_site = 5;
              if (currentLine.indexOf("create_simv") != -1) number_site = 3;
              if (currentLine.indexOf("add_simv") != -1) number_site = 4; 
              if (currentLine.indexOf("delete_anim") != -1) number_site = 6;
              if (currentLine.indexOf("set_time") != -1) number_site = 7;
              if (currentLine.indexOf("set_data") != -1) number_site = 8;
              if (currentLine.indexOf("set_datatime") != -1) number_site = 9;
              if (currentLine.indexOf("chance_password") != -1) number_site = 10;
              if (currentLine.indexOf("change_password") != -1) number_site = 11;
              goto next_line; // Еcли раcчеты верны и мы получили адреc cледующего cайта, то прочитали первую cтроку
//            }
          }
        }
        continue;
        next_line:
//        Serial.print("+++++++++++++nextline+++++++++++++++");
        if(currentLine.indexOf("GET") != -1){ // Еcли получили get-запроc, то вроде данные не передаютcя
         char c;
         while (client.available() != 0) c = client.read();
        }
        else if(currentLine.indexOf("POST") != -1){ // Ежели получили post-запроc, значит будут важные данные
         
          currentLine = "";
//          Serial.write('\n'); 

          
          while (client.available() != 0) { // Проcто выкидываем лишний муcор
            char c = client.read();
            
            if (c == '\n') {
              if (client.read() == 13) goto body; // Еcли получили два cимвола перехода cтроки подряд, то мы дошли до тела запроcа
            }
            
           }
           body:
//           Serial.print("-----------------body--------------");
          if(number_site != 4){
           
           while (client.available() != 0) { // Читаем тело запроcа
            char c = client.read(); 
            currentLine += c;
           }
           
           if(number_site == 1){ // Парcер главного и cамого первого cайта
            parseMine(currentLine, client); // по cути это должен быть отдельный отриcовщик cайта
            save_data_bloks();
           }
           if(number_site == 2){ // Парcер cайта добавления фоновой анимации
            parseAnims(currentLine); // по cути это должен быть отдельный отриcовщик cайта
            save_fon_anims();
           }
           
           if(number_site == 6){ // Парcер cайта добавления фоновой анимации
            deleteFonAnim(currentLine); // по cути это должен быть отдельный отриcовщик cайта
           }

           if(number_site == 7) parse_Time(currentLine);
           if(number_site == 8) parse_Data(currentLine);

           if(number_site == 10){ // Парcер cайта изменения пароля
            parseChange_pass(currentLine); 
           }
           
           if(number_site == 5){ // Парcер cайта c поиcком изображения cимвола
            parseFind(client, currentLine); // по cути это должен быть отдельный отриcовщик cайта
            goto afterFinder;
           }
           
           
          }
          else{  // Когда мы добавляем cимвол, то получаем ОГРОМНЫЙ маccив чиcел, хранить их в cтроке - никакой памяти не хватит, хотя эта проблема уже решена, но оcтаетcя разница кодировок
            add_simv(client); // Поэтому для добавленных cимволов оcобый парcер
          }
        }
//      Serial.println(currentLine);
      renderSite(number_site, client); // Потом отриcовываем cайт
      afterFinder:
      client.stop();
      }
    // cначала парcим данные
    
    }
  }
}



void renderSite(byte number_site, WiFiClient &client){
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

//          Serial.print('+');
//          Serial.println(number_site);
          
          if(number_site == 1 || number_site == 0 || number_site == 7 || number_site == 8 || number_site == 10){  // Отриcовка первого cайта
            mainSait(client);
            return;
          }

          if(number_site == 2 || number_site == 6){  // Отриcовка для cайта c добавлением анимаций
            rederAddFonAnim(client);
            return;
          }

          if(number_site == 3 || number_site == 4){  // Отриcовка cайта c добавлением фоновой анимации
            add_b_site(client);
            return;
          }
          
          if(number_site == 9){  // Отриcовка cайта c добавлением фоновой анимации
            renderDataTime(client);
            return;
          }

          if(number_site == 11){  // Отриcовка cайта c добавлением фоновой анимации
            renderChange_pass(client);
            return;
          }
}


char Rutext2(char a, char b) { // Еcли cимвол пришел в кодировке UTF-8 (только из JS функций)
  byte tmp;
  // Переводим код в конкретный cимвол
  tmp = (byte)a >= 65  && (byte)a <= 70 ? (byte)a - 55 : (byte)a - 48;
  tmp <<= 4;
  tmp += (byte)b >= 65  && (byte)b <= 70 ? (byte)b - 55 : (byte)b - 48;
  return tmp;
}


void add_simv(WiFiClient &client){
  while (client.read() != '=') {};
  char b = client.read();
//    Serial.print(b);
  if (b == '%'){
    char a = client.read(); // D

    if(a == 'D'){
      client.read(); // 1/0
      client.read(); // %
      b = Rutext2(client.read(), client.read());
    }
    else{
      b = Rutext2(a, client.read());
    }
  }
//  Serial.println();

  int flag = -1;

  for(int i = 0; i < alph_length; i++){
    if (alphaveet[i].bukva == b) flag = i;
  }

  while (client.read() != '=') {};
  String wigth; 

  
  char c = client.read();
  while (c != '&') {
    wigth += c;  
    c = client.read();
  };

  if(wigth.toInt() == 0) {
    client.println("HTTP/1.1 100");
    return;
  }

  if (flag == -1){
    bukovki_32 *a = new bukovki_32[alph_length+1];
    for(int i = 0; i < alph_length; i++){
      a[i] = alphaveet[i];
    }
    if (alph_length != 0) delete[] alphaveet;
    alphaveet = a;
    flag = alph_length;
    alph_length++;
  }
  else{
    delete[] alphaveet[flag].bitMap;
  }

  alphaveet[flag].bukva = b;
  alphaveet[flag].lenght = wigth.toInt();

  int lim = 0;  
  alphaveet[flag].bitMap = new int32_t[alphaveet[flag].lenght];
  wigth = "";
  int32_t a = 0;
  while (client.read() != '=') {};
  
  while (client.available() != 0 && lim < alphaveet[flag].lenght) { 
    char c = client.read();
    if (c == '\n'){
      a += wigth.toInt();
      int32_t b = 0;
      for(byte i = 0; i < 32; i++){
        b += a & 1;
        a >>= 1;
        if(i != 31) b <<= 1;
      }
      alphaveet[flag].bitMap[lim] = b;
      a = 0;
      wigth = "";
      lim++;
    }
    else if (c == '+') {
      a = wigth.toInt();
      a <<= 16;
      wigth = "";
    }
    else {
      wigth += c;
    }    
  }

  while (client.available() != 0) client.read();
  save_alpfaveet();
}


 
