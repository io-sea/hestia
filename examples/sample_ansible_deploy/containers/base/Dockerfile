FROM rockylinux:8

RUN dnf check-update; dnf update -y;
RUN dnf install -y python3 sudo openssh-server;

# Add a user for ansible, give non-password sudoer access
RUN useradd -ms /bin/bash -p "" ansible_user && usermod -aG wheel ansible_user && echo "ansible_user ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Generate hostkeys and allow empty password ssh access
RUN /usr/bin/ssh-keygen -A && echo "PermitEmptyPasswords yes" >> /etc/ssh/sshd_config && rm /run/nologin

# Non-systemd option (Docker) 
# Launch ssh server to allow ansible connections
CMD ["/usr/sbin/sshd", "-D"]

# Systemd Option (Podman)
# RUN systemctl enable sshd
# CMD [ "/sbin/init" ]