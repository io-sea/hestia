FROM rockylinux:8

RUN dnf check-update; dnf update -y; 
RUN dnf install -y ansible-core;

# Add a user for ansible, give non-password sudoer access
RUN useradd -ms /bin/bash ansible_user && usermod -aG wheel ansible_user && echo "ansible_user ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

COPY --chown=ansible_user examples/sample_ansible_deploy /sample_ansible_deploy
COPY --chown=ansible_user infra/ansible/roles /sample_ansible_deploy/roles

USER ansible_user
ENV ANSIBLE_HOST_KEY_CHECKING "False"
WORKDIR /sample_ansible_deploy

CMD ["/usr/bin/ansible-playbook", "playbook.yml", "-i", "inventory"]