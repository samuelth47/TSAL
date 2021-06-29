#!/bin/bash

which -s brew
if [ $? != 0 ]; then
	echo "Installing homebrew..."
	/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

	#export PATH=/usr/bin:$PATH
else
	echo "Homebrew detected"
	#export PATH=/usr/bin:$PATH
fi

echo
echo "Using Homebrew to install dependencies..."
echo

brew update

#Download and install the dependencies
echo "Installing autoconf..."
brew install autoconf

# Check if the install worked, if not change the ownership of the directory
# and try to install again
if [ $? -eq 0 ]; then
	echo ""
else
	chmod u+w /usr/local/share/man/man8
	sudo chown -R $(whoami) /usr/local/share/man/man8

	brew install autoconf
fi

echo "Installing gcc..."
brew install gcc

echo "Installing automake..."
brew install automake

echo "Installing libtool..."
brew install libtool

echo "Installing portaudio..."
brew install portaudio

#check the latest version of gcc
gccLatest=$(brew list --versions gcc | cut -f 2 -d ' ')
gcc_version_header=$(brew list --versions gcc | cut -f 2 -d ' ' | cut -f 1 -d .)

echo Latest gcc version ${gccLatest}

#Create symbolic links for g++ and gcc
if brew list | grep -q 'gcc*'
then
        #check if gcc/g++ is installed in the right spot and create a sym links
        if [ -e /usr/local/bin/g++ ] && [ -e /usr/local/bin/gcc ]
        then
                echo "Updating g++ and gcc links!"
                rm /usr/local/bin/gcc
                rm /usr/local/bin/g++
        fi

        #Create the g++ and gcc links
        ln -s /usr/local/Cellar/gcc/${gccLatest}/bin/gcc-${gcc_version_header}* /usr/local/bin/gcc
        ln -s /usr/local/Cellar/gcc/${gccLatest}/bin/g++-${gcc_version_header}* /usr/local/bin/g++
fi


echo
echo "Installing TSAL..."

#make and install TSAL
sudo autoreconf -vi

./configure

sudo make clean

make

make install
