#include "LOVESlangFilesystem.hpp"
#include "DataSlangBlob.hpp"
#include "common/runtime.h"
#include "filesystem/NativeFile.h"
#include "modules/filesystem/FileData.h"
#include "modules/filesystem/Filesystem.h"
#include <slang.h>

namespace loveslang {

LOVESlangFilesystem::LOVESlangFilesystem() {}

SlangResult LOVESlangFilesystem::loadFile(char const* path, ISlangBlob** outBlob) {
    // TODO: Embed this entire folder in the executable and read from there
    if (std::string_view(path).starts_with("__loveslang_lib_internal/love.slang")) {
        love::StrongRef n {new love::filesystem::NativeFile("./src/slang/love.slang", love::filesystem::File::Mode::MODE_READ)};
        love::StrongRef data {n->read()};
        *outBlob = new DataSlangBlob(data);
        return SLANG_OK;
    }
    love::filesystem::Filesystem* fs = (love::Module::getInstance<love::filesystem::Filesystem>(love::Module::M_FILESYSTEM));
    if (!fs->exists(path))
        return SLANG_E_NOT_FOUND;
    love::filesystem::FileData* data = fs->read(path);
    if (data == nullptr) {
        return SLANG_E_CANNOT_OPEN;
    }
    auto datablob = new DataSlangBlob(data);
    *outBlob = (static_cast<ISlangBlob *>(datablob));
    return SLANG_OK;
}

}
