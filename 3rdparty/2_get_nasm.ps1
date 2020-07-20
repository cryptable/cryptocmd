$url = "http://strawberryperl.com/download/5.30.2.1/strawberry-perl-5.30.2.1-64bit.zip"
$output = "$PSScriptRoot\strawberry-perl.zip"
(New-Object System.Net.WebClient).DownloadFile($url, $output)

$env:Path += ";$PSScriptRoot\nasm"