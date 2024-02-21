This example covers running against Hestia's S3 endpoint.

# Start a Hestia S3 Server

## Using A Container

First install a container composer, eg. [Docker compose](https://docs.docker.com/compose/) or [Podman Compose](https://github.com/containers/podman-compose).

Launch the container in the same directory as this file:

```sh
# swap in docker-compose as needed
podman-compose  -f compose.yaml up -d
```

This will launch a webserver on localhost port `8080` and an s3 server on port `8090`. The object-store cache and runtime logs with be in the local `.cache` directory, which has been mounted into the container.

When finished stop the container with:

```sh
podman-compose -f compose.yaml down -v
```

If you have issues installing the composer you can run the container as normal with `docker` or `podman`, using the `compose.yaml` file as a reference for required ports and mounts.

## Using a Local Build

Launch the server with:

```sh
cp $HESTIA_SRC_DIR/test/data/configs/s3/s3_server.yaml hestia.yaml
hestia server --config hestia.yaml
```

The CLI output will give the cache location. You can modify the ports as you like in the config file.

# Run the sample script

Create a Python virtual environment:

```sh
python -m venv --prompt hestia_demos .venv
```

Install the dependencies:

```sh
pip install -r requirements.txt
```

Run the example:

```sh
python sample_s3_client.py
```
