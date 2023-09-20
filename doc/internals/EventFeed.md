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

All events have `id` and `time` tags. The `id` is a unique identifier string for the storage object. The `time` is the unix time the event was created at (which should be close to the resource modification time) as an unsigned integer.

`create` and `update` events have an `attrs` field which is a Map type. Top-level keys are SYSTEM keys. `update` events have `user_metadata.` prefixed sub-keys of the `attr` map, these are USER provided object metadata.

Relevant SYSTEM attributes are `dataset.id` referring to the object's dataset and `name`, which is an optional human-friendly name for the object.

When data is added, copied or removed between storage tiers this will be reflected in object SYSTEM metadata. The `tier_x` keys will contain a snapshot of the extents of the object on each tier, with tier id 'x'.

```yaml
---
# An object is created - this one wasn't given a 'name'
!create
attrs:
  dataset.id: "9e91a60f-0bae-f0eb-b8d8-874ea0f89a9c"
time: 1695205250318580
id: "1234"
...
---
# An object is removed
!remove
time: 1695205250323688
id: "1234"
...
---
!create
# Another object, with the now free'd id is created
attrs:
  dataset.id: "9e91a60f-0bae-f0eb-b8d8-874ea0f89a9c"
time: 1695205250328623
id: "1234"
...
---
!update
# A user has added custom metadata to the object, e.g. 'my_key0 = my_value0'
attrs:
  user_metadata.my_key0: "my_value0"
  user_metadata.my_key1: "my_value1"
time: 1695205250331593
id: "1234"
...
---
!read
# The object data or USER metadata has been accessed
time: 1695205250333054
id: "1234"
...
---
!update
# 44 bytes of data have been added to tier '0' at offset 0
attrs:
  tier_0: "0,44"
time: 1695205250341016
id: "1234"
...
---
!read
# The object data or USER metadata has been accessed
time: 1695205250350855
id: "1234"
...
---
!update
# 44 bytes of data have been added or copied to tier '1' at offset 0
attrs:
  tier_1: "0,44"
  tier_0: "0,44"
time: 1695205250359687
id: "1234"
...
---
!update
# 44 bytes of data have been added or copied to tier '2' at offset 0
attrs:
  tier_1: "0,44"
  tier_0: "0,44"
  tier_2: "0,44"
time: 1695205250374210
id: "1234"
...
---
!update
# 44 bytes of data have been removed from tier '1' - it is now empty (reminder - this is a snapshot of the current tier state)
attrs:
  tier_0: "0,44"
  tier_2: "0,44"
time: 1695205250378590
id: "1234"
...
---
!update
# 44 bytes of data have been removed from tier '0' - it is now empty
attrs:
  tier_2: "0,44"
time: 1695205250393593
id: "1234"
...
```


