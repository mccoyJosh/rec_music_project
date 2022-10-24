/**
 * Author: Joshua McCoy
 * Project: SongRecommendations
 * This: Main.cpp
 * Purpose: Determine how to run the program while also then running the program
 */

#include <iostream>
#include <chrono>
#include "FileManager.cpp"
typedef std::string String;


int main() {
    String artist;
    String songName;
    bool running  = true;
    while (running) {
        std::cout << std::endl << "-----------------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << "\t\t\\/\\/\\/\\/\\/\\/\\/ Welcome to the Song-Recommendation-er! \\/\\/\\/\\/\\/\\/\\/" << std::endl;
        std::cout << "All you need to do is enter the song you'd like to find similar songs of and that song's arist" << std::endl;
        std::cout << "Keep in mind that the song name needs to spelled as they are in the real world" << std::endl;
        std::cout << "For example, if you were looking for songs similar to Mamma Mia by ABBA, \nplease write out each with the correct capitilzation (don't put something like mamma mia or abba)" << std::endl;
        std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
        
        // Gets user input
        std::cout << "Song Name: ";
        std::getline(std::cin, songName);
        std::cout << "Artist Name: ";
        std::getline(std::cin, artist);

        // Makes string for API to use
        String given = songName+ " " + artist;                  //  //ORIGINAL//

        // Determine how many pages of songs to search through to add to database
        // The more pages = the more likley you are to find song
        int numberOfPagesToSearch = 1;                          //  //ORIGINAL//

        //int numberOfPagesToSearch = 2;    // USE TO BUILD DATABASE !!!
        //String given = artist;            // USE TO BUILD DATABASE !!!

        FileManager fileObj;

        // Determines if the song is in the database
        bool IsSongNowInDatabaseAFTERapi = false;
        bool IsSongAlreadyInDatabase = fileObj.IsSongHere(songName, artist);

        
        // If song is not in the database, uses API to potentially find song
        if (!IsSongAlreadyInDatabase) {
            std::cout << "The song was not already in the database; requesting API"<< std::endl;
            fileObj.GetBandSongs(given, numberOfPagesToSearch);
            IsSongNowInDatabaseAFTERapi = fileObj.IsSongHere(songName, artist);
        }
        else {
            std::cout << "The song is already in the database :D"<< std::endl;
        }
        
        // If song was not added or was not found; don't look for recommendations
        ///*
        //auto start = std::chrono::high_resolution_clock::now();
        if (IsSongAlreadyInDatabase || IsSongNowInDatabaseAFTERapi) {
            std::cout << "Number of songs in dataabse: " << fileObj.GetNumberOfSongs() << std::endl;
            fileObj.DisplayFinalResult();
        }
        else {
            std::cout << "Song could not be found :("<< std::endl;
        }
        //auto finish = std::chrono::high_resolution_clock::now();
        //std::cout << "Runtime: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() << "ns\n";


        //*/

        // Update CSV file, if need be
        if (!IsSongAlreadyInDatabase) {
            fileObj.WriteToCSV();
        }

        // You can keep asking for more recommendations if you'd like
        std::cout << "Continue? (put either yes or no): ";
        String answer;
        std::getline(std::cin, answer);
        if (answer != "yes") {
            running = false;
        }
        else {
            std::cout << std::endl << std::endl << std::endl << std::endl;
        }
    }  
    return 0;
}