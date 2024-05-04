# the goal here is to document the build process on Linux
# so people can setup their dev envs

FROM ubuntu:24.04

RUN apt-get -y update 
RUN apt-get install -y build-essential bison flex libcurses-ocaml-dev libpng-dev libtheora-dev
RUN apt-get install -y libsdl1.2-dev libsdl-mixer1.2-dev libsdl1.2-compat-dev libsdl-ttf2.0-dev 
RUN apt-get install -y gdb

ADD . /src

WORKDIR /src
RUN ./sys/unix/setup.sh sys/unix/hints/linux
RUN make
RUN make install
RUN /root/nh/games/vulture-nethack-3.6.7