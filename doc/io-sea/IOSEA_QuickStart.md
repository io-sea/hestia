# IO-SEA Quick Start

Thist is a Hestia Quick Start guide with a focus on the needs of the IO-SEA project.

# Hestia on DEEP

Hestia version 1.1 is running on DEEP VisNode01 on port `8080`, which can be reached at '172.16.1.37:8080'. Currently it is just running the `http` interface. You can see the [sample_shell](/examples/sample_shell/README.md) example for how to use the [curl](https://curl.se) Http client in a terminal on DEEP to put or get objects.

It is planned to deploy version 1.2 with an `s3` interface shortly.

# Installing Hestia

## Server Only

### Using a container (Any platform)

The [sample_s3_server](/examples/sample_s3_server/README.md) demo shows how you can pull a [Hestia Docker container](https://hub.docker.com/repository/docker/jagrogan/hestia/general) and launch a combined http/s3 server with it.

You can then use the sample Python script in that demo to interact with the s3 interface.

## Server or Client

### Prebuilt RPMs (RHEL and similar)

Get an RPM from the [Hestia Release Collection](https://git.ichec.ie/io-sea-internal/hestia/-/releases)

Pick the version you would like, e.g. for [1.2.0](https://git.ichec.ie/io-sea-internal/hestia/-/releases/v1.2.0/downloads/binaries/hestia-1.2.0-1.x86_64.rpm)

Install it with:

```bash
sudo dnf install hestia-1.2.0-1.x86_64.rpm
```

### Building from Source (Mac and Linux)

Install a recent C++ compiler, `git`, `make` and `cmake`.

Clone the repo:

```sh
git clone https://git.ichec.ie/io-sea-internal/hestia
```

Build with:

```sh
mkdir build
cd build
cmake $PATH_TO_SOURCE
make
```

To run hestia you can then call `bin/hestia` or add `build/bin` to your `$PATH` and run `hestia`.

# Running a server

You can launch a server with:

```sh
hestia server 
```

which will default launch a `http` server on port `8080`. The CLI output will give the location of the cache, containing logs and by default the object store cache.

You may want to set some configuration, for example the S3 interface needs to be explicitly enabled. There are many example configs [here](/test/data/configs/), to run an S3 server we can do:

```sh
cp /test/data/configs/s3/s3_server.yaml hestia.yaml
hestia server --config hestia.yaml
```

Once running you can follow the the [sample_shell](/examples/sample_shell/README.md) demo for using curl to interact with the http server, or the []() demo for the s3 endpoint with [sample_s3_server](/examples/sample_s3_server/README.md). See below for info on using the deficated Hestia Python and C clients for the http endpoint. There is also a CLI client.

# Using the client

## Python

See the [sample_python_app](/examples/sample_python_app/README.md) for examples of using the Python client with Hestia.

## C Client

The IO-SEA C-API is in the file `hestia_iosea.h`. This exclusively uses the lower-level C-API in `hestia.h`, but more closely follows IO-SEA semantics and is easier to use.

There is an example of using it [here](/examples/sample_cmake_app/main_iosea.c).

The [integrations](/src/integrations/README.md) directory has several tools relevant to IO-SEA built on the high-level C-API, they are a good starting point for how to use it in real cases.

The Event Feed is another element of the IO-SEA API - it is documented [here](https://git.ichec.ie/io-sea-internal/hestia/-/blob/devel/doc/internals/EventFeed.md).
