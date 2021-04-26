# BSODConfigure
A kernel level driver for Windows built to configure the Blue Screen Of Death
Go see the writeup at https://www.phasetw0.com/configuring_windows_10_bsod

# Disclaimer
This was tested on Windows 10 x64 20H2, using UEFI.
This is super heavy proof of concept, so if it stops working at some point in the future and/or past, sucks to suck

When testing, make sure to to enable testing using `bcdedit /set testsigning on`.
