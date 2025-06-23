#pragma once
#include "LibraryPerson.h"
#include "Book.h"
#include "Periodicals.h"
#include "Series.h"

// една допълнителна структура, която да пази указател от базовия клас  и да помни кога е заета и 
// върната книгата
struct LoanInfo {
	unsigned int unitId;       // ID на заетата единица
	Date borrowDate;           // Дата на заемане
	Date returnDate;           // Дата на връщане (ако е върната)
	bool returned = false;     // Флаг дали е върната

	LoanInfo() = default;

	LoanInfo(unsigned int unitId, const Date& borrow, const Date& due, bool isReturned = false)
		: unitId(unitId), borrowDate(borrow), returnDate(due), returned(isReturned) {}

	void markReturned(const Date& date) {
		returnDate = date;
		returned = true;
	}

	void serialize(std::ostream& out) const {
		out.write(reinterpret_cast<const char*>(&unitId), sizeof(unitId));
		if (!out.good())
			throw std::ios_base::failure("Error writing unitId in LoanInfo");

		borrowDate.serialize(out);
		if (!out.good())
			throw std::ios_base::failure("Error writing borrowDate in LoanInfo");

		returnDate.serialize(out);
		if (!out.good())
			throw std::ios_base::failure("Error writing returnDate in LoanInfo");

		out.write(reinterpret_cast<const char*>(&returned), sizeof(returned));
		if (!out.good())
			throw std::ios_base::failure("Error writing returned flag in LoanInfo");
	}

	void deserialize(std::istream& in) {
		in.read(reinterpret_cast<char*>(&unitId), sizeof(unitId));
		if (!in.good())
			throw std::ios_base::failure("Error reading unitId in LoanInfo");

		borrowDate.deserialize(in);
		if (!in.good())
			throw std::ios_base::failure("Error reading borrowDate in LoanInfo");

		returnDate.deserialize(in);
		if (!in.good())
			throw std::ios_base::failure("Error reading returnDate in LoanInfo");

		in.read(reinterpret_cast<char*>(&returned), sizeof(returned));
		if (!in.good())
			throw std::ios_base::failure("Error reading returned flag in LoanInfo");
	}

	bool isReturned() const {
		return returned;
	}

	unsigned int getUnitId() const {
		return unitId;
	}
};

class Reader: public LibraryPerson
{
public:
	// аналогично правим голяма 6-тица
	// 
	//за фабриката
	Reader(std::istream& is)
	{
		deserialize(is);
	}

	Reader(const std::string& name, const std::string& pass, int day1, int month1, int year1,
		int day2, int month2, int year2, const std::vector<unsigned int>& v = {},
		const std::vector<LoanInfo>& history = {});
	Reader(const Reader& other);
	Reader& operator=(const Reader& other);
	Reader(Reader&& other)noexcept;
	Reader& operator=(Reader&& other)noexcept;
	virtual ~Reader() noexcept override;

	// имплементираме си вируталните функции
	virtual LibraryPerson* clone()const override;
	virtual void display(std::ostream& out = std::cout)const override;

	// гетъри
	inline const std::vector<unsigned int>& getUnits()const { return units; };
	inline std::vector<unsigned int>& getUnits() { return units; }; // трябва ми за промяната в системата, не е добра практика обаче
	inline const std::vector<LoanInfo>& getHistory()const { return history; };


	// свързани с логиката за добавяне на нови единици и тяхното взимане и връщане
	void addNewUnit(LibraryUnit* unit);
	virtual void borrow(LibraryUnit* unitToBorrow)override;
	virtual bool returnUnit(int id)override;

	// сериализация, десериализация
	virtual void serialize(std::ostream& out)const override;
	virtual void deserialize(std::istream& is)override;
	inline virtual TypeOfReader getType()const override { return TypeOfReader::READER; };

	// специални методи за търесне само за потребителите, които са читатели
	bool hasOverdueBooks(const Date& today)const;
	unsigned int borrowedLastMonth(const Date& today)const;
	int monthsSinceLastBorrow(const Date& today)const;
	bool hasBorrowedUnit(unsigned int unitId)const ; // търси в историята - тези, които са били заети някога от този потребител

	// статичен метод за интерактивен режим за фабриката
	static Reader* createInteractively();

	virtual std::vector<int> getTakenIds()const override;
protected:
	Reader();
protected:
	// важно за наследници
	// засега не са от полза
	void serializeReaderUnit(std::ostream& out)const;
	void deserializeReaderUnit(std::istream& is);

private:
	std::vector<unsigned int> units; // полиморфичен контейнър, пазещ какви единици е взел този читател
	std::vector<LoanInfo> history; // всеки от units ще бъде свързан с history
};

std::ostream& operator<<(std::ostream& out, const Reader& other);
