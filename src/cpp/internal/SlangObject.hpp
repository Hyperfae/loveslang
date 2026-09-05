#pragma once
#include "common/Object.h"
#include <cstdint>
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
    SlangObject();
    virtual ~SlangObject();
    SlangObject(TLoveObject* obj);
    uint32_t release() override;
    uint32_t addRef() override;
    SlangResult queryInterface(SlangUUID const& uuid, void** outObject) override;

protected:
    TLoveObject* object;
private:
    uint32_t refcount;
};


template <
    ConceptSlangCastable TSlangInterface,
    ConceptObject TLoveObject
    // typename = std::enable_if_t< std::is_base_of_v<love::Object, TLoveObject> >
>
class CastableSlangObject : public SlangObject<TSlangInterface, TLoveObject> {
public:
    void* castAs(const SlangUUID& guid) override;
    CastableSlangObject();
    virtual ~CastableSlangObject();

};

}