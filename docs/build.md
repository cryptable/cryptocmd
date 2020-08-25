Building the software
=====================

Local development
-----------------

Pre-requisites:
- python 3.7 (build native messaging extension)
- Visual Studio 2017 or 2019 (build native messaging extension)
- Wix Toolset (build native messaging extension installer)
- Nodejs + npm (build and sign the Firefox extension)
- Account and API key on Fire

CI/CD using Github Actions
--------------------------

When building on github using the Actions support you're able to build on different platform. You need to know what is pre-installed on the platforms. This you find in the following links.

Support for OS: 
- https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions#jobsjob_idruns-on
- https://docs.github.com/en/actions/reference/virtual-environments-for-github-hosted-runners
- https://docs.github.com/en/actions/reference/software-installed-on-github-hosted-runners

Git tags auto pushing:
- git config --global push.followTags true