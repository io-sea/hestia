# Event Feed

Hestia will optinally log resource-altering events to a YAML file specified by `event_feed_path` in the hestia config as shown below. 

```yaml
event_feed:
  path: event_feed.yaml
  active: y
```

## Hsm Event Sink

The Event Feed can output to multiple sinks. The `HsmEventSink` is used for the IO-SEA project, it outputs CRUD events related to HSM storage objects only.

A sample feed output is shown below (with manually added comments) using the HSM Event Sink. Each event is a separate YAML document, with tags corresponding to CRUD actions. Tags are one of [`create`, `read`, `update`, `remove`]. 

All events have `id` and `time` tags. The `id` is a unique identifier string for the storage object. The `time` is the unix time corresponding to the event (e.g. creation time for 'create' event, last accessed time for 'read' event).

`create` and `update` events have an `attrs` field which is a Map type. User supplied attributes are in the  `user_metadata.` sub-map of the `attr` map.

When data is added, copied or removed between storage tiers this will be reflected in object SYSTEM metadata. The `tiers` map will contain a snapshot of the extents of the object on each tier.

```yaml
---
!create
# The object has been created - the 'time' is the creation time of the resource. No user metadata was 
# given at creation time and no data has beena added yet.
attrs:
  user_metadata:
    {}
  size: 0
  tiers:
    []
time: 1701418948885961
id: "d198c172-35ff-d962-a3db-027cdcf9116c"
...
---
!update
# The user has added some metadata, 'my_key=my_value'. The 'time' is the resource's 'content' last modified time.
attrs:
  user_metadata:
    my_key: "my_value"
  size: 0
  tiers:
    []
time: 1701418995620744
id: "d198c172-35ff-d962-a3db-027cdcf9116c"
...
---
!update
# The user has added some data on tier with index(priority) 0. The 'time' is the resource's 'content' last modified time.
attrs:
  user_metadata:
    my_key: "my_value"
  size: 45333
  tiers:
    - extents:
        - length: 45333
          offset: 0
      index: 0
time: 1701419050925667
id: "d198c172-35ff-d962-a3db-027cdcf9116c"
...
---
!read
# The user has read some data - the 'time' is the resource's 'content' last accessed time.
time: 1701419080139292
id: "d198c172-35ff-d962-a3db-027cdcf9116c"
...
---
!update
# The user has released some data, there is none remaining in the object. The 'time' is the resource's 'content' last modified time.
attrs:
  user_metadata:
    my_key: "my_value"
  size: 0
  tiers:
    []
time: 1701419100896048
id: "d198c172-35ff-d962-a3db-027cdcf9116c"
...
---
# The user has removed the object entirely - the 'time' is the removal time.
!remove
time: 1701419111662100
id: "d198c172-35ff-d962-a3db-027cdcf9116c"
...

```


