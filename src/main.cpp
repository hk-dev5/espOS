
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

WebServer server(80);
Preferences prefs;

const char* AP_NAME = "ESP-OS";
const char* AP_PASS = "esp-os1234";

// Cac GPIO cho phep trong giao dien.
// Tranh GPIO9 vi day la chan BOOT tren nhieu board C3.
const int pins[] = {0, 1, 3, 4, 5, 6, 7, 8, 10};
const int PIN_COUNT = sizeof(pins) / sizeof(pins[0]);

const char PAGE[] PROGMEM = R"ESPHTML(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport"
 content="width=device-width,initial-scale=1">
<title>ESP OS</title>

<style>
*{box-sizing:border-box}
body{
 margin:0;font-family:Arial,sans-serif;color:#fff;
 background:#182538;overflow:hidden
}
#desktop{
 height:100dvh;
 background:
 linear-gradient(135deg,#172d47,#326c83,#182538);
 padding:20px 12px 58px
}
#top{
 position:fixed;top:0;left:0;right:0;height:35px;
 background:#101820dd;display:flex;align-items:center;
 justify-content:space-between;padding:0 14px;
 font-size:13px
}
#taskbar{
 position:fixed;bottom:0;left:0;right:0;height:48px;
 background:#101820ee;display:flex;align-items:center;
 gap:8px;padding:6px 10px
}
button,select,input{
 font:inherit;border:0;border-radius:6px;padding:9px
}
button{background:#287eaa;color:white;cursor:pointer}
button:active{opacity:.7}
.app{
 display:inline-flex;width:90px;height:90px;
 margin:18px 8px;flex-direction:column;
 align-items:center;justify-content:center;
 gap:10px;cursor:pointer;text-align:center;
 font-size:13px;border-radius:9px
}
.app:hover{background:#ffffff20}
.ico{font-size:32px}
.win{
 display:none;position:fixed;left:5%;top:13%;
 width:90%;height:72%;max-width:700px;
 background:#e8edf2;color:#17212b;
 border-radius:10px;overflow:hidden;
 box-shadow:0 12px 45px #0009
}
.titlebar{
 height:42px;background:#253e52;color:white;
 display:flex;align-items:center;justify-content:space-between;
 padding:0 12px
}
.content{padding:15px;height:calc(100% - 42px);overflow:auto}
input,select{background:#fff;color:#17212b;border:1px solid #aab6c2}
.row{display:flex;align-items:center;gap:8px;flex-wrap:wrap;margin:12px 0}
.card{padding:12px;background:white;border-radius:8px;margin:8px 0}
#browserFrame{width:100%;height:calc(100% - 55px);border:0}
small{color:#667785}
</style>
</head>

<body>
<div id="desktop">
 <div class="app" onclick="openWin('gpio')">
  <span class="ico">🔌</span>GPIO Control
 </div>
 <div class="app" onclick="openWin('settings')">
  <span class="ico">⚙️</span>Settings
 </div>
 <div class="app" onclick="openWin('browser')">
  <span class="ico">🌐</span>Mini Browser
 </div>
 <div class="app" onclick="openWin('about')">
  <span class="ico">💻</span>About ESP OS
 </div>
</div>

<div id="top">
 <b>ESP OS</b><span id="clock">--:--</span>
</div>

<div id="taskbar">
 <button onclick="menu()">☰ Menu</button>
 <button onclick="openWin('gpio')">GPIO</button>
 <button onclick="openWin('settings')">Settings</button>
 <button onclick="openWin('browser')">Browser</button>
</div>

<div class="win" id="gpio">
 <div class="titlebar">
  <b>GPIO Control</b>
  <button onclick="closeWin('gpio')">X</button>
 </div>
 <div class="content">
  <p>Chọn GPIO và chế độ hoạt động.</p>
  <div class="row">
   <label>GPIO:</label>
   <select id="pin"></select>
   <label>Mode:</label>
   <select id="mode" onchange="modeChanged()">
    <option value="in">INPUT</option>
    <option value="out">OUTPUT</option>
   </select>
  </div>
  <div class="row" id="outControls">
   <button onclick="setPin(1)">HIGH</button>
   <button onclick="setPin(0)">LOW</button>
  </div>
  <div class="card">
   Trạng thái: <b id="pinStatus">Chưa đọc</b>
  </div>
  <button onclick="readPin()">Đọc lại GPIO</button>
  <p><small>Không nối GPIO trực tiếp vào nguồn điện
  cao áp hoặc tải vượt giới hạn của chip.</small></p>
 </div>
</div>

<div class="win" id="settings">
 <div class="titlebar">
  <b>System Settings</b>
  <button onclick="closeWin('settings')">X</button>
 </div>
 <div class="content">
  <h3>ESP OS Settings</h3>
  <div class="card">
   <b>Wi-Fi Access Point</b>
   <p>SSID: ESP-OS</p>
   <p>IP: 192.168.4.1</p>
  </div>
  <div class="card">
   <b>Giao diện</b>
   <div class="row">
    <button onclick="setTheme('#182538')">Dark Blue</button>
    <button onclick="setTheme('#303030')">Dark Grey</button>
   </div>
  </div>
  <button onclick="location.reload()">Reload desktop</button>
 </div>
</div>

<div class="win" id="browser">
 <div class="titlebar">
  <b>Mini Browser</b>
  <button onclick="closeWin('browser')">X</button>
 </div>
 <div class="content" style="padding:8px">
  <div class="row">
   <input id="url" value="https://example.com"
    style="flex:1;min-width:120px">
   <button onclick="browse()">Go</button>
   <button onclick="external()">Open tab</button>
  </div>
  <iframe id="browserFrame"
   sandbox="allow-forms allow-scripts allow-same-origin allow-popups"
   referrerpolicy="no-referrer"></iframe>
  <small>Nhiều website chặn iframe. Dùng Open tab
  để mở bằng trình duyệt thật.</small>
 </div>
</div>

<div class="win" id="about">
 <div class="titlebar">
  <b>About ESP OS</b>
  <button onclick="closeWin('about')">X</button>
 </div>
 <div class="content">
  <h2>ESP OS</h2>
  <p>Web-based operating system interface</p>
  <p>Hardware: ESP32-C3</p>
  <p>Firmware: ESP OS 0.1</p>
  <p>Local web server: port 80</p>
 </div>
</div>

<script>
function openWin(id){
 document.querySelectorAll('.win').forEach(
  w=>w.style.display='none');
 document.getElementById(id).style.display='block';
}
function closeWin(id){
 document.getElementById(id).style.display='none';
}
function menu(){
 const id='gpio';
 const w=document.getElementById(id);
 if(w.style.display==='block')closeWin(id);
 else openWin(id);
}
function setTheme(color){
 document.getElementById('desktop').style.background=color;
}
function browse(){
 let u=document.getElementById('url').value.trim();
 if(!/^https?:\/\//i.test(u))u='https://'+u;
 try{
  const parsed=new URL(u);
  if(!['http:','https:'].includes(parsed.protocol))return;
  document.getElementById('url').value=parsed.href;
  document.getElementById('browserFrame').src=parsed.href;
 }catch(e){}
}
function external(){
 let u=document.getElementById('url').value.trim();
 if(!/^https?:\/\//i.test(u))u='https://'+u;
 try{
  if(['http:','https:'].includes(new URL(u).protocol))
   window.open(u,'_blank','noopener');
 }catch(e){}
}
function modeChanged(){
 document.getElementById('outControls').style.display=
  document.getElementById('mode').value==='out'?'flex':'none';
}
async function api(path){
 const r=await fetch(path);
 if(!r.ok)throw Error('HTTP '+r.status);
 return await r.json();
}
async function readPin(){
 try{
  const p=document.getElementById('pin').value;
  const d=await api('/api/gpio?pin='+p+'&mode=read');
  document.getElementById('pinStatus').textContent=
   'GPIO '+p+': '+d.value;
 }catch(e){
  document.getElementById('pinStatus').textContent='Lỗi kết nối';
 }
}
async function setPin(v){
 try{
  const p=document.getElementById('pin').value;
  const m=document.getElementById('mode').value;
  if(m!=='out')return;
  const d=await api('/api/gpio?pin='+p+
   '&mode=out&value='+v);
  document.getElementById('pinStatus').textContent=
   'GPIO '+p+': '+d.value;
 }catch(e){
  document.getElementById('pinStatus').textContent='Lỗi GPIO';
 }
}
async function init(){
 try{
  const d=await api('/api/status');
  const s=document.getElementById('pin');
  d.pins.forEach(p=>{
   const o=document.createElement('option');
   o.value=p;o.textContent='GPIO '+p;s.appendChild(o);
  });
  readPin();
 }catch(e){
  document.getElementById('pinStatus').textContent='ESP offline';
 }
}
function tick(){
 document.getElementById('clock').textContent=
  new Date().toLocaleTimeString('vi-VN',
   {hour:'2-digit',minute:'2-digit'});
}
tick();setInterval(tick,1000);init();
</script>
</body>
</html>
)ESPHTML";

bool validPin(int pin) {
  for (int i = 0; i < PIN_COUNT; i++) {
    if (pins[i] == pin) return true;
  }
  return false;
}

void sendError(int code, const char* message) {
  String json = "{\"error\":\"";
  json += message;
  json += "\"}";
  server.send(code, "application/json", json);
}

void handleStatus() {
  String json = "{\"name\":\"ESP OS\",\"pins\":[";
  for (int i = 0; i < PIN_COUNT; i++) {
    if (i) json += ",";
    json += String(pins[i]);
  }
  json += "]}";

  server.send(200, "application/json", json);
}

void handleGPIO() {
  if (!server.hasArg("pin") || !server.hasArg("mode")) {
    sendError(400, "Missing arguments");
    return;
  }

  int pin = server.arg("pin").toInt();
  String mode = server.arg("mode");

  if (!validPin(pin)) {
    sendError(400, "GPIO not allowed");
    return;
  }

  if (mode == "read") {
    // Neu pin dang OUTPUT, doc muc logic hien tai.
    // Neu dang INPUT, doc muc logic dau vao.
    int value = digitalRead(pin);
    String json = "{\"pin\":" + String(pin) +
                  ",\"value\":" + String(value) + "}";
    server.send(200, "application/json", json);
    return;
  }

  if (mode == "out") {
    if (!server.hasArg("value")) {
      sendError(400, "Missing value");
      return;
    }

    int value = server.arg("value").toInt();
    if (value != 0 && value != 1) {
      sendError(400, "Value must be 0 or 1");
      return;
    }

    pinMode(pin, OUTPUT);
    digitalWrite(pin, value);

    String json = "{\"pin\":" + String(pin) +
                  ",\"value\":" + String(value) + "}";
    server.send(200, "application/json", json);
    return;
  }

  if (mode == "in") {
    pinMode(pin, INPUT);

    String json = "{\"pin\":" + String(pin) +
                  ",\"value\":" +
                  String(digitalRead(pin)) + "}";
    server.send(200, "application/json", json);
    return;
  }

  sendError(400, "Unknown mode");
}

void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", PAGE);
}

void setup() {
  // Khong can Serial; he thong chay doc lap.
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);

  bool ok = WiFi.softAP(AP_NAME, AP_PASS);

  if (!ok) {
    // Neu tao AP that bai thi dung tai day.
    while (true) delay(1000);
  }

  prefs.begin("espos", false);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/gpio", HTTP_GET, handleGPIO);

  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
