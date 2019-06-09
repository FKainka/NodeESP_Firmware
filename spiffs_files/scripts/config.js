var connection = connect();

var msg_apps;

function onConnect() {
  connection.send(JSON.stringify({
    "cmd": "get_wifi_status"
  }));
}

function handler(msg) {
  autoFill(msg); //included in auto.js

  switch (msg.cmd) {
    case 'get_wifi_status':
      var elmnt = document.getElementById('stat_connected');
      if (msg.stat_connected) {
        elmnt.style.backgroundColor = "#00FF00";
      } else {
        elmnt.style.backgroundColor = "FF0000";
      }

    case 'get_mqtt_check':
      var elmnt = document.getElementById('get_mqtt_check');
      if (msg.connected) {
        elmnt.style.backgroundColor = "#00FF00";
      } else {
        elmnt.style.backgroundColor = "FF0000";
      }

    case 'get_wifi_scan':
      var ssidSelect = document.getElementById('wifi_ssid_select');
      ssidSelect.innerHTML = "";
      for (i in msg.ssids) {
        ssidSelect.innerHTML += '<option value="' + msg.ssids[i] + '"/>' + msg.ssids[i] + '</option>';
      }
      break;

    case 'get_gen_set':
      document.getElementById("wifi_con_time_label").value = msg.wifi_con_time + " Seconds";
      document.getElementById("touch_threshold_label").value = msg.touch_threshold;

      var elmts = document.getElementsByName("wifi_mode");
      for (var i = 0; i < elmts.length; i++) {
        (elmts[i].value == msg.wifi_mode) ? elmts[i].checked = true: elmts[i].checked = false;
      }
      break;

    case 'get_app_set':
      var appSelect = document.getElementById('app_select');
      var appSets = document.getElementById('app_set_select');
      var textRunning = document.getElementById('app_running');

      textRunning.innerHTML = msg.app_running;
      appSets.innerHTML = "Select Application Set";
      appSelect.innerHTML = "Select Application";

      msg_apps = msg.apps;

      for (i in msg.apps) {
        appSets.innerHTML += '<option data-value=' + i + '/>' + i + '</option>';
      }

      changeAppSet();
      break;

    case 'alert':
      alert(msg.msg);
      break;

    case 'confirm':
      if (confirm(msg.msg)) {
        if (msg.hasOwnProperty('alert')) alert(msg.alert);
        autoRequest(msg.confirm);
      }

    default:
      console.log('No handle: ' + msg);
      break;
  }
}

function changeAppSet() {
  var appSelect = document.getElementById('app_select');
  var appSets = document.getElementById('app_set_select');
  var app_set = appSets.value;

  appSelect.innerHTML = "Select Application";

  for (i in msg_apps[app_set]) {
    appSelect.innerHTML += '<option value="' + msg_apps[app_set][i].app + '"/>' + msg_apps[app_set][i].title + '</option>';
  }
  changeSelectedApp();
}

function changeSelectedApp() {
  var appSelect = document.getElementById('app_select');
  var div_selected = document.getElementById('app_selected');
  var app_selected = appSelect.value;
  var appSets = document.getElementById('app_set_select');
  var app_set = appSets.value;

  for (i in msg_apps[app_set]) {
    if (msg_apps[app_set][i].app == app_selected){
       // console.log(msg_apps[app_set][i].info);
         div_selected.innerHTML =  msg_apps[app_set][i].title + ":<br>"+msg_apps[app_set][i].info;
    }
  }



}

function setWifiSSID(slct) {
  document.getElementById("wifi_ssid").value = slct.value;
}

function setWifiTime(slct) {
  document.getElementById("wifi_con_time_label").value = slct.value + " Seconds";
}

function setTouchThreshold(slct) {
  document.getElementById("touch_threshold_label").value = slct.value;
}

function autoRequest(cmd) {
  var vjson = JSON.stringify({
    "cmd": cmd
  });
  console.log(vjson);
  connection.send(vjson);
}

function reqScan() {
  var ssidDetails = document.getElementById('wifi_station_details');
  var ssidSelect = document.getElementById('wifi_ssid_select');
  if (!ssidDetails.hasAttribute('open')) {
    ssidSelect.innerHTML = '<option>Scanning for WiFi networks in range... </option>';
    autoRequest("get_wifi_scan");
  }
}

function sendForm(form) {
  msg = autoJson(form);
  console.log(msg);
  connection.send(autoJson(form));
  alert("New Configuration. Board will reset");
  window.location.reload();
}

function sendConData() {
  var vjson = JSON.stringify(({
    "cmd": "connect",
    "ssid": ssidSelect.value,
    "pwd": document.getElementById('pass').value
  }))
  console.log(vjson);
  connection.send(vjson);
}

function sendRestore() {
  var vjson = JSON.stringify(({
    "cmd": "restore",
  }))
  console.log(vjson);
  connection.send(vjson);
}
