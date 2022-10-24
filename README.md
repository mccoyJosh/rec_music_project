# Recommend Music Project

### by Joshua McCoy

-------

This is a project I completed for a class. It takes in a song as an input and spits out a list of 100 similar songs based off of a few characteristics.

Line 421 in the SongManager.cpp file needs a key to connect to the genius lyrics api. Otherwise, it can only utilize the database it has on hand.


------
Scores for songs recommendations:
- If the artists match (100 points)
- If the genre matches (150 points)
- If the song was released within ten years of the given song (100 points)
- If a given key word from asong matches a key word from theuser's song, the number of times both songs appear multiplied by each is added on to the points. For instance, if "dude" appears in both song 10 times, 100 points would be added.


------
Dependent on:

- [Genius Lyrics API](https://rapidapi.com/Glavier/api/genius-song-lyrics1/)
- C++
- [cURL](https://stackoverflow.com/questions/22834233/ubuntu-include-curl-curl-h-no-such-file-or-directory)
- [OpenMP](https://www.openmp.org/)
