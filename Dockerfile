ARG OPENMRN_URL=https://github.com/evili/openmrn.git
ARG GNUCC_ARM_URL=https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2
ARG FREERTOS_URL="https://github.com/FreeRTOS/FreeRTOS-LTS/releases/download/202012.01-LTS/FreeRTOSv202012.01-LTS.zip"
#"https://github-releases.githubusercontent.com/318273159/8de70300-79e9-11eb-957c-bd421d6fba0c?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20210419%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20210419T122528Z&X-Amz-Expires=300&X-Amz-Signature=b3150a8b91892339070522aa256c16cdf049dd49dd8f745316b0c3be13848c7c&X-Amz-SignedHeaders=host&actor_id=2150480&key_id=0&repo_id=318273159&response-content-disposition=attachment%3B%20filename%3DFreeRTOSv202012.01-LTS.zip&response-content-type=application%2Foctet-stream"
FROM ubuntu:20.04

ARG OPENMRN_URL
ARG GNUCC_ARM_URL
ARG FREERTOS_URL

ENV DEBIAN_FRONTEND=noninteractive

RUN    apt-get update && \
       apt-get install --no-install-recommends -qq -y \
         tini ca-certificates wget unzip \
         git doxygen make gcc g++  \
         libavahi-client-dev && \
       update-ca-certificates && \
       apt-get purge && apt-get autoclean && apt-get clean all && \
       rm -rf /var/lib/apt/lists/*

RUN wget -O /tmp/gcc-arm.tar.bz2 ${GNUCC_ARM_URL}
RUN mkdir -pv /opt/armgcc
WORKDIR /opt/armgcc
RUN tar -xf /tmp/gcc-arm.tar.bz2
RUN ln -svi ./gcc-arm-* default
RUN wget -O /tmp/FreeRTOS.zip ${FREERTOS_URL}
WORKDIR /opt
RUN unzip /tmp/FreeRTOS.zip
RUN ln -svi FreeRTOS-LTS/FreeRTOS .
WORKDIR /opt/FreeRTOS
RUN ln -siv . default

COPY docker_entrypoint.sh /
RUN mkdir -pv /openmrn
WORKDIR /openmrn
ENV OPENMRN_URL=${OPENMRN_URL}
ENV OPENMRNPATH=/openmrn
VOLUME /openmrn
VOLUME /opt

ENTRYPOINT ["/usr/bin/tini", "--",  "/docker_entrypoint.sh"]
