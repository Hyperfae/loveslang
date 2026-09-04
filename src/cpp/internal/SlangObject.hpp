#pragma once
#include "common/Object.h"
#include <slang.h>
#include <type_traits>

namespace loveslang {

 
template <typename T>
concept ConceptObject = std::is_base_of<love::Object, T>::value;
template <typename T>
concept ConceptSlangUnknown = std::is_base_of<ISlangUnknown, T>::value;
template <typename T>
concept ConceptSlangCastable = std::is_base_of<ISlangCastable, T>::value;

template <
    ConceptSlangUnknown TSlangInterface,
    ConceptObject TLoveObject
    // typename = std::enable_if_t< std::is_base_of_v<love::Object, TLoveObject> >
>
class SlangObject : public TSlangInterface {
public:
    uint32_t release() override;
    uint32_t addRef() override;
    SlangResult queryInterface(SlangUUID const& uuid, void** outObject) override;

protected:
    TLoveObject* object;
};


template <
    ConceptSlangCastable TSlangInterface,
    ConceptObject TLoveObject
    // typename = std::enable_if_t< std::is_base_of_v<love::Object, TLoveObject> >
>
class CastableSlangObject : public SlangObject<TSlangInterface, TLoveObject> {
    void* castAs(const SlangUUID& guid) override;

};

}