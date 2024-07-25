# pocucandr Dockerfile for c++ dev 
FROM ubuntu:jammy AS ubuntu_cxx_shell_2204

RUN echo "UPPER CXX 22.04 Ubuntu Dev Image"
ENV APT_INSTALL="apt-get -y install"
ENV APT_UPDATE="apt update"

RUN apt-get update -y
RUN $APT_INSTALL curl software-properties-common vim

# Install C++ tools
RUN $APT_INSTALL build-essential=12.9ubuntu3
RUN $APT_INSTALL cppcheck=2.7-1
RUN $APT_INSTALL cmake=3.22.1-1ubuntu1.22.04.1
RUN $APT_INSTALL apt-transport-https curl gnupg -y
RUN $APT_INSTALL libopenblas-openmp-dev
RUN $APT_UPDATE 

# Install dependencies and add the deadsnakes PPA
RUN apt-get update && apt-get install -y \
    software-properties-common \
    && add-apt-repository ppa:deadsnakes/ppa \
    && apt-get update

# Install Python 3.11
RUN apt-get install -y python3.11 python3.11-venv python3.11-dev

# Verify the installation
RUN python3.11 --version

# Install pip for Python 3.11
RUN curl -sS https://bootstrap.pypa.io/get-pip.py | python3.11

# Ensure pip3.11 is available in /usr/bin
RUN ln -s /usr/bin/pip3.11 /usr/bin/pip3
RUN ln -s /usr/bin/python3.11 /usr/bin/python

RUN pip3 install pybind11[global]

# Set Python 3.11 as the default
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.11 1

RUN mkdir -p /home/app_user/app

WORKDIR /home/app_user/app/


RUN echo 'export PS1="\[$(tput setaf 165)\](ubuntu-jammy)\[$(tput setaf 171)\] \[$(tput setaf 219)\]\w\[$(tput sgr0)\] $: "' >> ~/.bashrc

ENV APP_DIR="/home/app_user/app/"
WORKDIR ${APP_DIR}
