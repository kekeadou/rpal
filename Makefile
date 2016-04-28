p2: argparse.cpp lexical.cpp main.cpp syntax.cpp tree.cpp
	g++ argparse.cpp lexical.cpp main.cpp syntax.cpp tree.cpp -o p2 

clean:
	rm -f p2 *.o
