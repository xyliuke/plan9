
$(document).ready(function() {
    $('#test').click(function() {
        alert("test");
        js_call("hello c++", {p:'p1'}, function(result){
            alert(result);
        });
    });
});
