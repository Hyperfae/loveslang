#include "DataSlangBlob.hpp"
#include "common/Data.h"
#include <cstddef>
#include <slang.h>

namespace loveslang {

DataSlangBlob::DataSlangBlob(love::Data* dataptr) {
    object = dataptr;
    object->retain();
}


void const* DataSlangBlob::getBufferPointer() {
    return object->getData();
}

size_t DataSlangBlob::getBufferSize() {
    return object->getSize();
}

};