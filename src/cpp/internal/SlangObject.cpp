#include "SlangObject.hpp"
#include "common/Data.h"
#include "common/Object.h"
#include <cstdint>
#include <slang.h>

namespace loveslang {

template <ConceptSlangUnknown TI, ConceptObject TO>
SlangObject<TI, TO>::SlangObject() : refcount(1)
{
}

template <ConceptSlangUnknown TI, ConceptObject TO>
SlangObject<TI, TO>::SlangObject(TO* obj)
    : refcount(1)
    , object(obj)
{
    object->retain();
}


template <ConceptSlangUnknown TI, ConceptObject TO>
SlangObject<TI, TO>::~SlangObject()
{
    if (object != nullptr) {
        object->release();
    }
}

template <ConceptSlangUnknown TI, ConceptObject TO>
uint32_t SlangObject<TI, TO>::release()
{
    uint32_t new_refcount = --refcount;
    if (refcount == 0) {
        delete this;
    }
    return new_refcount;
}

template <ConceptSlangUnknown TI, ConceptObject TO>
uint32_t SlangObject<TI, TO>::addRef()
{
    return ++refcount;
}


template <ConceptSlangUnknown TI, ConceptObject TO>
SlangResult SlangObject<TI, TO>::queryInterface(SlangUUID const& uuid, void** outObject) {
    return SLANG_E_NOT_IMPLEMENTED;
};


template <ConceptSlangCastable TI, ConceptObject TO>
void* CastableSlangObject<TI, TO>::castAs(const SlangUUID& uuid)
{
    SlangUUID other = TI::getTypeGuid();
    // TODO: Also check data4, which is harder since it's an array
    if (uuid.data1 == other.data1 && uuid.data2 == other.data2 && uuid.data3 == other.data3) {
        return this;
    }
    return nullptr;
}

template <ConceptSlangCastable TI, ConceptObject TO>
CastableSlangObject<TI, TO>::~CastableSlangObject()
{
}

template <ConceptSlangCastable TI, ConceptObject TO>
CastableSlangObject<TI, TO>::CastableSlangObject()
{
}



}

template class loveslang::CastableSlangObject<ISlangFileSystem, love::Object>;
template class loveslang::SlangObject<ISlangBlob, love::Data>;
