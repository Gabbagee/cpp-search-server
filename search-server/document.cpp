#include "document.h"

using namespace std;

ostream& operator<<(ostream& out, const Document& document) {
    out << "{ document_id = "s << document.id
        << ", relevance = "s << document.relevance
        << ", rating = "s << document.rating << " }"s;
    return out;
}