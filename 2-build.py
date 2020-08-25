from shutil import copyfile
import subprocess
import os

common_dir = os.path.abspath(os.getcwd() + '/common')

def get_common_dir():
    return common_dir

# TODO: evaluate git sub modules
def build_ksmgmnt():
    if not os.path.isdir("win32/build"):
        os.mkdir("win32/build")
    os.chdir("win32/build")
    subprocess.run('cmake .. -DCMAKE_BUILD_TYPE=Release -DCOMMON_DIR="' + get_common_dir() + '" -A x64', shell=True, check=True)
    subprocess.run('cmake --build . --config Release', shell=True, check=True)
    subprocess.run('.\\test\\Release\\tests.exe exclude:[ui]', shell=True, check=True)
    os.chdir("../..")
    return

def build_installer():
    os.chdir('installation')
    if os.path.isfile('ksmgmnt.msi'):
            os.remove('ksmgmnt.msi')
    subprocess.run('"C:\\Program Files (x86)\\WiX Toolset v3.11\\bin\\candle" .\\ksmgmnt.wxs', shell=True, check=True)
    subprocess.run('"C:\\Program Files (x86)\\WiX Toolset v3.11\\bin\\light.exe" -ext WixUIExtension .\\ksmgmnt.wixobj', shell=True, check=True)
    os.chdir('..')
    return

def run_scripts():
    build_ksmgmnt()
    if os.path.isfile('.\\installation\\ksmgmnt.exe'):
        os.remove('.\\installation\\ksmgmnt.exe')
    copyfile('.\\win32\\build\\src\\Release\\ksmgmnt.exe', '.\\installation\\ksmgmnt.exe')
    build_installer()
    return

if __name__ == "__main__":
    run_scripts()