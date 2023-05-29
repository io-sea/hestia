# syntax=docker/dockerfile:1
FROM hestia/ci-base:latest
LABEL description="Hestia rockylinux:8 CI build environment with phobos"

RUN yum install -y $(cat /deps/phobos); 

# Install phobos