#include "mediaplayer/LibraryDB.h"
#include "mediaplayer/MoodAIRecommender.h"
#include <cassert>
#include <cstdio>

int main() {
  const char *dbPath = "recommender.db";
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  assert(db.addMedia("happy1.mp3", "Happy Song", "Band", "Hits", "Pop"));
  assert(db.addMedia("sad1.mp3", "Sad Tune", "Band", "Hits", "Blues"));

  mediaplayer::MoodAIRecommender rec("Pop", 1);
  db.setRecommender(&rec);
  auto list = db.recommendations();
  assert(list.size() == 1 && list[0].genre == "Pop");

  db.close();
  std::remove(dbPath);
  return 0;
}
