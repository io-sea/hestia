FROM rockylinux:8

RUN dnf check-update; dnf update -y; 
RUN dnf install -y python3 createrepo;

COPY *.rpm /hestia-rpms/
WORKDIR /hestia-rpms
RUN createrepo .

CMD ["/usr/bin/python3", "-m", "http.server", "80"]