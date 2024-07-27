#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>



using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }

    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double TF = 1.0 / words.size();
        
        for (const string& word : words) {
            //doc_index_[word].insert({document_id, TF}); не обрабатывает повт. слова
            doc_index_[word][document_id] += TF;
        }

        ++document_count_;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);
        
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }

private:
    struct Query {
        set<string> inc_words;
        set<string> exc_words;
    };

    map<string, map<int, double>> doc_index_;
    set<string> stop_words_;
    int document_count_ = 0;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }

        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query_words;

        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-') {
                query_words.exc_words.insert(word.substr(1));
            } else {
                query_words.inc_words.insert(word);
            }
        }
        
        return query_words;
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map<int, double> rel_docs;
        
        if (query_words.inc_words.empty()) {
            return matched_documents;
        }
        
        for (const string& word : query_words.inc_words) {
            if (doc_index_.count(word) > 0) {
                const double IDF = log(static_cast<double>(document_count_) / doc_index_.at(word).size());
                
                for (const auto& [doc_id, TF] : doc_index_.at(word)) {
                    rel_docs[doc_id] += TF * IDF;
                }
            }
        }
        
        /*if (query_words.inc_words.empty()) {
            return matched_documents;
        }
        
        for (const auto& [word, doc_map] : doc_index_) {
            if (query_words.inc_words.count(word) > 0) {
                const double IDF = log(static_cast<double>(document_count_) / doc_map.size());
                for (const auto& [doc_id, TF] : doc_map) {
                    rel_docs[doc_id] += TF * IDF;
                }
            }
        }
        for (const auto& [word, doc_map] : doc_index_) {
            if (query_words.exc_words.count(word) > 0) {
                for (const auto& [doc_id, _] : doc_map) {
                    rel_docs.erase(doc_id);
                }
            }
        }*/ //медленная реализация

        for (const string& word : query_words.exc_words) {
            if (doc_index_.count(word) > 0) {
                for (const auto& [doc_id, _] : doc_index_.at(word)) {
                    rel_docs.erase(doc_id);
                }
            }
        }

        for (const auto& [doc_id, relevance] : rel_docs) {
            matched_documents.push_back({doc_id, relevance});
        }

        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}