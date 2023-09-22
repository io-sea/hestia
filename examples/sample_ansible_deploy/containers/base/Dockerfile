FROM rockylinux:8

RUN dnf check-update; dnf update -y; dnf install -y python3 sudo;

RUN useradd -ms /bin/bash ansible_user && usermod -aG wheel ansible_user && echo "ansible_user ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Launch systemd
# CMD [ "/sbin/init" ]