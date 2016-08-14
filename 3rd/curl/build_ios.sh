export IPHONEOS_DEPLOYMENT_TARGET="6.0"
export CC="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
sdk=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
simu_sdk=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk

chmod +x configure
chmod +x install-sh
pp=$PWD

rm libcurl-*.a
rm -R include-*

${arch1:="armv7s armv7"}
for a in $arch1; do
	echo "$a 架构编译中。。。"
	export CFLAGS="-arch $a -pipe -Os -gdwarf-2 -isysroot $sdk"
	export LDFLAGS="-arch $a -isysroot $sdk"
	mkdir $a
	echo "$pp/$a"
	./configure --disable-shared --enable-static --host="$a-apple-darwin" --with-ssl --prefix="$pp/$a"
	make -j `sysctl -n hw.logicalcpu_max`
	make install
	cp $a/lib/libcurl.a libcurl-$a.a
	cp -R $a/include include-$a
	make clean
done

arch2=arm64 
echo "$arch2 架构编译中。。。"
export CFLAGS="-arch $arch2 -pipe -Os -gdwarf-2 -isysroot $sdk"
export LDFLAGS="-arch $arch2 -isysroot $sdk"
mkdir $arch2
./configure --disable-shared --enable-static --host="aarch64-apple-darwin" --with-ssl --prefix="$pp/$arch2"
make -j `sysctl -n hw.logicalcpu_max`
make install
cp $arch2/lib/libcurl.a libcurl-$arch2.a
cp -R $arch2/include include-$arch2
make clean

${arch3:="i386 x86_64"}
for b in $arch3; do
	echo "$b 架构编译中。。。"
	export CFLAGS="-arch $b -pipe -Os -gdwarf-2 -isysroot $simu_sdk"
	export CPPFLAGS="-D__IPHONE_OS_VERSION_MIN_REQUIRED=60000"
	export LDFLAGS="-arch $b -isysroot $simu_sdk"
	mkdir $b
	./configure --disable-shared --enable-static --host="$b-apple-darwin" --with-ssl --prefix="$pp/$b"
	make -j `sysctl -n hw.logicalcpu_max`
	make install
	cp $b/lib/libcurl.a libcurl-$b.a
	cp -R $b/include include-$b
	make clean
done

#生成一个库
lipo -create -output libcurl.a libcurl-*



