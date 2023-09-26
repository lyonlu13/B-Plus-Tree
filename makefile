ALL:compile
compile:
	g++ b_plus.cpp main.cpp -o main -g
	g++ b_plus.cpp gui.cpp -o gui -pthread -g
clean:
	rm main