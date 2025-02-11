#include "1allInOne.h" 

const char addFanim1[] = R"raw(
<!DOCTYPE html>
<html>

<head>
  <Title>Добавление фоновой анимации</Title>
  <meta http-equiv="Content-Type" content="text/html; charset=Windows-1251">
</head>
<body>
  <br><br>
  <form method="post" accept-charset="UTF-8 Windows-1251">
    <p align=center>
      <label>
        Введите название новой фоновой анимации:
        <input type="text" value="#ЗНАЧ!" maxlength=\"50\" name="name">
      </label><br><br>

      <label>
        Введите символ, который будет болтаться на фоне:
        <input type="text" value="#" name="simv" maxlength="1">
      </label><br><br>

      <label>
        Размеры матрицы 32 * 256. Начало координат - левый нижний угол.<br>
        Введите вектор движения(скорости) частицы(символа).<br>
        Если не указаны точные координаты то будут выбраны случайные:<br>
        x:
        <input type="text" value="0" name="x1">
        y:
        <input type="text" value="0" name="y1">
      </label><br><br>
      <label>
        Время перемещения частицы:
        <input type="text" value="10" name="time">
      </label><br><br>
      <label>
        Введите возможные колебания символа в формате "x1; x2" и "y1; y2" (Возможны отрицательные значения).<br>
        Колебания - это случайные отклонения от заданного движения.<br>
        Выбирается случайное число из указанного промежутка и вектор скорости частицы сдвигается влево(-) или вправо(+) на указанное число.(пустое поле = 0;0)<br>

        По x:
        <input type="text" value="0" name="x3">
        По y:
        <input type="text" value="0" name="y3">
      </label><br><br>
      <label>
        Инверсия пикселей: <input type="checkbox" name="invers"><br>
        Если симол пересекается с текстом или с другой частицей то пиксель пересечения тухнет.<br><br>
        
      </label>
      <label>
        Координаты начала движения частиц(Число или промежуток в формате "a1; a2"):<br><br>
        x:
        <input type="text" value="0" name="x4">
        y:
        <input type="text" value="0" name="y4">
      </label><br><br>
      <label>
        Время между соседними частицами(одномоментно могут существовать 100 частиц) - число или интервал(t1; t2) допустимы дробные значения:
        <input type="text" value="1" name="p1">
      </label><br><br>
      <button type="submit">Сохранить</button>
    </p>
  </form>
  
  <form name="back" action="/go_back" method="get">
    <P align=center>
      <input type="submit" name="send" value="Назад"><br>
    </P>
  </form><br><br>
  <P align=center>
  ----------------------------ИЛИ----------------------------<br><br>
  </P>
  <form name="delete" action="/delete_anim" method="post">
    <P align=center>
     <select name="del_anims" id="anims_select">
     <option value="-1">Выберите анимацию.</option>
)raw";
const char addFanim2[] = R"raw(
     </select><br><br>
      <input type="submit" name="send" value="Удалить"><br>
    </P>
  </form>
  
</body>
</html>
)raw";



void rederAddFonAnim(WiFiClient &client) {
  Rutext(client, addFanim1);
  for(int i = 0; i < fon_anim_colich; i++){
    client.print("<option value=\"");
     client.print(i);
     client.print("\">");
    Rutext(client, fon_anims[i].name_);

//  int j = 0;
//    while (true) {
//      if(fon_anims[i].name_[j] == '\0') return;
//      client.println(fon_anims[i].name_[j]);
//    }
    
    
    client.print("</option>");
  }
  Rutext(client, addFanim2);
}

void deleteFonAnim(String& Data){
  int simv = 0;
  while(simv < Data.length()){
    if (Data.charAt(simv) == '='){
      simv++;
      simv = int(Data.charAt(simv)) - 48;
      if (simv < 0) return;
      else del_anims(simv);
      return;
    }
    simv++;
  }

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


void parseAnims(String& Data){
//    Serial.println(Data);

    unsigned int simv = 0;
    byte num = 0;
    String dannie = "";
  
    byte blok = 0;
    fon_animations anims;

    
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
            char c = Rutext(Data.charAt(simv), Data.charAt(simv + 1));
            if(c == 208 || c == 209) {
              simv += 3;
              dannie += Rutext(Data.charAt(simv), Data.charAt(simv + 1));
            }
            else{
              dannie += Rutext(Data.charAt(simv), Data.charAt(simv + 1));
            }
            simv += 2;
          }
          
        }
        
        
        switch (blok){
          case 0: {
            anims.name_ = new char[dannie.length() + 1];
            for(int i = 0; i < dannie.length(); i++) anims.name_[i] = dannie.charAt(i); 
            anims.name_[dannie.length()] = '\0';
            blok++; 
          } 
            break;
          case 1: {
            anims.simv = dannie.charAt(0);
            blok++; 
          } 
            break;
          case 2: {
            anims.x_vect = dannie.toInt();
            blok++;
          }break;
          case 3:{
            anims.y_vect = dannie.toInt();
            blok++; 
          }break;
          case 4: { 
            anims.time_an = dannie.toInt();
            blok++;
          }break;
          case 5: {
            if (dannie.indexOf(';') == -1){
              anims.x_col[0] = dannie.toInt();
              anims.x_col[1] = dannie.toInt();
            }
            else {
              anims.x_col[0] = dannie.substring(0, dannie.indexOf(';')).toInt();
              dannie = dannie.substring(dannie.indexOf(';') + 1);
              anims.x_col[1] = dannie.toInt();
            }            
            blok++;
          }break;
          case 6: {
            if (dannie.indexOf(';') == -1){
              anims.y_col[0] = dannie.toInt();
              anims.y_col[1] = dannie.toInt();
            }
            else {
              anims.y_col[0] = dannie.substring(0, dannie.indexOf(';')).toInt();
              dannie = dannie.substring(dannie.indexOf(';') + 1);
              anims.y_col[1] = dannie.toInt();
            } 
            blok++;
          }break;
          case 7: {
            if(dannie.indexOf("on") != -1) {
              anims.invers = 1;
              break;
            }
            if (dannie.indexOf(';') == -1){
              anims.x_nach[0] = dannie.toInt();
              anims.x_nach[1] = dannie.toInt();
            }
            else {
              anims.x_nach[0] = dannie.substring(0, dannie.indexOf(';')).toInt();
              dannie = dannie.substring(dannie.indexOf(';') + 1);
              anims.x_nach[1] = dannie.toInt();
            } 
            blok++;
          }break;
          case 8: {
            if (dannie.indexOf(';') == -1){
              anims.y_nach[0] = dannie.toInt();
              anims.y_nach[1] = dannie.toInt();
            }
            else {
              anims.y_nach[0] = dannie.substring(0, dannie.indexOf(';')).toInt();
              dannie = dannie.substring(dannie.indexOf(';') + 1);
              anims.y_nach[1] = dannie.toInt();
            } 
            blok++;
          }break;
          
          case 9: {
            dannie += ' ';
            if(dannie.indexOf(';') == -1){
              if(dannie.indexOf('.') == -1){
                anims.plotn[0] = dannie.toInt();
                anims.plotn[1] = anims.plotn[0];
              }
              else{
                anims.plotn[0] = dannie.substring(0, dannie.indexOf('.')).toInt();
                dannie = dannie.substring(dannie.indexOf('.'));
                float t = dannie.substring(1, dannie.indexOf(' ')).toInt();
                for (byte j = 0; j < (dannie.indexOf(' ') - 1); j++) t /= 10;
                anims.plotn[0] += t;
                anims.plotn[1] = anims.plotn[0];
              }
            }
            
            else{
              if(dannie.indexOf('.') == -1){
                anims.plotn[0] = dannie.substring(0, dannie.indexOf(';')).toInt();
              }
              else{
                anims.plotn[0] = dannie.substring(0, dannie.indexOf('.')).toInt();
                dannie = dannie.substring(dannie.indexOf('.'));
                float t = dannie.substring(1, dannie.indexOf(';')).toInt();
                for (byte j = 0; j < (dannie.indexOf(';') - 1); j++) t /= 10;
                anims.plotn[0] += t;
              }

              dannie = dannie.substring(dannie.indexOf(';')+1);

              if(dannie.indexOf('.') == -1){
                anims.plotn[1] = dannie.toInt();
              }
              else{
                anims.plotn[1] = dannie.substring(0, dannie.indexOf('.')).toInt();
                dannie = dannie.substring(dannie.indexOf('.'));
                float t = dannie.substring(1, dannie.indexOf(' ')).toInt();
                for (byte j = 0; j < (dannie.indexOf(' ') - 1); j++) t /= 10;
                anims.plotn[1] += t;
              }
            }



            blok = 0;
            fon_animations* f = new fon_animations[fon_anim_colich+1];
            for(int i = 0; i < fon_anim_colich; i++) f[i] = fon_anims[i];

            
            partikle* t = &pt1;
            partikle* t2;
            byte pl = part_length;
            part_length = 0;

            if(fon_anim_colich) delete[] fon_anims;
            
            if(pl > 0) t = t->next;
            for(int i = 0; i < pl - 2; i++){
              t2 = t->next->next;
              delete t->next;
              t->next = t2;
            }
            if(pl > 1) delete t->next;
            
  
            
            fon_anims = f;
            fon_anims[fon_anim_colich] = anims;
            fon_anim_colich++;
            anims.invers = 0;
          }break;
        }
        dannie = ""; 
      }
      simv++;
    }
}

/*
 * class fon_animations {
  public:
    char* name_;
    char simv;
    int x_veck;
    int y_vect;
    int time_an;
    
    int x_col[2];
    int y_col[2];
    bool invers;
    byte notprozr;

    int x_nach;
    int y_nach;
    int plotn;
    
    ~fon_animations() {
      delete[] name_;
    }
};
 */
