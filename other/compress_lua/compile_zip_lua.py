# -*- coding:utf-8 -*- 
import os
import shutil

passwd = 'soapy88Pict'  #压缩时的密码,请自行修改
zip_file = 'abc.zip'  #这个是默认生成zip包的文件名,不需要修改
lua_src_path = '/Users/liuke/workspace/plan9/common/lua' #原始lua源代码的路径,请自行修改


def compile_one_lua(lua_file, luac_file):
    os.system('luac -o ' + luac_file + ' ' + lua_file)


def compile_lua(lua_path, luac_path):
    if not os.path.exists(lua_path):
        print('error! the lua src path "' + lua_path + '" is not exist . ')
        return False
    if not os.path.exists(luac_path):
        os.makedirs(luac_path)
    files = os.listdir(lua_path)
    for file in files:
        full_file = os.path.abspath(os.path.join(lua_path, file))
        if os.path.isdir(full_file):
            new_path = os.path.join(luac_path, file)
            if not os.path.exists(new_path):
                os.makedirs(new_path)
            compile_lua(full_file, os.path.join(luac_path, file))
        elif os.path.isfile(full_file):
            if full_file.rfind(".lua") != -1 :
                new_path = os.path.join(luac_path, file)
                compile_one_lua(full_file, new_path)
    return True


def zip_lua(luac_path):
    os.system('./common -c ' + luac_path + ' ' + passwd + ' ./' + zip_file)


def unzip_lua(zip_path):
    os.system('./common -d ' + zip_path + ' ' + passwd + ' ./extract/')


def run(lua_path):
    if compile_lua(lua_path, './luac/'):
        zip_lua('./luac/')
        print("compress zip file is : " + os.path.abspath('./' + zip_file))
        shutil.rmtree('./luac')


if __name__ == "__main__":
    run(lua_src_path)