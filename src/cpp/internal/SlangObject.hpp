#pragma once
#include "common/Object.h"
#include <slang.h>
namespace loveslang {

class SlangObject : public ISlangUnknown, public love::Object {
public:
    SlangObject();
    virtual ~SlangObject();
    uint32_t release() override;
    uint32_t addRef() override;
    SlangResult queryInterface(SlangUUID const& uuid, void** outObject) override;
};

}