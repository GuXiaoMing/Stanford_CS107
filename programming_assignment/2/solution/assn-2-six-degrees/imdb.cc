#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "imdb.h"
using namespace std;

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;

  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) ||
	    (movieInfo.fd == -1) );
}

// you should be implementing these two methods right here...
bool imdb::getCredits(const string& player, vector<film>& films) const {
    player_key key = {.actorFile=actorFile, .player=player};
    void *base = (char *)actorFile + sizeof(int);
    int n = *(int *)actorFile;
    void *pos = bsearch(&key, base, n, sizeof(int), PlayerCmpFn);
    if(!pos)
        return false;
    int end_offset;
    GetPlayer(actorFile, *(int *)pos, end_offset);
    int *film_offsets_arr;
    short film_cnt;
    GetValues(actorFile, end_offset, film_offsets_arr, film_cnt);
    int film_end_offset;
    for(int i = 0; i < film_cnt; i++){
        int film_offset = *(film_offsets_arr + i);
        film f = GetFilm(movieFile, film_offset, film_end_offset);
        films.push_back(f);
    }
    return true;
}


bool imdb::getCast(const film& movie, vector<string>& players) const {
    film_key key = {.movieFile=movieFile, .movie=movie};
    void *base = (char *)movieFile + sizeof(int);
    int n = *(int *)movieFile;
    void *pos = bsearch(&key, base, n, sizeof(int), FilmCmpFn);
    if(!pos)
        return false;
    int end_offset;
    film tmp = GetFilm(movieFile, *(int *)pos, end_offset);
    int *actor_offsets_arr;
    short actor_cnt;
    GetValues(movieFile, end_offset, actor_offsets_arr, actor_cnt);
    int actor_end_offset;
    for(int i = 0; i < actor_cnt; i++){
        int actor_offset = *(actor_offsets_arr + i);
        string name = GetPlayer(actorFile, actor_offset, actor_end_offset);
        players.push_back(name);
    }
    return true;
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM..
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}


void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}


string imdb::GetPlayer(const void *file, int start_offset, int &end_offset){
    void *record = (char *)file + start_offset;
    string name = string((char *)record);
    end_offset = start_offset + name.size() + 2 - name.size() % 2;
    return name;
}


film imdb::GetFilm(const void *file, int start_offset, int &end_offset){
    void *record = (char *)file + start_offset;
    string title = string((char *)record);
    char delta = *((char *)record + title.size() + 1);
    film f = {.title = title, .year = 1900 + (int)delta};
    // cout << f.title << " " << f.year << endl;
    end_offset = start_offset + title.size() + 2 + title.size() % 2;
    return f;
}


void imdb::GetValues(const void *file, int offset, int *&base, short &length){
    length = *(short *)((char *)file + offset);
    // this only works because offset is always even
    base = (int *)((char *)file + offset + 2 + (offset + 2) % 4);
}


int imdb::PlayerCmpFn(const void *key, const void *offset){
    player_key *pk = (player_key *)key;
    string curr = string((char *)(pk->actorFile) + *(int *)offset);
    return pk->player.compare(curr);
}


int imdb::FilmCmpFn(const void *key, const void *offset){
    film_key *fk = (film_key *)key;
    int end_offset;
    film curr = GetFilm(fk->movieFile, *(int *)offset, end_offset);
    if(fk->movie == curr)
        return 0;
    else if(fk->movie < curr)
        return -1;
    else return 1;
}
