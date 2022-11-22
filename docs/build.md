Building the software
=====================

Local development
-----------------

Pre-requisites:
- python 3.7 (build native messaging extension & 64 bit version)
- Visual Studio 2017 or 2019 (build native messaging extension)
- Wix Toolset (build native messaging extension installer)
- Nodejs + npm (build and sign the Firefox extension)
- Account and API key on Fire

Python modules:
- pyyaml (version)
- cryptography (testing)

Windows compilation of openssl
------------------------------

### Install Strawberry Perl

```
curl from http://strawberryperl.com/download/5.30.2.1/strawberry-perl-5.30.2.1-64bit.zip
unzip and set the path
.\strawberry-perl\relocation.pl.bat
%cd%\strawberry-perl\c\bin
%cd%\strawberry-perl\perl\site\bin
%cd%\strawberry-perl\perl\bin
```

### Install NASM

```
curl --output nasm.zip https://www.nasm.us/pub/nasm/releasebuilds/2.15.03/win64/nasm-2.15.03-win64.zip
unzip and set the path
%cd%\nasm
```

### Setup Visual Studio 

This must be run in command prompt on Microsoft and not Powershell.
Setting up the command line options for Visual Studio
run vcvars -> 64 bit

```
C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat
```

run vcvars -> 32 bit

```
C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat
```

Or open command prompt from the Visual Studio <Year> start menu:
Start -> Visual Studio 2017 -> x64 ...

### Compiling OpenSSL

For 64bit:

```
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
"perl Configure --prefix="%cd%\..\..\common" --openssldir="%cd%\..\..\common" VC-WIN64A"
```

CI/CD using Github Actions
--------------------------

When building on github using the Actions support you're able to build on different platform. You need to know what is pre-installed on the platforms. This you find in the following links.

Support for OS: 
- https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions#jobsjob_idruns-on
- https://docs.github.com/en/actions/reference/virtual-environments-for-github-hosted-runners
- https://docs.github.com/en/actions/reference/software-installed-on-github-hosted-runners

Git tags auto pushing:
- git config --global push.followTags true

Version handling
----------------

- The "develop"-branch will use the .github/workflows/build-win32.yml build workflow. This will automatically create a development msi.
- The tags vX.Y.ZaN will use the .github/workflows/build-win32-alpha.yml build workflow for Alpha(N) releases.
```
python 4-set-version.py -c "Descriptive message" X.Y.ZaN
git commit -m "Descriptive message" .
git tag alpha-vX.Y.ZaN
git push origin alpha-vX.Y.ZaN
```
- The tags vX.Y.ZrcN will use the .github/workflows/build-win32-rc.yml build workflow for Release Candidate(N) releases.
```
python 4-set-version.py -c "Descriptive message" X.Y.ZrcN
git commit -m "Descriptive message" .
git tag rc-vX.Y.ZrcN
git push origin rc-vX.Y.ZrcN
```
- The tags vX.Y.Z will use the .github/workflows/build-win32-release.yml build workflow for end releases.
```
python 4-set-version.py -c "Descriptive message" X.Y.Z
git commit -m "Descriptive message" .
git tag vX.Y.Z
git push origin vX.Y.Z
```

Then git commit and tag the tree.