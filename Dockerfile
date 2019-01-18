FROM gcc:8.2

# Build V8.
RUN git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
ENV PATH="${PATH}:/depot_tools"
RUN gclient
WORKDIR /v8
RUN fetch v8
WORKDIR v8
RUN git fetch
RUN gclient sync
RUN tools/dev/v8gen.py x64.release.sample
RUN ninja -C out.gn/x64.release.sample v8_monolith

# Install build dependencies.
RUN apt -y update
RUN apt -y install python-pip
RUN pip install cmake

# Build the project.
WORKDIR /wasm-sandbox
COPY ./src /wasm-sandbox/src
COPY ./lib /wasm-sandbox/lib
COPY ./CMakeLists.txt /wasm-sandbox
WORKDIR build
RUN cmake .. \
    -DV8_INCLUDE_DIR=/v8/v8/include \
    -DV8_MONOLITH_LIB_PATH=/v8/v8/out.gn/x64.release.sample/obj/libv8_monolith.a
RUN make

ENTRYPOINT ["./wasm-sandbox"]
