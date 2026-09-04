#include "SlangObject.hpp"
#include "common/Data.h"
#include "common/Object.h"
#include <cstdint>
#include <slang.h>

namespace loveslang {

template <ConceptSlangUnknown TI, ConceptObject TO>
uint32_t SlangObject<TI, TO>::release()
{
    uint32_t refcount = object->getReferenceCount() - 1;
    object->release();
    if (refcount == 0) {
        delete this;
    }
    return refcount;
}

template <ConceptSlangUnknown TI, ConceptObject TO>
uint32_t SlangObject<TI, TO>::addRef()
{
    object->retain();
    return object->getReferenceCount();
}


template <ConceptSlangCastable TI, ConceptObject TO>
void* CastableSlangObject<TI, TO>::castAs(const SlangUUID& uuid)
{
    return this;
}

template <ConceptSlangUnknown TI, ConceptObject TO>
SlangResult SlangObject<TI, TO>::queryInterface(SlangUUID const& uuid, void** outObject) {
    return SLANG_E_NOT_IMPLEMENTED;
};

}

template class loveslang::CastableSlangObject<ISlangFileSystem, love::Object>;
template class loveslang::SlangObject<ISlangBlob, love::Data>;
