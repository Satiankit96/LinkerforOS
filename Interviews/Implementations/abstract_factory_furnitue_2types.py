"""
From https://refactoring.guru/oo_design-patterns/abstract-factory:

> Imagine that you’re creating a furniture shop simulator. Your code consists of classes that represent:
> - A family of related products, say: Chair + Sofa + CoffeeTable.
> - Several variants of this family. For example, products Chair + Sofa + CoffeeTable are available in these variants: Modern, Victorian, ArtDeco.


NOTE:
    This implementation differs from From https://www.geeksforgeeks.org/abstract-factory-pattern/, which I like more.
"""
from abc import ABC, abstractmethod
from enum import Enum

class ProductType(Enum):
    Chair          = 0
    Sofa           = 1
    CoffeeTable    = 2

class FurnitureVariant(Enum):
    Modern       = 0
    Victorian    = 1
    ArtDeco      = 2

######## Implement Chair interface and concrete classes
class ChairInterface(ABC):
    @abstractmethod
    def sit(self):
        pass
class Chair(ChairInterface):
    def sit(self):
        print("Sitting on Chair")
class ModernChair(Chair):
    def sit(self):
        print("Sitting on ModernChair")
class VictorianChair(Chair):
    def sit(self):
        print("Sitting on VictorianChair")
class ArtDecoChair(Chair):
    def sit(self):
        print("Sitting on ArtDecoChair")

######## Implement Sofa interface and concrete classes
class SofaInterface(ABC):
    @abstractmethod
    def sit(self):
        pass
class Sofa(SofaInterface):
    def sit(self):
        print("Sitting on Sofa")
class ModernSofa(Sofa):
    def sit(self):
        print("Sitting on ModernSofa")
class VictorianSofa(Sofa):
    def sit(self):
        print("Sitting on VictorianSofa")
class ArtDecoSofa(Sofa):
    def sit(self):
        print("Sitting on ArtDecoSofa")

######## Implement CoffeeTable interface and concrete classes
class CoffeeTableInterface(ABC):
    @abstractmethod
    def sit(self):
        pass
class CoffeeTable(CoffeeTableInterface):
    def sit(self):
        print("Sitting on CoffeeTable")
class ModernCoffeeTable(CoffeeTable):
    def sit(self):
        print("Sitting on ModernCoffeeTable")
class VictorianCoffeeTable(CoffeeTable):
    def sit(self):
        print("Sitting on VictorianCoffeeTable")
class ArtDecoCoffeeTable(CoffeeTable):
    def sit(self):
        print("Sitting on ArtDecoCoffeeTable")

######## Implement Furniture factory interface and concrete factory classes
class FurnitureFactoryInterface(ABC):
    @abstractmethod
    def createChair(self):
        pass
    @abstractmethod
    def createSofa(self):
        pass
    @abstractmethod
    def createCoffeeTable(self):
        pass

class ModernFurnitureFactory(FurnitureFactoryInterface):
    def createChair(self):
        return ModernChair()
    def createSofa(self):
        return ModernSofa()
    def createCoffeeTable(self):
        return ModernCoffeeTable()

class VictorianFurnitureFactory(FurnitureFactoryInterface):
    def createChair(self):
        return VictorianChair()
    def createSofa(self):
        return VictorianSofa()
    def createCoffeeTable(self):
        return VictorianCoffeeTable()

class ArtDecoFurnitureFactory(FurnitureFactoryInterface):
    def createChair(self):
        return ArtDecoChair()
    def createSofa(self):
        return ArtDecoSofa()
    def createCoffeeTable(self):
        return ArtDecoCoffeeTable()

######## Client code
modern_furniture_factory = ModernFurnitureFactory()
modern_furniture_factory.createChair().sit()

victorian_furniture_factory = VictorianFurnitureFactory()
victorian_furniture_factory.createSofa().sit()

artdeco_furnitue_factory = ArtDecoFurnitureFactory()
artdeco_furnitue_factory.createCoffeeTable().sit()






