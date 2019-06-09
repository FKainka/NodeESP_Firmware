var connection;

function connect() {
  try { //if Script is on Board
    connection = new WebSocket('ws://' + location.hostname + ':80/ws', ['arduino']);
  } catch (err) { //Local excution
    connection = new WebSocket('ws:/192.168.178.43/ws', ['arduino']);
    //connection = new WebSocket('ws:/nodeesp.local:80/ws', ['arduino']);
  }

  connection.onclose = function() {
    // Try to reconnect
    console.log("disconnect, trying to reconnect")
    connect();
  };

  connection.onopen = function() {
    console.log('WebSocket Connected');
  //  setInterval(ping, 30000);
    onConnect()
  }

  connection.onerror = function(error) {
    console.log('WebSocket Error ', error);
  };

  connection.onmessage = function(e) {
    console.log('Server: ', e.data);

    var msg = JSON.parse(e.data);

    if (msg.cmd == 'pong') {
      pong();
      return;
    }

    handler(msg);
  }

  return connection;
}

function pong() {
  clearTimeout(tm);
}

function wsSend(msg){
    console.log(connection.send(msg));
  if (connection.readyState == 1) {
    connection.send(msg);  //connected
  }
 else {
    while(connection.readyState != 1);
    connection.send(msg);  //connected
 }


}

function ping() {
  var vjson = JSON.stringify({
    "cmd": "ping"
  });
  console.log(vjson);
  sendWs(vjson);

  tm = setTimeout(function() {}, 5000);
}
