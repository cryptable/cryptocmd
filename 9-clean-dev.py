import os
import shutil

def run_scripts():
    if os.path.isdir("3rd-party"):
        shutil.rmtree("3rd-party")
    if os.path.isdir("common"):
        shutil.rmtree("common")

if __name__ == "__main__":
    run_scripts()
