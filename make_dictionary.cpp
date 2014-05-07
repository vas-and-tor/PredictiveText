#include <cstdio>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#if defined(__GNUC__)
  #include <tr1/unordered_map>
  #define hash_map tr1::unordered_map
#else
  #include <unordered_map>
  #define hash_map unordered_map
#endif

struct utf {
  unsigned char first, second;
  utf(): first(0), second(0) { }
  bool is_cyrillic() {
    return ((first == 45  && second == 0)                      || // dash
            (first == 208 && (144 <= second && second <= 191)) || // А-Я, а-п
            (first == 209 && (128 <= second && second <= 143)) || // р-я
            (first == 208 && second == 129)                    || // Ё
            (first == 209 && second == 145));                     // ё
  }
  bool is_upper() {
    return (first == 208 && ((144 <= second && second <= 175) || second == 129));
  }
  void to_lower() {
    int ss = second;
    if (!is_upper()) return;
    if (first == 208 && second == 129) {
      first = 209;
      second = 145;
      return;
    }
    ss += 32;
    if (ss > 191) {
      first++;
      ss -= 64;
    }
    second = (unsigned char)ss;
  }
  string to_s() {
    char res[3] = {(char)first, (char)second, 0};
    return res;
  }
};

string to_lower(const string& s) {
  string res;
  for (int i = 0; i < (int)s.size(); i++) {
    if ((unsigned char)s[i] < 128) {
      res += s[i];
    } else {
      utf letter;
      letter.first = s[i];
      letter.second = s[++i];
      letter.to_lower();
      res += letter.to_s();
    }
  }
  return res;
}

hash_map<string, int> dict;
hash_map<string, string> low_words;
hash_map<string, bool> used;
hash_map<string, int> cnt;

bool comp(const pair<string, int>& a, const pair<string, int>& b) {
  return a.second > b.second;
}

bool comp2(const pair<string, int>& a, const pair<string, int>& b) {
  return to_lower(a.first) < to_lower(b.first);
}

inline bool read_utf(FILE* f, utf& letter) {
  int first = fgetc(f);
  letter.first = (unsigned char)first;
  if (first > 127) {
    letter.second = (unsigned char)fgetc(f);
  }
  if (first > 223) {
    fgetc(f);
  }
  if (first > 239) {
    fgetc(f);
  }
  return first < 224;
}

inline void save_word(string& word, const string& low_word) {
  if (word[0] == '-') return;
  if (word[word.size()-1] == '-') word.erase(word.size()-1);
  if (dict[word] == 0) {
    low_words[word] = low_word;
  }
  dict[word]++;
}

int main(void) {
  FILE* f = fopen("text.xml", "rt");
  vector<pair<string,int> > all_words, result;
  string word, low_word;
  while (!feof(f)) {
    utf letter;
    bool twoBytes = read_utf(f, letter);
    if (letter.first == 204 && letter.second == 129) continue;                                // знак ударения
    if (letter.first == 208 && letter.second == 129) letter.second = 149;                     // Ё -> Е
    if (letter.first == 209 && letter.second == 145) letter.first = 208, letter.second = 181; // ё -> е
    if (twoBytes && letter.is_cyrillic()) {
      word += letter.to_s();
      letter.to_lower();
      low_word += letter.to_s();
    } else if (word.size()) {
      save_word(word, low_word);
      word = "";
      low_word = "";
    }
  }
  if (word.size()) {
    save_word(word, low_word);
  }
  
  fclose(f);

  for (hash_map<string,int>::iterator it = dict.begin(); it != dict.end(); ++it) {
    string low_word = low_words[it->first];
    all_words.push_back(*it);
    cnt[low_word] += it->second;
  }

  sort(all_words.begin(), all_words.end(), comp);
  for (int i = 0; i < (int)all_words.size(); i++) {
    string word = all_words[i].first;
    string low_word = low_words[word];
    if (used[low_word]) continue;
    if (word != low_word && dict.find(low_word) != dict.end() &&
        (double)dict[low_word]/(double)dict[word] > 0.1) continue;
    if (cnt[low_word] < 5) continue; // Отсечение опечаток и очень редких слов
    result.push_back(make_pair(word, cnt[low_word]));
    used[low_word] = 1;
  }
  sort(result.begin(),result.end(),comp2);
  f = fopen("dictionary.txt", "wt");
  fprintf(f, "%d\n", (int)result.size());
  for (int i = 0; i < (int)result.size(); i++) {
    fprintf(f, "%s %d\n", result[i].first.c_str(), result[i].second);
  }
  fclose(f);
  return 0;
}
