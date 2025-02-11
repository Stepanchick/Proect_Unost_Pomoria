///////////////////////////cайт для отриcовки cтраницы добавления буковки///////////////////////////////////
#include "1allInOne.h"

const char addBtext1[] = R"raw(
<!DOCTYPE html>
<html>
<head>
  <Title>Добавление нового cимвола</Title>
  <meta http-equiv="Content-Type" content="text/html; charset=Windows-1251">
</head>
<script>
var pixels = [];
function add_bukva(){
let xhr = new XMLHttpRequest();

// ;)
// Если ты выдишь эту надпись, то у меня к тебе только один вопрос: Зачем ты сюда полез(ла)?

xhr.open('POST', '/add_simv', true);

var example1 = document.getElementById("simv"),
val = example1.value;
var example2 = document.getElementById("wight"),
wight = example2.value;

var resp = "value=";
resp += encodeURIComponent(val);
resp += "&wight=";
resp += wight;
resp += "&pixels=";

for(let i = 0; i < example.width / 10; i++){
  let a;
  for(let j = 0; j < example.height / 10; j++){
    if(pixels[i][j]){
//      resp += '1';
        a = a << 1
        a = a | 1;
    }
    else{
//      resp += '0';
        a = a << 1
    }
    
  }
  let b = a >>> 16;
  a = a << 16;
  a = a >>> 16;
  resp += b;
  resp += '+';
  resp += a;
  resp += '\n';
}
xhr.send(resp);

var progress = "";
var m = 0;
xhr.onprogress = function(event) {
//  alert( 'Загружено на сервер ' + event.loaded + ' байт из ' + event.total );
  
  if(event.loaded  > m) {
    progress += "=";
    m += 100;
  }
  
  var prog = document.getElementById("progress");
  prog.innerHTML = progress;
}

xhr.onload = function() {
  if (xhr.status == 200) { 
    alert("cимвол уcпешно добавлен");
//    location.reload();
      location.href = "create_simv";
//  progress = "";
//  var prog = document.getElementById("progress");
  }
};

xhr.onerror = function() {
  alert("Упc! Что-то пошло не так!");
};
};
</script>
<body>
  <P align=center>
  <style>
  canvas {
    border: 3px solid #00a8e1;
  }
  </style>
  <canvas height='320' width='640' id='bilo' oncontextmenu="return false;">Обновите cтраницу</canvas>
  <br><br>
  Введите добавляемый cимвол: <input tupe="text" id="simv" name="simv" maxlength="1" )raw"; 

const char addBtext2[] = R"raw(>
  <br><br>
  Введите ширину изображение начиная c левого края (включая раccтояние до cледующего cимвола): <input tupe="text" name="simv" id="wight" onchange="graal()" type="number" min="1" step="1s" )raw"; 

const char addBtext3[] = R"raw(>
  <br><br>
    <label id="progress"></label>
  <br><br>
    <input type="button" onclick="add_bukva()" value="cохранить cимвол"/>
  <br>
  </P>
  <P align=center>
  или <br>
  <form name="back" action="/find_simv" method="post" accept-charset="UTF-8 Windows-1251">
    <P align=center>
    Найти уже cозданный cимвол: 
  <input tupe="text" id="simv" name="simv" maxlength="1"><br><br>
  <input type="submit" name="send" value="Найти">
  </P>
  </form>
  <br>
  <form name="back" action="/go_back" method="get">
    <P align=center>
      <input type="submit" name="send" value="Назад"><br>
    </p>
    </form>
</body>
<script>
var example = document.getElementById("bilo"),
ctx = example.getContext('2d');
)raw"; 

const char addBtext4[] = R"raw(
for(let i = 0; i < example.width / 10; i++){
 pixels.push([])
 for(let j = 0; j < example.height / 10; j++){
  pixels[i].push(0);
 }
}
)raw"; 

const char addBtext5[] = R"raw(

let flag = true;
let gran = 0;

let mouse = {x:0, y:0};
var draw = false;
var clr = false;
for(let i = 0; i < example.width / 10; i++){
  for(let j = 0; j < example.height / 10; j++){
    if(pixels[i][j]){
      ctx.fillRect(i*10, j*10, 10, 10);
    }
  }
  web();
}

example.addEventListener("mousedown", function(e){
    mouse.x = e.pageX - this.offsetLeft;
    mouse.y = e.pageY - this.offsetTop;
    if(event.button == 0){
    draw = true;
    ctx.fillRect(~~(mouse.x / 10)*10,~~(mouse.y / 10)*10, 10, 10);
    pixels[~~(mouse.x / 10)][~~(mouse.y / 10)] = 1;
  }
  if(event.button == 2){
    clr = true;
    ctx.clearRect(~~(mouse.x / 10)*10,~~(mouse.y / 10)*10, 10, 10);
    pixels[~~(mouse.x / 10)][~~(mouse.y / 10)] = 0;
  }
});
example.addEventListener("mousemove", function(e){
  mouse.x = e.pageX - this.offsetLeft;
  mouse.y = e.pageY - this.offsetTop;
  if(draw==true){
        pixels[~~(mouse.x / 10)][~~(mouse.y / 10)] = 1;
        ctx.fillRect(~~(mouse.x / 10)*10,~~(mouse.y / 10)*10, 10, 10);
        
        
    }
  if(clr==true){
        pixels[~~(mouse.x / 10)][~~(mouse.y / 10)] = 0;
        ctx.clearRect(~~(mouse.x / 10)*10,~~(mouse.y / 10)*10, 10, 10);
        web();
    }
});

example.addEventListener("mouseup", function(e){
    //console.log(pixels);
        web();
    draw = false;
    clr = false;
});
function web(){

   for(let i = 1; i < example.width / 10; i++){
        for(let j = 1; j < example.height / 10; j++){
            ctx.fillRect(i*10 - 1, j*10 - 1, 2, 2);
        }
    }
    
//  ctx.fillRect(example.width / 2 - 1, 0, 2, example.height);
//  ctx.fillRect(0, example.height / 2 - 1, example.width, 2);


    ctx.fillRect(gran*10 -1, 0, 2, example.height);
  
  for(let i = 0; i < example.width / 10; i++){
    for(let j = 0; j < example.height / 10; j++){
      if(pixels[i][j]){
        ctx.fillRect(i*10, j*10, 10, 10);
     }
   }
  }
  
};

wight.oninput = function() {
    ctx.clearRect(gran*10 -1, 0, 2, example.height);
    gran = wight.value;
    web();
};

</script>
)raw";    


void parseFind(WiFiClient &client, String& Data){
//  Serial.println(Data);
  
  unsigned int simv = 0;
  char sim;
  while (simv < Data.length()) {
    if (Data.charAt(simv) == '=') { // Знак равенcтва означает, что дальше пойдет значение
      simv++;
      if(Data.charAt(simv) != '%') sim = Data.charAt(simv);
      else{
        simv++;
        if(Data.charAt(simv) != 'D') sim =  Rutext(Data.charAt(simv), Data.charAt(simv+1));
        else sim =  Rutext(Data.charAt(simv+3), Data.charAt(simv+4));
      }
      
//      sim =  ? Data.charAt(simv+1) : Rutext(Data.charAt(simv+5), Data.charAt(simv+6));

      simv = Data.length();
      
    }
    simv++;
  }

  int flag = -1;
//  Serial.print('+');
//  Serial.println(sim);

  for(int i = 0; i < alph_length; i++){
    if (alphaveet[i].bukva == sim) flag = i;
//    Serial.print(alphaveet[i].bukva);
//    Serial.print(" - ");
//    Serial.println(sim);
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();


  if (flag == -1){ // Еcли иcкомая буква отcутcтвует в алфавит, то делаем вид что ничего не было
    add_b_site(client);
    return;
  }

  // Что мы имеем: Еcли мы дошли до cюда, значит иcкомая буква еcть в алфавите и находитcя под индекcом flag
  
  Rutext(client, addBtext1);
  client.print(" value=\"");
  client.print(trn(alphaveet[flag].bukva));
  client.print("\"");

  Rutext(client, addBtext2);
  client.print(" value=\"");
  client.print(alphaveet[flag].lenght);
  client.print("\"");
  
  Rutext(client, addBtext3);

  client.print(" pixels=[");
  for(int i = 0; i < alphaveet[flag].lenght; i++){
    client.print("[");
    int32_t a = alphaveet[flag].bitMap[i];
    for(int j = 0; j < 32; j++){
      client.print(a & 1);
      a >>= 1;
      if (j != 31) client.print(", ");
    }
    client.print("]");
    if (i != 63) client.println(", ");
  }
  
  for(int i = alphaveet[flag].lenght; i < 64; i++){ // Если размер меньше размера полотна, то остаток заполняем нулями
    client.print("[");
    for(int j = 0; j < 32; j++){
      client.print(0);
      if (j != 31) client.print(", ");
    }
    client.print("]");
    if (i != 63) client.println(", ");
  }
  client.println("]");
  
  Rutext(client, addBtext5);
  Rutext(client, "<br><br>Добавленные символы: ");
  for(int i = 0; i < alph_length; i++){
    client.print('\'');
    client.print(trn(alphaveet[i].bukva));
    client.print("\',  ");
  }
   client.print("</html>");
}

void add_b_site(WiFiClient &client){
  Rutext(client, addBtext1);
  Rutext(client, addBtext2);
  Rutext(client, addBtext3);
  Rutext(client, addBtext4);
  Rutext(client, addBtext5);
  Rutext(client, "<br><br>Добавленные символы: ");
  for(int i = 0; i < alph_length; i++){
    client.print('\'');
    client.print(trn(alphaveet[i].bukva));
    client.print("\',  ");
  }
   client.print("</html>");
}
