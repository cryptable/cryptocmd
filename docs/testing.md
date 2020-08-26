Testing
=======

Testing is done through using python, but because this is a 64 bit application you'll need 64 bit version of python.
Also create the viurtual environment with the name venv (this is included in the .gitignore).


Testing UI components
---------------------

### Installation

```
py integration-test\installation-test.py
```

### Unit testing Native messaging component

Use the same password

```
win32\build\bin\tests.exe [ui]
```


### Fuzzing testing

You need a the python cryptography library.
