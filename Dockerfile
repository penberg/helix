FROM fedora:22

MAINTAINER Pekka Enberg <penberg@iki.fi>

RUN yum -y update && yum clean all

RUN yum -y install boost-devel gcc gcc-c++ libuv-devel git cmake make ncurses-devel
