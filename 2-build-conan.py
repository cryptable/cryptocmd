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
    subprocess.run('conan install ..', shell=True, check=True)
    subprocess.run('cmake .. -DCMAKE_BUILD_TYPE=Release -DCONAN=1 -DCOMMON_DIR="' + get_common_dir() + '" -A x64', shell=True, check=True)
    subprocess.run('cmake --build . --config Release', shell=True, check=True)
    subprocess.run('.\\bin\\tests.exe exclude:[ui]', shell=True, check=True)
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

def build_extension():
    os.chdir('extensions/firefox/extension')
    subprocess.run('npm install --global web-ext', shell=True, check=True)
    subprocess.run('web-ext --version', shell=True, check=True)
    subprocess.run('web-ext lint -v -s .', shell=True, check=True)
    subprocess.run('web-ext build --overwrite-dest -v -s .', shell=True, check=True)
    if "WEB_EXT_SIGN" in os.environ:
        jwt_issuer=os.environ['AMO_JWT_ISSUER']
        jwt_secret=os.environ['AMO_JWT_SECRET']
        subprocess.run('web-ext sign --api-key={iss} --api-secret={sec}'.format(iss=jwt_issuer,sec=jwt_secret), shell=True, check=True)
    os.chdir('../../..')
    return

def run_scripts():
    build_extension()
    build_ksmgmnt()
    if os.path.isfile('.\\installation\\ksmgmnt.exe'):
        os.remove('.\\installation\\ksmgmnt.exe')
    copyfile('.\\win32\\build\\bin\\ksmgmnt.exe', '.\\installation\\ksmgmnt.exe')
    build_installer()
    return

if __name__ == "__main__":
    run_scripts()
