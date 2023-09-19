import yaml

class BaseEvent:
    def __init__(self, id, time) -> None:
        self.id = id
        self.time = time

class CreateEvent(BaseEvent):
    def __init__(self, id, time, attrs) -> None:
        super().__init__(id, time)
        self.tag = "create"
        self.attrs = attrs

    def __repr__(self):
        return "%s(id=%r, time=%r, attrs=%s)" % (
            self.__class__.__name__, self.id, self.time, self.attrs)

class UpdateEvent(BaseEvent):
    def __init__(self, id, time, attrs) -> None:
        super().__init__(id, time)
        self.tag = "update"
        self.attrs = attrs

    def __repr__(self):
        return "%s(id=%r, time=%r, attrs=%s)" % (
            self.__class__.__name__, self.id, self.time, self.attrs)

class RemoveEvent(BaseEvent):
    def __init__(self, id, time) -> None:
        super().__init__(id, time)
        self.tag = "remove"

    def __repr__(self):
        return "%s(id=%r, time=%r)" % (
            self.__class__.__name__, self.id, self.time)

class ReadEvent(BaseEvent):
    def __init__(self, id, time) -> None:
        super().__init__(id, time)
        self.tag = "read"

    def __repr__(self):
        return "%s(id=%r, time=%r)" % (
            self.__class__.__name__, self.id, self.time)

def load_create_event(loader, node):
    id = None
    time = None
    attrs = {} 
    for key, value in node.value:
        if key.value == "id":
            id = value.value
        elif key.value == "time":
            time = value.value
        elif key.value == "attrs":
            for attr_key, attr_value in value.value:
                attrs[attr_key.value] = attr_value.value
    return CreateEvent(id, time, attrs)

def load_update_event(loader, node):
    id = None
    time = None
    attrs = {} 
    for key, value in node.value:
        if key.value == "id":
            id = value.value
        elif key.value == "time":
            time = value.value
        elif key.value == "attrs":
            for attr_key, attr_value in value.value:
                attrs[attr_key.value] = attr_value.value
    return UpdateEvent(id, time, attrs)

def load_remove_event(loader, node):
    id = None
    time = None
    for key, value in node.value:
        if key.value == "id":
            id = value.value
        elif key.value == "time":
            time = value.value
    return RemoveEvent(id, time)

def load_read_event(loader, node):
    id = None
    time = None
    for key, value in node.value:
        if key.value == "id":
            id = value.value
        elif key.value == "time":
            time = value.value
    return ReadEvent(id, time)

class EventFeedLoader(yaml.SafeLoader):
    pass 
EventFeedLoader.add_constructor("!create", load_create_event)
EventFeedLoader.add_constructor("!update", load_update_event)
EventFeedLoader.add_constructor("!remove", load_remove_event)
EventFeedLoader.add_constructor("!read", load_read_event)

def load_file(path):
    with open(path, 'r') as f:
        documents = yaml.load_all(f, Loader=EventFeedLoader)
        for document in documents:
            print(document)

def load_last_event(path):
    with open(path, 'r') as f:
        documents = yaml.load_all(f, Loader=EventFeedLoader)
        *_, last_document = documents
        return last_document