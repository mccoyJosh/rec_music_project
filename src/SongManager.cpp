/**
 * Author: Joshua McCoy
 * Project: SongRecommendations
 * This: SongManager.cpp
 * Purpose: Hold various structures that are used in the process of finding song recommendations
 */

#include <iostream>
#include <curl/curl.h>
#include <sstream>
#include "omp.h"
typedef std::string String;


/**
 * Nodes to construct song's word list out of
 */
class node {     // Nodes in a wordList
    public:
    String word;  // Word at given node
    int count;    // Counter to store number of appearances of given word
    node *next;   // Pointer to next node
};

/**
 * Word list to hold all the (significant) words of a given song
 */
class WordList {
    public:
    node *head; 
    int size;

    /**
     *  Constructor for word list
     */
    WordList() {
        head = NULL;
        size = 0;
    }

    /**
     *  Clears list
     */
    void clear() {
        reclaimAllNodes(head);
        head = NULL;
        size = 0;
    }

    /**
     *  Goes through each word and deletes it
     */
    void reclaimAllNodes (node*& initialP) {
        if (initialP != NULL) {
            reclaimAllNodes(initialP->next);
            delete (initialP);
        }
    }

    /**
     *  Adds a value to the list of words
     */
    void add(String newWord) {
        if (head == NULL) {
            head = new node();
            head->word = newWord;
            head->count = 1;
            size++;
        }
        else {
            node * current = head;

            while (current->next != NULL) {     // Gets to the current last word in the list
                if (current->word == newWord) { // If the word is already in the lsit
                    current->count++;           // Adds 1 to the counter of the word
                    return;
                }
                current = current->next;
            }
            if (current->word == newWord) { // If the word is already in the lsit
                current->count++;           // Adds 1 to the counter of the word
                return;
            }
            
            node *newNode = new node();
            newNode->word = newWord;
            newNode->count = 1;
            current->next = newNode;
            newNode->next = NULL;
            size++;
            
        }

        
    }

    void addWithCount(String newWord, int counter) { // used for initial database induction
        if (size == 0) {
            head = new node();
            head->word = newWord;
            head->count = counter;
            size++;
        }
        else {
            node *newHead = new node();
            newHead->next = head;
            head = newHead;
            head->word = newWord;
            head->count = counter;
            size++;
        }
    }

    /**
     *  Returns an output string in this fashion: { [word1, countOfWord1], [word2, countOfWord2]...  } 
     */
    String outputList() {
        if (size != 0) {
            node * current = head;
            String ret = "{";
            while (current != NULL) { // goes through each word
                if (current != head) {
                    ret.append(",");
                }
                ret.append(" [");
                ret.append(current->word);
                ret.append(", ");
                ret.append(std::to_string(current->count));
                ret.append("]");
                current = current->next;
            }
            ret.append(" }");
            return ret;
        }
        return "{}";
    }

    String outputForCSV() {
        if (size != 0) {
            node * current = head;
            String ret = "";
            while (current != NULL) { // goes through each word
                ret.append(current->word);
                ret.append(",");
                ret.append(std::to_string(current->count));
                if (current->next != NULL) {
                    ret.append(",");
                }
                current = current->next;
            }
            return ret;
        }
        return "";
    }

    /**
     *  Returns the count of a given word; if the word was not found, returns 0 
     */
    int count(String findThisWord) {
        node * current = head;
        while (current->next != NULL) { // Gets to word, if it is there
            if (current->word == findThisWord) {
                return current->count;
            }
            else {
                current = current->next;
            }
        }
        return 0;
    }

};

/**
* class to hold individual songs
*/
class SongUnit {
    private:
        String name, artist, genre;     // name of the song, the artist of the song, and the genre which will all be used for comparison
        int year, score;                // year of song release, AND current score  
        WordList * words;               // Holds the lyric's key words
                    

    public:
        /**
        *  Constructs song container to hold song data
        */
        SongUnit(String song_name, String artist_name, String song_genre, int year_of_release, WordList * lyrics) {
            name = song_name;
            artist = artist_name;
            words = lyrics;
            genre = song_genre;
            year = year_of_release;
            score = 0; 
        }

        /**
        *  Default Constructer just in case
        */
        SongUnit() {
            name = "song_name";
            words = new WordList();
            genre = "song_genre";
            year = 0;
            score = 0; 
        }

        /**
        *  Gets aspects of the song
        */
        String getSongName() {
            return name;
        }
        String getArtist() {
            return artist;
        }
        WordList* getWordList() {
            return words;
        }
        String getGenre() {
            return genre;
        }
        int getYear() {
            return year;
        }

        int getScore() {
            return score;
        }


        /**
        *  Calculates a the score of THIS song; the parameter song ought to be the song that the user inputs
        *  This score is used to rank the songs
        *  The multiple multipliers are for the sake of adjusting how much each specific portion of the song is worth in ranking
        *  For instance, making the "genreMul" greater will make songs that have the same genre appear higher in the final sorting
        * 
        *  @param parameterSong Takes in a song to compare to the song of this object
        */
        void calcScore(SongUnit * parameterSong) {
            const int lyricMul = 1;
            const int artistMul = 100;
            const int genreMul = 150;
            const int yearMul = 100;

            //if (!(this->equals(parameterSong))) { // It will grade itself, but just don't display it at the end
            // Go through each word in each song 
            node * wordNodeThis;
            node * wordNodePara;
            wordNodeThis = words->head;
            while (wordNodeThis != NULL) {
                wordNodePara = parameterSong->getWordList()->head;
                while(wordNodePara != NULL) {
                    if (wordNodePara->word == wordNodeThis->word) {
                        score += (wordNodePara->count)*(wordNodeThis->count);
                    }
                    wordNodePara = wordNodePara->next;
                }
                wordNodeThis = wordNodeThis->next;
            }
            if (artist == parameterSong->getArtist()) {
                score += 1 * (artistMul); 
            }
            if (genre == parameterSong->getGenre()) {
                score += 1 * (genreMul);
            }
            int yearTest = year - parameterSong->getYear();
            if (yearTest < 0) {yearTest = yearTest * (-1);}
            if (yearTest < 10) {
                score += 1 * (yearMul);
            }
            //}
        }

        /**
        *  Outputs song in a form that is more readable for testing sake
        *  @return A string of the song data
        */
        String outputSong() {
            return  name + "\t" + artist + "\t" + genre + "\t" + std::to_string(year) + "\t" + words->outputList();
        }

        /**
        *  Outputs song in the form of how it will appear in the SongDatabase.csv file
        *  @return A string of the song data
        */
        String outputSongForCSV() {
            return  name + "," + artist + "," + std::to_string(year) + "," + genre + "," + words->outputForCSV();
        }

        /**
         * checks to see if given song is the same parameter song
         * 
         * @param sung song to check this song with 
         * @return true if the two songs are the same song
         * @return false if the two songs are not the same song
         */
        bool equals(SongUnit * sung) {
            //return (name == sung->getSongName().substr(0, name.length()) && artist == sung->getArtist());
            return (name == sung->getSongName() && artist == sung->getArtist());
        }
};

/**
 * Node To Store A Song and a pointer to the next song
 */
struct SongNode {
    SongUnit *song; 
    SongNode *next; 

    SongNode(SongUnit * s){
        song = s;
    }
};

/**
 *  List to hold all the songs of a given character
 */
struct SongList {
    SongNode *head; 
    int size;
    SongList() {
        head = NULL;
        size = 0;
    }

    void clear() {
        reclaimAllNodes(head);
        head = NULL;
        size = 0;
    }
    void reclaimAllNodes (SongNode*& initialP) {
        if (initialP != NULL) {
            reclaimAllNodes(initialP->next);
            delete (initialP);
        }
    }
    void add(SongUnit * newSong) {
        if (size == 0) {
            head = new SongNode(newSong);
            head->next = NULL;
            size++;
        }
        else {
            SongNode *newHead = new SongNode(newSong);
            newHead->next = head;
            head = newHead;
            size++;
        }
    }

    bool isEmpty() {
        return size==0;
    }
};


/**
* class to deal with getting songs from the Genius API
*/
class MusicAPI {

    private:
    // CURL writing to string helper method
    static size_t Write(void *contents, size_t size, size_t nmemb, void *userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }    

    public:
    static String getLyrics(int c) {
        CURL *hnd = curl_easy_init();
        String contents;

        //String code = std::to_string(2396871);
        String code = std::to_string(c);
        String preUrl = "https://genius-song-lyrics1.p.rapidapi.com/songs/" + code + "/lyrics";
        const char* url = preUrl.c_str();

        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(hnd, CURLOPT_URL, url);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "X-RapidAPI-Host: genius-song-lyrics1.p.rapidapi.com");
        std::string key ="";
        headers = curl_slist_append(headers, "X-RapidAPI-Key: " + key);
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
        
        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, Write);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &contents);
        
        CURLcode res = curl_easy_perform(hnd); 
        curl_easy_cleanup(hnd);

        //std::cout << "______top_________" << std::endl;
        //std::cout << contents << std::endl;
        //std::cout << "______bottom_______" << std::endl;
        return contents;
    }

    static String getBandsSongs(String bandName, int pageNum) {
        for (int i = 0; i < bandName.length(); i++) {
            if (bandName[i] == ' ') {
                bandName.replace(i, 1, "%20", 3);
            }
        }
        String contents;
        std::stringstream str;
        str << pageNum;
        String page;
        str >> page;

        CURL *hnd = curl_easy_init();

        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
        String preUrl = "https://genius-song-lyrics1.p.rapidapi.com/search?q=" + bandName + "&per_page=21&page=" + page;
        const char* url = preUrl.c_str();
        curl_easy_setopt(hnd, CURLOPT_URL, url);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "X-RapidAPI-Host: genius-song-lyrics1.p.rapidapi.com");
        std::string key = ""; // THIS NEEDS TO BE FILLED OUT TO CONNECT TO DATABASE
        headers = curl_slist_append(headers, "X-RapidAPI-Key: " +  key);
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, Write);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &contents);
        
        CURLcode res = curl_easy_perform(hnd); 
        curl_easy_cleanup(hnd);

        //std::cout << "______top_________" << std::endl;
        //std::cout << contents << std::endl;
        //std::cout << "______bottom_______" << std::endl;

        return contents;
    }

};

/**
* class that holds the multiple lists of songs that make up the database
*/
class SongMap {
    public:
    // Here is an array to store the list of songs; each list of song will hold songs that begin with a particular character; 
    // 1-26 will be their given corresponding alphabet letter; the 0th index will be a catch all for the rest of the characters
    SongList *BinsOfSongs[27];

    /**
    * constructor to set up bins
    */
    SongMap() {
        for (int i = 0; i <27; i++) {
            BinsOfSongs[i] = new SongList();
        }
    }
    
    /**
    * Sometimes you need a blank song object; this gives use that null song
    */
    SongUnit * GetNullSong() {
        SongUnit* ret;
        WordList * w = new WordList();
        w->addWithCount("null_word", 0);
        ret = new SongUnit("null_song", "null_artist", "null_genre", 0, w);
        return ret;
    }

    /**
     *  Adds song to correct bin & ensures song is not already present in the list
     *  @return boolean if the song was added
     */
    bool addToBin(SongUnit *songToAdd) {
        int index = (songToAdd->getSongName())[0] - 96;
        if (index < 1 || index > 26) { index = 0;} // Anything that's not a letter goes into index 0
        if(BinsOfSongs[index]->size != 0) {
            SongNode * current = BinsOfSongs[index]->head;
            while (current != NULL)  {
                if((current->song)->equals(songToAdd)) {
                    //std::cout << "Song is already in the database :)" << std::endl;
                    return false;
                }
                current = current->next;
            }
        }
        BinsOfSongs[index]->add(songToAdd);
        return true;
    }

    /**
     *  Adds song to correct bin; no check for when adding it from csv database, given csv database is not been compromised
     */
    void addToBinNoCheck(SongUnit * songToAdd) {
        int index = (songToAdd->getSongName())[0] - 96; 
        if (index < 1 || index > 26) { index = 0;} // Anything that's not a letter goes into index 0 
        BinsOfSongs[index]->add(songToAdd);
    }

    /**
     *  Prints Database to console
     */
    void PrintDatabase() {
        SongNode * node;
        for (int i = 0; i < 27; i++) {
            if (!(BinsOfSongs[i]->isEmpty())) {
                node = BinsOfSongs[i]->head;
                while (node != NULL) {
                    std::cout << (node->song)->outputSong() << std::endl; 
                    node = node->next;
                }
            }
        }
    }

    /**
     *  Prints Database to console
     */
    void PrintDatabaseCSV() {
        SongNode * node;
        for (int i = 0; i < 27; i++) {
            //std::cout << "bin number" << i <<std::endl;
            if (!(BinsOfSongs[i]->isEmpty())) {
                node = BinsOfSongs[i]->head;
                while (node != NULL) {
                    std::cout << (node->song)->outputSongForCSV()<< std::endl; 
                    node = node->next;
                }
            }
        }
    }

    /**
     *  Goes through bins and finds song
     *  @param songName String of the song name that IS ALL LOWERCASE
     *  @param artist String of the artist name that IS ALL LOWERCASE
     *  @return the pointer to the song one wants; If song is not found, returns a null song that will be considered "not found" by method that calls it
     */
    SongUnit * FindSong(String songName, String artist) {
        // Determines where song should be
        int index = songName[0] - 96; 
        if (index < 1 || index > 26) { index = 0;} 

        // Makes node to go through bin
        SongNode * node;
        if (!(BinsOfSongs[index]->isEmpty())) { // If bin is empty, song is definitley in there
            node = BinsOfSongs[index]->head;    // Starts at head
            while (node != NULL) {              // Goes until list ends
                //std::cout << "Song Name: " << node->song->getSongName() << std::endl;
                if ((node->song)->getSongName().substr(0, songName.length()) == songName && (node->song)->getArtist() == artist) { // If song title and artist match, that's good enough
                    return node->song;          // Returns that song's pointer
                }
                node = node->next;
            }
        }
        return GetNullSong(); // If it gets there, the song was not found
    }



    /**
     *  Prints Top 100 Recommended Songs
     */
    void SortAndDisplaysScores(SongUnit * songToNotDisplay) {
        std::cout << std::endl;
        //SongNode * node;
        //int numthreads;

        //#pragma omp parallel { //
        //numthreads = omp_get_num_threads();

        //#pragma omp for reduction
        SongNode * node;
        for (int i = 0; i < 27; i++) {
            if (!(BinsOfSongs[i]->isEmpty())) {
                node = BinsOfSongs[i]->head;
                while (node != NULL) {
                    if (!((node->song)->equals(songToNotDisplay))) {
                        (node->song)->calcScore(songToNotDisplay);
                    }
                    node = node->next;
                }
            }
        }
        
        //} // parallel line

        SongUnit * sortedList[100];
        for (int k = 0; k < 100; k++) {
            sortedList[k] = GetNullSong();
        }
        SongUnit* tempSong;

        bool foundSpot;
        for (int i = 0; i < 27; i++) {
            if (!(BinsOfSongs[i]->isEmpty())) {
                node = BinsOfSongs[i]->head;
                while (node != NULL) {
                    foundSpot = false;
                    //std::cout << node->song->getSongName() << std::endl;
                    for (int k = 0; k < 100 && !foundSpot; k++) {
                        //std::cout << node->song->getScore() << "\t\t" << sortedList[k]->getScore() << std::endl;
                        if ((node->song)->getScore() > sortedList[k]->getScore()) {
                            foundSpot = true;
                            tempSong = sortedList[k];
                            sortedList[k] = node->song;
                            k++;
                            while (k < 100 && sortedList[k]->getSongName() != "null_song") {
                                sortedList[k] = tempSong;
                                k++;
                                tempSong = sortedList[k];
                            }
                        }
                    }
                    node = node->next;
                }
            }
        }
        std::cout << "Rank: Song Title\t\tArtist\t\tScore" << std::endl;
        std::cout << "_____________________________________________________________" << std::endl;

        for (int k = 0; k < 100; k++) {
            if (sortedList[k]->getSongName() != "null_song") {
                if (k <= 8) {
                    std::cout << "   " <<k+1 << ": "<< sortedList[k]->getSongName() << "\t" << sortedList[k]->getArtist() << "\t" << sortedList[k]->getScore()  << std::endl;
                }
                else if (k <= 98) {
                    std::cout << "  " <<k+1 << ": "<< sortedList[k]->getSongName() << "\t\t" << sortedList[k]->getArtist() << "\t\t" << sortedList[k]->getScore()  << std::endl;
                }
                else {
                    std::cout << " " <<k+1 << ": "<< sortedList[k]->getSongName() << "\t\t" << sortedList[k]->getArtist() << "\t\t" << sortedList[k]->getScore()   << std::endl;
                }
            }
        }

        
    }


};

