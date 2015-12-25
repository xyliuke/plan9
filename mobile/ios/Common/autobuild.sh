xcodebuild -sdk iphonesimulator -target Common
xcodebuild -target Common
echo "正在打包库文件"
lipo -create ./build/Release-iphoneos/libCommon.a ./build/Release-iphonesimulator/libCommon.a -output libCommon.a
echo "库文件打包成功$PWD/libCommon.a" 
echo "删除编译中间文件"
rm -r ./build
echo "删除成功"
