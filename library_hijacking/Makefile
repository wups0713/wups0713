all:
	gcc -o sandbox.so -shared -fPIC lib_hijack.c -ldl
	gcc sandbox.c -o sandbox

sample:
	gcc sample.c -o sample_32
	gcc sample.c -o sample_64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

sample_s:
	gcc sample.c -o sample_32_s -static
	gcc sample.c -o sample_64_s -static -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

clean:
	rm sandbox
	rm sandbox.so
	rm sample_?*