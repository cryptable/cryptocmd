import argparse
import json
import re
import shutil
import xml.etree.ElementTree as ET
import datetime
from ruamel.yaml import YAML

from packaging.version import Version
from tempfile import mkstemp

def versionize_web_extensions(version):
    # This will versionize the web extension (firefox)
    with open("./extensions/firefox/extension/manifest.json", "r+") as manifestFile:
        data = json.load(manifestFile)
        data["version"] = version
        manifestFile.seek(0)  # rewind
        json.dump(data, manifestFile, indent=4)
        manifestFile.truncate()

def versionize_wix_installation(version):
    # This will versionize the wix installer
    ET.register_namespace('', "http://schemas.microsoft.com/wix/2006/wi")
    tree = ET.parse('./installation/ksmgmnt.wxs')
    root = tree.getroot()
    product = root.find('{http://schemas.microsoft.com/wix/2006/wi}Product')
    version_str = str(version.major) + "." + str(version.minor) + "." + str(version.micro)
    product.set('Version', version_str)
    tree.write('./installation/ksmgmnt.wxs', encoding='windows-1252', xml_declaration=True)
    return

def versionize_code(version):
    # This will versionize the code
    fn_out = mkstemp()
    fout = open(fn_out[0], "w+")
    new_version='#define VERSION "' + str(version) + '"'
    with open('./win32/src/version.h', 'r+' ) as fin:
        for line in fin:
            line_new = re.sub(r'#define\sVERSION\s"[.0-9a-zA-Z]*"', new_version, line)    
            fout.write(line_new)
    fout.close()
    shutil.move(fn_out[1], './win32/src/version.h')
    return

def versionize_github_workflow(version, full_version, changelog):
    filename = ".github/workflows/build-win32-release.yml"
    ff_ext = '-an+fx.xpi'
    if (version.is_prerelease):
        if version.pre[0] == "a":
            filename = ".github/workflows/build-win32-alpha.yml"
            ff_ext = '.zip'
        elif version.pre[0] == "b":
            filename = ".github/workflows/build-win32-alpha.yml"
            ff_ext = '.zip'
        elif version.pre[0] == "rc":
            filename = ".github/workflows/build-win32-rc.yml"
        else:
            filename = ".github/workflows/build-win32-rc.yml"
    if (version.is_devrelease):
        return

    yaml=YAML()
    with open(filename) as yaml_file:
        doc = yaml.load(yaml_file)
        for step in doc['jobs']['build-native-exe']['steps']:
            if ('id' in step) and step['id'] == 'create_release':
                step['with']['release_name'] = full_version
                step['with']['body'] = changelog
        for step in doc['jobs']['build-ff-ext']['steps']:
            if ('id' in step) and step['id'] == 'create_release':
                step['with']['release_name'] = full_version
                step['with']['body'] = changelog
            if ('id' in step) and step['id'] == 'upload-release-asset':
                step['with']['asset_path'] = 'extensions/firefox/extension/web-ext-artifacts/windows_crypto_key_generation-'+ str(version) + ff_ext
                step['with']['asset_name'] = 'windows_crypto_key_generation-' + str(version) + ff_ext
        fn_out = mkstemp()
        fout = open(fn_out[0], "w+")
        yaml.dump(doc,fout)
        fout.close()
        shutil.move(fn_out[1], filename)
    return

def update_changelog(version, changelog):
    # This will versionize the web extension (firefox)
    fout = open('./version.txt', 'w')
    fout.write(str(version))
    fout.close()

    # TODO: Implement prevention of duplicate versions
    fin = open('./CHANGELOG.md', 'r')
    fn_out = mkstemp()
    fout = open(fn_out[0], 'w+')
    changelogToWrite = True
    for line in fin:
        if changelogToWrite and re.match(r'##\s\[[.0-9a-zA-Z]*\]\s-\s[0-9]*-[0-9]*-[0-9]*', line):
            now = datetime.datetime.now()
            fout.write('## [' + str(version) +'] - ' + now.strftime("%Y-%m-%d") + '\n\n')
            fout.write(changelog)
            fout.write('\n\n')
            changelogToWrite = False
        fout.write(line)
    fout.close()
    fin.close()
    shutil.move(fn_out[1], './CHANGELOG.md')
    return

def is_int(s):
    try: 
        int(s)
        return True
    except ValueError:
        return False

def main():
    parser = argparse.ArgumentParser(description='Set version of the release in all files')
    parser.add_argument('version', 
        nargs='?',
        type=str,
        help='Use the version number as in python PEP440 x.y.z')
    parser.add_argument('-c', '--changelog', 
        nargs='?',
        type=str,
        help='Add some text as changelog',
        required=True)

    args = parser.parse_args()

    version = Version(args.version)

    publish_version_str = ""
    if (version.is_prerelease):
        if version.pre == None:
            print("Unsupported Pre Release tag")
            exit(-1)            
        if version.pre[0] == "a":
            publish_version_str = "Alpha(" + str(version.pre[1]) + ")"
        elif version.pre[0] == "b":
            publish_version_str = "Beta(" + str(version.pre[1]) + ")"
        elif version.pre[0] == "rc":
            publish_version_str = "Release Candidate(" + str(version.pre[1]) + ")"
        elif version.pre[0] == "pre":
            publish_version_str = "Pre Release(" + str(version.pre[1]) + ")"
        else:
            print("Unsupported Pre Release tag")
            exit(-1)
    if (version.is_postrelease):
        if version.is_prerelease:
            publish_version_str = "Post Release(" + str(version.post) + ") of " + publish_version_str
        else:
            publish_version_str = "Post Release(" + str(version.post) + ")"
    if (version.is_devrelease):
        if version.is_prerelease or version.is_prerelease:
            publish_version_str = "Development Release(" + str(version.dev) + ") of " + publish_version_str
        else:
            publish_version_str = "Development Release(" + str(version.dev) + ")"

    if len(publish_version_str) > 0:
        publish_version_str = publish_version_str + " - "
    publish_version_str = publish_version_str + "v" + str(version.major) + "." + str(version.minor) + "." + str(version.micro)

    print(str(version))

    versionize_web_extensions(str(version))
    versionize_wix_installation(version)
    versionize_code(version)
    versionize_github_workflow(version, publish_version_str, args.changelog)
    update_changelog(version, args.changelog)

if __name__ == '__main__':
    main()
