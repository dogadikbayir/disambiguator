#ifndef PATENT_FAKE_H
#define PATENT_FAKE_H

#include <cppunit/TestCase.h>

class FakeTest : public CppUnit::TestCase {

private:

  list<Record> source;
  vector<string> requested_columns;
  RecordPList record_pointers;
  // Accessory container for unit testing, not
  // present in the disambiguation code.
  vector<Record *> rpv;
  string csvfilename;
  map<string, const Record *> uid_dict;

public:

  FakeTest(string name, string filename);

  void load_fake_data(string csvfilename);

  RecordIndex * get_uid_dict();

  void runTest() {
    load_fake_data(csvfilename);
  }
};


#endif //PATENT_FAKE_H
