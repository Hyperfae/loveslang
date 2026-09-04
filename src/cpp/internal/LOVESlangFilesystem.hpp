#include <slang-com-ptr.h>
#include <slang.h>
#include "SlangObject.hpp"
#include "common/Object.h"

namespace loveslang {
class LOVESlangFilesystem : public ISlangFileSystem, public love::Object {
public:
    LOVESlangFilesystem();
    SlangResult loadFile(char const* path, ISlangBlob** outBlob) override;
    uint32_t release() override;
    uint32_t addRef() override;
    SlangResult queryInterface(SlangUUID const& uuid, void** outObject) override;
    void* castAs(const SlangUUID& guid) override;
};
}
