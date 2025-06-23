#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <sstream>
#include <vector>
#include "LibrarySystem.h"

/*
void testLibraryUnitSerialization() {
    // 1. Създай примерен Book обект
    std::vector<std::string> keyWords;
    keyWords.push_back("allen");
    keyWords.push_back("ballen");
    std::string title("The Hobbit");
    std::string description("Fantasy novel by J.R.R. Tolkien");
    std::string publishers("Allen & Unwin");
    std::string author("Allen");
    std::string ISBN("ISBN123");


    Book original(title, publishers, description,21, 9, 1937,
        Rate::Excellent, Genre::Fantasy, author, ISBN,keyWords);

    // 2. Сериализирай обекта
    std::ofstream ofs("book_test.bin", std::ios::binary);
    assert(ofs.is_open());
    original.serialize(ofs);
    ofs.close();

    // 3. Десериализирай в нов обект
    Book loaded;
    std::ifstream ifs("book_test.bin", std::ios::binary);
    assert(ifs.is_open());
    loaded.deserialize(ifs);
    ifs.close();

    // 4. Сравни стойностите
    assert(original.getTitle() == loaded.getTitle());
    assert(original.getPublishers() == loaded.getPublishers());
    assert(original.getDescription() == loaded.getDescription());
    assert(original.getGenre() == loaded.getGenre());
    assert(original.getRate() == loaded.getRate());
    assert(original.getDate().getDay() == loaded.getDate().getDay());
    assert(original.getDate().getMonth() == loaded.getDate().getMonth());
    assert(original.getDate().getYear() == loaded.getDate().getYear());
    assert(original.getTotalCopies() == loaded.getTotalCopies());

}
*/

/*
void testLibraryPersonSerialization() {
   
    std::string username("Test");
    std::string password("Test12+");
    std::vector<std::string> keyWords;
    keyWords.push_back("allen");
    keyWords.push_back("ballen");
    std::string title("The Hobbit");
    std::string description("Fantasy novel by J.R.R. Tolkien");
    std::string publishers("Allen & Unwin");
    std::string author("Allen");
    std::string ISBN("ISBN123");
    std::string ISSN("ISSN1111");
    LibraryUnit* original = new Book(title, publishers, description, 21, 9, 1937,
        Rate::Excellent, Genre::Fantasy, author, ISBN, keyWords);

    std::vector<Article>  articles;
    std::vector<std::string> keys;
    std::string artTitle("ArticleTitle");
    std::string articleAuthor("ArticleAuthor");


    keys.push_back("Article");
    keys.push_back("Barrara");
    
    Article first(articleAuthor,artTitle,keys);
    articles.push_back(first);
    

    LibraryUnit* original2 = new Periodicals(title, publishers, description,
        Genre::Fantasy, Rate::Poor, 2, 2002, 12, ISSN,articles);
 
    LibraryUnit* original3 = new Series(title,publishers,description,1,1,2006, Rate::Good,Genre::News, author, 16,
        ISBN, keyWords, ISSN,articles);
    std::vector<LibraryUnit*> vector;
    std::vector<LoanInfo> history;
    Date today = Date();
    Date returnDate = today;
    returnDate.addDays(14);
    history.push_back(LoanInfo(original, today, returnDate));
    history.push_back(LoanInfo(original2, today, returnDate));
    history.push_back(LoanInfo(original3, today, returnDate));

    vector.push_back(original);
    vector.push_back(original2);
    vector.push_back(original3);

    Reader reader(username, password, 1, 1, 2002, 1, 1, 2002, vector,history);

    // 2. Сериализирай обекта
    std::ofstream ofs("reader_test.bin", std::ios::binary);
    assert(ofs.is_open());
    reader.serialize(ofs);
    ofs.close();

    // 3. Десериализирай в нов обект
    Reader loaded;
    std::ifstream ifs("reader_test.bin", std::ios::binary);
    assert(ifs.is_open());
    loaded.deserialize(ifs);
    ifs.close();

    assert(reader.getUsername() == loaded.getUsername());
    assert(reader.getRegisterDate().getDay() == loaded.getRegisterDate().getDay());
    assert(reader.getRegisterDate().getMonth() == loaded.getRegisterDate().getMonth());
    assert(reader.getRegisterDate().getYear() == loaded.getRegisterDate().getYear());
    assert(reader.getLastLoginDate().getDay() == loaded.getLastLoginDate().getDay());
    assert(reader.getLastLoginDate().getMonth() == loaded.getLastLoginDate().getMonth());
    assert(reader.getLastLoginDate().getYear() == loaded.getLastLoginDate().getYear());
    assert(reader.getHistory().size() == loaded.getHistory().size());
    assert(reader.getUnits().size() == loaded.getUnits().size());
}
*/

/*
void testAdministratorSerialization() {
    std::string username("AdminHere");
    std::string password("Admin12+");
    std::string email("adminstrator@abv.bg");
    Administrator admin(username, password, Date(), Date(), email);

    // 2. Сериализирай обекта
    std::ofstream ofs("admin_test.bin", std::ios::binary);
    assert(ofs.is_open());
    admin.serialize(ofs);
    ofs.close();

    // 3. Десериализирай в нов обект
    Administrator loaded;
    std::ifstream ifs("admin_test.bin", std::ios::binary);
    assert(ifs.is_open());
    loaded.deserialize(ifs);
    ifs.close();

    assert(admin.getEmail() == loaded.getEmail());
    assert(admin.getUsername() == loaded.getUsername());
    assert(admin.getLastLoginDate().getDay() == loaded.getLastLoginDate().getDay());
    assert(admin.getLastLoginDate().getMonth() == loaded.getLastLoginDate().getMonth());
    assert(admin.getLastLoginDate().getYear() == loaded.getLastLoginDate().getYear());
    assert(admin.getRegisterDate().getDay() == loaded.getRegisterDate().getDay());
    assert(admin.getRegisterDate().getMonth() == loaded.getRegisterDate().getMonth());
    assert(admin.getRegisterDate().getYear() == loaded.getRegisterDate().getYear());

}
*/

/*
int main() {
    // минава -> с лека корекция в позволяване на един конструктур в привейт частта
    //testLibraryUnitSerialization();
    // минава -> с лека корекция в позволяване на един конструктур в привейт частта
    // този тества и periodicals,books,series и reader,article,history...
    //testLibraryPersonSerialization();
    // също минава -> с лека корекция в позволяване на един конструктур в привейт частта
    //testAdministratorSerialization();
}
*/

void printMessage() {
	std::cout << "Welcome to our LibrarySystem !\n";
	std::cout << "\n";
	std::cout << "Here is a list of commands, that you can perform, based on different rights (some commands are only for admins) :\n";
	std::cout << "1. (help) - prints a help message !\n";
	std::cout << "2. (login) - Usage: login <username> <password> !\n";
	std::cout << "3. (logout) - Usage: logout \n";
	std::cout << "4. (exit) - end of the program !\n";
	std::cout << "5. (books all) - Usage: <books> <all> !\n";
	std::cout << "6. (periodicals all) -  Usage: <periodicals> <all> !\n";
	std::cout << "7. (series all) -  Usage: <series> <all> !\n";
	std::cout << "8. (all) -  Usage: <all> !\n";
	std::cout << "9. (info)  with ISBN/ISSN number -  Usage: <info> <ISBN/ISSN/optional> !\n";
	std::cout << "10. (find units) -  Usage: <find> <unit> <type> <option> <value> <sortKey> <sortOrder> <topCount> !\n";
	std::cout << "11. (find users) - Usage: <find> <user> <option> <value> !\n";
	std::cout << "12. (add unit) -  Usage: <add> <unit> <type>! !\n";
	std::cout << "13. (add user) -  Usage: <add> <user> <""> | <add> <user> <'admin'> !\n";
	std::cout << "14. (remove unit) with id -  Usage: <remove> <id> !\n";
	std::cout << "15. (remove user) with username -  Usage: <remove> <username> !\n";
	std::cout << "16. (change unit) with id -  Usage: <change> <unit> <id>! !\n";
	std::cout << "17. (change user) -  Usage: <change> <user> <""> |  <change> <user> <username> !\n";

	std::cout << "\n";
}

// Function to split a string into tokens
std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Process the command and call the appropriate function in the LibrarySystem
bool processCommand(const std::vector<std::string>& tokens, LibrarySystem& system) {
    if (tokens.empty()) {
        return true; // Empty command, just continue
    }

    const std::string& command = tokens[0];

    if (command == "exit") {
        system.exit();
        return false;
    }
    else if (command == "help") {
        printMessage();
    }
    else if (command == "login") {
        if (tokens.size() < 3) {
            std::cout << "Usage: login <username> <password>\n";
        } else {
            system.login(tokens[1], tokens[2]);
        }
    }
    else if (command == "logout") {
        system.logout();
    }
    else if (command == "books" && tokens.size() > 1 && tokens[1] == "all") {
        system.booksAll();
    }
    else if (command == "periodicals" && tokens.size() > 1 && tokens[1] == "all") {
        system.periodicalsAll();
    }
    else if (command == "series" && tokens.size() > 1 && tokens[1] == "all") {
        system.seriesAll();
    }
    else if (command == "all") {
        system.all();
    }
    else if (command == "info" && tokens.size() > 1) {
        system.listInfo(tokens[1]);
    }
    else if (command == "find" && tokens.size() > 1) {
        if (tokens[1] == "unit" && tokens.size() >= 4) {
            // Parse find units command
            // Example: find units books title "Harry Potter" year desc 5
            std::string type = tokens.size() > 2 ? tokens[2] : "";
            std::string option = tokens.size() > 3 ? tokens[3] : "";
            std::string value = tokens.size() > 4 ? tokens[4] : "";
            std::string sortKey = tokens.size() > 5 ? tokens[5] : "";
            std::string sortOrder = tokens.size() > 6 ? tokens[6] : "";
            int topCount = tokens.size() > 7 ? std::stoi(tokens[7]) : -1;
            
            system.findUnits(type, option, value, sortKey, sortOrder, topCount);
        }

        else if (tokens[1] == "user" && tokens.size() >= 4) {
            // Parse find users command
            std::string option = tokens[2];
            std::string value = tokens[3];
            system.findReaders(option, value);
        }
        else {
            std::cout << "Invalid find command format.\n";
            std::cout << "Use 'find unit [type] [option] [value] [sortKey] [sortOrder] [topCount]' or 'find user [option] [value]'\n";
            std::cout << "<option> is one of title, ID, state!" << "\n";
        }
    }
    else if (command == "add") {
        if (tokens.size() > 1 && tokens[1] == "unit") {
            // Parse unit type
            if (tokens.size() > 2) {
                Type unitType;
                if (tokens[2] == "book") {
                    unitType = Type::BOOK;
                } else if (tokens[2] == "series") {
                    unitType = Type::SERIES;
                } else if (tokens[2] == "periodicals") {
                    unitType = Type::PERIODICALS;
                } else {
                    std::cout << "Invalid unit type. Must be 'book', 'series', or 'periodicals'\n";
                    return true;
                }
                system.addUnit(unitType);
            } else {
                std::cout << "Usage: add unit <type>\n";
                std::cout << "Where type is one of: book, series, periodicals\n";
            }
        }
        else if (tokens.size() >= 2 && tokens[1] == "user") {
            bool isAdmin = false;
            if (tokens.size() >= 3 && tokens[2] == "admin") {
                isAdmin = true;
            }
            system.addUser(isAdmin);
        }
        else {
            std::cout << "Usage: add <unit|user> ...\n";
        }
    }
    else if (command == "remove") {
        if (tokens.size() > 2 && tokens[1] == "unit") {
            try {
                unsigned int id = std::stoul(tokens[2]);
                system.removeUnit(id);
            }
            catch (std::exception& e) {
                std::cerr << "Invalid ID. Must be a number.\n";
            }
        }
        else if (tokens.size() > 2 && tokens[1] == "user") {
            system.removeUser(tokens[2]);
        }
        else {
            std::cout << "Usage: remove <unit|user> <id|username>\n";
        }
    }
    else if (command == "change") {
        if (tokens.size() > 2 && tokens[1] == "unit") {
            try {
                unsigned int id = std::stoul(tokens[2]);
                system.changeUnit(id);
            }
            catch (std::exception& e) {
                std::cerr << "Invalid ID. Must be a number.\n";
            }
        }
        else if (tokens.size() > 2 && tokens[1] == "user") {
            std::string empty = "";
            if (tokens.size() > 3) {
                system.changeUser(tokens[2], tokens[3]);
            }
            else {
                system.changeUser(tokens[2], empty);
            }
        }
        else {
            std::cout << "Usage: change unit <id> or change user <new password> <target username> \n";
        }
    }
    else if (command == "take" && tokens.size() > 1) {
        try {
            unsigned int id = std::stoul(tokens[1]);
            system.take(id);
        }
        catch (std::exception& e) {
            std::cerr << "Invalid ID. Must be a number.\n";
        }
    }
    else if (command == "return" && tokens.size() > 1) {
        try {
            unsigned int id = std::stoul(tokens[1]);
            system.returnUnit(id);
        }
        catch (std::exception& e) {
            std::cerr << "Invalid ID. Must be a number.\n";
        }
    }
    else {
        std::cout << "Unknown command. Type 'help' for a list of commands.\n";
    }

    return true;
}

int main() {
    printMessage();

    try {
        IDGenerator::loadLastIdFromFile("last_id.dat");
        std::cout << IDGenerator::getLastId();

        LibrarySystem system("users.dat", "units.dat");

        bool continueRunning = true;
        std::string input;

        // Main command loop
        while (continueRunning) {
            std::cout << "\n> ";
            std::getline(std::cin, input);
            

            std::vector<std::string> tokens = tokenize(input);
            

            continueRunning = processCommand(tokens, system);
        }

        //// Save the system state before exiting
        //std::ofstream usersFile("users.dat", std::ios::binary);
        //std::ofstream unitsFile("units.dat", std::ios::binary);
        //
        //if (!usersFile.is_open() || !unitsFile.is_open()) {
        //    std::cerr << "Failed to open files for saving data.\n";
        //} else {
        //    system.serialize(usersFile, unitsFile);
        //}
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";

        return 1;
    }

    std::cout << "Thank you for using our Library System. Goodbye!\n";

    return 0;
}