"""
From https://www.geeksforgeeks.org/abstract-factory-pattern/:

    Let’s take an example, Suppose we want to build a global car factory. If it was factory oo_design pattern, then it was suitable for a single location. But for this pattern, we need multiple locations and some critical oo_design changes.

    We need car factories in each location like IndiaCarFactory, USACarFactory and DefaultCarFactory. Now, our application should be smart enough to identify the location where it is being used, so we should be able to use appropriate car
    factory without even knowing which car factory implementation will be used internally. This also saves us from someone calling wrong factory for a particular location.

    Here we need another layer of abstraction which will identify the location and internally use correct car factory implementation without even giving a single hint to user. This is exactly the problem, which abstract factory pattern is used to solve.
    Somewhat the above example is also based on How the Cabs like uber and ola functions on the large scale.

Client code:
All these builds should be based on location:
    CarFactory.build_car(CarType.MICRO)
    CarFactory.build_car(CarType.MINI)
    CarFactory.build_car(CarType.LUXURY)
"""
from abc import ABC, abstractmethod
from enum import Enum
######## List all possible locations
class Locations(Enum):
    INDIA      = 0
    USA        = 1
    DEFAULT    = 2

######## Implement Car Interface and concrete classes for each car type
class CarTypes(Enum):
    MICRO     = 0
    MINI      = 1
    LUXURY    = 2

class CarInterface(ABC):
    def __init__(self):
        print("Building {}".format(self.__class__))
    @abstractmethod
    def drive(self):
        pass
class MicroCar(CarInterface):
    def drive(self):
        print("Driving {}".format(self.__class__))
class MiniCar(CarInterface):
    def drive(self):
        print("Driving {}".format(self.__class__))
class LuxuryCar(CarInterface):
    def drive(self):
        print("Driving {}".format(self.__class__))

CAR_TYPE_MAP = {
    CarTypes.MICRO     : MicroCar,
    CarTypes.MINI      : MiniCar,
    CarTypes.LUXURY    : LuxuryCar
}

######## Implement CarFactory Interface and concrete classes for each location
class CarFactoryInterface(ABC):
    @staticmethod
    @abstractmethod
    def build_car(type):
        pass
class IndiaCarFactory(CarFactoryInterface):
    @staticmethod
    def build_car(type):
        return CAR_TYPE_MAP[type]()
class USACarFactory(CarFactoryInterface):
    @staticmethod
    def build_car(type):
        return CAR_TYPE_MAP[type]()
class DefaultCarFactory(CarFactoryInterface):
    @staticmethod
    def build_car(type):
        return CAR_TYPE_MAP[type]()

CAR_FACTORY_MAP = {
    Locations.INDIA      : IndiaCarFactory,
    Locations.USA        : USACarFactory,
    Locations.DEFAULT    : DefaultCarFactory
}

#####################################################################################################
# This serves as the abstract car factory. It picks up the appropriate factory based on the location.
#####################################################################################################
class CarFactory(CarFactoryInterface):
    @staticmethod
    def build_car(type):
        CAR_FACTORY_MAP[CURRENT_LOCATION].build_car(type)

# Assume a current location. The Factory passed to client picks up the corresponding Factory based on location.
CURRENT_LOCATION = Locations.INDIA

######## Client code
CarFactory.build_car(CarTypes.MICRO)



