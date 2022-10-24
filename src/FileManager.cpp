/**
 * Author: Joshua McCoy
 * Project: SongRecommendations
 * This: FileManager.cpp
 * Purpose: Used to create/edit the various files associated with the project
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "SongManager.cpp"
#include <algorithm>

typedef std::string String;
typedef std::fstream FS; 

class FileManager {
private:
    String fileName;
    int numberOfSongs;
    SongMap map;
    int numOfStopWords;
    String stopWords[76];
    int codeHold[21];
    SongUnit * baseSong;
    String baseSongName;
    String baseSongArtist;
    
    /**
     *  Determines if the csv file exists 
     */
    bool ifFileExists() { // Determines if given file exists
        FS file;
        file.open(fileName);
        if (file.fail()) {
            std::cerr << fileName << " failed to open and probably doesn't exist!" << std::endl;
            file.close();
            return false;
        }
        file.close();
        return true;
    }

    /**
     *  Fills the map with the contents of th csv file
     */
    SongMap getContents() {
        SongMap contents;    // This will be the database holding the songs 
        String line;         // Current line of the file being looked at (1 song)
        String unit;         // will be built up to either be artist, title, song word, etc

        /**
         * whereCount is the variable which holds indicator of the current item we are attempting to add to a song unit;
         * 0 = song title
         * 1 = artist
         * 2 = year
         * 3 = genre
         * 4 = word1
         * 5 = count1
         * Once it is greater than 3, even numbers are words and even numbers are their corresponding count
         */
        int whereCount; 
        SongUnit *currentSong;

        // TEMP VARIABLES FOR ALL VARIABLES IN A SONGUNIT
        String tempTitle, tempArtist, tempGenre, tempWord;
        int tempYear, tempCount;
        WordList *tempList;
        std::stringstream str; 
        
        // Creates and opens file
        FS file;
        file.open(fileName, std::ios::in);

        // Goes through every line until the eof (end of file)
        while (!file.eof()) {
            whereCount = 0;
            std::getline(file, line);
            for (int j = 0; j < line.length(); j++) {
                if (line[j] != ',') {
                    unit += line[j];

                    if ((j == line.length() - 1) && (whereCount > 3)) { // Last number is skipped otherwise & Also needs to grab the last digit
                    // Given the whereCount is greater than 3, it ensures that there is atleast one word
                        str.clear();
                        str << unit;
                        str >> tempCount;
                        //std::cout << tempWord << ": " << tempCount << std::endl;
                        tempList->addWithCount(tempWord, tempCount);
                        unit = "";
                        whereCount++;
                    }
                }
                else {
                    if (whereCount ==0) {           // Sets title
                        tempTitle = unit;
                    }
                    else if (whereCount == 1) {     // Sets artist
                        tempArtist = unit;
                    }
                    else if (whereCount == 2) {     // sets Year 
                        str.clear();
                        str << unit;
                        str >> tempYear;
                    }
                    else if (whereCount == 3) {     // Sets genre
                        tempGenre = unit;
                        tempList = new WordList();
                    }
                    else {                          // Begins process of inserting words into word list for given song
                        if (whereCount%2 == 0) {
                            //Word
                            tempWord = unit;
                        }
                        else {
                            str.clear();
                            str << unit;
                            str >> tempCount;
                            //std::cout << tempWord << ": " << tempCount << std::endl;
                            tempList->addWithCount(tempWord, tempCount);
                        }
                    }
                    unit = "";
                    whereCount++;
                }
            }

            currentSong = new SongUnit(tempTitle, tempArtist, tempGenre, tempYear, tempList); // creates song
            //std::cout << "Song: " << currentSong->getSongName() << std::endl; 
            contents.addToBinNoCheck(currentSong);                                            // Adds song to appropriate bin
            //std::cout << "BinCheck Song: " << contents.BinsOfSongs[1].head->song->getSongName() << std::endl; 
            numberOfSongs++;
        }
        file.close();
        return contents;
    }

    /**
     *  Makes given string lowercase
     */
    void toLowerCase(String &str) {
        transform(str.begin(), str.end(), str.begin(), tolower);
    }



    /**
     *  USE AT THE END OF PROGRAM
     *  Will take current map, with any additional songs added to it, and put it in the SongDatabase.csv file
     */
    void updateFile() { 
        FS file;
        file.open(fileName, std::ios::out); //opens CSV file
        bool firstPrint = true;;

        SongNode * node;
        for (int i = 0; i < 27; i++) {              // Goes through each bin
            if (!(map.BinsOfSongs[i]->isEmpty())) { // Makes sure it is not empty
                node = map.BinsOfSongs[i]->head;    // starts at head
                while (node != NULL) {
                    if (!firstPrint) {
                        file << "\n";
                    }
                    file << node->song->outputSongForCSV(); // outputs to CSV file
                    if (firstPrint) {
                        firstPrint = false;
                    } 
                    node = node->next;              // Goes through each song
                }
            }
        }
        file.close();
    }

    /**
     *  Reads wordsToBeRemoved.txt file to get array of stop words
     */
    void getStopWords() {
        FS file;
        file.open("wordsToBeRemoved.txt", std::ios::in); // gets the stop words
        String line;

        // Goes through every line until the eof (end of file)
        for (int k = 0; k < numOfStopWords; k++) {
            std::getline(file, line);
            stopWords[k] = line;
        }
        file.close();
    }

    /**
     *  Tells whether a given word is a stop word
     *  @param word string to be comapred to stop list of words
     */
    bool isInStopWords(String word) {
        for (int i = 0; i < numOfStopWords; i++ ) {
            if (word == stopWords[i]) {
                return true;
            }
        }
        return false;
    } 

    /**
     *  Makes a HTML file that was received through the API into a song unit item that can be added to the map
     *  @param html a string of the html file that was recieved from API
     */
    SongUnit * converHTMLtoSong(String html) {
        String tempTitle = "blank", tempArtist = "blank", tempGenre = "blank", tempLyrics, unit;
        int tempYear = 0;
        std::stringstream str;
        WordList *tempList = new WordList();
        //std::cout << "Here!" << std::endl;
        
        int length = html.length() - 26;
        //std::cout << "Length: " << length << std::endl;
        for(int i = 0 ; i < length; i++) { // "i" is our current position
            //std::cout << "i: " << i << "\t";
            if ((html.substr(i, 8) == "<a href=") || (html.substr(i, 25) == "\"lyrics\":{\"body\":{\"html\":")) { // LYRICS TIME
                unit = "";
                // get to next bracker '>'
                while (html[i] != '>') {i++;}
                // now we are at the '>'
                while(html.substr(i, 13) != "\"dom\":{\"tag\":") {
                    // A 65 - Z 90     a 97 -  z 122
                    
                    if (html[i] == '<') {
                        if (unit != "") {
                            toLowerCase(unit);
                            if (!isInStopWords(unit)) {
                                tempList->add(unit);
                            }
                            unit = "";
                        }
                        while (html[i] != '>') {i++;}
                    }
                    else if (html[i] == '[') {
                        if (unit != "") {
                            toLowerCase(unit);
                            if (!isInStopWords(unit)) {
                                tempList->add(unit);
                            }
                            unit = "";
                        }
                        while (html[i] != ']') {i++;}
                    }
                    else if (html.substr(i, 2) == "\\n") {
                        if (unit != "") {
                            toLowerCase(unit);
                            if (!isInStopWords(unit)) {
                                tempList->add(unit);
                            }
                            unit = "";
                        }
                        i++;
                    }
                    else {
                        int ascii = (int)html[i];
                        if( (ascii >=65 && ascii <=90) || (ascii >=97 && ascii <=122) ) {
                            unit += html[i];
                        }
                        else {
                            // if its not a letter, you have made a word
                            if (unit != "") {
                                toLowerCase(unit);
                                if (!isInStopWords(unit)) {
                                    tempList->add(unit);
                                }
                                unit = "";
                            }
                        }
                    }
                    i++;
                }
            }
            else if (html.substr(i, 14) == "\"primary_tag\":") {
                unit = "";
                i += 15;
                while (html[i] != '\"') {
                    if( (int)html[i] != 44 ) {
                        unit += html[i];
                    }
                    else {
                        unit += ' ';
                    }
                    i++;
                }
                toLowerCase(unit);
                tempGenre = unit;
            }
            else if (html.substr(i, 17) == "\"primary_artist\":") {
                unit = "";
                i += 18; 
                while (html[i] != '\"') {
                    if( (int)html[i] != 44 ) {
                        unit += html[i];
                    }
                    else {
                        unit += ' ';
                    }
                    i++;
                }
                toLowerCase(unit);
                tempArtist = unit;
            }
            else if (html.substr(i, 8) == "\"title\":") {
                unit = "";
                i += 9;
                while (html[i] != '\"') {
                    if( (int)html[i] != 44 ) {
                        unit += html[i];
                    }
                    else {
                        unit += ' ';
                    }
                    i++;
                }
                toLowerCase(unit);
                if (unit.substr(0, 6) == "\\u200b") {
                    unit = unit.substr(6, unit.size()-1);
                }
                tempTitle = unit;
            }
            else if (html.substr(i, 15) == "\"created_year\":") {
                unit = "";
                i += 15;
                while (html[i] != ',') {
                    unit += html[i];
                    i++;
                }
                toLowerCase(unit);
                str.clear();
                str << unit;
                str >> tempYear;
            }
        }

        // If there are no words, inserts a filler word to ensure code works
        if (tempList->size == 0) {
            tempList->addWithCount("nullWords", 0);
        }

        for (int i = 0; i < tempTitle.length(); i++) {
            if (tempTitle.substr(i, 6) == "\\u2019") {
                tempTitle.replace(i, 6, "'", 1);
            }
            else if (tempTitle.substr(i, 6) == "\\u200b") {
                tempTitle.erase(i, 6);
            }
            else if (tempTitle.substr(i, 6) == "\\u00e9") {
                tempTitle.replace(i, 6, "e", 1);
            }
            else if (tempTitle.substr(i, 6) == "\\u00f6") {
                tempTitle.replace(i, 6, "o", 1);
            }
            else if (tempTitle.substr(i, 6) == "\\u00fc") {
                tempTitle.replace(i, 6, "u", 1);
            }
        }

        for (int i = 0; i < tempArtist.length(); i++) {
            if (tempArtist.substr(i, 6) == "\\u00d6") {
                tempArtist.replace(i, 6, "o", 1);
            }
            else if (tempArtist.substr(i, 6) == "\\u00e9") {
                tempArtist.replace(i, 6, "e", 1);
            }
            else if (tempArtist.substr(i, 6) == "\\u00f6") {
                tempArtist.replace(i, 6, "o", 1);
            }
            else if (tempArtist.substr(i, 6) == "\\u00fc") {
                tempArtist.replace(i, 6, "u", 1);
            }
        }

        return new SongUnit(tempTitle, tempArtist, tempGenre, tempYear, tempList);
    }


    /**
     *  Reads through song list file and gets all the codes available for given artist
     *  @param html a string of the html file that was recieved from API
     */
    void converHTMLtoSongList(String html) {
        for(int i =0; i<21; i++) {
            codeHold[i] = 0;
        }
        int tempCode;
        std::stringstream str;
        int index;
        String unit;
        WordList *tempList = new WordList();
        int length = html.length() - 22;
        for(int i = 0 ; i < length; i++) { // "i" is our current position
            if (html.substr(i, 19) == "\"api_path\":\"/songs/") {
                unit = "";
                str.clear();
                i += 19; 
                while (html[i] != '"') {
                    unit += html[i];
                    i++;
                }
                str << unit;
                str >> tempCode;
                index = 0;
                while(codeHold[index] != 0 && index < 21) {
                    index++;
                }
                codeHold[index] = tempCode;
            }
        }
    }

    /**
     *  Given a code for a song, gets song from API and adds it to the given bin
     *  @param code integer value reprenting the internal code for a song in the API
     */
    void GetAndAddSongToDatabase(int code) {
        String html = MusicAPI::getLyrics(code);
        // !! perhaps implement exponential backoff here!!! faded song id: 2396871
        SongUnit *song = converHTMLtoSong(html);
        if (!(song->getSongName() == "blank" && song->getArtist() == "blank" && song->getYear() == 0) && !(song->getGenre() == "non-music")) {
            if (!(song->getWordList()->head->word == "nullWords")) { // If a song with no words shows up; so yeah, no instrumentals
                if (map.addToBin(song)) { // If the song was added (it may be a repeat)
                    numberOfSongs++;
                }
            }
        }
    }


public:
    /**
     *  Constructor to set up file manager object which holds a number of crucial
     */
    FileManager() {
        // This is the database of songs that has been accumulated and will be updated at the end of the program
        // Don't hurt it please
        fileName = "SongDatabase.csv";
        numberOfSongs = 0;
        map = getContents();
        numOfStopWords = 76;
        getStopWords();
        baseSongArtist = "null_artist";
        baseSongName = "null_song";
        baseSong = map.GetNullSong();
    }


    /**
     *  Given a bandName, will request from API a list of their songs they have lyrics of
     *  Only half way through implementation did I realize this also works for song names, so bandName can be a song name too
     *  @param bandName string value reprenting the band name of the given artist
     *  @return true if desired song was added
     *  @return false if desired song was not added
     */
    void GetBandSongs(String bandName, int numberOfPagesToGoThrough) {
        bool hitEnd = false;
        //int numberOfPagesToGoThrough = 1;
        for (int pageNum = 1; pageNum < numberOfPagesToGoThrough+1 && !hitEnd; pageNum++) {
            String html = MusicAPI::getBandsSongs(bandName, pageNum);
            //std::cout << "page: " << html << std::endl;
            //std::cout << "page: " << html << std::endl;
            std::cout << "page_reading: " << pageNum << "/"<<numberOfPagesToGoThrough << "(potentially could be less than "<< numberOfPagesToGoThrough<<")" << std::endl;
            if (html != "{\"meta\":{\"status\":200},\"response\":{\"hits\":[]}}") {
                int numOfSongs = 21;
                converHTMLtoSongList(html); // address of beiginnning of array with 21 integers
                for (int i = 0; i < numOfSongs; i++) {
                    std::cout << ".";
                    if (codeHold[i] != 0) {                 // Makes sure a codes was actually inputted
                        GetAndAddSongToDatabase(codeHold[i]);
                    }
                }
            }
            else {
                hitEnd = true;
            }
            std::cout<< std::endl<< "Song(s) attained! :)" <<  std::endl;
        }        
    }

    /**
     * Determines if song is in database
     *  @param songName String of the song name that IS ALL LOWERCASE
     *  @param artist String of the artist name that IS ALL LOWERCASE
     *  @return true if the song is in current database
     *  @return false if the song is NOT in current database
     */
    bool IsSongHere(String songName, String artist) {
        toLowerCase(songName);
        toLowerCase(artist);
        baseSongName = songName;
        baseSongArtist = artist;

        SongUnit * returnSong = map.FindSong(songName, artist);
        // if not null song, means it found it
        if (returnSong->getSongName() == "null_song") {
            return false;
        }
        else {
            baseSong = returnSong;
            return true;
        }
    }

    /**
     *  Prints everything in database to console
     */
    void PrintDatabase_File() {
        map.PrintDatabaseCSV();
    }

    /**
     *  Last thing to be done; writes results to song database.csv file updating it with any additional songs collected along the way
     */
    void WriteToCSV() {
        updateFile();
    }

    /**
     *  Returns number of songs
     *  @return an integer value representing the number of songs in database
     */
    int GetNumberOfSongs() {
        return numberOfSongs;
    }

    /**
     * Calls method to calculate the scores, sort the songs by the scores, and display those results
     */
    void DisplayFinalResult() {
        map.SortAndDisplaysScores(baseSong);
    }
    




};