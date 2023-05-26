# Event Feed Format Specification

The following events are logged by Hestia in librobinhood-compatible YAML.
Only events which cause a change to the filesystem are logged. Robinhood
should be aware of read-only events.

*Limitation: There is no clear way of specifiying the tier of objects in librobinhood outside of xattrs, so at the moment this is encoded in the ID.*

## Put

A Hestia put event will be logged as a Robinhood "upsert". See below for the object
information logged currently.

```yaml
---
!upsert
id: !!binary # Object ID + Target Tier base64 encoded
statx:
  attributes:
    append: !!bool n
    automount: !!bool n
    compressed: !!bool n
    dax: !!bool n
    encrypted: !!bool n
    immutable: !!bool n
    mount-root: !!bool n
    nodump: !!bool n
    verity: !!bool n
  mode: 0777
  size: !uint64 44
  type: !!str file
xattrs:
  tier: !uint32 0
```

## Remove (Release)

A Hestia remove(aka release) event will be logged as a Robinhood "delete". See below
for the object information logged currently.

```yaml
---
!delete
id: !!binary # Object ID + Target Tier base64 encoded
...
```

## Remove All

A Hestia "remove all" event will be logged as several Robinhood "delete" events. See
below for the object information logged currently.

```yaml
---
!delete
id: !!binary # Object ID + Target Tier base64 encoded
... # Repeat for each tier
```

## Copy

A Hestia copy event will be logged as a Robinhood "upsert". See below for the object
information logged currently.

```yaml
---
!upsert
id: !!binary # Object ID + Target Tier base64 encoded
statx:
  attributes:
    append: !!bool n
    automount: !!bool n
    compressed: !!bool n
    dax: !!bool n
    encrypted: !!bool n
    immutable: !!bool n
    mount-root: !!bool n
    nodump: !!bool n
    verity: !!bool n
  mode: 0777
  size: !uint64 44
  type: !!str file
xattrs:
  source_tier: !uint32 0
  target_tier: !uint32 1
...
```

## Move

A Hestia move event will be logged as a Robinhood "delete" and "upsert". See below for
the object information logged currently.

```yaml
---
!delete
id: !!binary # Object ID + Source Tier base64 encoded
...
---
!upsert
id: !!binary # Object ID + Target Tier base64 encoded
statx:
  attributes:
    append: !!bool n
    automount: !!bool n
    compressed: !!bool n
    dax: !!bool n
    encrypted: !!bool n
    immutable: !!bool n
    mount-root: !!bool n
    nodump: !!bool n
    verity: !!bool n
  mode: 0777
  size: !uint64 44
  type: !!str file
xattrs:
  tier: !uint32 0
...
```

# Robinhood YAML Specification

As a reference, this is a complete specification of the YAML layout librobinhood supports.
Note, many type-specifier tags are implicitly detected based on the field name, but will always be used
by Hestia to avoid any ambiguity.

```yaml
# All fsevents supported by rbh-fsevents - https://github.com/io-sea/rbh-fsevents/ (and by extension, librobinhood)
--- !upsert # Upsert event
id: !!binary VGhpcyBTdHJpbmcgaXMgaW4gYmFzZTY0 # This is in base64, binary tag implicit
xattrs: # Parsing broken in rbh-fsevents (only for the upsert)
  tier: !uint32 0 # Field defining hestia tier
  test: ~ # Empty field
  test: !!bool y
  test: !!binary VGhpcyBTdHJpbmcgaXMgaW4gYmFzZTY0
  test: !uint32 4294967295 # There is an inconsistency in how they define tags
  test: !uint64 18446744073709551615
  test: !int64 -9223372036854775807
  test: !int32 -2147483647
  test: !regex
    regex: !!str "*" # There is no format validation in rbh-fsevents at this time
    options: 4294967295 # These are uint32 but the tag breaks parsing
  test: !!str string
  test: # In rbh-fsevents only string seq entries work (no maps)
    - test:
      - test: test
        test: test
  test:
    test
    test
  # Nested seqs work, but nested maps without kv pairs do not (in rbh-fsevents repo no nested maps work)
symlink: !!str test #
statx: # These are just the output of POSIX stat but to clarify
  type: !!str file # blockdev,chardev,directory,fifo,file,link,socket
  mode: 0777 # Octal unsigned integer, no tag allowed
  attributes:
    compressed: !!bool n
    immutable: !!bool n
    append: !!bool n
    nodump: !!bool n
    encrypted: !!bool n
    automount: !!bool n
    mount-root: !!bool n
    verity: !!bool n
    dax: !!bool n
  nlink: !uint32 0
  uid: !uint32 0
  gid: !uint32 0
  ino: !uint64 0
  size: !uint64 0
  blocks: !uint64 0
  blksize: !uint32 0
  atime:
    sec: !int64 0
    nsec: !uint32 0
  btime:
    sec: !int64 0
    nsec: !uint32 0
  ctime:
    sec: !int64 0
    nsec: !uint32 0
  mtime:
    sec: !int64 0
    nsec: !uint32 0
  dev:
    major: !uint32 0
    minor: !uint32 0
  rdev:
    major: !uint32 0
    minor: !uint32 0
...
--- !delete # Delete Event
id: !!binary MA==
...
--- !link # Link Event
id: !!binary MA==
xattrs: {} # Full xattrs support as described in upsert
parent: !!binary MA==
name: !!str name
...
--- !unlink # Unlink Event
id: !!binary MA==
parent: !!binary MA==
name: !!str name
...
--- !ns_xattr # Namespace xattr Event
id: !!binary MA==
xattrs: {} # Full xattrs support as described in upsert
parent: !!binary MA==
name: !!str name
...
--- !inode_xattr # Inode xattr Event
id: !!binary MA==
xattrs: {} # Full xattrs support as described in upsert
...
```
