
//js与c++交互通道
var callback_map = {};
var window_id;
js_call = function(method, param, callback) {
	var id = __js_caller__.getID();
	if (!callback) {
		__js_caller__.call(window_id, id, method, JSON.stringify(param), false);
	} else {
		callback_map[id] = callback;
		__js_caller__.call(window_id, id, method, JSON.stringify(param), true);
	}
}

__callback__ = function(id, result) {
	alert("callback : " + id + ' , ' + result);
	cb = callback_map[id];
	if (cb) {
		delete callback_map[id];
		cb(result);
	}
}

__notify__ = function(event, result) {
    alert("nofity " + event + "," + result);
}


__set_window_id__ = function (id) {
	window_id = id;
}
