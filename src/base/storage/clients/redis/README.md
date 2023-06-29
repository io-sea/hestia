# Redis Integration Notes

Redis support is enabled by default, but only tested when Hestia is compiled with `-DHESTIA_TEST_REDIS=ON`.

Redis support is provided via the hiredis API, if not installed this will be fetched at compile-time.

Setting up server:

You may need to edit the example config`test/data/configs/hestia_redis_tests.yaml` to include your server's IP address. 
This setting is found under `key_value_store_clients > backend_address:`, with default value localhost. 

For testing purposes, this config __will flush all entries__ from the redis database when run. To change this, set `flush_on_start: false`.

## Mac

```bash
brew install redis
redis-server
```

## Rockylinux

```bash
yum install -y redis
redis-server
```