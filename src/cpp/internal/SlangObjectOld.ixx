#ifdef SlangObject
#include <cstdint>
#include <slang.h>

namespace loveslang {

SlangResult SlangObject::queryInterface(SlangUUID const& uuid, void** outObject) {
    return SLANG_E_NOT_IMPLEMENTED;
}

uint32_t SlangObject::addRef() {
    this->love::Object::retain();
}

uint32_t SlangObject::release() {
    this->love::Object::release();
}

#ifdef SlangObjectCastable
void* SlangObject::castAs(const SlangUUID& guid) {
    return this;
};
#endif

}
#endif