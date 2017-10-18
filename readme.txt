-----------
readme.txt
-----------

Teseo Location demo git repo (ssh://gitolite@codex.cro.st.com/x-nucleodev/mbed/mbedClassic/TeseoLocation.git) 
including the following submodules:

- GPSProvider (ssh://gitolite@codex.cro.st.com/x-nucleodev/mbed/mbedClassic/GPSProvider.git)
- X_NUCLEO_GNSS1A1 (ssh://gitolite@codex.cro.st.com/x-nucleodev/mbed/mbedClassic/X_NUCLEO_GNSS1A1.git)
- mbed-os (tag mbed-os-5.6.2 https://github.com/ARMmbed/mbed-os.git)

Before pulling the github submodule, you have to run the following command export the HTTPS_PROXY env var:
$ export HTTPS_PROXY=username:pwd@appgw.sgp.st.com:8080

Once cloned the repo, run the following commands to pull the submodules:
$ git submodule init
$ git submodule update

Checkout the mbed-os tag for 5.6.2 release:
$ cd mbed-os
$ git checkout tags/mbed-os-5.6.2 -b mbed-os-5.6.2

Reference: http://blog.joncairns.com/2011/10/how-to-use-git-submodules/

