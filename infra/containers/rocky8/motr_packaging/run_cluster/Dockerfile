FROM rockylinux:8
EXPOSE 8600/udp
EXPOSE 8300
EXPOSE 8500
RUN yum install -y epel-release; yum config-manager --set-enabled powertools; yum install -y wget python3 procps-ng systemd httpd sudo
COPY install.sh .
COPY entrypoint.sh /usr/bin/
#ENTRYPOINT ["entrypoint.sh"]
# CMD ["usr/sbin/init"]