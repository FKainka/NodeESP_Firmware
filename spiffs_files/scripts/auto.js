function autoFill(msg) {
  var form = document.getElementById(msg.cmd);
  if (form !== null) {
    var elements = form.elements;
      Object.keys(msg).forEach(function(key) {
      try {
        var element = elements[key];
        //own defined attribte, legit if use data- before, used to indentify autoFill elements
        if (element.hasAttribute("data-auto")) {
          if (element.type == "checkbox") {
            element.checked = msg[key];
          } else elements[key].value = msg[key];
        }
      } catch (err) {
        //element doenst exist
      }
    });
  }
}

function autoJson(form) {
  var msg = {};
  var str = form.getAttribute("id");
  msg["cmd"] = str.replace("get","set");
  for (var i = 0, d, v; i < form.elements.length; i++) {
    d = form.elements[i];
    if (d.hasAttribute("data-auto")) {

      if (d.type == "checkbox") {
        v = (d.checked) ? true : false;
      } else if (d.type == "radio") {
        var elmts = document.getElementsByName(d.getAttribute("id")); //radio buttons must have unique name in document ( except connected radio buttons)
        for (var r = 0; r < elmts.length; r++) {
          if (elmts[r].checked) v = elmts[r].value;
        }
      } else v = d.value;
      msg[d.getAttribute("id")] = v;
    }
  }
  return JSON.stringify(msg);
}



/*
  for (i in msg) {
    ssidSelect.innerHTML += '<option value="' + msg.ssids[i] + '"/>' + msg.ssids[i] + '</option>';
  }
  for (var i = 0, d, v; i < form.elements.length; i++) {
      form.getElementById()
  		d = form.elements[i];
  		if (d.name && d.value) {
  			v = (d.type == "checkbox" || d.type == "radio" ? (d.checked ? d.value : "") : d.value);
  			if (v) formData += d.name + "=" + escape(v) + "&";
  		}
  	}
    var xhr = new XMLHttpRequest();
    xhr.open('POST', form.action, true);
    xhr.send(formData);
    return false;
  }

*/
