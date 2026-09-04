#include <slang-com-ptr.h>
#include <slang.h>
#include "SlangObject.hpp"
#include "common/Object.h"

namespace loveslang {
class LOVESlangFilesystem : public CastableSlangObject<ISlangFileSystem, love::Object> {
public:
    LOVESlangFilesystem();
    SlangResult loadFile(char const* path, ISlangBlob** outBlob) override;
};
}
