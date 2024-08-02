FROM ubuntu:jammy AS ubuntu_cxx_shell_2204

RUN echo "UPPER CXX 22.04 Ubuntu Dev Image"
ENV APT_INSTALL="apt-get -y install"
ENV APT_UPDATE="apt update"
ENV DEBIAN_FRONTEND noninteractive

# Use Bash as the default shell
SHELL ["/bin/bash", "-c"]

# Update and install basic packages
RUN apt-get update -y && apt-get install -y \
    curl software-properties-common vim

# Install C++ tools
RUN $APT_INSTALL build-essential=12.9ubuntu3 cppcheck=2.7-1 cmake protobuf-compiler libopenblas-openmp-dev apt-transport-https gnupg

# Install Python and dependencies
RUN apt-get update && apt-get install -y \
    software-properties-common \
    && add-apt-repository ppa:deadsnakes/ppa \
    && apt-get update

# Install Python 3.10 and pip
RUN apt-get install -y python3 python3-dev python3-pip

# Set Python3.10 as default
RUN ln -s /usr/bin/python3.10 /usr/bin/python

# Install Python packages
RUN pip install pybind11[global] numpy==2.0.1 matplotlib

RUN apt-get update

# Set up the working directory
RUN mkdir -p /home/app_user/app
ENV APP_DIR="/home/app_user/app/"
WORKDIR ${APP_DIR}

# Set a custom prompt in the shell
RUN echo 'export PS1="\[$(tput setaf 165)\](ubuntu-jammy)\[$(tput setaf 171)\] \[$(tput setaf 219)\]\w\[$(tput sgr0)\] $: "' >> ~/.bashrc

# Keep the container running (optional, if needed for debugging)
CMD ["/bin/bash"]
