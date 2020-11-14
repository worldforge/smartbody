// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sbmotion.proto

#include "sbmotion.pb.h"

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
extern PROTOBUF_INTERNAL_EXPORT_sbmotion_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Motion_MetaData_sbmotion_2eproto;
namespace SmartBodyBinary {
class Motion_MetaDataDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Motion_MetaData> _instance;
} _Motion_MetaData_default_instance_;
class MotionDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Motion> _instance;
} _Motion_default_instance_;
}  // namespace SmartBodyBinary
static void InitDefaultsscc_info_Motion_sbmotion_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::SmartBodyBinary::_Motion_default_instance_;
    new (ptr) ::SmartBodyBinary::Motion();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::SmartBodyBinary::Motion::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Motion_sbmotion_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Motion_sbmotion_2eproto}, {
      &scc_info_Motion_MetaData_sbmotion_2eproto.base,}};

static void InitDefaultsscc_info_Motion_MetaData_sbmotion_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::SmartBodyBinary::_Motion_MetaData_default_instance_;
    new (ptr) ::SmartBodyBinary::Motion_MetaData();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::SmartBodyBinary::Motion_MetaData::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Motion_MetaData_sbmotion_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_Motion_MetaData_sbmotion_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_sbmotion_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_sbmotion_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_sbmotion_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_sbmotion_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion_MetaData, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion_MetaData, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion_MetaData, metadataname_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion_MetaData, metadatavalue_),
  0,
  1,
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, name_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, numchannels_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, channelname_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, channeltype_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, numframes_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, keytimes_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, keyvalues_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, syncpoints_),
  PROTOBUF_FIELD_OFFSET(::SmartBodyBinary::Motion, metadata_),
  0,
  1,
  ~0u,
  ~0u,
  2,
  ~0u,
  ~0u,
  ~0u,
  ~0u,
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 7, sizeof(::SmartBodyBinary::Motion_MetaData)},
  { 9, 23, sizeof(::SmartBodyBinary::Motion)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::SmartBodyBinary::_Motion_MetaData_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::SmartBodyBinary::_Motion_default_instance_),
};

const char descriptor_table_protodef_sbmotion_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\016sbmotion.proto\022\017SmartBodyBinary\"\216\002\n\006Mo"
  "tion\022\014\n\004name\030\001 \002(\t\022\023\n\013numChannels\030\002 \002(\005\022"
  "\023\n\013channelName\030\003 \003(\t\022\023\n\013channelType\030\004 \003("
  "\005\022\021\n\tnumFrames\030\005 \002(\005\022\020\n\010keyTimes\030\006 \003(\002\022\021"
  "\n\tkeyValues\030\007 \003(\002\022\022\n\nsyncPoints\030\010 \003(\002\0222\n"
  "\010metaData\030\t \003(\0132 .SmartBodyBinary.Motion"
  ".MetaData\0327\n\010MetaData\022\024\n\014metaDataName\030\001 "
  "\002(\t\022\025\n\rmetaDataValue\030\002 \002(\t"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_sbmotion_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_sbmotion_2eproto_sccs[2] = {
  &scc_info_Motion_sbmotion_2eproto.base,
  &scc_info_Motion_MetaData_sbmotion_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_sbmotion_2eproto_once;
static bool descriptor_table_sbmotion_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_sbmotion_2eproto = {
  &descriptor_table_sbmotion_2eproto_initialized, descriptor_table_protodef_sbmotion_2eproto, "sbmotion.proto", 306,
  &descriptor_table_sbmotion_2eproto_once, descriptor_table_sbmotion_2eproto_sccs, descriptor_table_sbmotion_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_sbmotion_2eproto::offsets,
  file_level_metadata_sbmotion_2eproto, 2, file_level_enum_descriptors_sbmotion_2eproto, file_level_service_descriptors_sbmotion_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_sbmotion_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_sbmotion_2eproto), true);
namespace SmartBodyBinary {

// ===================================================================

void Motion_MetaData::InitAsDefaultInstance() {
}
class Motion_MetaData::_Internal {
 public:
  using HasBits = decltype(std::declval<Motion_MetaData>()._has_bits_);
  static void set_has_metadataname(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_metadatavalue(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

Motion_MetaData::Motion_MetaData()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:SmartBodyBinary.Motion.MetaData)
}
Motion_MetaData::Motion_MetaData(const Motion_MetaData& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  metadataname_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_metadataname()) {
    metadataname_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.metadataname_);
  }
  metadatavalue_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_metadatavalue()) {
    metadatavalue_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.metadatavalue_);
  }
  // @@protoc_insertion_point(copy_constructor:SmartBodyBinary.Motion.MetaData)
}

void Motion_MetaData::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Motion_MetaData_sbmotion_2eproto.base);
  metadataname_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  metadatavalue_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

Motion_MetaData::~Motion_MetaData() {
  // @@protoc_insertion_point(destructor:SmartBodyBinary.Motion.MetaData)
  SharedDtor();
}

void Motion_MetaData::SharedDtor() {
  metadataname_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  metadatavalue_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void Motion_MetaData::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Motion_MetaData& Motion_MetaData::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Motion_MetaData_sbmotion_2eproto.base);
  return *internal_default_instance();
}


void Motion_MetaData::Clear() {
// @@protoc_insertion_point(message_clear_start:SmartBodyBinary.Motion.MetaData)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      metadataname_.ClearNonDefaultToEmptyNoArena();
    }
    if (cached_has_bits & 0x00000002u) {
      metadatavalue_.ClearNonDefaultToEmptyNoArena();
    }
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

const char* Motion_MetaData::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // required string metaDataName = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_metadataname();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          #ifndef NDEBUG
          ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "SmartBodyBinary.Motion.MetaData.metaDataName");
          #endif  // !NDEBUG
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // required string metaDataValue = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_metadatavalue();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          #ifndef NDEBUG
          ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "SmartBodyBinary.Motion.MetaData.metaDataValue");
          #endif  // !NDEBUG
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* Motion_MetaData::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:SmartBodyBinary.Motion.MetaData)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required string metaDataName = 1;
  if (cached_has_bits & 0x00000001u) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      this->_internal_metadataname().data(), static_cast<int>(this->_internal_metadataname().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "SmartBodyBinary.Motion.MetaData.metaDataName");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_metadataname(), target);
  }

  // required string metaDataValue = 2;
  if (cached_has_bits & 0x00000002u) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      this->_internal_metadatavalue().data(), static_cast<int>(this->_internal_metadatavalue().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "SmartBodyBinary.Motion.MetaData.metaDataValue");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_metadatavalue(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:SmartBodyBinary.Motion.MetaData)
  return target;
}

size_t Motion_MetaData::RequiredFieldsByteSizeFallback() const {
// @@protoc_insertion_point(required_fields_byte_size_fallback_start:SmartBodyBinary.Motion.MetaData)
  size_t total_size = 0;

  if (_internal_has_metadataname()) {
    // required string metaDataName = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_metadataname());
  }

  if (_internal_has_metadatavalue()) {
    // required string metaDataValue = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_metadatavalue());
  }

  return total_size;
}
size_t Motion_MetaData::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:SmartBodyBinary.Motion.MetaData)
  size_t total_size = 0;

  if (((_has_bits_[0] & 0x00000003) ^ 0x00000003) == 0) {  // All required fields are present.
    // required string metaDataName = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_metadataname());

    // required string metaDataValue = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_metadatavalue());

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Motion_MetaData::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:SmartBodyBinary.Motion.MetaData)
  GOOGLE_DCHECK_NE(&from, this);
  const Motion_MetaData* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<Motion_MetaData>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:SmartBodyBinary.Motion.MetaData)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:SmartBodyBinary.Motion.MetaData)
    MergeFrom(*source);
  }
}

void Motion_MetaData::MergeFrom(const Motion_MetaData& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:SmartBodyBinary.Motion.MetaData)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _has_bits_[0] |= 0x00000001u;
      metadataname_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.metadataname_);
    }
    if (cached_has_bits & 0x00000002u) {
      _has_bits_[0] |= 0x00000002u;
      metadatavalue_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.metadatavalue_);
    }
  }
}

void Motion_MetaData::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:SmartBodyBinary.Motion.MetaData)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Motion_MetaData::CopyFrom(const Motion_MetaData& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:SmartBodyBinary.Motion.MetaData)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Motion_MetaData::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;
  return true;
}

void Motion_MetaData::InternalSwap(Motion_MetaData* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  metadataname_.Swap(&other->metadataname_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  metadatavalue_.Swap(&other->metadatavalue_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
}

::PROTOBUF_NAMESPACE_ID::Metadata Motion_MetaData::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void Motion::InitAsDefaultInstance() {
}
class Motion::_Internal {
 public:
  using HasBits = decltype(std::declval<Motion>()._has_bits_);
  static void set_has_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_numchannels(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_numframes(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
};

Motion::Motion()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:SmartBodyBinary.Motion)
}
Motion::Motion(const Motion& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      _has_bits_(from._has_bits_),
      channelname_(from.channelname_),
      channeltype_(from.channeltype_),
      keytimes_(from.keytimes_),
      keyvalues_(from.keyvalues_),
      syncpoints_(from.syncpoints_),
      metadata_(from.metadata_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_name()) {
    name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
  ::memcpy(&numchannels_, &from.numchannels_,
    static_cast<size_t>(reinterpret_cast<char*>(&numframes_) -
    reinterpret_cast<char*>(&numchannels_)) + sizeof(numframes_));
  // @@protoc_insertion_point(copy_constructor:SmartBodyBinary.Motion)
}

void Motion::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Motion_sbmotion_2eproto.base);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&numchannels_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&numframes_) -
      reinterpret_cast<char*>(&numchannels_)) + sizeof(numframes_));
}

Motion::~Motion() {
  // @@protoc_insertion_point(destructor:SmartBodyBinary.Motion)
  SharedDtor();
}

void Motion::SharedDtor() {
  name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void Motion::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Motion& Motion::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Motion_sbmotion_2eproto.base);
  return *internal_default_instance();
}


void Motion::Clear() {
// @@protoc_insertion_point(message_clear_start:SmartBodyBinary.Motion)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  channelname_.Clear();
  channeltype_.Clear();
  keytimes_.Clear();
  keyvalues_.Clear();
  syncpoints_.Clear();
  metadata_.Clear();
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    name_.ClearNonDefaultToEmptyNoArena();
  }
  if (cached_has_bits & 0x00000006u) {
    ::memset(&numchannels_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&numframes_) -
        reinterpret_cast<char*>(&numchannels_)) + sizeof(numframes_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

const char* Motion::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // required string name = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          #ifndef NDEBUG
          ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "SmartBodyBinary.Motion.name");
          #endif  // !NDEBUG
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // required int32 numChannels = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_numchannels(&has_bits);
          numchannels_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // repeated string channelName = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          ptr -= 1;
          do {
            ptr += 1;
            auto str = _internal_add_channelname();
            ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
            #ifndef NDEBUG
            ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "SmartBodyBinary.Motion.channelName");
            #endif  // !NDEBUG
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<26>(ptr));
        } else goto handle_unusual;
        continue;
      // repeated int32 channelType = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          ptr -= 1;
          do {
            ptr += 1;
            _internal_add_channeltype(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr));
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<32>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedInt32Parser(_internal_mutable_channeltype(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // required int32 numFrames = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_numframes(&has_bits);
          numframes_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // repeated float keyTimes = 6;
      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 53)) {
          ptr -= 1;
          do {
            ptr += 1;
            _internal_add_keytimes(::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr));
            ptr += sizeof(float);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<53>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 50) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedFloatParser(_internal_mutable_keytimes(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // repeated float keyValues = 7;
      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 61)) {
          ptr -= 1;
          do {
            ptr += 1;
            _internal_add_keyvalues(::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr));
            ptr += sizeof(float);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<61>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 58) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedFloatParser(_internal_mutable_keyvalues(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // repeated float syncPoints = 8;
      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 69)) {
          ptr -= 1;
          do {
            ptr += 1;
            _internal_add_syncpoints(::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr));
            ptr += sizeof(float);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<69>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 66) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedFloatParser(_internal_mutable_syncpoints(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // repeated .SmartBodyBinary.Motion.MetaData metaData = 9;
      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_metadata(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<74>(ptr));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* Motion::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:SmartBodyBinary.Motion)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required string name = 1;
  if (cached_has_bits & 0x00000001u) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      this->_internal_name().data(), static_cast<int>(this->_internal_name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "SmartBodyBinary.Motion.name");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_name(), target);
  }

  // required int32 numChannels = 2;
  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(2, this->_internal_numchannels(), target);
  }

  // repeated string channelName = 3;
  for (int i = 0, n = this->_internal_channelname_size(); i < n; i++) {
    const auto& s = this->_internal_channelname(i);
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      s.data(), static_cast<int>(s.length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "SmartBodyBinary.Motion.channelName");
    target = stream->WriteString(3, s, target);
  }

  // repeated int32 channelType = 4;
  for (int i = 0, n = this->_internal_channeltype_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(4, this->_internal_channeltype(i), target);
  }

  // required int32 numFrames = 5;
  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(5, this->_internal_numframes(), target);
  }

  // repeated float keyTimes = 6;
  for (int i = 0, n = this->_internal_keytimes_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFloatToArray(6, this->_internal_keytimes(i), target);
  }

  // repeated float keyValues = 7;
  for (int i = 0, n = this->_internal_keyvalues_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFloatToArray(7, this->_internal_keyvalues(i), target);
  }

  // repeated float syncPoints = 8;
  for (int i = 0, n = this->_internal_syncpoints_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFloatToArray(8, this->_internal_syncpoints(i), target);
  }

  // repeated .SmartBodyBinary.Motion.MetaData metaData = 9;
  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_metadata_size()); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(9, this->_internal_metadata(i), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:SmartBodyBinary.Motion)
  return target;
}

size_t Motion::RequiredFieldsByteSizeFallback() const {
// @@protoc_insertion_point(required_fields_byte_size_fallback_start:SmartBodyBinary.Motion)
  size_t total_size = 0;

  if (_internal_has_name()) {
    // required string name = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_name());
  }

  if (_internal_has_numchannels()) {
    // required int32 numChannels = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_numchannels());
  }

  if (_internal_has_numframes()) {
    // required int32 numFrames = 5;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_numframes());
  }

  return total_size;
}
size_t Motion::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:SmartBodyBinary.Motion)
  size_t total_size = 0;

  if (((_has_bits_[0] & 0x00000007) ^ 0x00000007) == 0) {  // All required fields are present.
    // required string name = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_name());

    // required int32 numChannels = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_numchannels());

    // required int32 numFrames = 5;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_numframes());

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated string channelName = 3;
  total_size += 1 *
      ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(channelname_.size());
  for (int i = 0, n = channelname_.size(); i < n; i++) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
      channelname_.Get(i));
  }

  // repeated int32 channelType = 4;
  {
    size_t data_size = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      Int32Size(this->channeltype_);
    total_size += 1 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_channeltype_size());
    total_size += data_size;
  }

  // repeated float keyTimes = 6;
  {
    unsigned int count = static_cast<unsigned int>(this->_internal_keytimes_size());
    size_t data_size = 4UL * count;
    total_size += 1 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_keytimes_size());
    total_size += data_size;
  }

  // repeated float keyValues = 7;
  {
    unsigned int count = static_cast<unsigned int>(this->_internal_keyvalues_size());
    size_t data_size = 4UL * count;
    total_size += 1 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_keyvalues_size());
    total_size += data_size;
  }

  // repeated float syncPoints = 8;
  {
    unsigned int count = static_cast<unsigned int>(this->_internal_syncpoints_size());
    size_t data_size = 4UL * count;
    total_size += 1 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_syncpoints_size());
    total_size += data_size;
  }

  // repeated .SmartBodyBinary.Motion.MetaData metaData = 9;
  total_size += 1UL * this->_internal_metadata_size();
  for (const auto& msg : this->metadata_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Motion::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:SmartBodyBinary.Motion)
  GOOGLE_DCHECK_NE(&from, this);
  const Motion* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<Motion>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:SmartBodyBinary.Motion)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:SmartBodyBinary.Motion)
    MergeFrom(*source);
  }
}

void Motion::MergeFrom(const Motion& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:SmartBodyBinary.Motion)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  channelname_.MergeFrom(from.channelname_);
  channeltype_.MergeFrom(from.channeltype_);
  keytimes_.MergeFrom(from.keytimes_);
  keyvalues_.MergeFrom(from.keyvalues_);
  syncpoints_.MergeFrom(from.syncpoints_);
  metadata_.MergeFrom(from.metadata_);
  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      _has_bits_[0] |= 0x00000001u;
      name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.name_);
    }
    if (cached_has_bits & 0x00000002u) {
      numchannels_ = from.numchannels_;
    }
    if (cached_has_bits & 0x00000004u) {
      numframes_ = from.numframes_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void Motion::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:SmartBodyBinary.Motion)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Motion::CopyFrom(const Motion& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:SmartBodyBinary.Motion)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Motion::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000007) != 0x00000007) return false;
  if (!::PROTOBUF_NAMESPACE_ID::internal::AllAreInitialized(metadata_)) return false;
  return true;
}

void Motion::InternalSwap(Motion* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  channelname_.InternalSwap(&other->channelname_);
  channeltype_.InternalSwap(&other->channeltype_);
  keytimes_.InternalSwap(&other->keytimes_);
  keyvalues_.InternalSwap(&other->keyvalues_);
  syncpoints_.InternalSwap(&other->syncpoints_);
  metadata_.InternalSwap(&other->metadata_);
  name_.Swap(&other->name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(numchannels_, other->numchannels_);
  swap(numframes_, other->numframes_);
}

::PROTOBUF_NAMESPACE_ID::Metadata Motion::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace SmartBodyBinary
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::SmartBodyBinary::Motion_MetaData* Arena::CreateMaybeMessage< ::SmartBodyBinary::Motion_MetaData >(Arena* arena) {
  return Arena::CreateInternal< ::SmartBodyBinary::Motion_MetaData >(arena);
}
template<> PROTOBUF_NOINLINE ::SmartBodyBinary::Motion* Arena::CreateMaybeMessage< ::SmartBodyBinary::Motion >(Arena* arena) {
  return Arena::CreateInternal< ::SmartBodyBinary::Motion >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
