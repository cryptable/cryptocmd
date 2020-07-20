




Windows compilation
-------------------

###Install Strawberry Perl
curl from http://strawberryperl.com/download/5.30.2.1/strawberry-perl-5.30.2.1-64bit.zip
unzip and set the path
.\strawberry-perl\relocation.pl.bat
%cd%\strawberry-perl\c\bin
%cd%\strawberry-perl\perl\site\bin
%cd%\strawberry-perl\perl\bin

###Install NASM
curl from https://www.nasm.us/pub/nasm/releasebuilds/2.15.03/win64/nasm-2.15.03-win64.zip
unzip and set the path
%cd%\nasm

###Setup Visual Studio 

Setting up the command line options for Visual Studio
run vcvars -> 64 bit
C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat
run vcvars -> 32 bit
C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat

Or open command prompt from the Visual Studio <Year> start menu:
Start -> Visual Studio 2017 -> x64 ...

###Compiling OpenSSL

For 64bit:
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
"perl Configure --prefix="%cd%\..\..\common" --openssldir="%cd%\..\..\common" VC-WIN64A"


Notes to forget
---------------
Adding path in Powershell:
$env:Path += ";SomeRandomPath"

Run batch file:
C:\Path\file.bat