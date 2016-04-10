all:
	ninja -C ./out/Debug

clean:
	ninja -C ./out/Debug -t clean
