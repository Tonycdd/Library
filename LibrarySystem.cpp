#include "LibrarySystem.h"
#include <fstream> // работим с двоични файлове
#include <algorithm> // само за std::sort

const int MAX = 100;

LibrarySystem::LibrarySystem(const std::string& usersFile, const std::string& unitsFile )
    :fileUsers(usersFile),fileUnits(unitsFile)
{
    // някакви валидации за името?
    if (fileUsers.empty()) {
        throw std::invalid_argument("Invalid name!");
    }
    else if (fileUnits.empty()) {
        throw std::invalid_argument("Invalid name!");
    }
    else if (fileUsers == fileUnits) {
        throw std::invalid_argument("Those names are equal!");
    }
    
    std::ifstream file1(usersFile, std::ios::binary);
    
    if (!file1.is_open()) {
        throw std::ios_base::failure("Error with opening fileUsers!");
    }

    std::ifstream file2(unitsFile, std::ios::binary);
    if (!file2.is_open()) {
            file1.close();
            file2.close();
            throw std::ios_base::failure("Error with opening fileUnits!");
        }   
    bool emptyCase = true;
    try
    {
        deserialize(file1,file2);
        if (emptyCase) {
            std::cout << "Creating default administrator!\n";
                // регистрираме по подразбиране един admin user
                Date today = Date(); // днешна дата
                Administrator* admin = new Administrator("admin", "i<3C++", today, today, "admin@email.bg");
                currentPerson = admin; // важно!
                std::cout << "Logged as: 'admin!'\n";
                try
                {
                    users.push_back(admin);  

                }
                catch (...)
                {
                    delete admin;
                    throw;
                }  
        }
    }

    catch (...)
    {
        // тук ще ги затворим, ако нещо се счупи
        file1.close();
        file2.close();
        throw std::runtime_error("Somethign went wrong with deserializing in LibrarySystem!");
    }
    file1.close();
    file2.close();
}


LibrarySystem::LibrarySystem(LibrarySystem&& other) noexcept
    :units(std::move(other.units)),users(std::move(other.users))
    ,infoUnits(std::move(other.infoUnits)), infoUsers(std::move(other.infoUsers)),
    fileUsers(std::move(other.fileUsers)), fileUnits(std::move(other.fileUnits))
{
    currentPerson = other.currentPerson;
    other.currentPerson = nullptr;
}

LibrarySystem& LibrarySystem::operator=(LibrarySystem&& other) noexcept
{
    if (this != &other) {
        std::swap(units, other.units);
        std::swap(users, other.users);
        std::swap(fileUsers, other.fileUsers);
        std::swap(fileUnits, other.fileUnits);
        std::swap(infoUnits, other.infoUnits);
        std::swap(infoUsers, other.infoUsers);
        std::swap(currentPerson, other.currentPerson);
    }
    return *this;
}

LibrarySystem::~LibrarySystem()
{
	// правим си наши копия, затова трябва да ги трием тук

	for (size_t i = 0; i < units.size(); i++)
	{
		delete units[i];
	}
	for (size_t i = 0; i < users.size(); i++)
	{
		delete users[i];
	}
    users.clear();
    units.clear();
}

bool LibrarySystem::login(const std::string& username, const std::string& password)
{
    // Check if a user is already logged in
    if (currentPerson != nullptr) {
        std::cerr << "You are already logged in as " << currentPerson->getUsername() << ". Please logout first.\n";
        return false;
    }

           
        //see if its already loaded?
        for (size_t i = 0; i < users.size(); i++)
        {
            if (users[i]->getUsername() == username && users[i]->getPassword() == password) {
                // тогава е ок
                currentPerson = users[i];
                std::cout << "Welcome, " << username << " !\n";
                return true;
            }
        }
  
        try {
            // Open the file and load the user
            std::ifstream file(fileUsers, std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Cannot open users file!\n";
                return false;
            }

            for (size_t i = 0; i < infoUsers.size(); i++)
            {

                if (infoUsers[i].username == username && infoUsers[i].password == password) {
                    file.seekg(infoUsers[i].pos, std::ios::beg);
                    if (!file.good()) {
                        file.close();
                        std::cerr << "Error seeking to user position in file!\n";
                        return false;
                    }
                    bool valid;
                    file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
                    if (!file) {
                        throw std::ios_base::failure("Error with file!");
                    }
                    if (!valid) {
                        continue;
                    }
                    // Read the user type
                    TypeOfReader type;
                    std::streampos posBeforeType = file.tellg(); // запазваме позицията
                    file.read(reinterpret_cast<char*>(&type), sizeof(type));
                    if (!file.good()) {
                        throw std::ios_base::failure("Error with file!");
                    }

                    // курсора обратно, за да може обектът сам да си прочете Type вътре
                    file.seekg(posBeforeType);
                    if (!file) {
                        throw std::ios_base::failure("Failed to seek back before reading unit!");
                    }

                    // Create the user object (this will read all data including username/password)
                    LibraryPerson* newUser = LibraryFactory::createPersonFromStream(file, type);
                    if (!newUser) {
                        file.close();
                        std::cerr << "Error creating user object from file!\n";
                        return false;
                    }
                    // Seek to the user's position in the file

                    // Update the last login date
                    newUser->lastLoginDate = Date(); // Current date

                    // Close the file and add user to the system
                    file.close();
                    try
                    {
                        users.push_back(newUser);
                    }
                    catch (...)
                    {
                        delete newUser;
                        throw;
                    }
                    currentPerson = newUser;

                    // Display welcome message
                    std::cout << "Welcome, " << username << "!\n";

                    return true;
                }
            }
        }
    
        catch (std::exception& e) 
        {
            std::cerr << "Error during login: " << e.what() << std::endl;
            return false;
        }
    
        std::cerr << "missmatch!\n";
        return false;
}

bool LibrarySystem::logout()
{
    if (!currentPerson) {
        std::cerr << "There is no user currently logged in!\n";
        return false;
    }

    std::string username = currentPerson->getUsername();
    currentPerson = nullptr;
    std::cout << "User " << username << " has been successfully logged out.\n";
    return true;
}

bool LibrarySystem::exit()
{
    std::fstream file1(fileUsers, std::ios::binary| std::ios::in| std::ios::out);
    if (!file1.is_open()) {
        throw std::ios::basic_ios::failure("Error with opening fileUsers!");
    }

    std::fstream file2(fileUnits, std::ios::binary | std::ios::in | std::ios::out);
    if (!file2.is_open()) {
        file1.close();
        throw std::ios::basic_ios::failure("Error with opening fileUnits!");
    }
    try
    {
        serialize(file1, file2);
        IDGenerator::saveLastIdToFile("last_id.dat");
    }
    catch (...)
    {
        file1.close();
        file2.close();
        throw;
    }
}

// тази фукнция търси в мета данните, не зареждаве, ако не ни трябват, бавно защото търсим във файла
void LibrarySystem::findUnits(const std::string& type, 
    const std::string& option, const std::string& value,
    const std::string& sortKey, const std::string& sortOrder,
    int topCount) const
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    std::vector<LibraryUnit*> filtered;
    std::ifstream file(fileUnits, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error with file in findUnits!\n";
        return;
    }

    try
    {
        for (size_t i = 0; i < infoUnits.size(); i++)
        {
            // тип - намираме първо правилния тип от колекцията
            if (type == "books" && infoUnits[i].type != Type::BOOK)
                continue;
            if (type == "series" && infoUnits[i].type != Type::SERIES)
                continue;
            if (type == "newsletters" && infoUnits[i].type != Type::PERIODICALS)
                continue;

            // намерили сме го и го зареждаме
            file.seekg(infoUnits[i].pos, std::ios::beg);
            bool isValid;
            file.read(reinterpret_cast<char*>(&isValid), sizeof(isValid));
            if (!file) {
                throw std::ios_base::failure("Error with file in find users!");
            }

            if (!isValid) {
                continue;
            }

            // Read the user type
            Type type;
            std::streampos posBeforeType = file.tellg(); // запазваме позицията
            file.read(reinterpret_cast<char*>(&type), sizeof(type));
            if (!file.good()) {
                throw std::ios_base::failure("Error with file!");
            }

            // курсора обратно, за да може обектът сам да си прочете Type вътре
            file.seekg(posBeforeType);
            if (!file) {
                throw std::ios_base::failure("Failed to seek back before reading unit!");
            }

            LibraryUnit* current = LibraryFactory::createUnitFromStream(file, infoUnits[i].type);
            if (!current) {
                std::cerr << "Couldnt found that unit or something happened!\n";
                file.close();
                return;
            }

            // критерий за търсенето 
            if (option == "title")
            {
                if (current->getTitle() == value)
                    filtered.push_back(current);
                else {
                    delete current;
                }
            }

            else if (option == "author")
            {
                // Book и Series  - те имат автор
                // тук ще го направим с каст-за да избегнем проверки
                if (Book* book = dynamic_cast<Book*>(current))
                {
                    if (book->getAuthor() == value) // string operator==
                        filtered.push_back(current);
                    else {
                        delete current;
                    }
                }

                // Periodicals и Series
                else if (Periodicals* periodical = dynamic_cast<Periodicals*>(current))
                {
                    bool found = false;
                    std::vector<Article> artc = periodical->getArticles();
                    for (size_t j = 0; j < artc.size(); j++)
                    {
                        if (artc[j].author == value) {
                            found = true;
                            filtered.push_back(current);
                        }
                    }
                    if (!found) {
                        delete current;
                    }
                }
            }

            else if (option == "tag")
            {
                // ключови думи 
                std::vector<std::string> keyWord = current->getKeyWordsForFind();
                bool found = false;
                for (size_t i = 0; i < keyWord.size(); i++)
                {

                    if (keyWord[i] == value) {
                        filtered.push_back(units[i]);
                    }
                }
                if (!found) {
                    delete current;
                }
            }
        }

        // сега дали имаме сортиране?
        if (!sortKey.empty()) {
            // значи трябва да сортираме 
            // взаимствано от интернет!
            std::sort(filtered.begin(), filtered.end(), [&](LibraryUnit* a, LibraryUnit* b) {
                if (sortKey == "title")
                return sortOrder != "desc" ? a->getTitle() <= b->getTitle() : a->getTitle() > b->getTitle();
            if (sortKey == "year")
                return sortOrder != "desc" ? a->getDate().getYear() <= b->getDate().getYear() : a->getDate().getYear() > b->getDate().getYear();
            return true;
                });
        }

        // Ограничение top
        size_t limit = (topCount > 0 && !sortKey.empty()) ? std::min<size_t>(topCount, filtered.size()) : filtered.size();

        // Извеждане
        for (size_t i = 0; i < limit; ++i)
        {
            filtered[i]->display();
        }
        for (size_t i = 0; i < filtered.size(); i++)
        {
            delete filtered[i];
        }
        filtered.clear();
    }
    catch (const std::exception&)
    {
        file.close();
        throw;
    }
    file.close();
}

// аналогично с юзърите, не ги добавяме всичките
void LibrarySystem::findReaders(const std::string& option, const std::string& value)
{
    if (currentPerson == nullptr) {
        std::cerr << "We need at least one active person!" << "\n";
        return;
    }

    std::ifstream file(fileUsers, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Couldn't open file!" << "\n";
        return;
    }

    try {
        Date today; // създава дата - която е днешната със std::time

        if (option == "name") {
            for (size_t i = 0; i < infoUsers.size(); i++) {
                if (infoUsers[i].username == value) {
                    file.seekg(infoUsers[i].pos, std::ios::beg);
                    bool valid;
                    file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
                    if (!file) {
                        throw std::ios_base::failure("Error with file in find users!");
                    }
                    if (!valid) {
                        continue;
                    }

                    // Прочитаме типа първо
                    TypeOfReader type;
                    std::streampos posBeforeType = file.tellg(); // запазваме позицията
                    file.read(reinterpret_cast<char*>(&type), sizeof(type));
                    if (!file.good()) {
                        throw std::ios_base::failure("Error with file!");
                    }

                    // курсора обратно, за да може обектът сам да си прочете Type вътре
                    file.seekg(posBeforeType);
                    if (!file) {
                        throw std::ios_base::failure("Failed to seek back before reading unit!");
                    }

                    LibraryPerson* reader = LibraryFactory::createPersonFromStream(file, type);
                    if (!reader) {
                        std::cerr << "Error creating reader!" << "\n";
                        file.close();
                        return;
                    }

                    reader->display();
                    delete reader;
                    file.close();
                    return;
                }
            }
            std::cerr << "No user with name: " << value << "\n";
        }

        else if (option == "ID") {
            int targetId = std::stoi(value);
            bool found = false;

            for (size_t i = 0; i < infoUsers.size(); i++) {
                file.seekg(infoUsers[i].pos, std::ios::beg);

                bool valid;
                file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
                if (!file) {
                    throw std::ios_base::failure("Error with file in find users!");
                }
                if (!valid || infoUsers[i].type != READER) {
                    continue;
                }


                // Прочитаме типа
                TypeOfReader type;
                std::streampos posBeforeType = file.tellg(); // запазваме позицията
                file.read(reinterpret_cast<char*>(&type), sizeof(type));
                if (!file.good()) {
                    throw std::ios_base::failure("Error with file!");
                }

                // курсора обратно, за да може обектът сам да си прочете Type вътре
                file.seekg(posBeforeType);
                if (!file) {
                    throw std::ios_base::failure("Failed to seek back before reading unit!");
                }

                LibraryPerson* person = LibraryFactory::createPersonFromStream(file, type);
                if (!person) {
                    std::cerr << "Error creating person!" << "\n";
                    continue;
                }

                // Проверяваме дали е Reader и има взетата книга
                Reader* reader = dynamic_cast<Reader*>(person);
                if (reader && reader->hasBorrowedUnit(targetId)) {
                    reader->display();
                    found = true;
                }

                delete person;  // Винаги изтриваме временния обект
            }

        }

        else if (option == "state") {

            std::vector<LibraryPerson*> tempReaders;

            // Първо зареждаме всички читатели в временен вектор
            for (size_t i = 0; i < infoUsers.size(); i++) {
                file.seekg(infoUsers[i].pos, std::ios::beg);
                bool valid;
                file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
                if (!file) {
                    throw std::ios_base::failure("Error with file in find users!");
                }
                if (!valid || infoUsers[i].type != READER) {
                    continue;
                }


                TypeOfReader type;
                std::streampos posBeforeType = file.tellg(); // запазваме позицията
                file.read(reinterpret_cast<char*>(&type), sizeof(type));
                if (!file.good()) {
                    throw std::ios_base::failure("Error with file!");
                }

                // курсора обратно, за да може обектът сам да си прочете Type вътре
                file.seekg(posBeforeType);
                if (!file) {
                    throw std::ios_base::failure("Failed to seek back before reading unit!");
                }
                LibraryPerson* person = LibraryFactory::createPersonFromStream(file, type);
                if (!person) continue;

                Reader* reader = dynamic_cast<Reader*>(person);
                if (reader) {
                    tempReaders.push_back(person);
                }
                else {
                    delete person;  // Не е Reader, изтриваме го
                }
            }

            // Сега филтрираме според състоянието
            bool found = false;
            for (size_t i = 0; i < tempReaders.size(); i++) {
                Reader* reader = dynamic_cast<Reader*>(tempReaders[i]);
                bool shouldDisplay = false;

                if (value == "overdue") {
                    shouldDisplay = reader->hasOverdueBooks(today);
                }
                else if (value == "active") {
                    shouldDisplay = reader->borrowedLastMonth(today) > 5;
                }
                else if (value == "inactive") {
                    shouldDisplay = reader->monthsSinceLastBorrow(today) > 3;
                }

                if (shouldDisplay) {
                    reader->display();
                    found = true;
                }
            }

            // Изтриваме всички временни обекти
            for (size_t i = 0; i < tempReaders.size(); i++) {
                delete tempReaders[i];
            }
            tempReaders.clear();

            if (!found) {
                std::cerr << "No users found with state: " << value << "\n";
            }

            if (value != "overdue" && value != "active" && value != "inactive") {
                std::cerr << "Invalid option for state!" << "\n";
            }
        }

        else {
            std::cerr << "Invalid option" << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in findReaders: " << e.what() << "\n";
        file.close();
        throw e;
    }

    file.close();
}

bool LibrarySystem::serialize(std::ostream& file1, std::ostream& file2) const
{
    if (!file1 || !file2) {
        throw std::invalid_argument("Invalid stream arguments for serialization!");
    }
    
    try {
        file2.seekp(0, std::ios::beg);

        // Write units count
        size_t size = units.size() + infoUnits.size();
        file2.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (size_t i = 0; i < units.size(); i++)
        {
            for (size_t j = 0; j < infoUnits.size(); j++)
            {
                if (units[i]->getUniqueNumber() == infoUnits[j].uniqueNumber) {
                    file2.seekp(infoUnits[j].pos);
                    bool invalid = false;
                    file2.write(reinterpret_cast<const char*>(&invalid), sizeof(invalid));
                    if (!file2) {
                        throw std::ios_base::failure("Error with file!");
                    }
                }
            }
        }
        
        if (!file2.good()) {
            throw std::ios_base::failure("Error writing units count");
        }

        file2.seekp(0, std::ios::end);
        // Write units

        for (size_t i = 0; i < units.size(); i++) {
            bool valid = true;
            file2.write(reinterpret_cast<const char*>(&valid), sizeof(valid));
            if (!file2.good()) {
                throw std::ios_base::failure("Error with writing in system::serialize method!");
            }
            
            units[i]->serialize(file2);
            
            if (!file2.good()) {
                throw std::ios_base::failure("Error serializing unit data");
            }
        }
        
        // Write users count
        file1.seekp(0, std::ios::beg);
        size_t sizeOfUsers = users.size() + infoUsers.size();

        for (size_t i = 0; i < users.size(); i++) {
            bool alreadyIn = false;
            for (size_t j = 0; j < infoUsers.size(); j++) {
                if (users[i]->getUsername() == infoUsers[j].username &&
                    users[i]->getPassword() == infoUsers[j].password) {
                    alreadyIn = true;
                    break; // Спри след първото съвпадение!
                }
            }
            if (alreadyIn) {
                sizeOfUsers--; // Намали само веднъж за потребител
            }
        }

        
        file1.write(reinterpret_cast<const char*>(&sizeOfUsers), sizeof(sizeOfUsers));
        if (!file1.good()) {
            throw std::ios_base::failure("Error writing users count");
        }
        
        // Write users

        file1.seekp(0, std::ios::end);

        for (size_t i = 0; i < users.size(); i++) {
            // Record position before writing this user
            size_t pos = file1.tellp();
            
            // Serialize the rest of the user data
            
            bool isAlreadeyIn = false;
            
            for (size_t j = 0; j < infoUsers.size(); j++)
            {
                if (users[i]->getUsername() == infoUsers[j].username &&
                    users[i]->getPassword() == infoUsers[j].password )
                {
                    isAlreadeyIn = true;
                    break;
                }
            }

            if (!isAlreadeyIn) {
                
                bool valid = true;
                file1.write(reinterpret_cast<const char*>(&valid), sizeof(valid));
                if (!file1.good()) {
                    throw std::ios_base::failure("Error with writing in system::serialize method!");
                }

                users[i]->serialize(file1);
                if (!file1.good()) {
                    throw std::ios_base::failure("Error serializing user data");
                }
            }
        }
        
        return true;
    }
    catch (std::exception& e) {
        std::cerr << "Error during serialization: " << e.what() << std::endl;
        throw;
    }
}

bool LibrarySystem::deserialize(std::istream& file1, std::istream& file2)
{
    if (!file1 || !file2) {
        throw std::invalid_argument("Invalid stream arguments for deserialization!");
    }
    
    // Check for empty files
    if ((file2.tellg() == 0 && file2.peek() == std::ifstream::traits_type::eof()) ||
        (file1.tellg() == 0 && file1.peek() == std::ifstream::traits_type::eof())) {
        return false;
    }
    
    try {
        // Clear existing metadata before deserializing
        infoUnits.clear();
        infoUsers.clear();
        
        // Read units count
        size_t unitsCount;
        unsigned maxId = 0;
        file2.read(reinterpret_cast<char*>(&unitsCount), sizeof(unitsCount));
        if (!file2.good()) {
            throw std::ios_base::failure("Error reading units count from file");
        }
        
        // Read units metadata
        for (size_t i = 0; i < unitsCount; ++i) {
            size_t startingPos = file2.tellg();

            bool valid;
            file2.read(reinterpret_cast<char*>(&valid), sizeof(valid));
            if (!file2) {
                throw std::ios_base::failure("Failed to read state of current unit!");
            }

            Type type;
            std::size_t posBeforeType = file2.tellg(); // запазваме позицията
            file2.read(reinterpret_cast<char*>(&type), sizeof(type));
            if (!file2.good()) {
                throw std::ios_base::failure("Error with file!");
            }

            // курсора обратно, за да може обектът сам да си прочете Type вътре
            file2.seekg(posBeforeType);
            if (!file2) {
                throw std::ios_base::failure("Failed to seek back before reading unit!");
            }

            // Skip the data by creating and then deleting a temporary object
            LibraryUnit* temp = LibraryFactory::createUnitFromStream(file2, type);
            if (temp->getId() > maxId) {

                maxId = temp->getId();
            }
            
            if (!temp) {
                throw std::runtime_error("Failed to create unit object from stream");
            }
            // Add metadata to our collection
            try
            {
                
                infoUnits.push_back(UniqueIDAndFilePositions{ temp->getId(), startingPos, temp->isAvailable(),temp->getType() });
                delete temp;
            }
            catch (...)
            {
                delete temp;
                throw;
            } 
            
        }
        IDGenerator::setLastId(maxId);


        size_t userCount;
        file1.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));
        if (!file1.good()) {
            throw std::ios_base::failure("Error reading user count from file");
        }
        
        // Now read exactly 'userCount' users
        for (size_t i = 0; i < userCount; ++i) {
            size_t startingPoint = file1.tellg();
            bool valid;
            file1.read(reinterpret_cast<char*>(&valid), sizeof(valid));
            if (!file1) {
                throw std::ios_base::failure("Failed to read state of current unit!");
            }

            TypeOfReader type;
            size_t posBeforeType = file1.tellg(); // запазваме позицията
            file1.read(reinterpret_cast<char*>(&type), sizeof(type));
            if (!file1.good()) {
                throw std::ios_base::failure("Error with file!");
            }

            // курсора обратно, за да може обектът сам да си прочете Type вътре
            file1.seekg(posBeforeType);
            if (!file1) {
                throw std::ios_base::failure("Failed to seek back before reading unit!");
            }
                        
            // Create the person object from the stream
            LibraryPerson* temp = LibraryFactory::createPersonFromStream(file1, type);
            if (!temp) {
                throw std::runtime_error("Failed to create person object from stream");
            }
            try
            {
                
                infoUsers.push_back(MetaInfoAboutUsers{ temp->getUsername(), temp->getPassword(),startingPoint,temp->getType() });
                delete temp;

            }
            catch (...)
            {
                
                delete temp;
                throw;
            }
        }

        return true;
    }
    catch (std::exception& e) {
        std::cerr << "Error during deserialization: " << e.what() << std::endl;
        throw; 
    }
}

void LibrarySystem::addUnit(const Type& type)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return;
    }

    LibraryUnit* newItem = LibraryFactory::createUnitInteractively(type);
    if (newItem == nullptr) {
        std::cerr << "Adding new unit been cancelled!" << "\n";
        return;
    }

    units.push_back(newItem);
    
    std::cout << "Successfully pushed new Unit in our system!" << "\n";
    return;
}

void LibrarySystem::removeUnit(unsigned int id)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return;
    }
    bool found = false;


    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        if (infoUnits[i].uniqueNumber == id) {
            found = true;
            std::fstream file(fileUnits, std::ios::in | std::ios::out | std::ios::binary);
            if (!file.is_open()){
                throw std::ios_base::failure("Error with file!");
            }

            file.seekp(infoUnits[i].pos);
            bool isValid = false;
            file.write(reinterpret_cast<const char*>(&isValid), sizeof(isValid));
            if (!file) {
                file.close();
                throw std::ios_base::failure("Error with file!");
            }
            file.close();

            std::cout << "Succesfully removed from dataBase unit with id: " << id << "\n";
        }
    }
    if (!found) {
        std::cout << "No unit with that id: " << id << " has been loaded into the system!\n";
        return;
    }
    // махаме ги от записите на читателите
    std::ifstream users(fileUsers, std::ios::binary);
    if (!users.is_open()) {
        throw std::ios_base::failure("Error with file!");
    }

    
    for (size_t i = 0; i < infoUsers.size(); i++)
    {
        users.seekg(infoUsers[i].pos);
        bool valid;
        users.read(reinterpret_cast<char*>(&valid), sizeof(valid));
        if (!users) {
            throw std::ios_base::failure("Error with file!");
        }
        if (!valid) {
            continue;
        }

        LibraryPerson* person = LibraryFactory::createPersonFromStream(users, infoUsers[i].type);
        if (!person) {
            throw std::invalid_argument("Error with creating userf from stream!");
        }
        Reader* isReader = dynamic_cast<Reader*>(person);
        if (isReader == nullptr) {
            continue;
        }
        // иначе

        // работим с & тоест променяме оригинала
        // unit-> текущо заетите му киги
        //hisory - всичките му заети някога

        std::vector<unsigned int> unit = isReader->getUnits();
        for (size_t j = 0; j < unit.size(); j++)
        {
            if (unit[i] == id) {
                std::swap(unit[i], unit.back());
                unit.pop_back();
            }
        }
    }
}

// тук ще ги зареждаме в системата вече
bool LibrarySystem::changeUnit(unsigned int id)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return false;
    }
    
    std::fstream file(fileUnits,std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "File coulndnt open!\n";
        return false;
    }

    LibraryUnit* unit = nullptr;

    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        if (infoUnits[i].uniqueNumber == id) {
            file.seekg(infoUnits[i].pos);
            bool valid;
            file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
            if (!file) {
                file.close();
                throw std::ios_base::failure("Error with file!");
            }
            if (!valid) {
                continue;
            }

            unit = LibraryFactory::createUnitFromStream(file, infoUnits[i].type);
            if (!unit) {
                std::cerr << "Error in changeUnit!\n";
                file.close();
                return false;
            }

            try
            {
                if (!unit->change()) {
                    return false;
                }

                units.push_back(unit);

                file.seekp(infoUnits[i].pos);
                valid = false;
                file.write(reinterpret_cast<char*>(&valid),sizeof(valid));
                if (!file) {
                    throw std::ios_base::failure("Error with file!");
                }

                file.close();
                std::cout << "Susccesfully changed unit with ID: " << id << "\n";
                return true;

            }
            catch (...)
            {
                delete unit;
                file.close();
                throw;
            }
        }
    }
    
    std::cerr << "Not found!\n";
    return false;
}

bool LibrarySystem::rewriteUnitById(int id) {

    std::fstream file(fileUnits, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for update.\n";
        return false;
    }

    for (const UniqueIDAndFilePositions& meta : infoUnits) {
        if (meta.uniqueNumber == id) {
            // Намери обекта в units
            for (LibraryUnit* unit : units) {
                if (unit->getId() == id) {
                    file.seekp(meta.pos);  // отиваме точно където е
                    unit->serialize(file);    // overwrite
                    file.close();
                    std::cout << "Successfully rewrote unit with ID: " << id << "\n";
                    return true;
                }
            }
            std::cerr << "Unit with ID found in meta but not in memory!\n";
            file.close();
            return false;
        }
    }

    std::cerr << "No metadata found for ID: " << id << "\n";
    return false;
}

void LibrarySystem::addUser(bool isAdmin)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return;
    }
    
    TypeOfReader type = READER;
    if(isAdmin){
        type = ADMINISTRATOR;
    }

    LibraryPerson* newItem = LibraryFactory::createPersonInteractively(type);
    if (newItem == nullptr) {
        std::cerr << "Adding new user been cancelled!" << "\n";
        return;
    }
    
    // Add the user to the users vector
    for (size_t i = 0; i < infoUsers.size(); i++)
    {
        if (newItem->getUsername() == infoUsers[i].username) {
            std::cerr << "We have user with that username!";
            delete newItem;
            return;
        }
    }

    for (size_t i = 0; i < users.size(); i++)
    {
        if(newItem->getUsername() == users[i]->getUsername())
        {
            std::cerr << "We have user with that username!";
            delete newItem;
            return;
        }
    }
    try
    {
        users.push_back(newItem);

    }
    catch (...)
    {
        delete newItem;
        throw;
    }
    
    std::cout << "Successfully added new user " << newItem->getUsername() << " to the system!\n";
}

void LibrarySystem::removeUser(const std::string& name)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return;
    }
    std::fstream fileUsers(this->fileUsers, std::ios::in | std::ios::out | std::ios::binary);
    if (!fileUsers.is_open()) {
        throw std::ios_base::failure("Error with file!");
    }

    size_t count = 0;
    for (size_t i = 0; i < infoUsers.size(); i++)
    {
        if (infoUsers[i].type == ADMINISTRATOR) {
            fileUsers.seekg(infoUsers[i].pos);
            bool valid;
            fileUsers.read(reinterpret_cast<char*>(&valid), sizeof(valid));
            if (!fileUsers) {
                fileUsers.close();
                throw std::ios_base::failure("Error with file");
            }
            if (valid) {
                count++;
            }
        }
    }
    
    std::string input;
    for (size_t i = 0; i < infoUsers.size(); i++)
    {
        if (infoUsers[i].username == name) {
            fileUsers.seekg(infoUsers[i].pos);
            bool isValid;
            fileUsers.read(reinterpret_cast<char*>(&isValid), sizeof(isValid));
            if (!fileUsers) {
                fileUsers.close();
                throw std::ios_base::failure("Error with file!");
            }

            if (!isValid) {
                continue;
            }
            
            // иначе е валиден 
            //  трябва да го махнем
            if (infoUsers[i].type == TypeOfReader::ADMINISTRATOR && count == 1) {
                std::cerr << "Cannot remove cause  this is the last Admin in our system!\n";
                fileUsers.close();
                return;
            }

            // иначе можем да го махнем
            if (currentPerson->getUsername() == infoUsers[i].username &&
                currentPerson->getPassword() == infoUsers[i].password)
            {
                if (!this->logout()) {
                    std::cerr << "Something happened!\n";
                    fileUsers.close();
                    return;
                };
                fileUsers.seekp(infoUsers[i].pos);
                bool valid = false;
                fileUsers.write(reinterpret_cast<const char*>(&valid), sizeof(valid));
                if (!fileUsers) {
                    fileUsers.close();
                    throw std::ios_base::failure("Error with file!");
                }
                std::cout << "Successfully removed current user from dataBase!\n";
            }

            // иначе не е текущо логнатият
            else if (currentPerson->getUsername() != infoUsers[i].username ||
                currentPerson->getPassword() != infoUsers[i].password) {
                bool valid = false;
                fileUsers.seekp(infoUsers[i].pos);
                fileUsers.write(reinterpret_cast<char*>(&valid), sizeof(valid));
                if (!fileUsers) {
                    fileUsers.close();
                    throw std::ios_base::failure("Error with file!");
                }
                std::cout << "Successfully removed current user from dataBase!\n";
            }

            std::cout << "Do you want to remove his books? 'yes' or 'no'\n";
            std::getline(std::cin, input);
            
            bool okayToGo = true;

            if (input == "yes") {
                if (infoUsers[i].type == ADMINISTRATOR) {

                    std::cerr << "Admins cannot take any books!\n";
                    okayToGo = false;
                }

                if (okayToGo) {
                    bool hasToBeFalse;
                    fileUsers.seekg(infoUsers[i].pos);
                    fileUsers.read(reinterpret_cast<char*>(&hasToBeFalse), sizeof(hasToBeFalse));
                    if (!fileUsers) {
                        throw std::ios_base::failure("Error with file!");
                    }

                    LibraryPerson* reader = LibraryFactory::createPersonFromStream(fileUsers, infoUsers[i].type);
                    if (!reader) {
                        fileUsers.close();
                        throw std::invalid_argument("Invalid creationg of user from stream in remove!");
                    }

                    std::vector<int> takenIds = reader->getTakenIds();
                    for (size_t i = 0; i < takenIds.size(); i++)
                    {
                        try
                        {
                            removeUnit(takenIds[i]);
                        }
                        catch (...)
                        {
                            fileUsers.close();
                            std::cerr << "Something happened in removeUsers function!\n";
                            throw;
                        }
                    }
                }
                
            }

            else if (input != "no") {
                std::cerr << "invalid answer!\n";
                fileUsers.close();
                return;
            }

            std::cout << "Successfully removed user: " << name << "\n";
            fileUsers.close();
            return;
        }
    }
    std::cerr << "Not found!\n";
    fileUsers.close();
    return;
}

bool LibrarySystem::changeUser(const std::string& newPassword,const std::string& targetUser) {
    if (currentPerson == nullptr) {
        std::cerr << "Only admins can change!\n";
        return false;
    }
  
    // Определяме кой ще се променя (self или друг)
    LibraryPerson* toModify = nullptr;
    if (targetUser.empty()) {
        if (currentPerson->getUsername() == "admin" &&
            currentPerson->getPassword() == "i<3C++") {
            std::cerr << "Cannot change optional admin!";
            return false;
        }
        toModify = currentPerson->clone();
    }
    else {
        try
        {
            toModify = findUserByUsername(targetUser); // може да хвърли 

        }
        catch (...)
        {
            std::cerr << "Something happend with finding tagert!\n";
            return false;
        }
    }

    if (!toModify) return false;

    // Актуализираме паролата в паметта
    toModify->setPassword(newPassword);

    // Намираме позицията в infoUsers, където стои старият запис
    size_t idx = 0;
    for (; idx < infoUsers.size(); ++idx) {
        if (infoUsers[idx].username == toModify->getUsername())
            break;
    }
    if (idx == infoUsers.size()) {
        delete toModify;
        return false;  // не би трябвало да се случи
    }

    size_t oldPos = infoUsers[idx].pos;

    // 5) Invalid-ираме стария запис 
    {
        std::fstream f(fileUsers,
            std::ios::in | std::ios::out | std::ios::binary);
        f.seekp(oldPos, std::ios::beg);
        bool valid = false;
        f.write(reinterpret_cast<char*>(&valid), sizeof(valid));
        if (!f) {
            delete toModify;
            f.close();
            throw std::ios_base::failure("Error with file!");
        }
        f.close();
    }

    

    // 7) Обновяваме само pos в meta-вектора
        try
    {
        users.push_back(toModify);
    }

    catch (...)
    {

        delete toModify;
        throw;
    }
    std::cout << "Succesfully changed password to: '" << newPassword << "'\n";
    return true;
}


/*
> take <ID>
Заема книга/издание с определен вътрешен идентификатор. Съответната книга се маркира като заета и се вписва в записа на читателя. 
Позволява се, когато има поне една незаета бройка от книгата.
*/

bool LibrarySystem::take(unsigned int id)
{
    // админите не взимат неща! само readers!
    if (currentPerson == nullptr ) {
        std::cerr << "We need a current user logged in our system!\n";
        return false;
    }
   
    if (currentPerson->getType() == ADMINISTRATOR) {
        std::cerr << "Administrators cannot take units!\n";
        return false;
    }

    LibraryUnit* unit = findUnitById(id);
   
    if (!unit) {
        std::cerr << "Couldnt find that book!\n";
        return false;
    }

    if (!unit->isAvailable()) {
        delete unit;
        std::cerr << "This book is not available!";
        return false;
    }
    std::fstream users;
    std::fstream file;

    if (unit->incrementTakenCopies()) {
        try
        {
            currentPerson->borrow(unit);
            users.open(fileUsers, std::ios::in | std::ios::out | std::ios::binary);
            if (!users.is_open())
            {
                throw std::ios_base::failure("Error with file!");
            }

            file.open(fileUnits, std::ios::in | std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                throw std::ios_base::failure("Error wtih file!");
            }
            
            for (size_t i = 0; i < infoUnits.size(); i++)
            {
                if (infoUnits[i].uniqueNumber == unit->getId()) {
                    file.seekp(infoUnits[i].pos);
                    bool validity = true;
                    file.write(reinterpret_cast<const char*>(&validity), sizeof(validity));
                    if (!file) {
                       
                        throw std::ios_base::failure("Error with file!");
                    }
                    // записваме промените, ок е, тъй като сме сменили само int - нямаме динамична смяна на нещо
                    unit->serialize(file); 
                }
            }

            for (size_t i = 0; i < infoUsers.size(); i++)
            {
                if(infoUsers[i].username == currentPerson->getUsername() && infoUsers[i].password == currentPerson->getPassword())
                {
                    size_t size = 0;
                    users.seekg(0, std::ios::beg);
                    users.read(reinterpret_cast<char*>(&size), sizeof(size));
                    if (!users) {
                        throw std::ios_base::failure("Error with file!");
                    }
                    users.seekp(0, std::ios::beg);
                    size_t newSize = size + 1;
                    users.write(reinterpret_cast<const char*>(&newSize), sizeof(newSize));
                    if (!users) {
                        throw std::ios_base::failure("Error with file!");
                    }

                    users.seekp(infoUsers[i].pos);
                    bool valid = false;
                    users.write(reinterpret_cast<const char*>(&valid), sizeof(valid));
                    if (!users) {
                        throw std::ios_base::failure("Error with file!");
                    }



                    users.seekp(0, std::ios::end);
                    size_t pos = users.tellp();
                    valid = true;
                    infoUsers[i].pos = pos;
                    users.write(reinterpret_cast<const char*>(&valid), sizeof(valid));
                    if (!users) {
                        throw std::ios_base::failure("Error with file!");
                    }

                    currentPerson->serialize(users);
                    break;
                }
            }
        }
        catch (...)
        {
            users.close();
            file.close();
            std::cerr << "Coulnt borrow that book with id: " << id << "\n";
            delete unit;
            throw;
        }
        std::cout << "Borrowed succesffully book with ID: " << id << "\n";
        delete unit;
        return true;
    }

    return false;
}

bool LibrarySystem::returnUnit(unsigned int id)
{
    if (currentPerson == nullptr || currentPerson->getType() == ADMINISTRATOR) {
        std::cerr << "We need a user logged in our system to perform this return operation! \nAdmins cannot perform it!\n";
        return false;
    }
    
    if (!currentPerson->returnUnit(id)) {
        std::cerr << " Couldnt return!\n";
        return false;
    };

    std::fstream users(fileUsers, std::ios::in | std::ios::out | std::ios::binary);
    if (!users.is_open()) {
        throw std::ios_base::failure("Error with file!");
    }
    
    // записваме промяната в users
    for (size_t i = 0; i < infoUsers.size(); i++)
    {
        if (infoUsers[i].username == currentPerson->getUsername() &&
            infoUsers[i].password == currentPerson->getPassword()) 
        {
            users.seekp(infoUsers[i].pos);
            bool validity = false;
            users.write(reinterpret_cast<const char*>(&validity), sizeof(validity));
            if (!users) {
                users.close();
                throw std::ios_base::failure("Error with file!");
            }


            try
            {
                users.seekg(0, std::ios::end);
                users.write(reinterpret_cast<const char*>(&validity), sizeof(validity));
                if (!users) {
                    throw std::ios_base::failure("Error with file!");
                }
                currentPerson->serialize(users);

            }
            catch (...)
            {
                users.close();
                throw;
            }
        }
    }
    users.close();

    std::fstream units(fileUnits, std::ios::in | std::ios::out | std::ios::binary);
    if (!units.is_open())
    {
        throw std::ios_base::failure("Error with file!");
    }

    // записваме промяната в units

    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        if (infoUnits[i].uniqueNumber == id) {
            units.seekg(infoUnits[i].pos);
            bool valid;
            units.read(reinterpret_cast<char*>(&valid), sizeof(valid));
            if (!units) {
                units.close();
                throw std::ios_base::failure("Error with file!");
            }

            if (!valid) {
                throw std::invalid_argument("Not possbile!");
            }

            LibraryUnit* unit = LibraryFactory::createUnitFromStream(units, infoUnits[i].type);
            if (!unit) {
                units.close();
                throw std::invalid_argument("Couldnt create unit!");
            }

            if (!unit->decrementTakenCopies())
            {
                units.close();
                std::cerr << "Couldnt decrement!\n";
                return false;
            }

            try
            {
                units.seekp(infoUnits[i].pos);
                valid = true;
                units.write(reinterpret_cast<const char*>(&valid), sizeof(valid));
                if (!units) {
                    throw std::ios_base::failure("Error with file!");
                }
                unit->serialize(units);
            }
            catch (...)
            {
                delete unit;
                units.close();
                throw;
            }
            std::cout << "Successfully returned!\n";
            return true;

        }

    }
    std::cout << "Something happened in return function!\n";
    return false;
}

void LibrarySystem::print()const
{
    for (size_t i = 0; i < units.size(); i++)
    {
        units[i]->display();
    }for (size_t i = 0; i < users.size(); i++)
    {
        users[i]->display();

    }
}

void LibrarySystem::incrementCopies(unsigned int id, unsigned int count)
{
    std::fstream file(fileUnits, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        throw std::ios_base::failure("Error with file!");
    }

    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        if (infoUnits[i].uniqueNumber == id) {
            bool isValid;
            
            file.seekg(infoUnits[i].pos);
            file.read(reinterpret_cast<char*>(&isValid), sizeof(isValid));
            if (!file) {
                file.close();
                throw std::ios_base::failure("Error with file!");
            }

            if (!isValid) {
                std::cerr << "Invalid ID";
                return;
            }

            LibraryUnit* unit = LibraryFactory::createUnitFromStream(file,infoUnits[i].type);
            if (!unit) {
                file.close();
                std::cerr << "Error with creating unit!";
                return;
            }

            if (unit->getTotalCopies() > count) {
                std::cerr << "You are trying to decrement copies!\n";
                delete unit;
                file.close();
                return;
            }
            try
            {
                file.seekp(infoUnits[i].pos);
                isValid = false;
                file.write(reinterpret_cast<const char*>(&isValid), sizeof(isValid));
                if (!file) {
                    throw std::ios_base::failure("Error with file!");
                }
                units.push_back(unit);
            }
            catch (...)
            {
                file.close();
                delete unit;
                throw;
            }
        }
    }
    std::cerr << "Not found!\n";
    return;
}

void LibrarySystem::booksAll(std::ostream& out)
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    std::cout << infoUnits.size();
    std::ifstream file(fileUnits,std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error with file while booksAll!\n";
        return;
    }

    //търсим в meta данните, ако е там -> принтираме, но не я зареждаме още

    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        LibraryUnit* unit = nullptr;
        if (infoUnits[i].type == Type::BOOK) {
            try
            {
                file.seekg(infoUnits[i].pos, std::ios::beg);
                bool valid;
                file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
                if (!file) {
                    file.close();
                    throw std::ios_base::failure("Error with file!");
                }
                if (!valid) {
                    continue;
                }

                Type type;
                std::streampos posBeforeType = file.tellg(); // запазваме позицията
                file.read(reinterpret_cast<char*>(&type), sizeof(type));
                if (!file.good()) {
                    file.close();
                    throw std::ios_base::failure("Error with file!");
                }

                // курсора обратно, за да може обектът сам да си прочете Type вътре
                file.seekg(posBeforeType);
                if (!file) {
                    file.close();
                    throw std::ios_base::failure("Failed to seek back before reading unit!");
                }

                LibraryUnit* unit = LibraryFactory::createUnitFromStream(file, type);
                if (!unit) {
                    std::cerr << "Creating unit error!\n";
                    file.close();
                }
                
                unit->printBase();
                delete unit;
            }
            catch (...)
            {
                delete unit;
                file.close();
                throw;
            } 
        }
        
    }
}

void LibrarySystem::periodicalsAll(std::ostream& out) 
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    
    std::ifstream file(fileUnits, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error with file while booksAll!\n";
        return;
    }

    //търсим в meta данните, ако е там -> зареждаме книгата
    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        LibraryUnit* unit = nullptr;
        if (infoUnits[i].type == Type::PERIODICALS) {
            try
            {

                file.seekg(infoUnits[i].pos, std::ios::beg);
                bool valid;
                file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
                if (!file) {
                    file.close();
                    throw std::ios_base::failure("Error with file!");
                }
                if (!valid) {
                    continue;
                }
                Type type;
                std::streampos posBeforeType = file.tellg(); // запазваме позицията
                file.read(reinterpret_cast<char*>(&type), sizeof(type));
                if (!file.good()) {
                    file.close();
                    throw std::ios_base::failure("Error with file!");
                }

                // курсора обратно, за да може обектът сам да си прочете Type вътре
                file.seekg(posBeforeType);
                if (!file) {
                    file.close();
                    throw std::ios_base::failure("Failed to seek back before reading unit!");
                }



                unit = LibraryFactory::createUnitFromStream(file, type);
                if (!unit) {
                    std::cerr << "Creating unit error!\n";
                    file.close();
                }
             
                unit->printBase();
                delete unit;
            }
            catch (...)
            {
                delete unit;
                file.close();
                throw;
            }
        }
    }
}

void LibrarySystem::seriesAll(std::ostream& out) 
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }

    std::ifstream file(fileUnits, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error with file while booksAll!\n";
        return;
    }

    //търсим в meta данните, ако е там -> зареждаме книгата
    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        LibraryUnit* unit = nullptr;
        if (infoUnits[i].type == Type::SERIES) {
            try
            {
                file.seekg(infoUnits[i].pos, std::ios::beg);
               
                bool valid;
                file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
                if (!file) {
                    file.close();
                    throw std::ios_base::failure("Error with file!");
                }
                if (!valid) {
                    continue;
                }
                Type type;
                std::streampos posBeforeType = file.tellg(); // запазваме позицията
                file.read(reinterpret_cast<char*>(&type), sizeof(type));
                if (!file.good()) {
                    file.close();
                    throw std::ios_base::failure("Error with file!");
                }

                // курсора обратно, за да може обектът сам да си прочете Type вътре
                file.seekg(posBeforeType);
                if (!file) {
                    file.close();
                    throw std::ios_base::failure("Failed to seek back before reading unit!");
                }

                 unit = LibraryFactory::createUnitFromStream(file, type);
                if (!unit) {
                    std::cerr << "Creating unit error!\n";
                    file.close();
                }

                unit->printBase();
                delete unit;

            }
            catch (...)
            {
                delete unit;
                file.close();
                throw;
            }
        }
    }
}

void LibrarySystem::all(std::ostream& out) 
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }


    std::ifstream file(fileUnits, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error with file while booksAll!\n";
        return;
    }

    
    //търсим в meta данните, ако е там -> зареждаме книгата
    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        LibraryUnit* unit = nullptr;
            try
            {
                file.seekg(infoUnits[i].pos, std::ios::beg);
                bool valid;
                file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
             
                if (!valid) {
                    continue;
                }

                Type type;
                std::streampos posBeforeType = file.tellg(); // запазваме позицията
                file.read(reinterpret_cast<char*>(&type), sizeof(type));
                if (!file.good()) {
                    throw std::ios_base::failure("Error with file!");
                }

                // курсора обратно, за да може обектът сам да си прочете Type вътре
                file.seekg(posBeforeType);
                if (!file) {
                    throw std::ios_base::failure("Failed to seek back before reading unit!");
                }


                unit = LibraryFactory::createUnitFromStream(file, type);
                if (!unit) {
                    std::cerr << "Creating unit error!\n";
                    file.close();
                }

                unit->printBase();
                delete unit;
            }
            catch (...)
            {
                delete unit;
                file.close();
                throw;
            }
    }
}

void LibrarySystem::listInfo(const std::string& ISNN_OR_ISBN) 
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    
    std::ifstream file(fileUnits, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error with seraching information in listInfo()!\n";
        return;
    }

    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        file.seekg(infoUnits[i].pos);
        bool valid;
        file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
        if (!file) {
            file.close();
            throw std::ios_base::failure("Error with file!");
        }
        if (!valid) 
        {
            continue;
        }
        LibraryUnit* current = LibraryFactory::createUnitFromStream(file, infoUnits[i].type);
        if (current == nullptr)
        {
            std::cerr << "Error with creating unit in listInfo()!\n";
            return;
        }

        if (current->getType() == SERIES) {
            Series* itsASerie = dynamic_cast<Series*>(current);
            if (itsASerie!=nullptr && (itsASerie->getISBN() == ISNN_OR_ISBN || itsASerie->getISSN() == ISNN_OR_ISBN)) {
                itsASerie->display();
                delete current;
                return;
            }
        }
        
        if (current->getISSNorISBN() == ISNN_OR_ISBN) {
            try
            {
                current->display();
                delete current;
                return;
            }
            catch (...)
            {
                delete current;
                throw;
            }
        }
    }

    std::cerr << "Item was not found!" << "\n";
    return;
}

bool LibrarySystem::canAddNewUser(const std::string& name, const std::string& password) const
{
    // дали можем да добавин нов потребител
    // ако е в системата - не
    // ако има несъответствие с паролата - пак не

    // ако е добавен вече
    for (size_t i = 0; i < users.size(); i++)
    {
        // operator== of string
        if (users[i]->getUsername() == name) {
            return false;
        }
    }

    // ако ли пък не е добавен
    for (size_t i = 0; i < infoUsers.size(); i++)
    {
        if(infoUsers[i].username == name && infoUsers[i].password == password) 
        {
            return true;
        }
    }
    return false;
}

LibraryPerson* LibrarySystem::findUserByUsername(const std::string& n) const
{
    if (n.empty()) {
        return nullptr;
    }

    LibraryPerson* newperson = nullptr;
    std::ifstream file(fileUsers, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Couldnt open file!");
    }

    for (size_t i = 0; i < infoUsers.size(); i++)
    {
        
        if (infoUsers[i].username == n) {
            bool valid;
            file.seekg(infoUsers[i].pos);
            file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
            if (!file) {
                file.close();
                return nullptr;
            }
            if (!valid) {
                return nullptr;
            }
            newperson = LibraryFactory::createPersonFromStream(file, infoUsers[i].type);
            if (!newperson) {
                return nullptr;
            }
        }
    }
    file.close();
    return newperson;
}

LibraryUnit* LibrarySystem::findUnitById(unsigned int id) const
{
    LibraryUnit* newunit= nullptr;
    
    std::ifstream file(fileUnits, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Couldnt open file!");
    }

    for (size_t i = 0; i < infoUnits.size(); i++)
    {
        if (infoUnits[i].uniqueNumber == id &&
            infoUnits[i].isFree) {
            file.seekg(infoUnits[i].pos);
            bool valid;
            file.read(reinterpret_cast<char*>(&valid), sizeof(valid));
            if (!file) {
                file.close();
                throw std::runtime_error("File error!");
            }

            if (valid == true) {
                newunit = LibraryFactory::createUnitFromStream(file, infoUnits[i].type);
                break;
            }
        }
    }
    file.close();
    return newunit;
}
