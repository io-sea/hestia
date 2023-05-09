FROM rockylinux:8
RUN yum install -y epel-release; yum config-manager --set-enabled powertools; \
    yum group install -y "Development Tools"; \
    yum install -y wget python3 sudo cmake catch-devel
COPY install.sh .
