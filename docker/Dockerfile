ARG BUILDER_IMAGE=python:3.9-slim-bullseye
FROM ${BUILDER_IMAGE} as builder

USER root
WORKDIR /

SHELL [ "/bin/bash", "-c" ]

COPY . /root/madanalysis5

# Set PATH to pickup virtualenv by default
ENV PATH=/usr/local/venv/bin:"${PATH}"
RUN apt-get -qq -y update && \
    apt-get -qq -y install --no-install-recommends \
      gcc \
      g++ \
      make \
      zlib1g \
      bash-completion \
      python3-dev \
      less \
      tree \
      wget \
      curl \
      gnuplot \
      git && \
    apt-get -y clean && \
    apt-get -y autoremove && \
    rm -rf /var/lib/apt/lists/* && \
    python -m venv /usr/local/venv && \
    . /usr/local/venv/bin/activate && \
    python -m pip --no-cache-dir install --upgrade pip setuptools wheel && \
    python -m pip --no-cache-dir install pip-tools && \
    python -m pip list && \
    python -m piptools compile \
        --generate-hashes \
        --output-file /root/madanalysis5/requirements.lock \
        /root/madanalysis5/docker/requirements.txt && \
    python -m pip --no-cache-dir install --upgrade --requirement /root/madanalysis5/requirements.lock && \
    python -m pip list && \
    export PATH="$(find / -type d -iname madanalysis5)/bin:${PATH}" && \
    python -c 'import multiprocessing; print(multiprocessing.cpu_count())' | ma5 && \
    printf '\nexport PATH=/usr/local/venv/bin:"${PATH}"\n' >> /root/.bashrc && \
    printf '\nexport PATH='"$(find / -type d -iname madanalysis5)/bin"':"${PATH}"\n' >> /root/.bashrc

# Enable tab completion by uncommenting it from /etc/bash.bashrc
# The relevant lines are those below the phrase "enable bash completion in interactive shells"
RUN export SED_RANGE="$(($(sed -n '\|enable bash completion in interactive shells|=' /etc/bash.bashrc)+1)),$(($(sed -n '\|enable bash completion in interactive shells|=' /etc/bash.bashrc)+7))" && \
    sed -i -e "${SED_RANGE}"' s/^#//' /etc/bash.bashrc && \
    unset SED_RANGE

# Use C.UTF-8 locale to avoid issues with ASCII encoding
ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

# Default user is root to avoid uid write permission problems with volumes
ENV HOME /root
WORKDIR ${HOME}/data

ENV PATH="${HOME}/.local/bin:${PATH}"
ENV PATH="/root/madanalysis5/bin:${PATH}"

ENTRYPOINT ["/bin/bash", "-l", "-c"]
CMD ["/bin/bash"]
