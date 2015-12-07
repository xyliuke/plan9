window.app.jscall = function(method, param, callback) {
	window.app.__jscall__(method, JSON.stringify(param), function(json){
		callback(JSON.parse(json))
    });
}

window.app.notify = function(json) {
	alert(json.ret)
}

window.app.__notify__ = function(result) {
	window.app.notify(JSON.parse(result))
}


$(document).ready(function() {
    $('.btn').click(function() {
        window.app.ChangeTextInJS('Hello World!');
    });
    $('#test').click(function() {
    	var json = {a:"a"}
        window.app.jscall("m1", json, function(json){
        	alert(json.ret)
        });
    });
});

function ChangeText(text) {
    $('#text').html(text);
}
