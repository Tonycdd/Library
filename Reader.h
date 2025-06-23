#pragma once
#include "LibraryPerson.h"
#include "Book.h"
#include "Periodicals.h"
#include "Series.h"

// ���� ������������ ���������, ����� �� ���� �������� �� ������� ����  � �� ����� ���� � ����� � 
// ������� �������
struct LoanInfo {
	unsigned int unitId;       // ID �� ������� �������
	Date borrowDate;           // ���� �� �������
	Date returnDate;           // ���� �� ������� (��� � �������)
	bool returned = false;     // ���� ���� � �������

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
	// ���������� ������ ������ 6-����
	// 
	//�� ���������
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

	// �������������� �� ����������� �������
	virtual LibraryPerson* clone()const override;
	virtual void display(std::ostream& out = std::cout)const override;

	// ������
	inline const std::vector<unsigned int>& getUnits()const { return units; };
	inline std::vector<unsigned int>& getUnits() { return units; }; // ������ �� �� ��������� � ���������, �� � ����� �������� �����
	inline const std::vector<LoanInfo>& getHistory()const { return history; };


	// �������� � �������� �� �������� �� ���� ������� � ������� ������� � �������
	void addNewUnit(LibraryUnit* unit);
	virtual void borrow(LibraryUnit* unitToBorrow)override;
	virtual bool returnUnit(int id)override;

	// ������������, ��������������
	virtual void serialize(std::ostream& out)const override;
	virtual void deserialize(std::istream& is)override;
	inline virtual TypeOfReader getType()const override { return TypeOfReader::READER; };

	// ��������� ������ �� ������� ���� �� �������������, ����� �� ��������
	bool hasOverdueBooks(const Date& today)const;
	unsigned int borrowedLastMonth(const Date& today)const;
	int monthsSinceLastBorrow(const Date& today)const;
	bool hasBorrowedUnit(unsigned int unitId)const ; // ����� � ��������� - ����, ����� �� ���� ����� ������ �� ���� ����������

	// �������� ����� �� ������������ ����� �� ���������
	static Reader* createInteractively();

	virtual std::vector<int> getTakenIds()const override;
protected:
	Reader();
protected:
	// ����� �� ����������
	// ������ �� �� �� �����
	void serializeReaderUnit(std::ostream& out)const;
	void deserializeReaderUnit(std::istream& is);

private:
	std::vector<unsigned int> units; // ������������ ���������, ����� ����� ������� � ���� ���� �������
	std::vector<LoanInfo> history; // ����� �� units �� ���� ������� � history
};

std::ostream& operator<<(std::ostream& out, const Reader& other);
