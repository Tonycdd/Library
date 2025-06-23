#include "Factory.h"
#include "Book.h"
#include "Series.h"
#include "Periodicals.h"
#include "Reader.h"
#include "Administrator.h"

LibraryPerson* LibraryFactory::createPersonFromStream(std::istream& in,const TypeOfReader& type)
{
    if (!in.good()) throw std::ios_base::failure("Bad stream for person creating");
    switch (type)
    {
    case TypeOfReader::READER:
        return new Reader(in);
    case TypeOfReader::ADMINISTRATOR:
        return new Administrator(in);
    default:
        throw std::runtime_error("Unknown User type!");
    }
}

LibraryUnit* LibraryFactory::createUnitFromStream(std::istream& in,const Type& type)
{
    if (!in.good()) throw std::ios_base::failure("Bad stream for unit creation");

    LibraryUnit* unit = nullptr;
    switch (type) {
    case Type::BOOK:
        unit = new Book();        
        break;
    case Type::PERIODICALS:
        unit = new Periodicals();
        break;
    case Type::SERIES:
        unit = new Series();
        break;
    }
    if (unit) {
        unit->deserialize(in);       
    }
    return unit;
}

LibraryUnit* LibraryFactory::createUnitInteractively(const Type& type)
{
    switch (type)
    {
    case Type::BOOK:
        return Book::createInteractively();  // статичен метод
    case Type::SERIES:
        return Series::createInteractively();
    case Type::PERIODICALS:
        return Periodicals::createInteractively();
    default:
        throw std::runtime_error("Unknown LibraryUnit type!");
    }
}

LibraryPerson* LibraryFactory::createPersonInteractively(const TypeOfReader& type)
{
    switch (type)
    {
    case TypeOfReader::READER:
        return Reader::createInteractively();  // статичен метод
    case TypeOfReader::ADMINISTRATOR:
        return Administrator::createInteractively();
    default:
        throw std::runtime_error("Unknown LibraryUnit type!");
    }
}
