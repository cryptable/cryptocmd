import subprocess
import os
import urllib.request
import shutil
import zipfile
import sys

common_dir = os.path.abspath(os.getcwd() + '/common')

def get_common_dir():
    return common_dir

def append_gitignore(file_to_ignore):
    with open('.gitignore', 'a+') as gitignore:
        gitignore.write(file_to_ignore + '\n')

def install_conan():
    subprocess.run('py -m pip install conan', shell=True, check=True)

# TODO: evaluate git sub modules
def install_cryptablepki():
    if not os.path.isdir("cryptablepki"):
        subprocess.run('git clone https://github.com/cryptable/cryptablepki.git', shell=True, check=True)
        append_gitignore('cryptablepki')
    else:
        os.chdir("cryptablepki")
        subprocess.run('git pull', shell=True, check=True)
        os.chdir("..")


    if not os.path.isdir("cryptablepki/pki/build"):
        os.mkdir("cryptablepki/pki/build")
    os.chdir("cryptablepki/pki/build")
    subprocess.run('conan install ..', shell=True, check=True)
    subprocess.run('cmake .. -DCMAKE_BUILD_TYPE=Release -DCONAN=1 -A x64', shell=True, check=True)
    subprocess.run('cmake --build . --config Release', shell=True, check=True)
    subprocess.run('cmake --install . --prefix "' + get_common_dir() + '" --config Release', shell=True, check=True)
    # debug
    subprocess.run('cmake .. -DCMAKE_BUILD_TYPE=Debug -DCONAN=1 -A x64', shell=True, check=True)
    subprocess.run('cmake --build . --config Debug', shell=True, check=True)
    subprocess.run('cmake --install . --prefix "' + get_common_dir() + '" --config Debug', shell=True, check=True)
    os.chdir("..")
    return

def run_scripts():
    if not os.path.isdir("3rd-party"):
        os.mkdir("3rd-party")
    os.chdir("3rd-party")
    install_conan()
    install_cryptablepki()
   
    os.chdir("..")
    return

if __name__ == "__main__":
    run_scripts()
