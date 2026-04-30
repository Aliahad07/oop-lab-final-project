#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
using namespace std;

class SeatUnavailableException : public exception
{
    string msg;

public:
    SeatUnavailableException(int seatNo)
        : msg("Seat " + to_string(seatNo) + " is already booked.") {}

    const char *what() const noexcept override { return msg.c_str(); }
};

class FlightCancelledException : public exception
{
    string msg;

public:
    FlightCancelledException(const string &flightNo)
        : msg("Flight " + flightNo + " has been cancelled.") {}

    const char *what() const noexcept override { return msg.c_str(); }
};

class InsufficientBalanceException : public exception
{
public:
    const char *what() const noexcept override
    {
        return "Insufficient wallet balance to complete this booking.";
    }
};

class FlightNotFoundException : public exception
{
public:
    const char *what() const noexcept override
    {
        return "No matching flight found.";
    }
};

class Printable
{
public:
    virtual void print() const = 0;
    virtual ~Printable() {}
};

class Searchable
{
public:
    virtual void searchFlight(const string &origin, const string &destination) = 0;
    virtual ~Searchable() {}
};

class Refundable
{
public:
    virtual float calculateRefund() const = 0;
    virtual void processRefund() = 0;
    virtual ~Refundable() {}
};

class ReservationBase
{
public:
    virtual void bookTicket() = 0;
    virtual void cancelTicket() = 0;
    virtual ~ReservationBase() {}
};

class NotificationBase
{
public:
    virtual void notify(const string &message) = 0;
    virtual ~NotificationBase() {}
};

class Person
{
protected:
    int id;
    string name;
    string email;
    string password;

public:
    Person(int i, string n, string e, string p)
        : id(i), name(n), email(e), password(p) {}

    bool login(int uid, const string &pass) const
    {
        if (uid == this->id && this->password == pass)
        {
            cout << "\n-------- Logged In! Welcome, " << name << " --------\n";
            return true;
        }
        cout << "\n-------- Incorrect ID or Password! --------\n";
        return false;
    }

    void logout() const
    {
        char confirm;
        cout << "Do you really want to logout? (y/n): ";
        cin >> confirm;
        if (confirm == 'y' || confirm == 'Y')
            cout << "-------- Logged Out! --------\n";
    }

    int getId() const { return id; }
    string getName() const { return name; }
    string getEmail() const { return email; }

    virtual string getRole() const = 0;
    virtual ~Person() {}
};

class Seat
{
public:
    int seatNo;
    bool isBooked;
    float price;

    Seat(int n, bool i, float p) : seatNo(n), isBooked(i), price(p) {}

    virtual float calculateFare() const = 0;
    virtual Seat *upgrade(int n, bool i, float p) = 0;
    virtual string getSeatType() const = 0;
    virtual void display() const = 0;

    virtual ~Seat() {}
};

class BusinessSeat;
class FirstClassSeat;

class EconomySeat : public Seat
{
public:
    EconomySeat(int n, bool i, float p) : Seat(n, i, p) {}

    float calculateFare() const override { return 2000.00f; }
    string getSeatType() const override { return "Economy"; }
    Seat *upgrade(int n, bool i, float p) override;

    void display() const override
    {
        cout << "\n-------- Seat Details --------\n";
        cout << "Type         : Economy\n";
        cout << "Seat Number  : " << seatNo << "\n";
        cout << "Status       : " << (isBooked ? "Booked" : "Available") << "\n";
        cout << "Fare         : PKR " << calculateFare() << "\n";
        cout << "------------------------------\n";
    }
};

class BusinessSeat : public Seat
{
public:
    BusinessSeat(int n, bool i, float p) : Seat(n, i, p) {}

    float calculateFare() const override { return 8000.00f; }
    string getSeatType() const override { return "Business"; }
    Seat *upgrade(int n, bool i, float p) override;

    void display() const override
    {
        cout << "\n-------- Seat Details --------\n";
        cout << "Type         : Business\n";
        cout << "Seat Number  : " << seatNo << "\n";
        cout << "Status       : " << (isBooked ? "Booked" : "Available") << "\n";
        cout << "Fare         : PKR " << calculateFare() << "\n";
        cout << "------------------------------\n";
    }
};

class FirstClassSeat : public Seat
{
public:
    FirstClassSeat(int n, bool i, float p) : Seat(n, i, p) {}

    float calculateFare() const override { return 10000.00f; }
    string getSeatType() const override { return "First Class"; }
    Seat *upgrade(int n, bool i, float p) override { return nullptr; }

    void display() const override
    {
        cout << "\n-------- Seat Details --------\n";
        cout << "Type         : First Class\n";
        cout << "Seat Number  : " << seatNo << "\n";
        cout << "Status       : " << (isBooked ? "Booked" : "Available") << "\n";
        cout << "Fare         : PKR " << calculateFare() << "\n";
        cout << "------------------------------\n";
    }
};

inline Seat *EconomySeat::upgrade(int n, bool i, float p) { return new BusinessSeat(n, i, p); }
inline Seat *BusinessSeat::upgrade(int n, bool i, float p) { return new FirstClassSeat(n, i, p); }

class LuckyDip
{
    int winChance;

public:
    LuckyDip(int chance = 5) : winChance(chance) { srand((unsigned)time(nullptr)); }

    bool spin() const
    {
        int roll = rand() % winChance;
        return roll == 0;
    }

    Seat *tryUpgrade(Seat *current)
    {
        if (!spin())
        {
            cout << "\n Lucky Dip: Better luck next time! Your seat is unchanged.\n";
            return current;
        }
        Seat *upgraded = current->upgrade(current->seatNo, current->isBooked, current->price);
        if (!upgraded)
        {
            cout << "\n Lucky Dip: You're already in First Class — you won anyway!\n";
            return current;
        }
        cout << "\n Lucky Dip: CONGRATULATIONS! Upgraded from "
             << current->getSeatType() << " to " << upgraded->getSeatType() << " for FREE!\n";
        delete current;
        return upgraded;
    }
};

class Flight : public Printable
{
private:
    string flightNo;
    string origin;
    string destination;
    string departureTime;
    Seat **seats;
    int seatCount;
    string status;

public:
    Flight(string f, string o, string d, string t,
           Seat *seatsArr[], int count, string s)
        : flightNo(f), origin(o), destination(d),
          departureTime(t), seatCount(count), status(s)
    {
        seats = new Seat *[seatCount];
        for (int i = 0; i < seatCount; i++)
            seats[i] = seatsArr[i];
    }

    void print() const override { displayFlightDetails(); }

    void displayFlightDetails() const
    {
        cout << "\n-------- Flight Details --------\n";
        cout << "Flight No    : " << flightNo << "\n";
        cout << "Origin       : " << origin << "\n";
        cout << "Destination  : " << destination << "\n";
        cout << "Departure    : " << departureTime << "\n";
        cout << "Status       : " << status << "\n";
        cout << "Available    : " << getAvailableSeats() << " seats\n";
        cout << "--------------------------------\n";
    }

    void displayAllSeats() const
    {
        for (int i = 0; i < seatCount; i++)
            if (seats[i])
                seats[i]->display();
    }

    string getFlightNo() const { return flightNo; }
    string getOrigin() const { return origin; }
    string getDestination() const { return destination; }
    string getStatus() const { return status; }

    int getAvailableSeats() const
    {
        int count = 0;
        for (int i = 0; i < seatCount; i++)
            if (seats[i] && !seats[i]->isBooked)
                count++;
        return count;
    }

    Seat *getSeatByNumber(int no) const
    {
        for (int i = 0; i < seatCount; i++)
            if (seats[i] && seats[i]->seatNo == no)
                return seats[i];
        return nullptr;
    }

    void updateStatus(const string &s) { status = s; }

    bool isCancelled() const { return status == "Cancelled"; }

    ~Flight()
    {
        for (int i = 0; i < seatCount; i++)
            delete seats[i];
        delete[] seats;
    }
};

class FlightCatalog : public Searchable
{
private:
    Flight **flights;
    int capacity;
    int flightCount;

public:
    FlightCatalog(int cap = 50) : capacity(cap), flightCount(0)
    {
        flights = new Flight *[capacity]();
    }

    void addFlight(Flight *flight)
    {
        if (flightCount >= capacity)
        {
            cout << "Catalog is full!\n";
            return;
        }
        flights[flightCount++] = flight;
        cout << "Flight " << flight->getFlightNo() << " added to catalog.\n";
    }

    void removeFlight(const string &flightNo)
    {
        for (int i = 0; i < flightCount; i++)
        {
            if (flights[i] && flights[i]->getFlightNo() == flightNo)
            {
                delete flights[i];
                flights[i] = flights[--flightCount];
                flights[flightCount] = nullptr;
                cout << "Flight " << flightNo << " removed.\n";
                return;
            }
        }
        cout << "Flight " << flightNo << " not found.\n";
    }

    void searchFlight(const string &origin, const string &destination) override
    {
        cout << "\n-------- Search Results --------\n";
        bool found = false;
        for (int i = 0; i < flightCount; i++)
        {
            if (flights[i] &&
                flights[i]->getOrigin() == origin &&
                flights[i]->getDestination() == destination &&
                !flights[i]->isCancelled())
            {
                flights[i]->displayFlightDetails();
                found = true;
            }
        }
        if (!found)
            cout << "No flights found from " << origin << " to " << destination << ".\n";
        cout << "--------------------------------\n";
    }

    Flight *getFlightByNo(const string &flightNo) const
    {
        for (int i = 0; i < flightCount; i++)
            if (flights[i] && flights[i]->getFlightNo() == flightNo)
                return flights[i];
        return nullptr;
    }

    void displayAll() const
    {
        if (flightCount == 0)
        {
            cout << "No flights in catalog.\n";
            return;
        }
        for (int i = 0; i < flightCount; i++)
            if (flights[i])
                flights[i]->displayFlightDetails();
    }

    int getCount() const { return flightCount; }

    ~FlightCatalog()
    {
        for (int i = 0; i < flightCount; i++)
            delete flights[i];
        delete[] flights;
    }
};

class Passenger;

class Booking : public ReservationBase, public Refundable, public Printable
{
    int bookingID;
    Flight *flight;
    Passenger *passenger;
    Seat *seat;
    string bookingDate;
    bool active;

public:
    Booking() : bookingID(0), flight(nullptr), passenger(nullptr),
                seat(nullptr), bookingDate(""), active(false) {}

    Booking(int id, Flight *f, Passenger *p, Seat *s, const string &date)
        : bookingID(id), flight(f), passenger(p),
          seat(s), bookingDate(date), active(false) {}

    void bookTicket() override
    {
        if (!seat)
            throw SeatUnavailableException(0);
        if (seat->isBooked)
            throw SeatUnavailableException(seat->seatNo);
        if (flight->isCancelled())
            throw FlightCancelledException(flight->getFlightNo());
        seat->isBooked = true;
        active = true;
        cout << "\n-------- Booking Confirmed! --------\n";
        cout << "Booking ID   : " << bookingID << "\n";
        cout << "Flight       : " << flight->getFlightNo() << "\n";
        cout << "Seat         : " << seat->seatNo << " (" << seat->getSeatType() << ")\n";
        cout << "Fare         : PKR " << seat->calculateFare() << "\n";
        cout << "------------------------------------\n";
    }

    void cancelTicket() override
    {
        if (!active || !seat->isBooked)
        {
            cout << "No active booking to cancel.\n";
            return;
        }
        seat->isBooked = false;
        active = false;
        cout << "\n-------- Booking Cancelled! --------\n";
        cout << "Refund       : PKR " << calculateRefund() << " will be credited.\n";
        cout << "------------------------------------\n";
    }

    float calculateRefund() const override
    {
        if (!seat)
            return 0.0f;
        return seat->calculateFare() * 0.80f;
    }

    void processRefund() override
    {
        cout << "Processing refund of PKR " << calculateRefund() << " ...\n";
        cout << "Refund credited to wallet.\n";
    }

    void print() const override { generateTicket(); }

    void generateTicket() const;

    int getBookingID() const { return bookingID; }
    bool isActive() const { return active; }
    Seat *getSeat() const { return seat; }
    Flight *getFlight() const { return flight; }

    void applySeatUpgrade(Seat *newSeat) { seat = newSeat; }
};

class Passenger : public Person, public NotificationBase, public Searchable
{
private:
    Booking *bookingHistory;
    int bookingCount;
    int bookingCapacity;
    float walletBalance;

public:
    Passenger(int i, const string &n, const string &e,
              const string &p, float w)
        : Person(i, n, e, p), walletBalance(w),
          bookingCount(0), bookingCapacity(10)
    {
        bookingHistory = new Booking[bookingCapacity];
    }

    string getRole() const override { return "Passenger"; }

    void notify(const string &message) override
    {
        cout << "\n[Notification for " << name << "]: " << message << "\n";
    }

    void searchFlight(const string &origin, const string &destination) override
    {
        cout << "\n-------- Searching: " << origin << " → " << destination << " --------\n";
    }

    void addBooking(const Booking &b)
    {
        if (bookingCount >= bookingCapacity)
        {
            cout << "Booking history full.\n";
            return;
        }
        bookingHistory[bookingCount++] = b;
    }

    void viewBookingHistory() const
    {
        if (bookingCount == 0)
        {
            cout << "No bookings yet.\n";
            return;
        }
        for (int i = 0; i < bookingCount; i++)
            bookingHistory[i].generateTicket();
    }

    bool deductBalance(float amount)
    {
        if (walletBalance < amount)
            throw InsufficientBalanceException();
        walletBalance -= amount;
        return true;
    }

    void creditBalance(float amount) { walletBalance += amount; }

    float getBalance() const { return walletBalance; }

    ~Passenger() { delete[] bookingHistory; }
};

inline void Booking::generateTicket() const
{
    cout << "\n====== TICKET ======\n";
    cout << "Booking ID   : " << bookingID << "\n";
    cout << "Flight       : " << (flight ? flight->getFlightNo() : "N/A") << "\n";
    cout << "Passenger    : " << (passenger ? passenger->getName() : "N/A") << "\n";
    cout << "Seat         : " << (seat ? to_string(seat->seatNo) + " (" + seat->getSeatType() + ")" : "N/A") << "\n";
    cout << "Date         : " << bookingDate << "\n";
    cout << "Fare         : PKR " << (seat ? seat->calculateFare() : 0.0f) << "\n";
    cout << "Status       : " << (active ? "Active" : "Cancelled") << "\n";
    cout << "====================\n";
}

class Employee : public Person, public NotificationBase
{
private:
    string department;
    float salary;

public:
    Employee(int i, const string &n, const string &e,
             const string &p, const string &dept, float sal)
        : Person(i, n, e, p), department(dept), salary(sal) {}

    string getRole() const override { return "Employee"; }

    void notify(const string &message) override
    {
        cout << "\n [Employee Notification]: " << message << "\n";
    }

    void viewAllBookings(Booking bookings[], int count) const
    {
        cout << "\n-------- All Bookings --------\n";
        if (count == 0)
        {
            cout << "No bookings found.\n";
            return;
        }
        for (int i = 0; i < count; i++)
            bookings[i].generateTicket();
    }

    void cancelFlight(Flight *flight, Passenger *passengers[], int passengerCount)
    {
        if (!flight)
            return;
        flight->updateStatus("Cancelled");
        cout << "Flight " << flight->getFlightNo() << " has been cancelled.\n";
        for (int i = 0; i < passengerCount; i++)
        {
            if (passengers[i])
            {
                passengers[i]->notify("Flight " + flight->getFlightNo() +
                                      " has been cancelled. Your refund is being processed.");
            }
        }
    }

    void rescheduleFlight(Flight *flight, const string &newTime)
    {
        if (!flight)
            return;
        flight->updateStatus("Rescheduled  " + newTime);
        cout << "Flight " << flight->getFlightNo() << " rescheduled to " << newTime << ".\n";
    }

    void displayProfile() const
    {
        cout << "\n-------- Employee Profile --------\n";
        cout << "ID           : " << id << "\n";
        cout << "Name         : " << name << "\n";
        cout << "Department   : " << department << "\n";
        cout << "--------------------------------\n";
    }
};

class Admin : public Person
{
public:
    Admin(int i, const string &n, const string &e, const string &p)
        : Person(i, n, e, p) {}

    string getRole() const override { return "Admin"; }

    void addFlight(FlightCatalog &catalog, Flight *flight)
    {
        catalog.addFlight(flight);
    }

    void removeFlight(FlightCatalog &catalog, const string &flightNo)
    {
        catalog.removeFlight(flightNo);
    }

    void viewSystemReport(const FlightCatalog &catalog) const
    {
        cout << "\n====== System Report ======\n";
        cout << "Total flights in catalog: " << catalog.getCount() << "\n";
        catalog.displayAll();
        cout << "===========================\n";
    }

    void displayProfile() const
    {
        cout << "\n-------- Admin Profile --------\n";
        cout << "ID   : " << id << "\n";
        cout << "Name : " << name << "\n";
        cout << "-------------------------------\n";
    }
};

class FileManager
{
    string bookingsFile;
    string flightsFile;

public:
    FileManager(const string &bf = "bookings.txt",
                const string &ff = "flights.txt")
        : bookingsFile(bf), flightsFile(ff) {}

    void saveBooking(const Booking &b)
    {
        ofstream out(bookingsFile, ios::app);
        if (!out)
        {
            cout << "Error opening bookings file.\n";
            return;
        }
        out << b.getBookingID() << ","
            << (b.getFlight() ? b.getFlight()->getFlightNo() : "N/A") << ","
            << (b.getSeat() ? b.getSeat()->seatNo : 0)
            << "\n";
        out.close();
        cout << "Booking saved to file.\n";
    }

    void loadBookings() const
    {
        ifstream in(bookingsFile);
        if (!in)
        {
            cout << "No booking records found.\n";
            return;
        }
        string line;
        cout << "\n-------- Saved Bookings --------\n";
        while (getline(in, line))
            cout << line << "\n";
        in.close();
    }

    void saveFlight(const Flight &f)
    {
        ofstream out(flightsFile, ios::app);
        if (!out)
        {
            cout << "Error opening flights file.\n";
            return;
        }
        out << f.getFlightNo() << ","
            << f.getOrigin() << ","
            << f.getDestination() << "\n";
        out.close();
    }
};