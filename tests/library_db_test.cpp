#include "mediaplayer/LibraryDB.h"
#include <gtest/gtest.h>

using mediaplayer::LibraryDB;

TEST(LibraryDB, BasicCrud) {
  LibraryDB db(":memory:");
  ASSERT_TRUE(db.open());
  ASSERT_TRUE(db.addMedia("song.mp3", "Song", "Artist", "Album"));
  auto all = db.allMedia();
  EXPECT_EQ(all.size(), 1u);
  EXPECT_EQ(all[0].title, "Song");
  ASSERT_TRUE(db.createPlaylist("pl"));
  ASSERT_TRUE(db.addToPlaylist("pl", "song.mp3"));
  auto items = db.playlistItems("pl");
  EXPECT_EQ(items.size(), 1u);
  EXPECT_EQ(items[0].path, "song.mp3");
  db.close();
}
