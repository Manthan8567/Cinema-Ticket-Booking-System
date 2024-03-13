#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <memory>

using namespace std;

class Movie {
public:
    string title;
    string genre;
    int duration;
    string classification;
    vector<string> showtimes;
};

struct DecisionNode {
    string question;
    string genre;
    vector<Movie> movies;
    unique_ptr<DecisionNode> yesChild;
    unique_ptr<DecisionNode> noChild;
};

unique_ptr<DecisionNode> buildDecisionTree(const vector<Movie>& movies) {
    if (movies.empty()) {
        return nullptr;
    }

    string genre = movies.front().genre;
    if (all_of(movies.begin() + 1, movies.end(), [&](const Movie& m) { return m.genre == genre; })) {
        auto node = make_unique<DecisionNode>();
        node->question = "Do you want to watch a " + genre + " movie?";
        node->genre = genre;
        node->movies = movies;
        return node;
    }

    string bestQuestion;
    int bestQuestionScore = 0;
    vector<Movie> bestYesMovies, bestNoMovies;

    for (const auto& movie : movies) {
        vector<Movie> yesMovies, noMovies;
        for (const auto& otherMovie : movies) {
            if (otherMovie.genre == movie.genre) {
                yesMovies.push_back(otherMovie);
            }
            else {
                noMovies.push_back(otherMovie);
            }
        }

        int score = min(yesMovies.size(), noMovies.size());
        if (score > bestQuestionScore) {
            bestQuestionScore = score;
            bestQuestion = "Do you want to watch a " + movie.genre + " movie?";
            bestYesMovies = yesMovies;
            bestNoMovies = noMovies;
        }
    }

    auto node = make_unique<DecisionNode>();
    node->question = bestQuestion;
    node->genre = "";
    node->movies = {};
    node->yesChild = buildDecisionTree(bestYesMovies);
    node->noChild = buildDecisionTree(bestNoMovies);
    return node;
}

class Showtime {
private:
    string movieTitle;
    string cinemaHall;
    string startDateTime;
    string endDateTime;
    float price;

public:
    Showtime(string movieTitle, string cinemaHall, string startDateTime, string endDateTime, float price)
        : movieTitle(movieTitle), cinemaHall(cinemaHall), startDateTime(startDateTime), endDateTime(endDateTime), price(price) {}

    string getMovieTitle() const {
        return movieTitle;
    }

    string getCinemaHall() const {
        return cinemaHall;
    }

    string getStartDateTime() const {
        return startDateTime;
    }

    string getEndDateTime() const {
        return endDateTime;
    }

    float getPrice() const {
        return price;
    }

    void display() const {
        cout << "Movie title: " << movieTitle << endl;
        cout << "Cinema hall: " << cinemaHall << endl;
        cout << "Start time: " << startDateTime << endl;
        cout << "End time: " << endDateTime << endl;
        cout << "Price: $" << price << endl;
    };
};

class Reservation {
public:
    string movieTitle;
    string cinemaHall;
    int showtime;
    int numTickets = 0; // Initialize numTickets to zero
};

void searchMovies(const vector<Movie>& movies) {
    string searchCriteria;
    cout << "Please enter your search criteria: ";
    cin >> searchCriteria;

    vector<Movie> filteredMovies;
    for (const auto& movie : movies) {
        if (movie.title.find(searchCriteria) != string::npos ||
            movie.genre.find(searchCriteria) != string::npos ||
            movie.classification.find(searchCriteria) != string::npos) {
            filteredMovies.push_back(movie);
        }
    }

    if (filteredMovies.empty()) {
        cout << "No movies found. Please try again." << endl;
    }
    else {
        cout << "Available movies:" << endl;
        for (const auto& movie : filteredMovies) {
            cout << "- " << movie.title << " (" << movie.duration << " min, " << movie.classification << ")" << endl;
        }
    }
}

void reserveTicket(queue<Reservation>& reservations, const vector<Movie>& movies) {
    string movieTitle;
    cout << "Please enter the title of the movie you want to reserve: ";
    cin.ignore();
    getline(cin, movieTitle);

    auto movieIt = find_if(movies.begin(), movies.end(), [&](const Movie& movie) {
        return movie.title == movieTitle;
        });

    if (movieIt == movies.end()) {
        cout << "Movie not found. Please try again." << endl;
        return;
    }

    cout << "Available showtimes for " << movieTitle << ":" << endl;
    for (int i = 0; i < movieIt->showtimes.size(); i++) {
        cout << "- Showtime " << i + 1 << ": " << movieIt->showtimes[i] << endl;
    }

    string cinemaHall;
    int showtime;
    int numTickets;
    cout << "Please enter the cinema hall: ";
    getline(cin, cinemaHall);
    cout << "Please enter the showtime (1-" << movieIt->showtimes.size() << "): ";
    cin >> showtime;
    cout << "Please enter the number of tickets: ";
    cin >> numTickets;

    Reservation reservation;
    reservation.movieTitle = movieTitle;
    reservation.cinemaHall = cinemaHall;
    reservation.showtime = showtime;
    reservation.numTickets = numTickets;
    reservations.push(reservation);
}

void purchaseTicket(const vector<Movie>& movies) {
    string movieTitle;
    cout << "Please enter the title of the movie you want to purchase: ";
    cin.ignore();
    getline(cin, movieTitle);

    auto movieIt = find_if(movies.begin(), movies.end(), [&](const Movie& movie) {
        return movie.title == movieTitle;
        });

    if (movieIt == movies.end()) {
        cout << "Movie not found. Please try again." << endl;
        return;
    }

    cout << "Available showtimes for " << movieTitle << ":" << endl;
    for (int i = 0; i < movieIt->showtimes.size(); i++) {
        cout << "- Showtime " << i + 1 << ": " << movieIt->showtimes[i] << endl;
    }

    string cinemaHall;
    int showtime;
    int numTickets;
    cout << "Please enter the cinema hall: ";
    getline(cin, cinemaHall);
    cout << "Please enter the showtime (1-" << movieIt->showtimes.size() << "): ";
    cin >> showtime;
    cout << "Please enter the number of tickets: ";
    cin >> numTickets;

    float ticketPrice = 10.0f;
    float totalPrice = ticketPrice * numTickets;

    cout << "Total cost: $" << totalPrice << endl;
    string paymentMethod;
    cout << "Please choose a payment method (credit card, debit card, cash): ";
    cin.ignore();
    getline(cin, paymentMethod);

    cout << "Purchase successful. Enjoy the movie!" << endl;
}

void recommendationSystem(const vector<Movie>& movies) {
    auto root = buildDecisionTree(movies);
    auto currentNode = root.get();

    while (!currentNode->movies.empty()) {
        cout << currentNode->question << " (y/n): ";
        char answer;
        cin >> answer;
        if (answer == 'y') {
            currentNode = currentNode->yesChild.get();
        }
        else {
            currentNode = currentNode->noChild.get();
        }
    }

    cout << "Here are some movies you might like:" << endl;
    for (const auto& movie : currentNode->movies) {
        cout << movie.title << " (" << movie.genre << ")" << endl;
    }
}

void checkBookedTickets(const queue<Reservation>& reservations) {
    if (reservations.empty()) {
        cout << "No tickets have been booked yet." << endl;
        return;
    }

    cout << "Booked Tickets:" << endl;
    int count = 1;
    queue<Reservation> tempReservations = reservations; // Create a temporary copy of the queue
    while (!tempReservations.empty()) { // Iterate over the temporary queue
        const Reservation& reservation = tempReservations.front();
        cout << "Ticket " << count++ << ":" << endl;
        cout << "Movie Title: " << reservation.movieTitle << endl;
        cout << "Cinema Hall: " << reservation.cinemaHall << endl;
        cout << "Showtime: " << reservation.showtime << endl;
        cout << "Number of Tickets: " << reservation.numTickets << endl;
        tempReservations.pop(); // Move to the next element in the queue
    }
}


void mainMenu() {
    vector<Movie> movies = {
        {"Movie 1", "Action", 120, "PG-13", {"9:00 AM", "12:00 PM"}},
        {"Movie 2", "Comedy", 110, "PG", {"10:00 AM", "1:00 PM"}},
        {"Movie 3", "Drama", 130, "R", {"11:00 AM", "2:00 PM"}}
    };

    queue<Reservation> reservations;

    int choice;
    do {
        cout << "\nPlease select an option:" << endl;
        cout << "1. Search for available movies" << endl;
        cout << "2. Reserve a ticket" << endl;
        cout << "3. Purchase a ticket" << endl;
        cout << "4. Get movie recommendations" << endl;
        cout << "5. Check booked tickets" << endl;
        cout << "0. Quit" << endl;

        cin >> choice;

        switch (choice) {
        case 1:
            searchMovies(movies);
            break;
        case 2:
            reserveTicket(reservations, movies);
            break;
        case 3:
            purchaseTicket(movies);
            break;
        case 4:
            recommendationSystem(movies);
            break;
        case 5:
            checkBookedTickets(reservations);
            break;
        case 0:
            cout << "Thank you for using the cinema ticket reservation system. Goodbye!" << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }

    } while (choice != 0);
}

int main() {
    mainMenu();
    return 0;
}
