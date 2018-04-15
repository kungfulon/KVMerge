#ifndef KEYVALUES_H
#define KEYVALUES_H

#include <string>

#define INVALID_KV (KeyValues *)-1

class KeyValues {
private:
  std::wstring m_sKeyName;
  std::wstring m_sValue;

  KeyValues *m_pParent;
  KeyValues *m_pFirstChild;
  KeyValues *m_pNext;

public:
  KeyValues(const wchar_t *sKeyName = L"", const wchar_t *sValue = L"");
  ~KeyValues();

  const wchar_t *GetKeyName();
  const wchar_t *GetValue();
  void SetKeyName(const wchar_t *sKeyName);
  void SetValue(const wchar_t *sValue);

  KeyValues *GetParent();
  KeyValues *GetFirstChild();
  KeyValues *GetNext();
  void SetParent(KeyValues *pParent);
  void SetFirstChild(KeyValues *pFirstChild);
  void SetNext(KeyValues *pNext);

  bool SaveToFile(const wchar_t *sFileName);

  static KeyValues *LoadFromFile(const wchar_t *sFileName);
};

#endif
