All : a2search

a2search: a2search.cpp porter2_stemmer.cpp
	g++ -std=c++11 -o a3search a2search.cpp porter2_stemmer.cpp

clean:
			rm  *.o a2search
