import subprocess
from winreg import ConnectRegistry, OpenKey, QueryValue, EnumValue, HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY, KEY_READ 
from os import path
import winapps

# You need to run in Python virtual environment
# install winapps


def verify_registry():
    regHandle = ConnectRegistry(None, HKEY_LOCAL_MACHINE)
    keyHandle = OpenKey(regHandle, r"SOFTWARE\Mozilla\NativeMessagingHosts\org.cryptable.pki.keymgmnt", access=KEY_READ)
    data = QueryValue(keyHandle, None)
    print(data)
    if data != r"C:\Program Files\Cryptable\Key Management Web Extension\org.cryptable.pki.keymgmnt.json":
        raise "registry verification error"

def check_file_exists():
    if not path.exists(r"C:\Program Files\Cryptable\Key Management Web Extension\org.cryptable.pki.keymgmnt.json"):
        raise "missing json file"
    if not path.exists(r"C:\Program Files\Cryptable\Key Management Web Extension\ksmgmnt.exe"):
        raise "missing executable file"
    if not path.exists(r"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Cryptable\Key Management Web Extension\Uninstall Key Management Web Extension.lnk"):
        raise "shortcut is missing"

def test_installation():
    subprocess.run('installation\\ksmgmnt.msi', shell=True, check=True)

def test_uninstallation():
    subprocess.run('msiexec /x installation\\ksmgmnt.msi', shell=True, check=True)

def test_uninstallation_using_shortcut():
    subprocess.run(r"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Cryptable\Key Management Web Extension\Uninstall Key Management Web Extension.lnk", shell=True, check=True)

def scenario1():
    test_installation()
    check_file_exists()
    verify_registry() 
    test_uninstallation()

def scenario2():
    test_installation()
    check_file_exists()
    verify_registry() 
    test_uninstallation_using_shortcut()

def main():
    scenario1()
    scenario2()

if __name__ == '__main__':
    main()