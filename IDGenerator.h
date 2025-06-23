#pragma once
#include <fstream>
#include <string>

class IDGenerator
{
private:
    static unsigned int lastId;

public:
    // Връща ново уникално ID
    static int getNextId() {
        return ++lastId;
    }

    // Ръчно задаване (при зареждане на обекти от файл)
    static void setLastId(int id) {
        if (id > lastId)
            lastId = id;
    }

    // Зареждане от бинарен файл
    static void loadLastIdFromFile(const std::string& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open()) return;

        in.read(reinterpret_cast<char*>(&lastId), sizeof(lastId));
        if (!in) {
            throw std::ios_base::failure("Error with file in IDGenerator!");
            in.close();
        }
        in.close();
    }

    // Запис в бинарен файл
    static void saveLastIdToFile(const std::string& path) {
        std::ofstream out(path, std::ios::binary);
        if (!out.is_open()) return;

        out.write(reinterpret_cast<const char*>(&lastId), sizeof(lastId));
        if (!out) {
            throw std::ios_base::failure("Error with file in IDGenerator!");
            out.close();
        }
        out.close();
    }

    // Достъп до текущото (без да го увеличаваме)
    static int getLastId() {
        return lastId;
    }
};

