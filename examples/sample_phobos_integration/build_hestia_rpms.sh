export HESTIA_VERSION=1.3.0
wget https://ocean.eupex.eu/download/3.8/ocean/x86_64/phobos-1.95.1-2.el8.x86_64.rpm
dnf install -y phobos-1.95.1-2.el8.x86_64.rpm
wget https://git.ichec.ie/io-sea-internal/hestia/-/releases/v$HESTIA_VERSION/downloads/binaries/hestia-$HESTIA_VERSION-1.src.rpm
dnf install -y 
rpmbuild --rebuild -with phobos hestia-$HESTIA_VERSION-1.src.rpm