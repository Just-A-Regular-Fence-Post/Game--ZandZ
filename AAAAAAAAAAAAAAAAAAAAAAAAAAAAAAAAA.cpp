#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

class Media {
public:
    std::string title;
    int year;

    Media() : title(""), year(0) {} // initialize at runtime

    Media(std::string title, int year) : title(title), year(year) {}

    // Virtual print function so derived classes can override it
    virtual void print() const {
        std::cout << "Title: " << title << "\nYear: " << year << std::endl;
    }

    // Virtual destructor ensures proper cleanup when deleting derived objects
    virtual ~Media() {}
};

class VideoGame : public Media {
public:
    std::string genre;
    double playTime;

    VideoGame() : Media(), genre(""), playTime(0.0) {}

    VideoGame(std::string title, int year, std::string genre, double playTime)
        : Media(title, year), genre(genre), playTime(playTime) {}

    void print() const override {
        Media::print();
        std::cout << "Genre: " << genre << "\nPlay Time: " << playTime << " hrs" << std::endl;
    }
};

class Movie : public Media {
public:
    double runTime;
    std::string actors;

    Movie() : Media(), runTime(0.0), actors("") {}
    Movie(std::string title, int year, double runTime, std::string actors)
        : Media(title, year), runTime(runTime), actors(actors) {}

    void print() const override {
        Media::print();
        std::cout << "Run Time: " << runTime << " mins\nActors: " << actors << std::endl;
    }
};

class Music : public Media {
public:
    std::string lyrics;
    double songTime;

    Music() : Media(), lyrics(""), songTime(0.0) {}
    Music(std::string title, int year, std::string lyrics, double songTime)
        : Media(title, year), lyrics(lyrics), songTime(songTime) {}

    void print() const override {
        Media::print();
        std::cout << "Lyrics: " << lyrics << "\nSong Time: " << songTime << " mins" << std::endl;
    }
};

// Vector to store media objects using smart pointers
std::vector<std::shared_ptr<Media>> library;

void addMedia() {
    int type;
    std::cout << "\n=== Add Media ===\n";
    std::cout << "1. Video Game\n2. Movie\n3. Music\n> ";
    std::cin >> type;
    std::cin.ignore(); // Clear leftover newline from input buffer

    std::string title;
    int year;

    // Collect shared input fields
    std::cout << "Enter title: ";
    std::getline(std::cin, title);
    std::cout << "Enter year: ";
    std::cin >> year;
    std::cin.ignore();

    // Check which type of media to create
    if (type == 1) {
        std::string genre;
        double playTime;

        std::cout << "Enter genre: ";
        std::getline(std::cin, genre);
        std::cout << "Enter play time (hours): ";
        std::cin >> playTime;

        library.push_back(std::make_shared<VideoGame>(title, year, genre, playTime));
        std::cout << "Video game added!\n";
    }
    else if (type == 2) {
        double runTime;
        std::string actors;

        std::cout << "Enter runtime (minutes): ";
        std::cin >> runTime;
        std::cin.ignore();
        std::cout << "Enter main actors: ";
        std::getline(std::cin, actors);

        library.push_back(std::make_shared<Movie>(title, year, runTime, actors));
        std::cout << "Movie added!\n";
    }
    else if (type == 3) {
        std::string lyrics;
        double songTime;

        std::cout << "Enter lyrics (short excerpt): ";
        std::getline(std::cin, lyrics);
        std::cout << "Enter song time (minutes): ";
        std::cin >> songTime;

        library.push_back(std::make_shared<Music>(title, year, lyrics, songTime));
        std::cout << "Music added!\n";
    }
    else {
        std::cout << "Invalid type selected.\n";
    }
}

void removeMedia() {
    if (library.empty()) {
        std::cout << "No media to remove.\n";
        return;
    }

    std::cout << "\n=== Remove Media ===\n";
    for (size_t i = 0; i < library.size(); ++i) {
        // Display index + title + year
        std::cout << i + 1 << ". " << library[i]->title << " (" << library[i]->year << ")\n";
    }

    int index;
    std::cout << "Enter number of media to remove: ";
    std::cin >> index;

    // Validate index
    if (index < 1 || index > static_cast<int>(library.size())) {
        std::cout << "Invalid index.\n";
        return;
    }

    // Remove item from vector
    library.erase(library.begin() + (index - 1));
    std::cout << "Media removed successfully.\n";
}

void listMedia() {
    if (library.empty()) {
        std::cout << "No media in library yet.\n";
    } else {
        std::cout << "\n=== ALL MEDIA ===\n";
        for (size_t i = 0; i < library.size(); i++) {
            std::cout << "\n[" << i + 1 << "]\n";
            library[i]->print(); // Polymorphic call
        }
    }
    std::cout << "======================\n\n\n";
}

// Main menu loop
int main() {
    int choice;
    do {
        std::cout << "\n==== Media Database ====\n";
        std::cout << "1. Add Media\n2. Remove Media\n3. List Media\n4. Exit\n> ";
        std::cin >> choice;

        switch (choice) {
        case 1: addMedia(); break;
        case 2: removeMedia(); break;
        case 3: listMedia(); break;
        case 4: std::cout << "Goodbye!\n"; break;
        default: std::cout << "Invalid choice.\n"; break;
        }
    } while (choice != 4);

    return 0;
}