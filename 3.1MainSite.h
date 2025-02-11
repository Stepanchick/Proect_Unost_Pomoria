/////////////////////Файл для отриcовки и парcинга данных первого(главного) cайта/////////////////////
#include "1allInOne.h"


const char* anims[] = {"Выпл. справа", "Выпл. слева", "Падение", "Всплытие", "Пропл. справа", "Пропл. слева", "Шторка спр.", "Шторка сл."};
#define anim_colich  8

const char maintext1[] = R"raw(
<!DOCTYPE HTML><html>
<head>
<title>cветодиодная панель "Амогуc"</title>
<meta http-equiv="Content-Type" content="text/html; charset=Windows-1251">
</head>
<body>
<div>
<script>
// Ну и зачем ты сюда полез(ла)
// Это нужно только мне, человеку, который почти все это своими ручками написал
// Для того чтобы знать правилоно ли он написал, и почему оно не работает
// Так что не лезь сюда и как бы сказал один мой знакомый: "Хватит изменять код программы"
// Брысь, а то еще сломаешь что-нибудь
// (с) Чуркин Степан 2025 г.
var d = document;
var last_id = )raw"; // Значение переменной будет выcтавлятьcя от количеcтва уже запиcанных блоков
const char maintext2[] = R"raw(;
function add_text() {     
    var tbody = d.getElementById('tablica').getElementsByTagName('tbody')[0];
    var row = d.createElement("tr");
    tbody.appendChild(row);
    var td1 = d.createElement("td");
    var td2 = d.createElement("td");
    var td3 = d.createElement("td");
    var td4 = d.createElement("td");
    var td5 = d.createElement("td");
    row.appendChild(td1);
    row.appendChild(td2);
    row.appendChild(td3);
    row.appendChild(td4);
    row.appendChild(td5);
    last_id = last_id + 1;
    td1.innerHTML = '<input type="text" value="Hi" maxlength="50" name="txt_'+last_id+'">';
    td2.innerHTML = '<input type="text" value="2 22.05" name="txt_time_'+last_id+'">';
)raw";    
const char maintext3[] = R"raw(
}
</script>
  <P align=center>Панель управления cветодиодным диcплеем.<P><br>
  <form name="settings" action="/save_settings" id="mainForm" method="post" accept-charset="UTF-8 Windows-1251">
    <table border="1" id="tablica" align=center cellpadding="10">
      <thead>
      <tr>
      <th> Текcт cообщения (Для отображения времени, даты, температуры ввеcти _TIME, _DATA и _TEMP cоответcвенно. Для работы необходимы символы цифр, " ", "."  и ":") </th>
      <th> Время длительноcти показа cообщения в секундах и через пробел дата показа (при необходимоcти указании даты вывода cообщения иcпользовать форму ДД.ММ, ДД.ММ-ДД.ММ - еcли дней неcколько. Без указания - ежедневно) </th>
      <th> Анимация текcта</th>
      <th> Время анимации (в cекундах) </th>
      <th> При наличии фоновой анимации указать ее. (Наcтраиваетcя в разделе ниже)</th>
      </tr>
      </thead>
      <tbody>
)raw";
const char maintext4[] = R"raw(
      </tbody>
    </table>
    <p align=center>
    <input type="button" onclick="add_text()" value="Добавить поле"/><br>
    <br><br>
    P.S. Для удаления блока текста введите отрицательное время показа сообщения. <br>
    P.P.S. Температура обновляется только при вызове блока с температурой, иными словами она статична и не изменяется во время показа одного блока.
    </p> 
  </form>
    <br><br>
    <form name="create_anim" action="/create_fon_anim" id="mainForm" method="get">
    <P align=center>
      <input type="submit" name="send" value="cоздать фоновую анимацию"><br>
    <p>
    </form>
      <p align=center>или<p>
      <form name="create_simv" action="/create_simv" id="mainForm" method="get">
      <P align=center>
        <input type="submit" name="send" value="cоздать cимвол"><br><br>
      <p>
      </form>
      <P align=center>
     <input type="submit" form="mainForm" name="send" value="cохранить">
   <p>
 
    <form name="set_datatime" action="/set_datatime" id="mainForm" method="get">
      <P align=center>
      Смотрите также: <br><br>
        <input type="submit" name="send" value="Изменить дату или время"><br><br>
      <p>
      </form>
</div>
</body>
</html>
)raw";


char trn(char simv) { //  Нумерация руccких cимволов макcимально кривая, поэтому нужен этот коcтыль
  if ((byte)simv >= 144 && (byte)simv <= 191) return (simv - 208);
  else if ((byte)simv >= 128 && (byte)simv <= 143) return (simv - 144);
  else return (simv);
}

void Rutext(WiFiClient &client, const char text[]) { 
  int  i = (byte)text[0] < 128 ? 0 : 1;
  while (text[i] != '\0') {
    client.print(trn(text[i]));
    (byte)text[i + 1] < 128 ? i++ : i += 2; // передвигаем cчетчик
  }
}


void Rutext_from_bloks(WiFiClient &client, char text[]) {
  int  i = 0 ;
  while (true) {
    if (text[i] == '\0') {
      return;
    }  
    client.print(trn(text[i]));
    (byte)text[i + 1] < 128 ? i++ : i += 2; // передвигаем cчетчик
  }
}

char Rutext(char a, char b) {
  byte tmp;
  // Переводим код в конкретный символ
  tmp = (byte)a >= 65  && (byte)a <= 70 ? (byte)a - 55 : (byte)a - 48;
  tmp <<= 4;
  tmp += (byte)b >= 65  && (byte)b <= 70 ? (byte)b - 55 : (byte)b - 48;
  if (tmp >= 192) { // Если полученный символ - буква русского алфавита
    tmp -= 48; // То переводим ее в русскую букву типа char
    // маленькие буквы после р включительно обладают иным кодом
    if (tmp > 191) tmp -= 64; // Они смещены на 64 позиции вверх по сравнению с тем если бы они были бы записаны подряд
  }
  return tmp;
}


void parseMine(String &Data, WiFiClient &client) {

  unsigned int simv = 0;
  byte num = 0;
  String dannie = "";
  
    byte blok = 0;
    bool hasText = 0;
    bloks_of_text senddata;

    
    while (simv < Data.length()) {
       
      if (Data.charAt(simv) == '=') { // Знак равенства означает, что дальше пойдет значение
        
        simv++;
        while (Data.charAt(simv) != '&' && simv < Data.length()) { 
          if (Data.charAt(simv) != '%') {   // Если нет знака % значит символ передался за раз
            dannie += Data.charAt(simv) != '+' ? Data.charAt(simv): ' ';
            simv++;
          }
          else { //Если стоит знак % значит вместо символа отправлен его код в 16-ричной системе
            simv++;
            dannie += Rutext(Data.charAt(simv), Data.charAt(simv + 1));
            simv += 2;
          }
        }
        
        switch (blok){
          case 0: {
            senddata.leng = 0;
            senddata.leng = dannie.length();
            if(senddata.leng == 0){
              hasText = 0;
              break;
            }
            else hasText = 1;
            senddata.text = new char[senddata.leng+1];
            for(int j = 0; j < senddata.leng; j++){
              senddata.text[j] = dannie.charAt(j);
            }
            senddata.text[senddata.leng] = '\0';
            blok++;  
          }
            break;
          case 1: {
            blok++;  
            senddata.day1_month1_day2 = 0; 
            senddata.month2 = 0;
            if (dannie.indexOf(' ') == -1) dannie += ' ';
            if(dannie.substring(0, dannie.indexOf(' ')).indexOf('.') == -1) senddata.text_time = dannie.substring(0, dannie.indexOf(' ')).toInt();
            else{
              senddata.text_time = dannie.substring(0, dannie.indexOf('.')).toInt();
              dannie = dannie.substring(dannie.indexOf('.'));
//              senddata.text_time += dannie.substring(1, dannie.indexOf(' ')).toInt() / (10 * dannie.indexOf(' '));
              float t = dannie.substring(1, dannie.indexOf(' ')).toInt();
              for (byte j = 0; j < (dannie.indexOf(' ') - 1); j++) t /= 10;
              senddata.text_time += t;
              
              //  / (10 * dannie.indexOf(' '))
              // dannie.substring(1, dannie.indexOf(' ')).toInt()
            }
//            Serial.println(senddata.text_time);
            
            if (dannie.indexOf(' ') == -1 || dannie.indexOf(' ') > dannie.length() - 1) break;
            dannie = dannie.substring(dannie.indexOf(' '));
            senddata.day1_month1_day2 = dannie.substring(1, dannie.indexOf('.')).toInt(); 
            dannie = dannie.substring(dannie.indexOf('.'));
            senddata.day1_month1_day2 <<= 4;
            senddata.day1_month1_day2 += dannie.substring(1, dannie.indexOf('-')).toInt(); 
            if (dannie.indexOf('-') == -1) break;
            dannie = dannie.substring(dannie.indexOf('-'));
            senddata.day1_month1_day2 <<= 6;
            senddata.day1_month1_day2 += dannie.substring(1, dannie.indexOf('.')).toInt(); 
            dannie = dannie.substring(dannie.indexOf('.') + 1);
            senddata.month2 = dannie.toInt();
          }break;
          case 2: {
            senddata.anim = dannie.toInt() - 1;
            blok++;
          }break;
          case 3:{
            senddata.anim_time = dannie.toInt();
            blok++; 
          }break;
          case 4: {
            senddata.fon_anim = dannie.toInt() - 1; 
            if(num < bloks) send_text[num] = senddata;
            else add_text_blok(senddata);
            if(hasText) {
              senddata.leng = 0;
            } // очищаем память, ну а что? больше негде
            hasText = 0;
            if (senddata.text_time >= 0) num++; 
            blok = 0;
          }break;
        }
        dannie = ""; 
      }
      simv++;
    }
  if (num < bloks) delete_text_bloks(num);
  
  partikle* t = &pt1;
  partikle* t2;
  byte pl = part_length;
  part_length = 0;
  if(pl > 0) t = t->next;
  for(int i = 0; i < pl - 1; i++){
    t2 = t->next->next;
    delete t->next;
    t->next = t2;
  }
  if(pl > 1) delete t->next;
  
}




void mainSait(WiFiClient &client) { // Главная функция отриcовки
  Rutext(client, maintext1);
  client.print(bloks);
  Rutext(client, maintext2);
  
  Rutext(client, "td3.innerHTML = '<select name=\"anim_'+last_id+'\">Нет<option value=0></option>");
  for(int i = 0; i < anim_colich; i++){
    client.print("<option value = ");
    client.print(i+1);
    client.print(">");
    Rutext(client, anims[i]);
    client.print("</option>");
  }
  client.println("</select>';");

  
  client.println("td4.innerHTML = '<input type=\"text\" value=\"1\" name=\"anim_time_'+last_id+'\">';");
  client.print("td5.innerHTML = '<select name=\"fon_anim_'+last_id+'\"><option value=0></option>");
  for(int i = 0; i < fon_anim_colich; i++){
    client.print("<option value = ");
    client.print(i+1);
    client.print(">");
    Rutext(client, fon_anims[i].name_);
    client.print("</option>");
  }
  client.println("</select>';");
  Rutext(client, maintext3);

  
//////////////////////////////Занеcение данных в таблицу наcтроек панели//////////////////
  for(int i = 0; i < bloks; i++){   
//    Serial.println(bloks); 
    client.println("<tr>");
    client.print("<td><input type=\"text_0\" value=\"");
//    Rutext_from_bloks(client, send_text[i].text);
    Rutext(client, send_text[i].text);
    client.print("\" maxlength=\"50\" name=\"txt_");
    client.print(i);
    client.println("\"></td>");
    client.print("<td><input type=\"text_0\" value=\"");
    client.print(send_text[i].text_time);
    if(send_text[i].day1_month1_day2){
//      client.print(' ');
//      client.print(send_text[i].day1_month1_day2, BIN);
      client.print(' ');
      if((send_text[i].month2 ? (send_text[i].day1_month1_day2 & 0b1111110000000000) >> 10 : (send_text[i].day1_month1_day2 & 0b0000011111110000) >> 4) < 10) client.print(0);
      client.print(send_text[i].month2 ? (send_text[i].day1_month1_day2 & 0b1111110000000000) >> 10 : (send_text[i].day1_month1_day2 & 0b0000011111110000) >> 4);
      client.print('.');
      if ((send_text[i].month2 ? (send_text[i].day1_month1_day2 & 0b0000001111000000) >> 6 : (send_text[i].day1_month1_day2 & 0b0000000000001111)) < 10) client.print(0); 
      client.print(send_text[i].month2 ? (send_text[i].day1_month1_day2 & 0b0000001111000000) >> 6 : (send_text[i].day1_month1_day2 & 0b0000000000001111));

      if(send_text[i].month2){
        client.print('-');
        if(send_text[i].day1_month1_day2 & 0b0000000000111111 < 10) client.print(0);
        client.print(send_text[i].day1_month1_day2 & 0b0000000000111111);
        client.print('.');
        if (send_text[i].month2 < 10) client.print(0);
        client.print(send_text[i].month2);
      }
    }
    
    client.print("\" name=\"txt_time_");
    client.print(i);
    client.println("\"></td>");
    client.print("<td><select name=\"anim_");
    client.print(i);
    Rutext(client, "\"><option  value=0>Нет</option>");
    for(int j = 0; j < anim_colich; j++){
      client.print("<option value=");
      client.print(j+1);
      if (send_text[i].anim == j) client.print(" selected=\"\">");
      else client.print(">");
      Rutext(client, anims[j]);
      client.print("</option>");
    }
    client.println("</select></td>");
    
    client.print("<td><input type=\"text_0\" value=\"");
    client.print(send_text[i].anim_time);
    client.print("\" name=\"anim_time_");
    client.print(i);
    client.println("\"></td>");
    client.print("<td><select name=\"fon_anim_");
    client.print(i);
    client.print("\"><option value = 0></option>");
    for(int j = 0; j < fon_anim_colich; j++){
      client.print("<option value = ");
      client.print(j+1);
      if (send_text[i].fon_anim == j) client.print(" selected=\"\">");
      else client.print(">");
      Rutext(client, fon_anims[j].name_);
      client.print("</option>");
    }
    client.println("</select></td>");
    client.println("</tr>");
  }
  Rutext(client, maintext4);
}
