//flight reservation system main application file
#include "iostream"
#include "string"
#include "flightreservationsystem.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void printBanner() {
    cout << "\n";
    cout << "  |------------------------------------------|\n";
    cout << "  |       FLIGHT RESERVATION SYSTEM          |\n";
    cout << "  |         OOP Project  Spring 2026         |\n";
    cout << "  |------------------------------------------|\n\n";
}


FlightCatalog* catalog = nullptr;

void setupDemoFlights() {
    Seat* seats1[] = {
        new EconomySeat(1, false, 2000),
        new EconomySeat(2, false, 2000),
        new BusinessSeat(3, false, 8000),
        new FirstClassSeat(4, false, 10000),
        nullptr
    };
    Flight* f1 = new Flight("PK-101", "Karachi", "Islamabad",
                             "08:00 AM", seats1, 4, "On Time");

    Seat* seats2[] = {
        new EconomySeat(1, false, 2000),
        new EconomySeat(2, true,  2000),   
        new BusinessSeat(3, false, 8000),
        nullptr
    };
    Flight* f2 = new Flight("PK-202", "Lahore", "Dubai",
                             "02:00 PM", seats2, 3, "On Time");

    Seat* seats3[] = {
        new EconomySeat(1, false, 2000),
        new FirstClassSeat(2, false, 10000),
        nullptr
    };
    Flight* f3 = new Flight("PK-303", "Islamabad", "Karachi",
                             "06:00 PM", seats3, 2, "On Time");

    catalog = new FlightCatalog(20);
    catalog->addFlight(f1);
    catalog->addFlight(f2);
    catalog->addFlight(f3);
}

void passengerMenu() {
    Passenger passenger(1001, "Haris Muhammad", "haris@numl.edu.pk", "pass123", 25000.0f);
    int nextBookingID = 5001;
    LuckyDip luckyDip;
    FileManager fm;

    int choice;
    do {
        clearScreen();
        printBanner();
        cout << "  Welcome, " << passenger.getName() << "  (Balance: PKR " << passenger.getBalance() << ")\n\n";
        cout << "  |--- PASSENGER MENU --------------|\n";
        cout << "  | 1. Search Flights               |\n";
        cout << "  | 2. View All Flights             |\n";
        cout << "  | 3. Book a Seat                  |\n";
        cout << "  | 4. View My Bookings             |\n";
        cout << "  | 5. Cancel a Booking             |\n";
        cout << "  | 6. View Wallet Balance          |\n";
        cout << "  | 0. Back to Main Menu            |\n";
        cout << "  |---------------------------------|\n";
        cout << "  Choice: ";
        cin >> choice;

        if (choice == 1) {
            string origin, dest;
            cout << "Enter Origin      : "; cin >> origin;
            cout << "Enter Destination : "; cin >> dest;
            catalog->searchFlight(origin, dest);
            pressEnter();
        }
        else if (choice == 2) {
            catalog->displayAll();
            pressEnter();
        }
        else if (choice == 3) {
            string flightNo;
            int seatNo;
            cout << "Enter Flight Number : "; cin >> flightNo;

            Flight* flight = catalog->getFlightByNo(flightNo);
            if (!flight) { cout << "Flight not found.\n"; pressEnter(); continue; }
            if (flight->isCancelled()) {
                cout << "This flight is cancelled.\n"; pressEnter(); continue;
            }

            flight->displayAllSeats();
            cout << "Enter Seat Number   : "; cin >> seatNo;

            Seat* seat = flight->getSeatByNumber(seatNo);
            if (!seat) { cout << "Seat not found.\n"; pressEnter(); continue; }

            try {
                passenger.deductBalance(seat->calculateFare());

                char lucky;
                cout << "\n Try Lucky Dip for a free upgrade? (y/n): "; cin >> lucky;
                if (lucky == 'y' || lucky == 'Y')
                    seat = luckyDip.tryUpgrade(seat);

                Booking b(nextBookingID++, flight, &passenger, seat, "2026-05-01");
                b.bookTicket();
                passenger.addBooking(b);
                fm.saveBooking(b);

            } catch (const SeatUnavailableException& ex) {
                cout << "\n " << ex.what() << "\n";
            } catch (const InsufficientBalanceException& ex) {
                cout << "\n " << ex.what() << "\n";
            } catch (const FlightCancelledException& ex) {
                cout << "\n " << ex.what() << "\n";
            }
            pressEnter();
        }
        else if (choice == 4) {
            passenger.viewBookingHistory();
            pressEnter();
        }
        else if (choice == 5) {
            cout << "Cancel functionality available via booking history.\n";
            pressEnter();
        }
        else if (choice == 6) {
            cout << "\nWallet Balance: PKR " << passenger.getBalance() << "\n";
            pressEnter();
        }
    } while (choice != 0);
}

void employeeMenu() {
    Employee emp(2001, "Ali Ahad", "ali@numl.edu.pk", "emp123", "Operations", 80000.0f);

    int choice;
    do {
        clearScreen();
        printBanner();
        cout << "  Logged in as Employee: " << emp.getName() << "\n\n";
        cout << "  |--- EMPLOYEE MENU ----------------|\n";
        cout << "  |  1. View All Flights             |\n";
        cout << "  |  2. Cancel a Flight              |\n";
        cout << "  |  3. Reschedule a Flight          |\n";
        cout << "  |  4. My Profile                   |\n";
        cout << "  |  0. Back to Main Menu            |\n";
        cout << "  |----------------------------------|\n";
        cout << "  Choice: ";
        cin >> choice;

        if (choice == 1) {
            catalog->displayAll();
            pressEnter();
        }
        else if (choice == 2) {
            string flightNo;
            cout << "Enter Flight Number to Cancel: "; cin >> flightNo;
            Flight* flight = catalog->getFlightByNo(flightNo);
            if (!flight) { cout << "Flight not found.\n"; }
            else {
                Passenger* noPassengers[] = {};
                emp.cancelFlight(flight, noPassengers, 0);
            }
            pressEnter();
        }
        else if (choice == 3) {
            string flightNo, newTime;
            cout << "Enter Flight Number : "; cin >> flightNo;
            cout << "Enter New Time      : "; cin >> newTime;
            Flight* flight = catalog->getFlightByNo(flightNo);
            if (!flight) cout << "Flight not found.\n";
            else emp.rescheduleFlight(flight, newTime);
            pressEnter();
        }
        else if (choice == 4) {
            emp.displayProfile();
            pressEnter();
        }
    } while (choice != 0);
}

void adminMenu() {
    Admin admin(3001, "System Admin", "admin@flightapp.com", "admin123");

    int choice;
    do {
        clearScreen();
        printBanner();
        cout << "  Logged in as: ADMIN\n\n";
        cout << "  |--- ADMIN MENU -------------------|\n";
        cout << "  |  1. View All Flights             |\n";
        cout << "  |  2. Add a New Flight             |\n";
        cout << "  |  3. Remove a Flight              |\n";
        cout << "  |  4. View System Report           |\n";
        cout << "  |  0. Back to Main Menu            |\n";
        cout << "  -----------------------------------\n";
        cout << "  Choice: ";
        cin >> choice;

        if (choice == 1) {
            catalog->displayAll();
            pressEnter();
        }
        else if (choice == 2) {
            string fNo, origin, dest, time, status;
            int    seatCount, stype;
            cout << "Flight Number    : "; cin >> fNo;
            cout << "Origin           : "; cin >> origin;
            cout << "Destination      : "; cin >> dest;
            cout << "Departure Time   : "; cin >> time;
            cout << "Number of Seats  : "; cin >> seatCount;

            Seat** newSeats = new Seat*[seatCount];
            for (int i = 0; i < seatCount; i++) {
                cout << "Seat " << (i+1) << " type (1=Economy, 2=Business, 3=FirstClass): ";
                cin >> stype;
                float price = (stype == 1) ? 2000 : (stype == 2) ? 8000 : 10000;
                if      (stype == 1) newSeats[i] = new EconomySeat(i+1, false, price);
                else if (stype == 2) newSeats[i] = new BusinessSeat(i+1, false, price);
                else                 newSeats[i] = new FirstClassSeat(i+1, false, price);
            }

            Flight* f = new Flight(fNo, origin, dest, time, newSeats, seatCount, "On Time");
            delete[] newSeats; 
            admin.addFlight(*catalog, f);
            pressEnter();
        }
        else if (choice == 3) {
            string flightNo;
            cout << "Enter Flight Number to Remove: "; cin >> flightNo;
            admin.removeFlight(*catalog, flightNo);
            pressEnter();
        }
        else if (choice == 4) {
            admin.viewSystemReport(*catalog);
            pressEnter();
        }
    } while (choice != 0);
}

bool loginGate(const string& role) {
    int    id;
    string pass;
    cout << "\n  Enter ID       : "; cin >> id;
    cout << "  Enter Password : "; cin >> pass;

    if (role == "Passenger" && id == 1001 && pass == "pass123") return true;
    if (role == "Employee"  && id == 2001 && pass == "emp123")  return true;
    if (role == "Admin"     && id == 3001 && pass == "admin123") return true;

    cout << "\n   Invalid credentials.\n";
    return false;
}

int main() {
    setupDemoFlights();

    cout << "\n  Demo Credentials:\n";
    cout << "  Passenger  ID: 1001, Pass: pass123\n";
    cout << "  Employee   ID: 2001, Pass: emp123\n";
    cout << "  Admin      ID: 3001, Pass: admin123\n";
    pressEnter();

    int choice;
    do {
        clearScreen();
        printBanner();
        cout << "  |---SELECT ROLE ------------------|\n";
        cout << "  |  1. Passenger                   |\n";
        cout << "  |  2. Employee                    |\n";
        cout << "  |  3. Admin                       |\n";
        cout << "  |  0. Exit                        |\n";
        cout << "  |---------------------------------|\n";
        cout << "  Choice: ";
        cin >> choice;

        if (choice == 1) {
            if (loginGate("Passenger")) passengerMenu();
            else pressEnter();
        }
        else if (choice == 2) {
            if (loginGate("Employee")) employeeMenu();
            else pressEnter();
        }
        else if (choice == 3) {
            if (loginGate("Admin")) adminMenu();
            else pressEnter();
        }
        else if (choice != 0) {
            cout << "  Invalid option.\n";
            pressEnter();
        }
    } while (choice != 0);

    delete catalog;
    cout << "\n  Thank you for using Flight Reservation System. Goodbye! ✈\n\n";
    return 0;
}