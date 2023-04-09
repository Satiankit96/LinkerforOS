"""
Apple store should notify about new releases.
Saim should get notified only of new iPhone released and Emma should get notified of only Mac releases.

Terminologies used from https://www.geeksforgeeks.org/observer-pattern-set-2-implementation/
"""

# Subject - AppleStore
#
# Observer - Saim and Emma
#
from abc import ABC, abstractmethod
from enum import Enum
from weakref import WeakKeyDictionary

class EventType(Enum):
    IPHONE_RELEASE = 0
    MAC_RELEASE    = 1

########## Subject class - we can consider having an interface if there are multiple Observer classes
class AppleStore(ABC):

    observers = WeakKeyDictionary()

    def register(self, observer, event_type):
        if observer not in self.observers:
            self.observers[observer] = set()
        self.observers[observer].add(event_type)

    def unregister(self, observer):
        if observer in self.observers:
            self.observers.pop(observer)

    def notify(self, event_type, msg):
        for observer, observer_event_types in self.observers.items():
            for observer_event_type in observer_event_types:
                if observer_event_type is event_type:
                    observer.update(msg)

########## Define an interface for Observers to implement
class Observer(ABC):
    @abstractmethod
    def update(self, msg):
        pass

class SaimObserver(Observer):
    def update(self, msg):
        print(f"{self.__class__.__name__} got notified: {msg!r}")

class EmmaObserver(Observer):
    def update(self, msg):
        print(f"{self.__class__.__name__} got notified: {msg!r}")

########## Client code
saim = SaimObserver()
emma = EmmaObserver()

subject = AppleStore()
subject.register(saim, EventType.IPHONE_RELEASE)
subject.register(emma, EventType.MAC_RELEASE)

######### Subject notifies all observers
subject.notify(EventType.IPHONE_RELEASE, "New IPhone is released") # only saim should get this notification
subject.notify(EventType.MAC_RELEASE,    "New Mac is released")    # only emma should get this notification

assert len(list(subject.observers.keys())) == 2
# Handle LAPSED LISTENER PROBLEM - Subject shuould hold only weak references of Observers
del emma
assert len(list(subject.observers.keys())) == 1




