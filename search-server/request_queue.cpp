#include "request_queue.h"

using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server)
    : search_server_(search_server)
    , current_time_(0)
    , no_results_reqs_(0) {
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    const auto result_docs = search_server_.FindTopDocuments(raw_query, status);
    ProcessRequest(result_docs.size());
    return result_docs;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    const auto result_docs = search_server_.FindTopDocuments(raw_query);
    ProcessRequest(result_docs.size());
    return result_docs;
}

int RequestQueue::GetNoResultRequests() const {
    return no_results_reqs_;
}

void RequestQueue::ProcessRequest(int res_num) {
    if (current_time_ == 1440) {
        current_time_ = 0;
    } else {
        ++current_time_;
    }

    if (requests_.size() == min_in_day_) {
        if (requests_.front().results == 0) {
            --no_results_reqs_;
        }
        requests_.pop_front();
    }
    
    requests_.push_back({current_time_, res_num});
    if (res_num == 0) {
        ++no_results_reqs_;
    }
}