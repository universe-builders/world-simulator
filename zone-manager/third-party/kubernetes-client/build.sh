# Clone the repo
git clone https://github.com/kubernetes-client/c
CLIENT_REPO_ROOT=${PWD}/c

# Install pre-requisites
apt-get install libssl-dev libcurl4-openssl-dev uncrustify

# Build pre-requisite: libwebsockets
git clone https://libwebsockets.org/repo/libwebsockets --depth 1 --branch v4.2-stable
cd libwebsockets
mkdir build
cd build
cmake -DLWS_WITHOUT_TESTAPPS=ON -DLWS_WITHOUT_TEST_SERVER=ON-DLWS_WITHOUT_TEST_SERVER_EXTPOLL=ON \
      -DLWS_WITHOUT_TEST_PING=ON -DLWS_WITHOUT_TEST_CLIENT=ON -DCMAKE_C_FLAGS="-fpic" -DCMAKE_INSTALL_PREFIX=/usr/local ..
make
make install

# Build pre-requisite: libyaml
git clone https://github.com/yaml/libyaml --depth 1 --branch release/0.2.5
cd libyaml
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_TESTING=OFF  -DBUILD_SHARED_LIBS=ON ..
make
make install

# Move into the Kubernetes directory
cd ${CLIENT_REPO_ROOT}/kubernetes

# Build
mkdir build
cd build
# If you don't need to debug the C client library:
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
# If you want to use `gdb` to debug the C client library, add `-DCMAKE_BUILD_TYPE=Debug` to the cmake command line, e.g.
# cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr/local ..
make
make install