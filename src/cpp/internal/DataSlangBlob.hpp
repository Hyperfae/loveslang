#include "common/Data.h"
#include "common/Object.h"
#include <cstddef>
#include <cstdint>
#include <slang.h>
namespace loveslang {

class DataSlangBlob : public ISlangBlob, public love::Object {
public:
    DataSlangBlob(love::Data* data);
    // virtual ~DataSlangBlob();
    void const* getBufferPointer() override;
    size_t getBufferSize() override;
    uint32_t release() override;
    uint32_t addRef() override;
    SlangResult queryInterface(SlangUUID const& uuid, void** outObject) override;
private:
    love::Data* data;
};

}