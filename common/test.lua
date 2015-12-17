
width = 100
height = 200

function test(obj)
    print(obj.param)
    callback(obj)
end


function app(obj)
    print("run app in lua " .. obj)
end

a = {}
a.b = {}
a.b.c = {}
a.b.c.d = function(obj)
    print("run a.b in lua")
    callback(obj)
end