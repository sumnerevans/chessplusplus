TARGETS=src/*.cpp
OUTFILE=chesspp

# I like clang++ personally, but if you want to use g++, change this
# If you're debating clang or GCC, clang gives you pretty compile errors
CXX=clang++

# That's all you have to configure. Make sure to install SFML on your system.
# If you are on NixOS or have the Nix package manager on your machine, you can
# just run:
#
#     nix-shell to open a shell with all of the dependecies.
#
# If you are using direnv, then it will automatically add all of the
# dependecies from Nix shell to your path.
#
# Arch Linux:
#     sudo pacman -S sfml
#
# Debian/Ubuntu/Mint:
#     sudo apt-get install libsfml-dev
#
# Fedora:
#     sudo yum install SFML
#
# macOS / OS X using Homebrew:
#     sudo brew install sfml
#
# And come to LUG!

all: $(TARGETS)
	$(CXX) -Wall -Werror -g -std=c++11 -o $(OUTFILE) $(TARGETS) -lsfml-system -lsfml-graphics -lsfml-window

run: $(TARGETS)
	./$(OUTFILE)

clean:
	rm $(OUTFILE)
