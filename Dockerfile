ARG OPENMRN_URL=https://github.com/evili/openmrn.git
ARG GNUCC_ARM_URL=https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2
ARG FREERTOS_URL="https://github.com/FreeRTOS/FreeRTOS/archive/refs/tags/V10.1.1.zip"
ARG FIRMWARE_PATH=/opt/stm32cubef7
ARG FIRMWARE_VAR=STM32CUBEF7PATH

FROM ubuntu:20.04

ARG OPENMRN_URL
ARG GNUCC_ARM_URL
ARG FREERTOS_URL
ARG FIRMWARE_PATH
ARG FIRMWARE_VAR

ENV DEBIAN_FRONTEND=noninteractive

RUN    apt-get update && \
       apt-get install --no-install-recommends -qq -y \
         tini ca-certificates wget unzip \
         less vim \
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
RUN ln -svi FreeRTOS*/FreeRTOS .
WORKDIR /opt/FreeRTOS
RUN ln -siv . default
ENV FREERTOSPATH /opt/FreeRTOS

COPY docker_entrypoint.sh /
RUN useradd openmrn -m -d /openmrn
WORKDIR /openmrn
ENV OPENMRN_URL=${OPENMRN_URL}
ENV OPENMRNPATH=/openmrn
VOLUME /openmrn
VOLUME /opt/FreeRTOS
VOLUME /opt/armgcc
VOLUME ${FIRMWARE_PATH}
ENV ${FIRMWARE_VAR}=${FIRMWARE_PATH}
USER openmrn

ENTRYPOINT ["/usr/bin/tini", "--",  "/docker_entrypoint.sh"]
