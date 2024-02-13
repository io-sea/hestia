FROM rockylinux:8

RUN yum check-update; yum update -qy; yum install -qy epel-release
RUN yum config-manager -q --set-enabled powertools
RUN yum install -qy ca-certificates cmake doxygen gcc gcc-c++ git glibc-devel glib2-devel graphviz hiredis-devel libcurl-devel jansson-devel
RUN yum install -qy which libasan libubsan make openssl-devel python3 rpm-build wget nano ncurses libxml2-devel
RUN yum install -qy autoconf automake libtool python3-devel libini_config-devel libattr-devel libcmocka-devel sg3_utils-devel protobuf-c-devel openssl-devel libpq-devel postgresql postgresql-contrib postgresql-server xxhash-devel

COPY * /
RUN chmod 755 build_phobos.sh start_phobos.sh phobos_add_dir.sh

RUN ./build_phobos.sh
RUN pip3 install -r phobos_requirements.txt
RUN export PHOBOS_SRC=/phobos;export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64/phobos
#RUN ./start_phobos.sh phobos.conf
#RUN ./phobos_add_dir.sh /home/test-phobos-dir test_phobos_dir




