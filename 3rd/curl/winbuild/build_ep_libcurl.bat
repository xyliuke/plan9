nmake /f Makefile.vc mode=static WITH_SSL=static VC=10 RTLIBCFG=static DEBUG=yes
nmake /f Makefile.vc mode=static WITH_SSL=static VC=10 RTLIBCFG=static
copy /y ..\builds\libcurl-vc10-x86-debug-static-ssl-static-ipv6-sspi\lib\libcurl_a_debug.lib %TX_ROOT%\client\build\lib\Debug\libcurl_a.lib
copy /y ..\builds\libcurl-vc10-x86-release-static-ssl-static-ipv6-sspi\lib\libcurl_a.lib %TX_ROOT%\client\build\lib\Release\libcurl_a.lib
