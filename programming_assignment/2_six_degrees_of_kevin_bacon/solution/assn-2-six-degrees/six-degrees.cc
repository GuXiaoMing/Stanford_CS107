#include <vector>
#include <list>
#include <set>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include "imdb.h"
#include "path.h"
using namespace std;

/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */

static string promptForActor(const string& prompt, const imdb& db)
{
  string response;
  while (true) {
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);
    if (response == "") return "";
    vector<film> credits;
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
	 << "Please try again." << endl;
  }
}

static unordered_map<string, path> getNewFrontierActors(set<string> &exhausted_actors,
        set<film> &exhausted_films,
        unordered_map<string, path> &frontier_actors, const imdb& db){
    unordered_map<string, path> new_frontier_actors;
    for(auto it: frontier_actors){
        vector<film> frontier_films;
        db.getCredits(it.first, frontier_films);
        for(auto film: frontier_films){
            if(exhausted_films.count(film) > 0)
                continue;
            exhausted_films.insert(film);
            vector<string> costars;
            db.getCast(film, costars);
            for(auto costar: costars){
                if(exhausted_actors.count(costar) > 0)
                    continue;
                path new_path(it.second);
                new_path.addConnection(film, costar);
                new_frontier_actors[costar] = new_path;
            }
            exhausted_actors.insert(it.first);
        }
    }
    return new_frontier_actors;

}


static path generateShortestPath (string name1, string name2, const imdb& db){
    set<string> exhausted_actors;
    set<film> exhausted_films;
    unordered_map<string, path> frontier_actors1;
    unordered_map<string, path> frontier_actors2;

    frontier_actors1[name1] = path(name1);
    frontier_actors2[name2] = path(name2);

    unordered_map<string, path> frontier_actors_arr[2] = {frontier_actors1, frontier_actors2};

    for(int i = 0; i < 3; i++){
        // cout << "i = " << i << endl;
        for(int j = 0; j < 2; j++){
            /*
            cout << "j = " << j << endl;
            cout << "old frontier_actors[" << j << "]:" << endl;
            for(auto it: frontier_actors_arr[j]){
                cout << it.first << " " << it.second;
            }
            */
            frontier_actors_arr[j] = getNewFrontierActors(exhausted_actors, exhausted_films,
                    frontier_actors_arr[j], db);

            for(auto it: frontier_actors_arr[1 - j]){
                if(frontier_actors_arr[j].count(it.first) == 0)
                    continue;
                path path1 = frontier_actors_arr[0][it.first];
                path path2 = frontier_actors_arr[1][it.first];
                path2.reverse();
                path1.append(path2);
                return path1;
            }
        }
    }
    return path(name1);
}


/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */

int main(int argc, const char *argv[])
{
  imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    exit(1);
  }

  while (true) {
    string source = promptForActor("Actor or actress", db);
    if (source == "") break;
    string target = promptForActor("Another actor or actress", db);
    if (target == "") break;
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } else {
      // replace the following line by a call to your generateShortestPath routine... 
      // cout << endl << "No path between those two people could be found." << endl << endl;
      path res = generateShortestPath(source, target, db);
      cout << res << endl;
    }
  }

  /*
  string test_cases[][2] = {
      {"Jack Nicholson", "Meryl Streep"},
      {"Mary Tyler Moore", "Red Buttons"},
      {"Barry Manilow", "Lou Rawls"},
      {"Carol Eby", "Debra Muubu"},
      {"Jerry Cain", "Kevin Bacon"}
  };
  for(int i = 0; i < 6; i++){
    path res = generateShortestPath(test_cases[i][0], test_cases[i][1], db);
    cout << res << endl;
  }
  */

  cout << "Thanks for playing!" << endl;
  return 0;
}

