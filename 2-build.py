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
    subprocess.run('cmake .. -DCMAKE_BUILD_TYPE=Release -DCOMMON_DIR="' + get_common_dir() + '" -A x64', shell=True)
    subprocess.run('cmake --build . --config Release', shell=True)
    subprocess.run('.\\test\\Release\\tests.exe exclude:[ui]', shell=True)
    os.chdir("../..")
    return

def build_installer():
    os.chdir('installation')
    if os.path.isfile('ksmgmnt.msi'):
            os.remove('ksmgmnt.msi')
    subprocess.run('candle .\\ksmgmnt.wxs', shell=True)
    subprocess.run('light.exe -ext WixUIExtension .\\ksmgmnt.wixobj', shell=True)
    os.chdir('..')
    return

def build_extension():
    os.chdir('extensions/firefox/extension')
    subprocess.run('npm install --global web-ext', shell=True)
    subprocess.run('web-ext --version', shell=True)
    subprocess.run('web-ext build', shell=True)
    subprocess.run('web-ext lint', shell=True)
    jwt_issuer=os.environ['AMO_JWT_ISSUER']
    jwt_secret=os.environ['AMO_JWT_SECRET']
    if "WEB_EXT_SIGN" in os.environ:
        subprocess.run('web-ext sign --api-key={iss} --api-secret={sec}'.format(iss=jwt_issuer,sec=jwt_secret), shell=True)
    os.chdir('../../..')
    return

def run_scripts():
    build_ksmgmnt()
    if os.path.isfile('.\\installation\\ksmgmnt.exe'):
        os.remove('.\\installation\\ksmgmnt.exe')
    copyfile('.\\win32\\build\\src\\Release\\ksmgmnt.exe', '.\\installation\\ksmgmnt.exe')
    build_installer()
    build_extension()
    return

if __name__ == "__main__":
    run_scripts()