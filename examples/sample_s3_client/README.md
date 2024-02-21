# Start an S3 server

## Hestia Server

You can follow the [sample_s3_server](/examples/sample_s3_server/) example to start a Hestia server. Alternatively a Minio server is simple to set up - detailed in the next section.

The Hestia S3 client hasn't been tried against Amazon S3 but it may work for simple operations.

## Minio

To run a Minio server do:

```sh
# https://min.io/download#/macos
brew install minio/stable/minio
MINIO_ROOT_USER=admin MINIO_ROOT_PASSWORD=password minio server /mnt/data --console-address ":9001"
```

From there you can log into the admin console, create access tokens, buckets and objects.