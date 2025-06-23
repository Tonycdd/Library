#include "Reader.h"
#include "Factory.h"

Reader::Reader(const std::string& name,
    const std::string& pass,
    int day1, int month1, int year1,
    int day2, int month2, int year2,
    const std::vector<unsigned int>& v,
    const std::vector<LoanInfo>& history)
    :LibraryPerson(name, pass, day1, month1, year1, day2, month2, year2)
{
    units = v;
    this->history = history;
}

Reader::Reader(const Reader& other)
    :LibraryPerson(other), units(other.units), // (shallow copy)
    history(other.history) 
{

}

Reader& Reader::operator=(const Reader& other)
{
    // copy and swap
    if (this != &other) {
        Reader cpy(other);
        *this = std::move(cpy); // move operator= 
    }
    return *this;
}

Reader::Reader(Reader&& other) noexcept
    :LibraryPerson(std::move(other)), units(std::move(other.units))
{
   // няма какво повече да се прави тук
    std::swap(history, other.history);
}

Reader& Reader::operator=(Reader&& other) noexcept
{
    if (this != &other) {
        LibraryPerson::operator=(std::move(other));
        units = std::move(other.units);
        std::swap(history, other.history);
    }
    return *this;
}

Reader::~Reader() noexcept
{
    // не притежава ресурси
    units.clear();
    history.clear();
}

LibraryPerson* Reader::clone() const
{
    return new Reader(*this);
}

void Reader::display(std::ostream& out) const
{
    LibraryPerson::display(out);
}

void Reader::addNewUnit(LibraryUnit* unit)
{
    if (!unit) {
        std::cout << "Unit is nullptr!" << "\n";
        return; // не добавяме
    }

    for (size_t i = 0; i < units.size(); i++)
    {
        if (units[i] == unit->getId()) {
            std::cout << "This unit was already added!";
            return;
        }
    }

    units.push_back(unit->getId());

    // Добавяне на нов запис в историята с настоящата дата и връщане 14 дни след това
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);
    Date borrowDate(localTime.tm_mday, localTime.tm_mon + 1, localTime.tm_year + 1900);

    Date returnDate = borrowDate;
    returnDate.addDays(14);

    history.emplace_back(unit->getId(), borrowDate, returnDate, false);
}

void Reader::serialize(std::ostream& out) const
{
    TypeOfReader t = getType(); // READER
    out.write(reinterpret_cast<const char*>(&t), sizeof(t));
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing type {READER}!");
    }

    LibraryPerson::serializeBase(out);
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing base part of reader!");
    }
    Reader::serializeReaderUnit(out);
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing Reader part!");
    }
}

void Reader::deserialize(std::istream& is)
{
    // аналогично искаме strong excpetion
    TypeOfReader t;
    is.read(reinterpret_cast<char*>(&t), sizeof(t));
    if (!is) {
        throw std::invalid_argument("Invalid type of user in reader::deserialize!");
    }

    if (!is.good()) {
        throw std::invalid_argument("Invalid stream before reading in Reader!");
    }
    Reader r;
    r.deserializeBase(is);
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading base part of Reader!");
    }
    r.deserializeReaderUnit(is);
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading Reader part!");
    }
    *this = std::move(r);
}

bool Reader::hasOverdueBooks(const Date& today) const {
    for (size_t i = 0; i < history.size(); i++) {
        if (!history[i].returned && history[i].returnDate < today) {
            return true;
        }
    }
    return false;
}

unsigned int Reader::borrowedLastMonth(const Date& today) const
{
    unsigned int count = 0;

    int currentMonth = today.getMonth();
    int currentYear = today.getYear();

    int lastMonth, yearOfLastMonth;

    if (currentMonth == 1) {
        lastMonth = 12;
        yearOfLastMonth = currentYear - 1;
    }
    else {
        lastMonth = currentMonth - 1;
        yearOfLastMonth = currentYear;
    }

    for (size_t i = 0; i < history.size(); i++)
    {
        int borrowMonth = history[i].borrowDate.getMonth();
        int borrowYear = history[i].borrowDate.getYear();

        if (borrowMonth == lastMonth && borrowYear == yearOfLastMonth) {
            count++;
        }
    }

    return count;
}

int Reader::monthsSinceLastBorrow(const Date& today) const {
    if (history.empty()) {
        return -1; // няма заемания
    }

    Date lastBorrowDate = history[0].borrowDate;
    for (size_t i = 0; i < history.size(); i++) {
        if (history[i].borrowDate > lastBorrowDate)
        {
            lastBorrowDate = history[i].borrowDate;
        }
    }

    int yearDiff = today.getYear() - lastBorrowDate.getYear();
    int monthDiff = today.getMonth() - lastBorrowDate.getMonth();

    return yearDiff * 12 + monthDiff;
}


bool Reader::hasBorrowedUnit(unsigned int unitId)const
{
    for (size_t i = 0; i < history.size(); i++)
    {
        if (history[i].unitId == unitId && history[i].isReturned() == false) {
            return true;
        }
    }
    return false;
}

Reader* Reader::createInteractively()
{
    std::string name, password;
    int d1, m1, y1, d2, m2, y2;

    std::cout << "Enter username (or 'cancel' to abort): ";
    std::getline(std::cin, name);
    if (name == "cancel") return nullptr;

    std::cout << "Enter password (or 'cancel' to abort): ";
    std::getline(std::cin, password);
    if (password == "cancel") return nullptr;

    std::cout << "Enter registration date (day month year): ";
    if (!(std::cin >> d1 >> m1 >> y1)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cerr << "Invalid registration date input. Aborting.\n";
        return nullptr;
    }

    std::cout << "Enter last login date (day month year): ";
    if (!(std::cin >> d2 >> m2 >> y2)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cerr << "Invalid login date input. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore(); 

    return new Reader(name, password, d1, m1, y1, d2, m2, y2);
}

std::vector<int> Reader::getTakenIds() const
{
   
    std::vector<int> taken;
    taken.reserve(units.size());

    for (size_t i = 0; i < units.size(); i++)
    {
        taken.push_back(units[i]);
    }
    return taken;
}

void Reader::borrow(LibraryUnit* unit) {
    if (!unit) {
        throw std::invalid_argument("Null pointer passed to borrow!.");
    }

    // Добавяме в текущите заети единици

    units.push_back(unit->getId());

    // Взимаме текущата дата
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);
    Date borrowDate(localTime.tm_mday, localTime.tm_mon + 1, localTime.tm_year + 1900);

    // Дата на връщане след 14 дни
    Date returnDate = borrowDate;
    returnDate.addDays(14);

    // Добавяме в историята
    history.emplace_back(unit->getId(), borrowDate, returnDate, false); // създава обекта
}

bool Reader::returnUnit(int id) {
    bool found = false;

    for (size_t i = 0; i < units.size(); ++i) {
        if (units[i] == id) {

            // търсим съответния запис в history
            for (size_t j = 0; j < history.size(); j++) {
                if (history[j].unitId == id && !history[j].returned) {

                    history[j].returned = true; // маркираме като върната и свободна
                    break;
                }
            }

            // премахваме от списъка с активни заеми
            std::swap(units[i], units.back());
            units.pop_back(); // не вика dtor, не притежаваме самите книги, а само ги ползваме
            
            found = true;
            break;
        }
    }

    
    return found;
}

Reader::Reader() :LibraryPerson(), units(), history()
{
}

void Reader::serializeReaderUnit(std::ostream& out) const
{
   
    // сериализираме само неговите части 
    
    // units -> които държи
    size_t size = units.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing size of units in ReaderUnit!");
    }
    size_t sizeOfUnits = size;
    for (size_t i = 0; i < sizeOfUnits; i++)
    {
        out.write(reinterpret_cast<const char*>(&units[i]),sizeof(unsigned int)); 
        if (!out) {
            throw std::ios_base::failure("Error with file!");
        }
    }

    // history
    size = history.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing size of history in ReaderUnit!");
    }
    for (size_t i = 0; i < size; i++)
    {
        history[i].serialize(out);
    }
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing history in ReaderUnit!");
    }
}

void Reader::deserializeReaderUnit(std::istream& is)
{
    if (!is.good()) {
        throw std::invalid_argument("Error with stream before reading in ReaderUnit");
    }

    // units
    size_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading size of units in ReaderUnit!");
    }


    // временно хранилище за units
    std::vector<unsigned int> tempUnits;
    tempUnits.reserve(size);
    std::vector<LoanInfo> tempHistory;

    for (size_t i = 0; i < size; ++i){
        unsigned int id;
        is.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
        if (!is) {
            throw std::ios_base::failure("Error with file!");
        }
        tempUnits.push_back(id);
    }


    // history
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!is.good()) throw std::ios_base::failure("Failed reading history size!");

    size_t historySize = size;
      

      tempHistory.reserve(size);
      for (size_t i = 0; i < historySize; ++i) {
          LoanInfo info;
          info.deserialize(is);
          tempHistory.push_back(info);
      }

    // вс е било успешно
    // изчистваме старитe
    
    units = std::move(tempUnits);
    history = std::move(tempHistory);
}

std::ostream& operator<<(std::ostream& out, const Reader& other)
{
    other.display(out);
    return out;
}
