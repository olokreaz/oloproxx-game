// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config.engine.proto

#include "config.engine.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace app {
namespace config {
namespace engine {
PROTOBUF_CONSTEXPR EngineConfig::EngineConfig(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.version_)*/0u
  , /*decltype(_impl_.window_mode_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct EngineConfigDefaultTypeInternal {
  PROTOBUF_CONSTEXPR EngineConfigDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~EngineConfigDefaultTypeInternal() {}
  union {
    EngineConfig _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 EngineConfigDefaultTypeInternal _EngineConfig_default_instance_;
}  // namespace engine
}  // namespace config
}  // namespace app
static ::_pb::Metadata file_level_metadata_config_2eengine_2eproto[1];
static const ::_pb::EnumDescriptor* file_level_enum_descriptors_config_2eengine_2eproto[1];
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_config_2eengine_2eproto = nullptr;

const uint32_t TableStruct_config_2eengine_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::app::config::engine::EngineConfig, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::app::config::engine::EngineConfig, _impl_.version_),
  PROTOBUF_FIELD_OFFSET(::app::config::engine::EngineConfig, _impl_.window_mode_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::app::config::engine::EngineConfig)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::app::config::engine::_EngineConfig_default_instance_._instance,
};

const char descriptor_table_protodef_config_2eengine_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\023config.engine.proto\022\021app.config.engine"
  "\"T\n\014EngineConfig\022\017\n\007version\030\001 \001(\r\0223\n\013win"
  "dow_mode\030\002 \001(\0162\036.app.config.engine.EWind"
  "owMode*>\n\013EWindowMode\022\r\n\tkWindowed\020\000\022\017\n\013"
  "kBorderless\020\001\022\017\n\013kFullscreen\020\002b\006proto3"
  ;
static ::_pbi::once_flag descriptor_table_config_2eengine_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_config_2eengine_2eproto = {
    false, false, 198, descriptor_table_protodef_config_2eengine_2eproto,
    "config.engine.proto",
    &descriptor_table_config_2eengine_2eproto_once, nullptr, 0, 1,
    schemas, file_default_instances, TableStruct_config_2eengine_2eproto::offsets,
    file_level_metadata_config_2eengine_2eproto, file_level_enum_descriptors_config_2eengine_2eproto,
    file_level_service_descriptors_config_2eengine_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_config_2eengine_2eproto_getter() {
  return &descriptor_table_config_2eengine_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_config_2eengine_2eproto(&descriptor_table_config_2eengine_2eproto);
namespace app {
namespace config {
namespace engine {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* EWindowMode_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_config_2eengine_2eproto);
  return file_level_enum_descriptors_config_2eengine_2eproto[0];
}
bool EWindowMode_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}


// ===================================================================

class EngineConfig::_Internal {
 public:
};

EngineConfig::EngineConfig(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:app.config.engine.EngineConfig)
}
EngineConfig::EngineConfig(const EngineConfig& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  EngineConfig* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.version_){}
    , decltype(_impl_.window_mode_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&_impl_.version_, &from._impl_.version_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.window_mode_) -
    reinterpret_cast<char*>(&_impl_.version_)) + sizeof(_impl_.window_mode_));
  // @@protoc_insertion_point(copy_constructor:app.config.engine.EngineConfig)
}

inline void EngineConfig::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.version_){0u}
    , decltype(_impl_.window_mode_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

EngineConfig::~EngineConfig() {
  // @@protoc_insertion_point(destructor:app.config.engine.EngineConfig)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void EngineConfig::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void EngineConfig::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void EngineConfig::Clear() {
// @@protoc_insertion_point(message_clear_start:app.config.engine.EngineConfig)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&_impl_.version_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.window_mode_) -
      reinterpret_cast<char*>(&_impl_.version_)) + sizeof(_impl_.window_mode_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* EngineConfig::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // uint32 version = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          _impl_.version_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // .app.config.engine.EWindowMode window_mode = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_window_mode(static_cast<::app::config::engine::EWindowMode>(val));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* EngineConfig::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:app.config.engine.EngineConfig)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // uint32 version = 1;
  if (this->_internal_version() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt32ToArray(1, this->_internal_version(), target);
  }

  // .app.config.engine.EWindowMode window_mode = 2;
  if (this->_internal_window_mode() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      2, this->_internal_window_mode(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:app.config.engine.EngineConfig)
  return target;
}

size_t EngineConfig::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:app.config.engine.EngineConfig)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // uint32 version = 1;
  if (this->_internal_version() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt32SizePlusOne(this->_internal_version());
  }

  // .app.config.engine.EWindowMode window_mode = 2;
  if (this->_internal_window_mode() != 0) {
    total_size += 1 +
      ::_pbi::WireFormatLite::EnumSize(this->_internal_window_mode());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData EngineConfig::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    EngineConfig::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*EngineConfig::GetClassData() const { return &_class_data_; }


void EngineConfig::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<EngineConfig*>(&to_msg);
  auto& from = static_cast<const EngineConfig&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:app.config.engine.EngineConfig)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_version() != 0) {
    _this->_internal_set_version(from._internal_version());
  }
  if (from._internal_window_mode() != 0) {
    _this->_internal_set_window_mode(from._internal_window_mode());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void EngineConfig::CopyFrom(const EngineConfig& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:app.config.engine.EngineConfig)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool EngineConfig::IsInitialized() const {
  return true;
}

void EngineConfig::InternalSwap(EngineConfig* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(EngineConfig, _impl_.window_mode_)
      + sizeof(EngineConfig::_impl_.window_mode_)
      - PROTOBUF_FIELD_OFFSET(EngineConfig, _impl_.version_)>(
          reinterpret_cast<char*>(&_impl_.version_),
          reinterpret_cast<char*>(&other->_impl_.version_));
}

::PROTOBUF_NAMESPACE_ID::Metadata EngineConfig::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_config_2eengine_2eproto_getter, &descriptor_table_config_2eengine_2eproto_once,
      file_level_metadata_config_2eengine_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace engine
}  // namespace config
}  // namespace app
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::app::config::engine::EngineConfig*
Arena::CreateMaybeMessage< ::app::config::engine::EngineConfig >(Arena* arena) {
  return Arena::CreateMessageInternal< ::app::config::engine::EngineConfig >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
