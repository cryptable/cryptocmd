import os

def run_scripts():
    if os.path.isdir("3rd-party"):
        os.rmdir("3rd-party")
    if os.path.isdir("common"):
        os.rmdir("common")

if __name__ == "__main__":
    run_scripts()
