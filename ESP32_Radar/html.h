const char html_page[] PROGMEM = R"RawString(
<!DOCTYPE html>
<html lang="th">
<head>
  <meta charset="UTF-8">
  <title>ESP32 เรดาร์ ระบุระยะวัตถุ</title>
  <style>
    body {
      font-family: 'Noto Sans Thai', sans-serif;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      margin: 0;
      background-color: #000;
      color: #4CAF50;
    }
    h1 {
      text-align: center; 
      font-size: 50px;
    }
    p {
      text-align: center; 
      font-size: 40px;
    }
    .container {
      display: flex;
      flex-direction: column;
      align-items: center;
      margin-top: 20px;
    }
    .button {
      background-color: #4CAF50; /* สีพื้นหลังของปุ่ม */
      border: none; /* ไม่แสดงกรอบ */
      color: white; /* สีข้อความ */
      padding: 15px 32px; /* ระยะห่างในปุ่ม */
      text-align: center; /* จัดกึ่งกลางข้อความ */
      text-decoration: none; /* ไม่มีเส้นใต้ */
      display: inline-block; /* ให้ปุ่มแสดงในแนวนอน */
      font-size: 20px; /* ขนาดข้อความ */
      margin: 4px 2px; /* ระยะห่างระหว่างปุ่ม */
      cursor: pointer; /* แสดง cursor เป็นมือ */
      border-radius: 8px; /* มุมโค้งของปุ่ม */
      transition: background-color 0.3s; /* เอฟเฟกต์การเปลี่ยนสี */
    }
    .button:hover {
      background-color: #45a049; /* เปลี่ยนสีเมื่อเลื่อนเมาส์ไปที่ปุ่ม */
    }
    .input-container {
      margin: 10px;
    }
    input[type="number"] {
      padding: 10px;
      font-size: 20px;
      border: 2px solid #4CAF50; /* สีกรอบ */
      border-radius: 5px; /* มุมโค้งของกล่องข้อความ */
      width: 200px; /* กว้าง */
    }
    input[type="number"]:focus {
      border-color: #45a049; /* สีกรอบเมื่อเลือก */
      outline: none; /* ไม่มีกรอบเมื่อเลือก */
    }
  </style>
  <link href="https://fonts.googleapis.com/css2?family=Noto+Sans+Thai&display=swap" rel="stylesheet">
  <script src="https://cdnjs.cloudflare.com/ajax/libs/processing.js/1.6.0/processing.min.js"></script>
</head>
<body>
  <h1>ESP32 เรดาร์ ระบุระยะวัตถุ</h1><br>
  <p>ระยะทาง : <span id="_CM">0</span> ซม</p>
  <p>มุม : <span id="_ANGLE">0</span> องศา</p>
  
  <div class="container">
    <div class="input-container">
      <input type="number" id="buzzerDistance" placeholder="กำหนดระยะ (cm)">
      <button class="button" onclick="setBuzzerDistance()">เซ็ตระยะ</button>
    </div>
    <div class="input-container">
      <input type="number" id="servoAngle" placeholder="กำหนดองศา (0-180)">
      <button class="button" onclick="setServoAngle()">เซ็ตองศา</button>
    </div>
  </div>

<script>
function setBuzzerDistance() {
  var distance = document.getElementById("buzzerDistance").value;
  if (distance !== "") {  // ตรวจสอบว่ามีการป้อนค่าหรือไม่
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        console.log(this.responseText);  // แสดงผลการตอบสนองใน console
      }
    };
    xhttp.open("GET", "/setBuzzerDistance?distance=" + distance, true);  // ใช้ /setBuzzerDistance
    xhttp.send();
  } else {
    alert("Please enter a valid distance");
  }
}

function setServoAngle() {
  var angle = document.getElementById("servoAngle").value;
  if (angle !== "") {  // ตรวจสอบว่ามีการป้อนค่าหรือไม่
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        console.log(this.responseText);  // แสดงผลการตอบสนองใน console
      }
    };
    xhttp.open("GET", "/setAngle?angle=" + angle, true);  // ใช้ /setAngle
    xhttp.send();
  } else {
    alert("Please enter a valid angle");
  }
}


</script>

<script type="application/processing">
String angle="";
String distance="";
String data="";
String noObject;
float pixsDistance;
int iAngle, iDistance;
int index1=0;
int index2=0;
PFont orcFont;

  setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        const text = this.responseText;
        const myArr = JSON.parse(text);
        document.getElementById("_CM").innerHTML = myArr[0];
        document.getElementById("_ANGLE").innerHTML = myArr[1];
        angle = myArr[1];
        distance = myArr[0];
        iAngle = int(angle);
        iDistance = int(distance);
      }
    };
    xhttp.open("GET", "readDistance", true);
    xhttp.send();
  },50);

void setup() {
  
 size (1200, 700);
 smooth();

}
void draw() {
  
  fill(98,245,31);
  noStroke();
  fill(0,4); 
  rect(0, 0, width, height-height*0.065); 
  
  fill(98,245,31);
  drawRadar(); 
  drawLine();
  drawObject();
  drawText();
}

void drawRadar() {
  pushMatrix();
  translate(width/2,height-height*0.074);
  noFill();
  strokeWeight(2);
  stroke(98,245,31);
  arc(0,0,(width-width*0.0625),(width-width*0.0625),PI,TWO_PI);
  arc(0,0,(width-width*0.27),(width-width*0.27),PI,TWO_PI);
  arc(0,0,(width-width*0.479),(width-width*0.479),PI,TWO_PI);
  arc(0,0,(width-width*0.687),(width-width*0.687),PI,TWO_PI);

  line(-width/2,0,width/2,0);
  line(0,0,(-width/2)*cos(radians(30)),(-width/2)*sin(radians(30)));
  line(0,0,(-width/2)*cos(radians(60)),(-width/2)*sin(radians(60)));
  line(0,0,(-width/2)*cos(radians(90)),(-width/2)*sin(radians(90)));
  line(0,0,(-width/2)*cos(radians(120)),(-width/2)*sin(radians(120)));
  line(0,0,(-width/2)*cos(radians(150)),(-width/2)*sin(radians(150)));
  line((-width/2)*cos(radians(30)),0,width/2,0);
  popMatrix();
}
void drawObject() {
  pushMatrix();
  translate(width/2,height-height*0.074);
  strokeWeight(9);
  stroke(255,10,10);
  pixsDistance = iDistance*((height-height*0.1666)*0.025);

  if(iDistance<40){
  line(pixsDistance*cos(radians(iAngle)),-pixsDistance*sin(radians(iAngle)),(width-width*0.505)*cos(radians(iAngle)),-(width-width*0.505)*sin(radians(iAngle)));
  }
  popMatrix();
}
void drawLine() {
  pushMatrix();
  strokeWeight(9);
  stroke(30,250,60);
  translate(width/2,height-height*0.074);
  line(0,0,(height-height*0.12)*cos(radians(iAngle)),-(height-height*0.12)*sin(radians(iAngle)));
  popMatrix();
}
void drawText() {
  
  pushMatrix();
  if(iDistance>40) {
  noObject = "Out of Range";
  }
  else {
  noObject = "In Range";
  }
  fill(0,0,0);
  noStroke();
  rect(0, height-height*0.0648, width, height);
  fill(98,245,31);
  textSize(25);
  
  text("10cm",width-width*0.3854,height-height*0.0833);
  text("20cm",width-width*0.281,height-height*0.0833);
  text("30cm",width-width*0.177,height-height*0.0833);
  text("40cm",width-width*0.0729,height-height*0.0833);
  textSize(40);
  text("เรด้า", width-width*0.875, height-height*0.0277);
  text("องศา: " + iAngle +" °", width-width*0.48, height-height*0.0277);
  text("ระยะทาง: ", width-width*0.26, height-height*0.0277);
  if(iDistance<40) {
  text("              " + iDistance +" cm", width-width*0.225, height-height*0.0277);
  }
  textSize(25);
  fill(98,245,60);
  translate((width-width*0.4994)+width/2*cos(radians(30)),(height-height*0.0907)-width/2*sin(radians(30)));
  rotate(-radians(-60));
  text("30°",0,0);
  resetMatrix();
  translate((width-width*0.503)+width/2*cos(radians(60)),(height-height*0.0888)-width/2*sin(radians(60)));
  rotate(-radians(-30));
  text("60°",0,0);
  resetMatrix();
  translate((width-width*0.507)+width/2*cos(radians(90)),(height-height*0.0833)-width/2*sin(radians(90)));
  rotate(radians(0));
  text("90°",0,0);
  resetMatrix();
  translate(width-width*0.513+width/2*cos(radians(120)),(height-height*0.07129)-width/2*sin(radians(120)));
  rotate(radians(-30));
  text("120°",0,0);
  resetMatrix();
  translate((width-width*0.5104)+width/2*cos(radians(150)),(height-height*0.0574)-width/2*sin(radians(150)));
  rotate(radians(-60));
  text("150°",0,0);
  popMatrix(); 
}
</script>

<canvas></canvas>

</body>
</html>
)RawString";
