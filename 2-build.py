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
        os.remove('ksmgmnt.wixobj')
        os.remove('ksmgmnt.wixpdb')
    if os.path.isfile('ksmgmnt.log'):
        os.remove('ksmgmnt.log')
    subprocess.run('"C:\\Program Files (x86)\\WiX Toolset v3.11\\bin\\candle" .\\ksmgmnt.wxs', shell=True, check=True)
    subprocess.run('"C:\\Program Files (x86)\\WiX Toolset v3.11\\bin\\light.exe" -ext WixUIExtension .\\ksmgmnt.wixobj', shell=True, check=True)
    os.chdir('..')
    return

def build_extension():
    os.chdir('extensions/firefox/extension')
    subprocess.run('npm install --global web-ext', shell=True, check=True)
    subprocess.run('web-ext --version', shell=True, check=True)
    subprocess.run(['web-ext', 'lint'], shell=True, check=True)
    subprocess.run(['web-ext', 'build', '--overwrite-dest'], shell=True, check=True)
    if "WEB_EXT_SIGN" in os.environ:
        jwt_issuer=os.environ['AMO_JWT_ISSUER']
        jwt_secret=os.environ['AMO_JWT_SECRET']
        subprocess.run('web-ext sign --api-key={iss} --api-secret={sec} -n firefox-addon.xpi'.format(iss=jwt_issuer,sec=jwt_secret), shell=True, check=True)
    os.chdir('../../..')
    return

def run_scripts():
    build_ksmgmnt()
    if os.path.isfile('.\\installation\\ksmgmnt.exe'):
        os.remove('.\\installation\\ksmgmnt.exe')
    copyfile('.\\win32\\build\\src\\Release\\ksmgmnt.exe', '.\\installation\\ksmgmnt.exe')
    build_installer()
# TODO: Support local build
#    build_extension()
    return

if __name__ == "__main__":
    run_scripts()