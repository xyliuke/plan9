
$(document).ready(function() {
    $('.btn').click(function() {
        window.app.ChangeTextInJS('Hello World!');
    });
    $('#test').click(function() {
    	var json = {a:"a"}
        window.app.jscall("hellll", JSON.stringify(json), function(json){
        	alert("callback" + json)
        });
    });
});

function ChangeText(text) {
    $('#text').html(text);
}
