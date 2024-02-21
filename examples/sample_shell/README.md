This example shows how the `curl` HTTP client can be used to put and get objects from Hestia's `http` interface.

It requires the following packages:

```sh
dnf install -y curl jq # jq is a json parser that runs in the shell
```

(available in brew on Mac also).

# Start a server

You can follow the steps in the [sample_s3_server](/examples/sample_s3_server/README.md) demo to start a Hestia server, which covers options including using a Docker container, pre-built binaries, or building from source.

# Run the script

You can run the script with:

```sh
./hestia_sample.sh
```

which will create an object, upload a temporary file to it, download the file and then remove the object.

