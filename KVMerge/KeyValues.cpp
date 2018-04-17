#include <fstream>
#include <codecvt>
#include "KeyValues.h"

KeyValues::KeyValues(const wchar_t *sKeyName, const wchar_t *sValue) :
  m_pParent(NULL), m_pFirstChild(NULL), m_pNext(NULL), m_sKeyName(sKeyName), m_sValue(sValue) {

}

KeyValues::~KeyValues() {

}

const wchar_t *KeyValues::GetKeyName() {
  return m_sKeyName.c_str();
}

const wchar_t *KeyValues::GetValue() {
  return m_sValue.c_str();
}

void KeyValues::SetKeyName(const wchar_t *sKeyName) {
  m_sKeyName = sKeyName;
}

void KeyValues::SetValue(const wchar_t *sValue) {
  m_sValue = sValue;
}

KeyValues *KeyValues::GetParent() {
  return m_pParent;
}

KeyValues *KeyValues::GetFirstChild() {
  return m_pFirstChild;
}

KeyValues *KeyValues::GetNext() {
  return m_pNext;
}

void KeyValues::SetParent(KeyValues *pParent) {
  m_pParent = pParent;
}

void KeyValues::SetFirstChild(KeyValues *pFirstChild) {
  m_pFirstChild = pFirstChild;
}

void KeyValues::SetNext(KeyValues *pNext) {
  m_pNext = pNext;
}

bool KeyValues::SaveToFile(const wchar_t *sFileName) {
  std::ofstream f(sFileName, std::ofstream::binary);

  if (!f.good()) {
    return false;
  }

  f.write("\xFF\xFE", 2);
  KeyValues *cur = this;

  while (cur) {
    if (cur->m_sKeyName.size() > 0) {
      f.write((char *)L"\"", 2);
      f.write((char *)cur->m_sKeyName.c_str(), cur->m_sKeyName.length() * 2);
      f.write((char *)L"\"", 2);
    }

    if (!cur->m_pFirstChild) {
      if (cur->m_sKeyName.size() > 0) {
        f.write((char *)L"\t\"", 4);
        f.write((char *)cur->m_sValue.c_str(), cur->m_sValue.length() * 2);
        f.write((char *)L"\"\r\n", 6);
      }

      while (true) {
        if (cur->m_pNext) {
          cur = cur->m_pNext;
          break;
        }

        if (cur->m_pParent) {
          f.write((char *)L"}\r\n", 6);
          cur = cur->m_pParent;
        } else {
          cur = NULL;
          break;
        }
      }
    } else {
      f.write((char *)L"\r\n{\r\n", 10);
      cur = cur->m_pFirstChild;
    }
  }

  return true;
}

KeyValues *KeyValues::LoadFromFile(const wchar_t *sFileName) {
  std::wifstream f(sFileName, std::wifstream::binary);

  if (!f.good()) {
    return NULL;
  }

  KeyValues *cur = new KeyValues();
  KeyValues *root = cur;
  std::wstring sToken;
  bool isValue = false;
  bool isInToken = false;
  bool isEscaped = false;
  bool isCheckingComment = false;
  bool isComment = false;
  bool isUCS2 = (f.get() == 0xFF && f.get() == 0xFE);

  if (isUCS2) {
    f.imbue(std::locale(f.getloc(), new std::codecvt_utf16<wchar_t, 0x10FFFF, std::little_endian>));
  } else {
    f.imbue(std::locale(f.getloc(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::little_endian>));
  }

  f.seekg(0, f.beg);

  while (!f.eof()) {
    wchar_t ch = f.get();
    
    if (isUCS2 && ch == 0xFFFFu) {
      break;
    }

    if (!isInToken && iswspace(ch)) {
      if (ch == '\n') {
        isCheckingComment = false;
        isComment = false;
      }

      continue;
    }

    if (isComment) {
      continue;
    }

    if (isEscaped) {
      isEscaped = false;
      sToken.push_back('\\');
      sToken.push_back(ch);
      continue;
    }

    if (ch == '/') {
      if (!isInToken) {
        if (!isCheckingComment) {
          isCheckingComment = true;
        } else {
          isCheckingComment = false;
          isComment = true;
        }
      } else {
        sToken.push_back(ch);
      }

      continue;
    }
    
    if (isCheckingComment) {
      isCheckingComment = false;
      sToken.push_back('/');
    }

    if (ch == '\\') {
      if (isInToken) {
        isEscaped = true;
      }
    } else if (ch == '"') {
      if (!isInToken) {
        isInToken = true;

        if (cur->m_sKeyName.size() != 0 && cur->m_pFirstChild == NULL && !isValue) {
          cur->m_pNext = new KeyValues();
          cur->m_pNext->m_pParent = cur->m_pParent;
          cur = cur->m_pNext;
        }
      } else {
        if (!isValue) {
          cur->m_sKeyName = sToken;
        } else {
          cur->m_sValue = sToken;
        }

        isInToken = false;
        isValue = !isValue;
        sToken.clear();
      }
    } else if (ch == '{') {
      if (!isInToken) {
        isValue = false;
        cur->m_pFirstChild = new KeyValues();
        cur->m_pFirstChild->m_pParent = cur;
        cur = cur->m_pFirstChild;
      } else {
        sToken.push_back(ch);
      }
    } else if (ch == '}') {
      if (!isInToken) {
        cur = cur->m_pParent;
      } else {
        sToken.push_back(ch);
      }
    } else if (isInToken) {
      sToken.push_back(ch);
    }
  }

  return root;
}
