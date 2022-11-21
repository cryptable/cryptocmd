import subprocess
import os
import urllib.request
import shutil
import zipfile
import sys

# Catch22 library version for C++ unit testen
CATCH2_VERSION="2.13.10"
# Strawberry Perl version to support the openssl build
STRAWBERRY_PERL_VERSION="5.32.1.1"
# NASM version to support the openssl build
NASM_VERSION="2.15.05"
# Openssl version to build
OPENSSL_VERSION="3.0.7"



# Visual Studio command line tools to build C/C++ projects
#
# "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat" -arch=amd64
# ref: https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=vs-2019#developer_command_file_locations
# Fall back to Windows 2019 Github location
# C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Enterprise\\VC\\Auxiliary\\Build\\vcvarsall.bat
# Fall back to Windows 2017 Github location
# C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Enterprise\\VC\\Auxiliary\\Build\\vcvarsall.bat
# Microsoft Visual Studio 2019 BuildTools Community edition
# C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat
VCVARSALL = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat"

STRAWBERRY_PERL_URL=f"http://strawberryperl.com/download/{ STRAWBERRY_PERL_VERSION }/strawberry-perl-{ STRAWBERRY_PERL_VERSION }-64bit.zip"

common_dir = os.path.abspath(os.getcwd() + '/common')

def get_common_dir():
    return common_dir

def install_catch2():
    if not os.path.isdir("Catch2"):
        subprocess.run('git clone https://github.com/catchorg/Catch2.git', shell=True, check=True)
        append_gitignore('Catch2')
    else:
        os.chdir("Catch2")
        subprocess.run('git pull origin devel', shell=True, check=True)
        os.chdir("..")

    os.chdir("Catch2")
    subprocess.run('git fetch --all --tags', shell=True, check=True)
    try:
        subprocess.run(f'git checkout tags/v{ CATCH2_VERSION } -b v{ CATCH2_VERSION }', shell=True, check=True, capture_output=True)
    except subprocess.CalledProcessError as e:
        if not "already exists" in str(e.stderr):
            raise e

    subprocess.run('cmake -DCMAKE_INSTALL_PREFIX=' + get_common_dir() + ' -Bbuild -H. -DBUILD_TESTING=OFF', shell=True, check=True)
    subprocess.run('cmake --build build/ --target install', shell=True, check=True)

    os.chdir("..")
    return

def vs_env_dict():
    vsvar64 = ''
    if 'VS150COMNTOOLS' in os.environ:
        vsvarsall = '{vscomntools}\\VsDevCmd.bat'.format(vscomntools=os.environ['VS150COMNTOOLS'])
        cmd = [vsvarsall, '-arch=amd64', '&&', 'set']
    elif 'VS160COMNTOOLS' in os.environ:
        vsvarsall = '{vscomntools}\\VsDevCmd.bat'.format(vscomntools=os.environ['VS160COMNTOOLS'])
        cmd = [vsvarsall, '-arch=amd64', '&&', 'set']
    elif 'VS140COMNTOOLS' in os.environ:
        vsvarsall = VCVARSALL.format(vscomntools=os.environ['VS140COMNTOOLS'])
        cmd = [vsvarsall, 'x86_amd64', '&&', 'set']
    else:
        vsvarsall = VCVARSALL
        cmd = [vsvarsall, 'x86_amd64', '&&', 'set']
    popen = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = popen.communicate()
    if popen.wait() != 0:
        raise ValueError(stderr.decode("mbcs")) 
    output = stdout.decode("mbcs").split("\r\n")
    return dict((e[0].upper(), e[1]) for e in [p.rstrip().split("=", 1) for p in output] if len(e) == 2)


def append_gitignore(file_to_ignore):
    with open('.gitignore', 'a+') as gitignore:
        gitignore.write(file_to_ignore + '\n')

def install_openssl_win32():
    if not os.path.isdir("openssl"):
        subprocess.run('git clone https://github.com/openssl/openssl.git', shell=True, check=True)
        append_gitignore('openssl')
    else:
        os.chdir("openssl")
        subprocess.run('git pull origin master', shell=True, check=True)
        os.chdir("..")

    openssl_build_env = os.environ.copy()
    # Visual Studio Setup
    openssl_build_env.update(vs_env_dict())

    # Strawberry perl
    if not os.path.isdir('strawberryperl'):
        with urllib.request.urlopen(STRAWBERRY_PERL_URL) as response, open("strawberryperl.zip", 'wb') as out_file:
            shutil.copyfileobj(response, out_file)
        with zipfile.ZipFile("strawberryperl.zip", 'r') as zip_ref:
            zip_ref.extractall("strawberryperl")
        append_gitignore('strawberryperl.zip')
        append_gitignore('strawberryperl')
    subprocess.run('.\\strawberryperl\\relocation.pl.bat', shell=True, check=True)
    openssl_build_env['PATH'] = os.getcwd() + '\\strawberryperl\\c\\bin;' + openssl_build_env["PATH"]
    openssl_build_env['PATH'] = os.getcwd() + '\\strawberryperl\\perl\\site\\bin;' + openssl_build_env["PATH"]
    openssl_build_env['PATH'] = os.getcwd() + '\\strawberryperl\\perl\\bin;' + openssl_build_env["PATH"]

    # Nasm
    if not os.path.isdir(f'nasm-{ NASM_VERSION }'):
        with urllib.request.urlopen(f'https://www.nasm.us/pub/nasm/releasebuilds/{ NASM_VERSION }/win64/nasm-{ NASM_VERSION }-win64.zip') as response, open("nasm64.zip", 'wb') as out_file:
            shutil.copyfileobj(response, out_file)
        with zipfile.ZipFile("nasm64.zip", 'r') as zip_ref:
            zip_ref.extractall(".")
        append_gitignore('nasm64.zip')
        append_gitignore(f'nasm-{ NASM_VERSION }')
    openssl_build_env['PATH'] = os.getcwd() + f'\\nasm-{ NASM_VERSION };' + openssl_build_env["PATH"]

    # OpenSSL
    os.chdir("openssl")
    pwait = subprocess.run('git fetch --all --tags', shell=True, check=True)
    pwait = subprocess.run(f'git checkout tags/openssl-{ OPENSSL_VERSION } -b v{ OPENSSL_VERSION }', shell=True, check=True)
    # DEBUG version
    # pwait = subprocess.Popen(['perl', 'Configure', 'no-ssl2', 'no-ssl3', 'no-asm', 'no-shared', '-d', 'disable-capieng',
    #     '--prefix=' + get_common_dir(), '--openssldir=' + get_common_dir(), 
    #     'VC-WIN64A'], env=openssl_build_env, shell=True, check=True)
    # if (pwait.wait()):
    #     sys.exit(pwait.returncode)
    # pwait = subprocess.Popen(['nmake', 'clean'], env=openssl_build_env, shell=True, check=True)
    # if (pwait.wait()):
    #     sys.exit(pwait.returncode)
    # pwait = subprocess.Popen(['nmake'], env=openssl_build_env, shell=True, check=True)
    # if (pwait.wait()):
    #     sys.exit(pwait.returncode)
    # pwait = subprocess.Popen(['nmake', 'install'], env=openssl_build_env, shell=True, check=True)
    # if (pwait.wait()):
    #     sys.exit(pwait.returncode)
    # if (os.path.isfile(get_common_dir() + '/bin/openssld.exe')):
    #     os.remove(get_common_dir() + '/bin/openssld.exe')
    # os.rename(get_common_dir() + '/bin/openssl.exe', get_common_dir() + '/bin/openssld.exe')
    # if (os.path.isfile(get_common_dir() + '/lib/libcryptod.lib')):
    #     os.remove(get_common_dir() + '/lib/libcryptod.lib')
    # os.rename(get_common_dir() + '/lib/libcrypto.lib', get_common_dir() + '/lib/libcryptod.lib')
    # if (os.path.isfile(get_common_dir() + '/lib/libssld.lib')):
    #     os.remove(get_common_dir() + '/lib/libssld.lib')
    # os.rename(get_common_dir() + '/lib/libssl.lib', get_common_dir() + '/lib/libssld.lib')

    # Release
    pwait = subprocess.Popen(['perl.exe', 'Configure', 'no-ssl2', 'no-ssl3', 'no-asm', 'no-shared', 'disable-capieng',
        '--prefix=' + get_common_dir(), '--openssldir=' + get_common_dir(), 
        'VC-WIN64A'], env=openssl_build_env, shell=True)
    if (pwait.wait()):
        sys.exit(pwait.returncode)
    pwait = subprocess.Popen(['nmake', 'clean'], env=openssl_build_env, shell=True)
    if (pwait.wait()):
        sys.exit(pwait.returncode)
    pwait = subprocess.Popen(['nmake'], env=openssl_build_env, shell=True)
    if (pwait.wait()):
        sys.exit(pwait.returncode)
    pwait = subprocess.Popen(['nmake', 'test'], env=openssl_build_env, shell=True)
    if (pwait.wait()):
        print('------------------ WARNING: Investigate TEST errors: see failed-tests.log ----------------')
        log = open('failed-tests.log', 'w')
        openssl_build_env['V']='1'
        openssl_build_env['TESTS']='test_verify'
        pwait = subprocess.Popen(['nmake', 'test'], env=openssl_build_env, stdout=log, shell=True)
        pwait.wait()
        log.flush()
        log.close()
        print('------------------ WARNING: Investigate TEST errors ----------------')
    pwait = subprocess.Popen(['nmake', 'install'], env=openssl_build_env, shell=True)
    if (pwait.wait()):
        sys.exit(pwait.returncode)
    os.chdir("..")
    return

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
    subprocess.run('cmake .. -DCMAKE_BUILD_TYPE=Release -DCOMMON_DIR="' + get_common_dir() + '" -A x64', shell=True, check=True)
    subprocess.run('cmake --build . --config Release', shell=True, check=True)
    subprocess.run('cmake --install . --prefix "' + get_common_dir() + '" --config Release', shell=True, check=True)
    # debug
    subprocess.run('cmake .. -DCMAKE_BUILD_TYPE=Debug -DCOMMON_DIR="' + get_common_dir() + '" -A x64', shell=True, check=True)
    subprocess.run('cmake --build . --config Debug', shell=True, check=True)
    subprocess.run('cmake --install . --prefix "' + get_common_dir() + '" --config Debug', shell=True, check=True)
    os.chdir("../../..")
    return

# TODO: check conan instead of the amateurish stuff ;-)
def install_nlohmann_json():
    if not os.path.isdir('json'):
        subprocess.run('git clone https://github.com/nlohmann/json.git', shell=True, check=True)
        append_gitignore('json')
    else:
        os.chdir('json')
#        subprocess.run('git pull origin master', shell=True, check=True)
        os.chdir("..")


    if not os.path.isdir("json/build"):
        os.mkdir("json/build")
    os.chdir("json")
    subprocess.run('git fetch --all --tags', shell=True, check=True)
    subprocess.run('git checkout v3.9.1', shell=True, check=True)
    os.chdir("build")
    subprocess.run('cmake .. -DCMAKE_BUILD_TYPE=RELEASE -A x64', shell=True, check=True)
    subprocess.run('cmake --build . --config Release', shell=True, check=True)
    subprocess.run('cmake --install . --prefix "' + get_common_dir() + '" --config Release', shell=True, check=True)
    os.chdir("..")
    os.chdir("..")
    return


def run_scripts():
    if not os.path.isdir("3rd-party"):
        os.mkdir("3rd-party")
    os.chdir("3rd-party")
    install_catch2()
#    install_openssl_win32()
#    install_nlohmann_json()
    install_cryptablepki()
   
    os.chdir("..")
    return

if __name__ == "__main__":
    run_scripts()
