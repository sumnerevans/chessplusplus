TARGETS=chesspp/*.cpp
OUTFILE=chesspp

# I like clang++ personally, but if you want to use g++, change this
# If you're debating clang or GCC, clang gives you pretty compile errors
CXX=clang++

# That's all you have to configure. Make sure to install SFML on your system:
# Arch Linux:
#     sudo pacman -S sfml
# Debian/Ubuntu/Mint:
#     sudo apt-get install libsfml-dev
# Fedora:
#     sudo yum install SFML
#
# macOS / OS X using Homebrew:
#     sudo brew install sfml
#
# And come to LUG!

all: $(TARGETS)
	$(CXX) -Wall -Werror -g -std=c++11 -o $(OUTFILE).out $(TARGETS) -lsfml-system -lsfml-graphics -lsfml-window

run: $(TARGETS)
	./$(OUTFILE).out

clean:
	rm $(OUTFILE).out
