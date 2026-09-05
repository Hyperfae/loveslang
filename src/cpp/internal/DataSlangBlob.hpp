#pragma once

#include "SlangObject.hpp"
#include "common/Data.h"
#include "common/Object.h"
#include <cstddef>
#include <cstdint>
#include <slang.h>
namespace loveslang {

class DataSlangBlob : public SlangObject<ISlangBlob, love::Data> {
public:
    DataSlangBlob(love::Data* data);
    // virtual ~DataSlangBlob();
    void const* getBufferPointer() override;
    size_t getBufferSize() override;
};

}