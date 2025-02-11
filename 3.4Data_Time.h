//////////Файл для настройки времени на панеле////////////////
#include "1allInOne.h"

const char DataTime1[] = R"raw(
<!DOCTYPE html>
<html>

<head>
    <Title>Смена даты и времени</Title>
</head>
<body>

    <form name="back" action="/set_time" method="post">
        <P align=center>
            Изменить дату и время.<br><br>
            Часы:
            <input tupe="text" id="hour" name="simv" value="
)raw";

const char DataTime2[] = R"raw(
" maxlength="2">
            Минуты:
            <input tupe="text" id="minutes" name="simv" value="
)raw";

const char DataTime3[] = R"raw(
" maxlength="2">
            <br><br>
            <input type="submit" name="send" value="Установить время"><br>
        </P>
    </form>

    <form name="back" action="/set_data" method="post">
        <P align=center>
            День:
            <input tupe="text" id="hour" name="day" value="
)raw";

const char DataTime4[] = R"raw(
" maxlength="2">
            Месяц:
            <input tupe="text" id="minutes" name="month" value="
)raw";
const char DataTime5[] = R"raw(
" maxlength="2">
            Год:
            <input tupe="text" id="minutes" name="year" value="
)raw";

const char DataTime6[] = R"raw(
" maxlength="4">
            <br><br>
            <input type="submit" name="send" value="Установить дату"><br>
        </P>
    </form>


    <form name="back" action="/go_back" method="get">
        <P align=center>
            <input type="submit" name="send" value="Назад"><br>
        </P>
    </form>

</body>
</html>
)raw";

void renderDataTime(WiFiClient &client) {
  Datime dt2(dsdt);
  Rutext(client, DataTime1);
  client.print(dt2.hour);
  Rutext(client, DataTime2);
  client.print(dt2.minute);
  Rutext(client, DataTime3);
  client.print(dt2.day);
  Rutext(client, DataTime4);
  client.print(dt2.month);
  Rutext(client, DataTime5);
  client.print(dt2.year);
  Rutext(client, DataTime6);
}

void parse_Data(String &Data){
  unsigned int simv = 0;
  byte num = 0;
  String dannie = "";

  byte blok = 0;

  Datime dt2(dsdt);

  while (simv < Data.length()) {
      if (Data.charAt(simv) == '=') { // Знак равенства означает, что дальше пойдет значение
        simv++;
        dannie = "";
        while (Data.charAt(simv) != '&' && simv < Data.length()) { 
            dannie += Data.charAt(simv);
            simv++;
          
     }
     switch (blok){
        case 0: {
          dt2.day = dannie.toInt();
          blok++; 
        } 
          break;
        case 1: {
          dt2.month = dannie.toInt();
          blok++; 
        } 
          break;
        case 2: {
          dt2.year = dannie.toInt();
          blok++;
        }break;
     }
    }
    simv++;
  }
  dsdt.setTime(dt2);
}

void parse_Time(String &Data){

  unsigned int simv = 0;
  byte num = 0;
  String dannie = "";

  byte blok = 0;

  Datime dt2(dsdt);

  while (simv < Data.length()) {
      if (Data.charAt(simv) == '=') { // Знак равенства означает, что дальше пойдет значение
        dannie = "";
        simv++;
        while (Data.charAt(simv) != '&' && simv < Data.length()) { 
            dannie += Data.charAt(simv);
            simv++;
          
     }
     switch (blok){
        case 0: {
          dt2.hour = dannie.toInt();
          blok++; 
        } 
          break;
        case 1: {
          dt2.minute = dannie.toInt();
          blok++; 
        } 
          break;
     }
    }
    simv++;
  }
  dsdt.setTime(dt2);
}


//////////////////////////////Сайт для смены пароля//////////////////////////////////

const char Change_password[] = R"raw(
<!DOCTYPE html>
<html>
<head>
    <Title>Смена даты и времени</Title>
</head>
<body>

    <form name="back" action="/chance_password" method="post">
        <P align=center>
            Изменить пароль.<br><br>
            Старый пароль:
            <input tupe="text" id="hour" name="simv"><br><br>
            Новый пароль(не менее 8 символов):
            <input tupe="text" id="minutes" name="simv">
            <br><br>
            <input type="submit" name="send" value="Изменить пароль"><br>
        </P>
    </form>

    <form name="back" action="/go_back" method="get">
        <P align=center>
            <input type="submit" name="send" value="Назад"><br>
        </P>
    </form>
</body>
</html>
)raw";


void renderChange_pass(WiFiClient &client) {
  Rutext(client, Change_password);
}


void parseChange_pass(String &Data){
  unsigned int simv = 0;
  byte num = 0;
  String dannie = "";

  byte blok = 0;

  while (simv < Data.length()) {
      if (Data.charAt(simv) == '=') { // Знак равенства означает, что дальше пойдет значение
        dannie = "";
        simv++;
        while (Data.charAt(simv) != '&' && simv < Data.length()) { 
            dannie += Data.charAt(simv);
            simv++;
          
     }
     switch (blok){
        case 0: {
          byte pass_l = 0;
          
          for(; password[pass_l] != '\0'; pass_l++);

          
          if(pass_l != dannie.length()) return;

          
          for(int i = 0; i < pass_l; i++){
            if(password[i] != dannie.charAt(i)) return;
          }
          
          blok++; 
        } 
          break;
        case 1: {
          
//          Serial.println("---------------------");
          
          if(dannie.length() < 8) return;

          if(password) delete[] password;
          password = new char[dannie.length()+1];
          
          byte pass_l = dannie.length()+1;
          for(int i = 0; i < pass_l; i++){
            password[i] = dannie.charAt(i);
          }
          password[dannie.length()] = '\0';
          
          blok++; 
        } 
          break;
     }
    }
    simv++;
  }
  save_data_bloks();
  ESP.restart();
}
