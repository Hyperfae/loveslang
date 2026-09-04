#include "DataSlangBlob.hpp"
#include "common/Data.h"
#include <cstddef>
#include <slang.h>

namespace loveslang {

DataSlangBlob::DataSlangBlob(love::Data* dataptr) : data(dataptr) {
    data->retain();
}

DataSlangBlob::~DataSlangBlob() {
    data->release();
}

void const* DataSlangBlob::getBufferPointer() {
    return data->getData();
}

size_t DataSlangBlob::getBufferSize() {
    return data->getSize();
}

};

#define SlangObject DataSlangBlob
#include "SlangObject.ixx"