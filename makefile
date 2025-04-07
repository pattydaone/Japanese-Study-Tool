all:
	echo "build, run"
build:
	g++ -I/usr/lib/wx/include/gtk3-unicode-3.2 -I/usr/include/wx-3.2 -DWXUSINGDLL -D__WXGTK3__ -D__WXGTK__ -D_FILE_OFFSET_BITS=64 -Wall -g -I pwd -c main.cpp -o obj/Debug/main.o
	g++ -o bin/Debug/testing obj/Debug/main.o  -lwx_gtk3u_xrc-3.2 -lwx_gtk3u_html-3.2 -lwx_gtk3u_qa-3.2 -lwx_gtk3u_core-3.2 -lwx_baseu_xml-3.2 -lwx_baseu_net-3.2 -lwx_baseu-3.2
run:
	bin/Debug/testing
br:
	g++ -I/usr/lib/wx/include/gtk3-unicode-3.2 -I/usr/include/wx-3.2 -DWXUSINGDLL -D__WXGTK3__ -D__WXGTK__ -D_FILE_OFFSET_BITS=64 -Wall -g -I pwd -c main.cpp -o obj/Debug/main.o
	g++ -o bin/Debug/testing obj/Debug/main.o  -lwx_gtk3u_xrc-3.2 -lwx_gtk3u_html-3.2 -lwx_gtk3u_qa-3.2 -lwx_gtk3u_core-3.2 -lwx_baseu_xml-3.2 -lwx_baseu_net-3.2 -lwx_baseu-3.2
	bin/Debug/testing
testbuild:
	g++ -I/home/patrick/Japanese-Study-Tool test.cpp -o a.out
	./a.out
