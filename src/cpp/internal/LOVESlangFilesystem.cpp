#include "LOVESlangFilesystem.hpp"
#include "DataSlangBlob.hpp"
#include "common/runtime.h"
#include "modules/filesystem/FileData.h"
#include "modules/filesystem/Filesystem.h"
#include <slang.h>

namespace loveslang {

LOVESlangFilesystem::LOVESlangFilesystem() {}

SlangResult LOVESlangFilesystem::loadFile(char const* path, ISlangBlob** outBlob) {
    love::filesystem::Filesystem* fs = (love::Module::getInstance<love::filesystem::Filesystem>(love::Module::M_FILESYSTEM));
    if (!fs->exists(path))
        return SLANG_E_NOT_FOUND;
    love::filesystem::FileData* data = fs->read(path);
    if (data == nullptr) {
        return SLANG_E_CANNOT_OPEN;
    }
    auto datablob = new DataSlangBlob(data);
    datablob->AddRef();
    *outBlob = (static_cast<ISlangBlob *>(datablob));
    return SLANG_OK;
}

}

#define SlangObject LOVESlangFilesystem
#define SlangObjectCastable
#include "SlangObjectOld.ixx"