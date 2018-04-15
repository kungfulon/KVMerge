#include "KeyValues.h"
#include <iostream>
#include <unordered_map>

int wmain(int argc, wchar_t *argv[]) {
  if (argc < 5) {
    std::wcout << L"Usage: " << argv[0] << ' ' << L"<first file> <second file> <output file> <merge|replace>";
    return 0;
  }

  if (!wcscmp(argv[4], L"merge")) {
    KeyValues *first = KeyValues::LoadFromFile(argv[1]);
    KeyValues *second = KeyValues::LoadFromFile(argv[2]);

    if (!first || !second) {
      std::wcout << L"Cannot open one or more input files";
      return 0;
    }

    KeyValues *firstTokenRoot = first->GetFirstChild()->GetNext();
    KeyValues *firstTokenTail = firstTokenRoot->GetFirstChild();

    while (firstTokenTail->GetNext()) {
      firstTokenTail = firstTokenTail->GetNext();
    }

    firstTokenTail->SetNext(second->GetFirstChild()->GetNext()->GetFirstChild());

    while (firstTokenTail->GetNext()) {
      firstTokenTail = firstTokenTail->GetNext();
      firstTokenTail->SetParent(firstTokenRoot);
    }

    first->SaveToFile(argv[3]);
  } else if (!wcscmp(argv[4], L"replace")) {
    KeyValues *first = KeyValues::LoadFromFile(argv[1]);
    KeyValues *second = KeyValues::LoadFromFile(argv[2]);

    if (!first || !second) {
      std::wcout << L"Cannot open one or more input files";
      return 0;
    }

    KeyValues *firstTokenTail = first->GetFirstChild()->GetNext()->GetFirstChild();
    KeyValues *secondTokenTail = second->GetFirstChild()->GetNext()->GetFirstChild();
    std::unordered_map<std::wstring, const wchar_t *> secondMap;

    while (secondTokenTail) {
      secondMap[secondTokenTail->GetKeyName()] = secondTokenTail->GetValue();
      secondTokenTail = secondTokenTail->GetNext();
    }

    while (firstTokenTail) {
      if (secondMap.count(firstTokenTail->GetKeyName())) {
        firstTokenTail->SetValue(secondMap[firstTokenTail->GetKeyName()]);
      }

      firstTokenTail = firstTokenTail->GetNext();
    }

    first->SaveToFile(argv[3]);
  } else {
    std::wcout << L"Unknown option: " << argv[4];
  }
}
