all: sum_serial sum_multi_thread

sum_serial: sum_serial.c
		gcc -o sum_serial sum_serial.c

sum_multi_thread: sum_multi_thread.c
		gcc -pthread -o sum_multi_thread sum_multi_thread.c
clean:
	    rm -f sum_serial sum_multi_thread