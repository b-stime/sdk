// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#ifndef RUNTIME_VM_RAW_OBJECT_H_
#define RUNTIME_VM_RAW_OBJECT_H_

#if defined(SHOULD_NOT_INCLUDE_RUNTIME)
#error "Should not include runtime"
#endif

#include "platform/assert.h"
#include "platform/atomic.h"
#include "platform/thread_sanitizer.h"
#include "vm/class_id.h"
#include "vm/compiler/method_recognizer.h"
#include "vm/compiler/runtime_api.h"
#include "vm/exceptions.h"
#include "vm/globals.h"
#include "vm/pointer_tagging.h"
#include "vm/snapshot.h"
#include "vm/tagged_pointer.h"
#include "vm/token.h"
#include "vm/token_position.h"

// Currently we have two different axes for offset generation:
//
//  * Target architecture
//  * DART_PRECOMPILED_RUNTIME (i.e, AOT vs. JIT)
//
// That is, fields in UntaggedObject and its subclasses should only be included
// or excluded conditionally based on these factors. Otherwise, the generated
// offsets can be wrong (which should be caught by offset checking in dart.cc).
//
// TODO(dartbug.com/43646): Add DART_PRECOMPILER as another axis.

namespace dart {

// Forward declarations.
class Isolate;
class IsolateGroup;
#define DEFINE_FORWARD_DECLARATION(clazz) class Untagged##clazz;
CLASS_LIST(DEFINE_FORWARD_DECLARATION)
#undef DEFINE_FORWARD_DECLARATION
class CodeStatistics;
class StackFrame;

#define VISIT_FROM(type, first)                                                \
  type* from() { return reinterpret_cast<type*>(&first##_); }

#define VISIT_TO(type, last)                                                   \
  type* to() { return reinterpret_cast<type*>(&last##_); }

#define VISIT_TO_LENGTH(type, last)                                            \
  type* to(intptr_t length) { return reinterpret_cast<type*>(last); }

#define VISIT_NOTHING() int NothingToVisit();

#define ASSERT_UNCOMPRESSED(Type)                                              \
  ASSERT(SIZE_OF_DEREFERENCED_RETURNED_VALUE(Untagged##Type, from) ==          \
         sizeof(ObjectPtr))

#define ASSERT_COMPRESSED(Type)                                                \
  ASSERT(SIZE_OF_DEREFERENCED_RETURNED_VALUE(Untagged##Type, from) ==          \
         sizeof(CompressedObjectPtr))

#define ASSERT_NOTHING_TO_VISIT(Type)                                          \
  ASSERT(SIZE_OF_RETURNED_VALUE(Untagged##Type, NothingToVisit) == sizeof(int))

enum TypedDataElementType {
#define V(name) k##name##Element,
  CLASS_LIST_TYPED_DATA(V)
#undef V
};

#define SNAPSHOT_WRITER_SUPPORT()                                              \
  void WriteTo(SnapshotWriter* writer, intptr_t object_id,                     \
               Snapshot::Kind kind, bool as_reference);                        \
  friend class SnapshotWriter;

#define VISITOR_SUPPORT(object)                                                \
  static intptr_t Visit##object##Pointers(object##Ptr raw_obj,                 \
                                          ObjectPointerVisitor* visitor);

#define HEAP_PROFILER_SUPPORT() friend class HeapProfiler;

#define RAW_OBJECT_IMPLEMENTATION(object)                                      \
 private: /* NOLINT */                                                         \
  VISITOR_SUPPORT(object)                                                      \
  friend class object;                                                         \
  friend class UntaggedObject;                                                 \
  friend class Heap;                                                           \
  friend class Simulator;                                                      \
  friend class SimulatorHelpers;                                               \
  friend class OffsetsTable;                                                   \
  DISALLOW_ALLOCATION();                                                       \
  DISALLOW_IMPLICIT_CONSTRUCTORS(Untagged##object)

#define RAW_HEAP_OBJECT_IMPLEMENTATION(object)                                 \
 private:                                                                      \
  RAW_OBJECT_IMPLEMENTATION(object);                                           \
  SNAPSHOT_WRITER_SUPPORT()                                                    \
  HEAP_PROFILER_SUPPORT()                                                      \
  friend class object##SerializationCluster;                                   \
  friend class object##DeserializationCluster;                                 \
  friend class Serializer;                                                     \
  friend class Deserializer;                                                   \
  friend class Pass2Visitor;

// RawObject is the base class of all raw objects; even though it carries the
// tags_ field not all raw objects are allocated in the heap and thus cannot
// be dereferenced (e.g. RawSmi).
class UntaggedObject {
 public:
  // The tags field which is a part of the object header uses the following
  // bit fields for storing tags.
  enum TagBits {
    kCardRememberedBit = 0,
    kOldAndNotMarkedBit = 1,      // Incremental barrier target.
    kNewBit = 2,                  // Generational barrier target.
    kOldBit = 3,                  // Incremental barrier source.
    kOldAndNotRememberedBit = 4,  // Generational barrier source.
    kCanonicalBit = 5,
    kReservedTagPos = 6,
    kReservedTagSize = 2,

    kSizeTagPos = kReservedTagPos + kReservedTagSize,  // = 8
    kSizeTagSize = 8,
    kClassIdTagPos = kSizeTagPos + kSizeTagSize,  // = 16
    kClassIdTagSize = 16,
    kHashTagPos = kClassIdTagPos + kClassIdTagSize,  // = 32
    kHashTagSize = 32,
  };

  static const intptr_t kGenerationalBarrierMask = 1 << kNewBit;
  static const intptr_t kIncrementalBarrierMask = 1 << kOldAndNotMarkedBit;
  static const intptr_t kBarrierOverlapShift = 2;
  COMPILE_ASSERT(kOldAndNotMarkedBit + kBarrierOverlapShift == kOldBit);
  COMPILE_ASSERT(kNewBit + kBarrierOverlapShift == kOldAndNotRememberedBit);

  // The bit in the Smi tag position must be something that can be set to 0
  // for a dead filler object of either generation.
  // See Object::MakeUnusedSpaceTraversable.
  COMPILE_ASSERT(kCardRememberedBit == 0);

  // Encodes the object size in the tag in units of object alignment.
  class SizeTag {
   public:
    typedef intptr_t Type;

    static constexpr intptr_t kMaxSizeTagInUnitsOfAlignment =
        ((1 << UntaggedObject::kSizeTagSize) - 1);
    static constexpr intptr_t kMaxSizeTag =
        kMaxSizeTagInUnitsOfAlignment * kObjectAlignment;

    static constexpr uword encode(intptr_t size) {
      return SizeBits::encode(SizeToTagValue(size));
    }

    static constexpr uword decode(uword tag) {
      return TagValueToSize(SizeBits::decode(tag));
    }

    static constexpr uword update(intptr_t size, uword tag) {
      return SizeBits::update(SizeToTagValue(size), tag);
    }

    static constexpr bool SizeFits(intptr_t size) {
      DEBUG_ASSERT(Utils::IsAligned(size, kObjectAlignment));
      return (size <= kMaxSizeTag);
    }

   private:
    // The actual unscaled bit field used within the tag field.
    class SizeBits
        : public BitField<uword, intptr_t, kSizeTagPos, kSizeTagSize> {};

    static constexpr intptr_t SizeToTagValue(intptr_t size) {
      assert(Utils::IsAligned(size, kObjectAlignment));
      return !SizeFits(size) ? 0 : (size >> kObjectAlignmentLog2);
    }
    static constexpr intptr_t TagValueToSize(intptr_t value) {
      return value << kObjectAlignmentLog2;
    }
  };

  class ClassIdTag : public BitField<uword,
                                     ClassIdTagType,
                                     kClassIdTagPos,
                                     kClassIdTagSize> {};
  COMPILE_ASSERT(kBitsPerByte * sizeof(ClassIdTagType) == kClassIdTagSize);

#if defined(HASH_IN_OBJECT_HEADER)
  class HashTag : public BitField<uword, uint32_t, kHashTagPos, kHashTagSize> {
  };
#endif

  class CardRememberedBit
      : public BitField<uword, bool, kCardRememberedBit, 1> {};

  class OldAndNotMarkedBit
      : public BitField<uword, bool, kOldAndNotMarkedBit, 1> {};

  class NewBit : public BitField<uword, bool, kNewBit, 1> {};

  class CanonicalBit : public BitField<uword, bool, kCanonicalBit, 1> {};

  class OldBit : public BitField<uword, bool, kOldBit, 1> {};

  class OldAndNotRememberedBit
      : public BitField<uword, bool, kOldAndNotRememberedBit, 1> {};

  class ReservedBits
      : public BitField<uword, intptr_t, kReservedTagPos, kReservedTagSize> {};

  template <typename T>
  class Tags {
   public:
    Tags() : tags_(0) {}

    operator T() const { return tags_.load(std::memory_order_relaxed); }

    T operator=(T tags) {
      tags_.store(tags, std::memory_order_relaxed);
      return tags;
    }

    T load(std::memory_order order) const { return tags_.load(order); }

    bool compare_exchange_weak(T old_tags,
                               T new_tags,
                               std::memory_order order) {
      return tags_.compare_exchange_weak(old_tags, new_tags, order);
    }

    template <class TagBitField>
    typename TagBitField::Type Read() const {
      return TagBitField::decode(tags_.load(std::memory_order_relaxed));
    }

    template <class TagBitField>
    NO_SANITIZE_THREAD typename TagBitField::Type ReadIgnoreRace() const {
      return TagBitField::decode(*reinterpret_cast<const T*>(&tags_));
    }

    template <class TagBitField>
    void UpdateBool(bool value) {
      if (value) {
        tags_.fetch_or(TagBitField::encode(true), std::memory_order_relaxed);
      } else {
        tags_.fetch_and(~TagBitField::encode(true), std::memory_order_relaxed);
      }
    }

    template <class TagBitField>
    void Update(typename TagBitField::Type value) {
      T old_tags = tags_.load(std::memory_order_relaxed);
      T new_tags;
      do {
        new_tags = TagBitField::update(value, old_tags);
      } while (!tags_.compare_exchange_weak(old_tags, new_tags,
                                            std::memory_order_relaxed));
    }

    template <class TagBitField>
    void UpdateUnsynchronized(typename TagBitField::Type value) {
      tags_.store(
          TagBitField::update(value, tags_.load(std::memory_order_relaxed)),
          std::memory_order_relaxed);
    }

    template <class TagBitField>
    bool TryAcquire() {
      T mask = TagBitField::encode(true);
      T old_tags = tags_.fetch_or(mask, std::memory_order_relaxed);
      return !TagBitField::decode(old_tags);
    }

    template <class TagBitField>
    bool TryClear() {
      T mask = ~TagBitField::encode(true);
      T old_tags = tags_.fetch_and(mask, std::memory_order_relaxed);
      return TagBitField::decode(old_tags);
    }

   private:
    std::atomic<T> tags_;
    COMPILE_ASSERT(sizeof(std::atomic<T>) == sizeof(T));
  };

  // Assumes this is a heap object.
  bool IsNewObject() const {
    uword addr = reinterpret_cast<uword>(this);
    return (addr & kObjectAlignmentMask) == kNewObjectAlignmentOffset;
  }
  // Assumes this is a heap object.
  bool IsOldObject() const {
    uword addr = reinterpret_cast<uword>(this);
    return (addr & kObjectAlignmentMask) == kOldObjectAlignmentOffset;
  }

  // Support for GC marking bit. Marked objects are either grey (not yet
  // visited) or black (already visited).
  static bool IsMarked(uword tags) { return !OldAndNotMarkedBit::decode(tags); }
  bool IsMarked() const {
    ASSERT(IsOldObject());
    return !tags_.Read<OldAndNotMarkedBit>();
  }
  bool IsMarkedIgnoreRace() const {
    ASSERT(IsOldObject());
    return !tags_.ReadIgnoreRace<OldAndNotMarkedBit>();
  }
  void SetMarkBit() {
    ASSERT(IsOldObject());
    ASSERT(!IsMarked());
    tags_.UpdateBool<OldAndNotMarkedBit>(false);
  }
  void SetMarkBitUnsynchronized() {
    ASSERT(IsOldObject());
    ASSERT(!IsMarked());
    tags_.UpdateUnsynchronized<OldAndNotMarkedBit>(false);
  }
  void ClearMarkBit() {
    ASSERT(IsOldObject());
    ASSERT(IsMarked());
    tags_.UpdateBool<OldAndNotMarkedBit>(true);
  }
  // Returns false if the bit was already set.
  DART_WARN_UNUSED_RESULT
  bool TryAcquireMarkBit() {
    ASSERT(IsOldObject());
    return tags_.TryClear<OldAndNotMarkedBit>();
  }

  // Canonical objects have the property that two canonical objects are
  // logically equal iff they are the same object (pointer equal).
  bool IsCanonical() const { return tags_.Read<CanonicalBit>(); }
  void SetCanonical() { tags_.UpdateBool<CanonicalBit>(true); }
  void ClearCanonical() { tags_.UpdateBool<CanonicalBit>(false); }

  bool InVMIsolateHeap() const;

  // Support for GC remembered bit.
  bool IsRemembered() const {
    ASSERT(IsOldObject());
    return !tags_.Read<OldAndNotRememberedBit>();
  }
  void SetRememberedBit() {
    ASSERT(!IsRemembered());
    ASSERT(!IsCardRemembered());
    tags_.UpdateBool<OldAndNotRememberedBit>(false);
  }
  void ClearRememberedBit() {
    ASSERT(IsOldObject());
    tags_.UpdateBool<OldAndNotRememberedBit>(true);
  }

  DART_FORCE_INLINE
  void AddToRememberedSet(Thread* thread) {
    ASSERT(!this->IsRemembered());
    this->SetRememberedBit();
    thread->StoreBufferAddObject(ObjectPtr(this));
  }

  bool IsCardRemembered() const { return tags_.Read<CardRememberedBit>(); }
  void SetCardRememberedBitUnsynchronized() {
    ASSERT(!IsRemembered());
    ASSERT(!IsCardRemembered());
    tags_.UpdateUnsynchronized<CardRememberedBit>(true);
  }

  intptr_t GetClassId() const { return tags_.Read<ClassIdTag>(); }

#if defined(HASH_IN_OBJECT_HEADER)
  uint32_t GetHeaderHash() const { return tags_.Read<HashTag>(); }
  void SetHeaderHash(uint32_t h) { tags_.Update<HashTag>(h); }
#endif

  intptr_t HeapSize() const {
    uword tags = tags_;
    intptr_t result = SizeTag::decode(tags);
    if (result != 0) {
#if defined(DEBUG)
      // TODO(22501) Array::MakeFixedLength has a race with this code: we might
      // have loaded tags field and then MakeFixedLength could have updated it
      // leading to inconsistency between HeapSizeFromClass() and
      // SizeTag::decode(tags). We are working around it by reloading tags_ and
      // recomputing size from tags.
      const intptr_t size_from_class = HeapSizeFromClass(tags);
      if ((result > size_from_class) && (GetClassId() == kArrayCid) &&
          (tags_ != tags)) {
        result = SizeTag::decode(tags_);
      }
      ASSERT(result == size_from_class);
#endif
      return result;
    }
    result = HeapSizeFromClass(tags);
    ASSERT(result > SizeTag::kMaxSizeTag);
    return result;
  }

  // This variant must not deference this->tags_.
  intptr_t HeapSize(uword tags) const {
    intptr_t result = SizeTag::decode(tags);
    if (result != 0) {
      return result;
    }
    result = HeapSizeFromClass(tags);
    ASSERT(result > SizeTag::kMaxSizeTag);
    return result;
  }

  bool Contains(uword addr) const {
    intptr_t this_size = HeapSize();
    uword this_addr = UntaggedObject::ToAddr(this);
    return (addr >= this_addr) && (addr < (this_addr + this_size));
  }

  void Validate(IsolateGroup* isolate_group) const;
  bool FindObject(FindObjectVisitor* visitor);

  // This function may access the class-ID in the header, but it cannot access
  // the actual class object, because the sliding compactor uses this function
  // while the class objects are being moved.
  intptr_t VisitPointers(ObjectPointerVisitor* visitor) {
    // Fall back to virtual variant for predefined classes
    intptr_t class_id = GetClassId();
    if (class_id < kNumPredefinedCids) {
      return VisitPointersPredefined(visitor, class_id);
    }

    // Calculate the first and last raw object pointer fields.
    intptr_t instance_size = HeapSize();
    uword obj_addr = ToAddr(this);
    uword from = obj_addr + sizeof(UntaggedObject);
    uword to = obj_addr + instance_size - kWordSize;
    const auto first = reinterpret_cast<ObjectPtr*>(from);
    const auto last = reinterpret_cast<ObjectPtr*>(to);

#if defined(SUPPORT_UNBOXED_INSTANCE_FIELDS)
    const auto unboxed_fields_bitmap =
        visitor->shared_class_table()->GetUnboxedFieldsMapAt(class_id);

    if (!unboxed_fields_bitmap.IsEmpty()) {
      intptr_t bit = sizeof(UntaggedObject) / kWordSize;
      for (ObjectPtr* current = first; current <= last; current++) {
        if (!unboxed_fields_bitmap.Get(bit++)) {
          visitor->VisitPointer(current);
        }
      }
    } else {
      visitor->VisitPointers(first, last);
    }
#else
    // Call visitor function virtually
    visitor->VisitPointers(first, last);
#endif  // defined(SUPPORT_UNBOXED_INSTANCE_FIELDS)

    return instance_size;
  }

  template <class V>
  intptr_t VisitPointersNonvirtual(V* visitor) {
    // Fall back to virtual variant for predefined classes
    intptr_t class_id = GetClassId();
    if (class_id < kNumPredefinedCids) {
      return VisitPointersPredefined(visitor, class_id);
    }

    // Calculate the first and last raw object pointer fields.
    intptr_t instance_size = HeapSize();
    uword obj_addr = ToAddr(this);
    uword from = obj_addr + sizeof(UntaggedObject);
    uword to = obj_addr + instance_size - kWordSize;
    const auto first = reinterpret_cast<ObjectPtr*>(from);
    const auto last = reinterpret_cast<ObjectPtr*>(to);

#if defined(SUPPORT_UNBOXED_INSTANCE_FIELDS)
    const auto unboxed_fields_bitmap =
        visitor->shared_class_table()->GetUnboxedFieldsMapAt(class_id);

    if (!unboxed_fields_bitmap.IsEmpty()) {
      intptr_t bit = sizeof(UntaggedObject) / kWordSize;
      for (ObjectPtr* current = first; current <= last; current++) {
        if (!unboxed_fields_bitmap.Get(bit++)) {
          visitor->V::VisitPointers(current, current);
        }
      }
    } else {
      visitor->V::VisitPointers(first, last);
    }
#else
    // Call visitor function non-virtually
    visitor->V::VisitPointers(first, last);
#endif  // defined(SUPPORT_UNBOXED_INSTANCE_FIELDS)

    return instance_size;
  }

  // This variant ensures that we do not visit the extra slot created from
  // rounding up instance sizes up to the allocation unit.
  void VisitPointersPrecise(IsolateGroup* isolate_group,
                            ObjectPointerVisitor* visitor);

  static ObjectPtr FromAddr(uword addr) {
    // We expect the untagged address here.
    ASSERT((addr & kSmiTagMask) != kHeapObjectTag);
    return static_cast<ObjectPtr>(addr + kHeapObjectTag);
  }

  static uword ToAddr(const UntaggedObject* raw_obj) {
    return reinterpret_cast<uword>(raw_obj);
  }
  static uword ToAddr(const ObjectPtr raw_obj) {
    return static_cast<uword>(raw_obj) - kHeapObjectTag;
  }

  static bool IsCanonical(intptr_t value) {
    return CanonicalBit::decode(value);
  }

 private:
  Tags<uword> tags_;  // Various object tags (bits).

  intptr_t VisitPointersPredefined(ObjectPointerVisitor* visitor,
                                   intptr_t class_id);

  intptr_t HeapSizeFromClass(uword tags) const;

  void SetClassId(intptr_t new_cid) { tags_.Update<ClassIdTag>(new_cid); }
  void SetClassIdUnsynchronized(intptr_t new_cid) {
    tags_.UpdateUnsynchronized<ClassIdTag>(new_cid);
  }

  // All writes to heap objects should ultimately pass through one of the
  // methods below or their counterparts in Object, to ensure that the
  // write barrier is correctly applied.
 protected:
  template <typename type, std::memory_order order = std::memory_order_relaxed>
  type LoadPointer(type const* addr) const {
    return reinterpret_cast<std::atomic<type>*>(const_cast<type*>(addr))
        ->load(order);
  }
  template <typename type,
            typename compressed_type,
            std::memory_order order = std::memory_order_relaxed>
  type LoadCompressedPointer(compressed_type const* addr) const {
    compressed_type v = reinterpret_cast<std::atomic<compressed_type>*>(
                            const_cast<compressed_type*>(addr))
                            ->load(order);
    return static_cast<type>(v.Decompress(heap_base()));
  }

  uword heap_base() const {
    return reinterpret_cast<uword>(this) & kHeapBaseMask;
  }

  template <typename type, std::memory_order order = std::memory_order_relaxed>
  void StorePointer(type const* addr, type value) {
    reinterpret_cast<std::atomic<type>*>(const_cast<type*>(addr))
        ->store(value, order);
    if (value->IsHeapObject()) {
      CheckHeapPointerStore(value, Thread::Current());
    }
  }

  template <typename type,
            typename compressed_type,
            std::memory_order order = std::memory_order_relaxed>
  void StoreCompressedPointer(compressed_type const* addr, type value) {
    reinterpret_cast<std::atomic<compressed_type>*>(
        const_cast<compressed_type*>(addr))
        ->store(static_cast<compressed_type>(value), order);
    if (value->IsHeapObject()) {
      CheckHeapPointerStore(value, Thread::Current());
    }
  }

  template <typename type>
  void StorePointer(type const* addr, type value, Thread* thread) {
    *const_cast<type*>(addr) = value;
    if (value->IsHeapObject()) {
      CheckHeapPointerStore(value, thread);
    }
  }

  void StorePointer(CompressedObjectPtr* addr,
                    ObjectPtr value,
                    Thread* thread) {
    *addr = value;
    if (value->IsHeapObject()) {
      CheckHeapPointerStore(value, thread);
    }
  }

  template <typename type>
  void StorePointerUnaligned(type const* addr, type value, Thread* thread) {
    StoreUnaligned(const_cast<type*>(addr), value);
    if (value->IsHeapObject()) {
      CheckHeapPointerStore(value, thread);
    }
  }

  template <typename type, std::memory_order order = std::memory_order_relaxed>
  void StoreArrayPointer(type const* addr, type value) {
    reinterpret_cast<std::atomic<type>*>(const_cast<type*>(addr))
        ->store(value, order);
    if (value->IsHeapObject()) {
      CheckArrayPointerStore(addr, value, Thread::Current());
    }
  }

  template <typename type>
  void StoreArrayPointer(type const* addr, type value, Thread* thread) {
    *const_cast<type*>(addr) = value;
    if (value->IsHeapObject()) {
      CheckArrayPointerStore(addr, value, thread);
    }
  }

  void StoreArrayPointer(CompressedObjectPtr* addr,
                         ObjectPtr value,
                         Thread* thread) {
    *addr = value;
    if (value->IsHeapObject()) {
      CheckArrayPointerStore(addr, value, thread);
    }
  }

  template <std::memory_order order = std::memory_order_relaxed>
  SmiPtr LoadSmi(SmiPtr const* addr) const {
    return reinterpret_cast<std::atomic<SmiPtr>*>(const_cast<SmiPtr*>(addr))
        ->load(order);
  }
  template <std::memory_order order = std::memory_order_relaxed>
  SmiPtr LoadCompressedSmi(CompressedSmiPtr const* addr) const {
    return static_cast<SmiPtr>(reinterpret_cast<std::atomic<CompressedSmiPtr>*>(
                                   const_cast<CompressedSmiPtr*>(addr))
                                   ->load(order)
                                   .DecompressSmi());
  }

  // Use for storing into an explicitly Smi-typed field of an object
  // (i.e., both the previous and new value are Smis).
  template <std::memory_order order = std::memory_order_relaxed>
  void StoreSmi(SmiPtr const* addr, SmiPtr value) {
    // Can't use Contains, as array length is initialized through this method.
    ASSERT(reinterpret_cast<uword>(addr) >= UntaggedObject::ToAddr(this));
    reinterpret_cast<std::atomic<SmiPtr>*>(const_cast<SmiPtr*>(addr))
        ->store(value, order);
  }
  template <std::memory_order order = std::memory_order_relaxed>
  void StoreCompressedSmi(CompressedSmiPtr const* addr, SmiPtr value) {
    // Can't use Contains, as array length is initialized through this method.
    ASSERT(reinterpret_cast<uword>(addr) >= UntaggedObject::ToAddr(this));
    reinterpret_cast<std::atomic<CompressedSmiPtr>*>(
        const_cast<CompressedSmiPtr*>(addr))
        ->store(static_cast<CompressedSmiPtr>(value), order);
  }

 private:
  DART_FORCE_INLINE
  void CheckHeapPointerStore(ObjectPtr value, Thread* thread) {
    uword source_tags = this->tags_;
    uword target_tags = value->untag()->tags_;
    if (((source_tags >> kBarrierOverlapShift) & target_tags &
         thread->write_barrier_mask()) != 0) {
      if (value->IsNewObject()) {
        // Generational barrier: record when a store creates an
        // old-and-not-remembered -> new reference.
        AddToRememberedSet(thread);
      } else {
        // Incremental barrier: record when a store creates an
        // old -> old-and-not-marked reference.
        ASSERT(value->IsOldObject());
#if !defined(TARGET_ARCH_IA32)
        if (ClassIdTag::decode(target_tags) == kInstructionsCid) {
          // Instruction pages may be non-writable. Defer marking.
          thread->DeferredMarkingStackAddObject(value);
          return;
        }
#endif
        if (value->untag()->TryAcquireMarkBit()) {
          thread->MarkingStackAddObject(value);
        }
      }
    }
  }

  template <typename type>
  DART_FORCE_INLINE void CheckArrayPointerStore(type const* addr,
                                                ObjectPtr value,
                                                Thread* thread) {
    uword source_tags = this->tags_;
    uword target_tags = value->untag()->tags_;
    if (((source_tags >> kBarrierOverlapShift) & target_tags &
         thread->write_barrier_mask()) != 0) {
      if (value->IsNewObject()) {
        // Generational barrier: record when a store creates an
        // old-and-not-remembered -> new reference.
        ASSERT(!this->IsRemembered());
        if (this->IsCardRemembered()) {
          RememberCard(addr);
        } else {
          this->SetRememberedBit();
          thread->StoreBufferAddObject(static_cast<ObjectPtr>(this));
        }
      } else {
        // Incremental barrier: record when a store creates an
        // old -> old-and-not-marked reference.
        ASSERT(value->IsOldObject());
#if !defined(TARGET_ARCH_IA32)
        if (ClassIdTag::decode(target_tags) == kInstructionsCid) {
          // Instruction pages may be non-writable. Defer marking.
          thread->DeferredMarkingStackAddObject(value);
          return;
        }
#endif
        if (value->untag()->TryAcquireMarkBit()) {
          thread->MarkingStackAddObject(value);
        }
      }
    }
  }

  friend class StoreBufferUpdateVisitor;  // RememberCard
  void RememberCard(ObjectPtr const* slot);
#if defined(DART_COMPRESSED_POINTERS)
  void RememberCard(CompressedObjectPtr const* slot);
#endif

  friend class Array;
  friend class ByteBuffer;
  friend class CidRewriteVisitor;
  friend class Closure;
  friend class Code;
  friend class Pointer;
  friend class Double;
  friend class DynamicLibrary;
  friend class ForwardPointersVisitor;  // StorePointer
  friend class FreeListElement;
  friend class Function;
  friend class GCMarker;
  friend class GCSweeper;
  friend class ExternalTypedData;
  friend class ForwardList;
  friend class GrowableObjectArray;  // StorePointer
  friend class Heap;
  friend class ClassStatsVisitor;
  template <bool>
  friend class MarkingVisitorBase;
  friend class Mint;
  friend class Object;
  friend class OneByteString;  // StoreSmi
  friend class UntaggedInstance;
  friend class Scavenger;
  template <bool>
  friend class ScavengerVisitorBase;
  friend class ImageReader;  // tags_ check
  friend class ImageWriter;
  friend class AssemblyImageWriter;
  friend class BlobImageWriter;
  friend class SnapshotReader;
  friend class Deserializer;
  friend class SnapshotWriter;
  friend class String;
  friend class WeakProperty;            // StorePointer
  friend class Instance;                // StorePointer
  friend class StackFrame;              // GetCodeObject assertion.
  friend class CodeLookupTableBuilder;  // profiler
  friend class Simulator;
  friend class SimulatorHelpers;
  friend class ObjectLocator;
  friend class WriteBarrierUpdateVisitor;  // CheckHeapPointerStore
  friend class OffsetsTable;
  friend class Object;
  friend void ReportImpossibleNullError(intptr_t cid,
                                        StackFrame* caller_frame,
                                        Thread* thread);

  DISALLOW_ALLOCATION();
  DISALLOW_IMPLICIT_CONSTRUCTORS(UntaggedObject);
};

inline intptr_t ObjectPtr::GetClassId() const {
  return untag()->GetClassId();
}

#define POINTER_FIELD(type, name)                                              \
 public:                                                                       \
  template <std::memory_order order = std::memory_order_relaxed>               \
  type name() const {                                                          \
    return LoadPointer<type, order>(&name##_);                                 \
  }                                                                            \
  template <std::memory_order order = std::memory_order_relaxed>               \
  void set_##name(type value) {                                                \
    StorePointer<type, order>(&name##_, value);                                \
  }                                                                            \
                                                                               \
 protected:                                                                    \
  type name##_;

#define COMPRESSED_POINTER_FIELD(type, name)                                   \
 public:                                                                       \
  template <std::memory_order order = std::memory_order_relaxed>               \
  type name() const {                                                          \
    return LoadCompressedPointer<type, Compressed##type, order>(&name##_);     \
  }                                                                            \
  template <std::memory_order order = std::memory_order_relaxed>               \
  void set_##name(type value) {                                                \
    StoreCompressedPointer<type, Compressed##type, order>(&name##_, value);    \
  }                                                                            \
                                                                               \
 protected:                                                                    \
  Compressed##type name##_;

#define ARRAY_POINTER_FIELD(type, name)                                        \
 public:                                                                       \
  template <std::memory_order order = std::memory_order_relaxed>               \
  type name() const {                                                          \
    return LoadPointer<type, order>(&name##_);                                 \
  }                                                                            \
  template <std::memory_order order = std::memory_order_relaxed>               \
  void set_##name(type value) {                                                \
    StoreArrayPointer<type, order>(&name##_, value);                           \
  }                                                                            \
                                                                               \
 protected:                                                                    \
  type name##_;

#define VARIABLE_POINTER_FIELDS(type, accessor_name, array_name)               \
 public:                                                                       \
  template <std::memory_order order = std::memory_order_relaxed>               \
  type accessor_name(intptr_t index) const {                                   \
    return LoadPointer<type, order>(&array_name()[index]);                     \
  }                                                                            \
  template <std::memory_order order = std::memory_order_relaxed>               \
  void set_##accessor_name(intptr_t index, type value) {                       \
    StoreArrayPointer<type, order>(&array_name()[index], value);               \
  }                                                                            \
                                                                               \
 protected:                                                                    \
  type* array_name() { OPEN_ARRAY_START(type, type); }                         \
  type const* array_name() const { OPEN_ARRAY_START(type, type); }

#define SMI_FIELD(type, name)                                                  \
 public:                                                                       \
  template <std::memory_order order = std::memory_order_relaxed>               \
  type name() const {                                                          \
    type result = LoadSmi<order>(&name##_);                                    \
    ASSERT(!result.IsHeapObject());                                            \
    return result;                                                             \
  }                                                                            \
  template <std::memory_order order = std::memory_order_relaxed>               \
  void set_##name(type value) {                                                \
    ASSERT(!value.IsHeapObject());                                             \
    StoreSmi<order>(&name##_, value);                                          \
  }                                                                            \
                                                                               \
 protected:                                                                    \
  type name##_;

#define COMPRESSED_SMI_FIELD(type, name)                                       \
 public:                                                                       \
  template <std::memory_order order = std::memory_order_relaxed>               \
  type name() const {                                                          \
    type result = LoadCompressedSmi<order>(&name##_);                          \
    ASSERT(!result.IsHeapObject());                                            \
    return result;                                                             \
  }                                                                            \
  template <std::memory_order order = std::memory_order_relaxed>               \
  void set_##name(type value) {                                                \
    ASSERT(!value.IsHeapObject());                                             \
    StoreCompressedSmi(&name##_, value);                                       \
  }                                                                            \
                                                                               \
 protected:                                                                    \
  Compressed##type name##_;

class UntaggedClass : public UntaggedObject {
 public:
  enum ClassFinalizedState {
    kAllocated = 0,  // Initial state.
    kPreFinalized,   // VM classes: size precomputed, but no checks done.
    kFinalized,      // Class parsed, code compiled, not ready for allocation.
    kAllocateFinalized,  // CHA invalidated, class is ready for allocation.
  };
  enum ClassLoadingState {
    // Class object is created, but it is not filled up.
    // At this state class can only be used as a forward reference during
    // class loading.
    kNameOnly = 0,
    // Class declaration information such as type parameters, supertype and
    // implemented interfaces are loaded. However, types in the class are
    // not finalized yet.
    kDeclarationLoaded,
    // Types in the class are finalized. At this point, members can be loaded
    // and class can be finalized.
    kTypeFinalized,
  };

 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(Class);

  VISIT_FROM(CompressedObjectPtr, name)
  COMPRESSED_POINTER_FIELD(StringPtr, name)
  COMPRESSED_POINTER_FIELD(StringPtr, user_name)
  COMPRESSED_POINTER_FIELD(ArrayPtr, functions)
  COMPRESSED_POINTER_FIELD(ArrayPtr, functions_hash_table)
  COMPRESSED_POINTER_FIELD(ArrayPtr, fields)
  COMPRESSED_POINTER_FIELD(ArrayPtr, offset_in_words_to_field)
  COMPRESSED_POINTER_FIELD(ArrayPtr, interfaces)  // Array of AbstractType.
  COMPRESSED_POINTER_FIELD(ScriptPtr, script)
  COMPRESSED_POINTER_FIELD(LibraryPtr, library)
  // Array of TypeParameter.
  COMPRESSED_POINTER_FIELD(TypeArgumentsPtr, type_parameters)
  COMPRESSED_POINTER_FIELD(AbstractTypePtr, super_type)
  // Canonicalized const instances of this class.
  COMPRESSED_POINTER_FIELD(ArrayPtr, constants)
  // Declaration type for this class.
  COMPRESSED_POINTER_FIELD(TypePtr, declaration_type)
  // Cache for dispatcher functions.
  COMPRESSED_POINTER_FIELD(ArrayPtr, invocation_dispatcher_cache)
  // Stub code for allocation of instances.
  COMPRESSED_POINTER_FIELD(CodePtr, allocation_stub)
  // Array of Class.
  COMPRESSED_POINTER_FIELD(GrowableObjectArrayPtr, direct_implementors)
  // Array of Class.
  COMPRESSED_POINTER_FIELD(GrowableObjectArrayPtr, direct_subclasses)
  // CHA optimized codes.
  COMPRESSED_POINTER_FIELD(ArrayPtr, dependent_code)
  VISIT_TO(CompressedObjectPtr, dependent_code)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) {
    switch (kind) {
      case Snapshot::kFullAOT:
        return reinterpret_cast<CompressedObjectPtr*>(&allocation_stub_);
      case Snapshot::kFull:
      case Snapshot::kFullCore:
        return reinterpret_cast<CompressedObjectPtr*>(&direct_subclasses_);
      case Snapshot::kFullJIT:
        return reinterpret_cast<CompressedObjectPtr*>(&dependent_code_);
      case Snapshot::kMessage:
      case Snapshot::kNone:
      case Snapshot::kInvalid:
        break;
    }
    UNREACHABLE();
    return NULL;
  }

  TokenPosition token_pos_;
  TokenPosition end_token_pos_;

  classid_t id_;                // Class Id, also index in the class table.
  int16_t num_type_arguments_;  // Number of type arguments in flattened vector.
  uint16_t num_native_fields_;
  uint32_t state_bits_;

  // Size if fixed len or 0 if variable len.
  int32_t host_instance_size_in_words_;

  // Offset of type args fld.
  int32_t host_type_arguments_field_offset_in_words_;

  // Offset of the next instance field.
  int32_t host_next_field_offset_in_words_;

#if !defined(DART_PRECOMPILED_RUNTIME)
  // Size if fixed len or 0 if variable len (target).
  int32_t target_instance_size_in_words_;

  // Offset of type args fld.
  int32_t target_type_arguments_field_offset_in_words_;

  // Offset of the next instance field (target).
  int32_t target_next_field_offset_in_words_;
#endif  // !defined(DART_PRECOMPILED_RUNTIME)

#if !defined(DART_PRECOMPILED_RUNTIME)
  uint32_t kernel_offset_;
#endif  // !defined(DART_PRECOMPILED_RUNTIME)

  friend class Instance;
  friend class IsolateGroup;
  friend class Object;
  friend class UntaggedInstance;
  friend class UntaggedInstructions;
  friend class UntaggedTypeArguments;
  friend class SnapshotReader;
  friend class InstanceSerializationCluster;
  friend class TypeSerializationCluster;
  friend class CidRewriteVisitor;
  friend class Api;
};

class UntaggedPatchClass : public UntaggedObject {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(PatchClass);

  VISIT_FROM(CompressedObjectPtr, patched_class)
  COMPRESSED_POINTER_FIELD(ClassPtr, patched_class)
  COMPRESSED_POINTER_FIELD(ClassPtr, origin_class)
  COMPRESSED_POINTER_FIELD(ScriptPtr, script)
  COMPRESSED_POINTER_FIELD(ExternalTypedDataPtr, library_kernel_data)
  VISIT_TO(CompressedObjectPtr, library_kernel_data)

  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) {
    switch (kind) {
      case Snapshot::kFullAOT:
        return reinterpret_cast<CompressedObjectPtr*>(&script_);
      case Snapshot::kFull:
      case Snapshot::kFullCore:
      case Snapshot::kFullJIT:
        return reinterpret_cast<CompressedObjectPtr*>(&library_kernel_data_);
      case Snapshot::kMessage:
      case Snapshot::kNone:
      case Snapshot::kInvalid:
        break;
    }
    UNREACHABLE();
    return NULL;
  }

  NOT_IN_PRECOMPILED(intptr_t library_kernel_offset_);

  friend class Function;
};

class UntaggedFunction : public UntaggedObject {
 public:
  // When you add a new kind, please also update the observatory to account
  // for the new string returned by KindToCString().
  // - runtime/observatory/lib/src/models/objects/function.dart (FunctionKind)
  // - runtime/observatory/lib/src/elements/function_view.dart
  //   (_functionKindToString)
  // - runtime/observatory/lib/src/service/object.dart (stringToFunctionKind)
#define FOR_EACH_RAW_FUNCTION_KIND(V)                                          \
  /* an ordinary or operator method */                                         \
  V(RegularFunction)                                                           \
  /* a user-declared closure function */                                       \
  V(ClosureFunction)                                                           \
  /* an implicit closure (i.e., tear-off) */                                   \
  V(ImplicitClosureFunction)                                                   \
  /* a signature only without actual code */                                   \
  V(GetterFunction)                                                            \
  /* setter functions e.g: set foo(..) { .. } */                               \
  V(SetterFunction)                                                            \
  /* a generative (is_static=false) or factory (is_static=true) constructor */ \
  V(Constructor)                                                               \
  /* an implicit getter for instance fields */                                 \
  V(ImplicitGetter)                                                            \
  /* an implicit setter for instance fields */                                 \
  V(ImplicitSetter)                                                            \
  /* represents an implicit getter for static fields with initializers */      \
  V(ImplicitStaticGetter)                                                      \
  /* the initialization expression for a static or instance field */           \
  V(FieldInitializer)                                                          \
  /* return a closure on the receiver for tear-offs */                         \
  V(MethodExtractor)                                                           \
  /* builds an Invocation and invokes noSuchMethod */                          \
  V(NoSuchMethodDispatcher)                                                    \
  /* invokes a field as a closure (i.e., call-through-getter) */               \
  V(InvokeFieldDispatcher)                                                     \
  /* a generated irregexp matcher function. */                                 \
  V(IrregexpFunction)                                                          \
  /* a forwarder which performs type checks for arguments of a dynamic call */ \
  /* (i.e., those checks omitted by the caller for interface calls). */        \
  V(DynamicInvocationForwarder)                                                \
  V(FfiTrampoline)

  enum Kind {
#define KIND_DEFN(Name) k##Name,
    FOR_EACH_RAW_FUNCTION_KIND(KIND_DEFN)
#undef KIND_DEFN
  };

  static const char* KindToCString(Kind k) {
    switch (k) {
#define KIND_CASE(Name)                                                        \
  case Kind::k##Name:                                                          \
    return #Name;
      FOR_EACH_RAW_FUNCTION_KIND(KIND_CASE)
#undef KIND_CASE
      default:
        UNREACHABLE();
        return nullptr;
    }
  }

  static bool ParseKind(const char* str, Kind* out) {
#define KIND_CASE(Name)                                                        \
  if (strcmp(str, #Name) == 0) {                                               \
    *out = Kind::k##Name;                                                      \
    return true;                                                               \
  }
    FOR_EACH_RAW_FUNCTION_KIND(KIND_CASE)
#undef KIND_CASE
    return false;
  }

  enum AsyncModifier {
    kNoModifier = 0x0,
    kAsyncBit = 0x1,
    kGeneratorBit = 0x2,
    kAsync = kAsyncBit,
    kSyncGen = kGeneratorBit,
    kAsyncGen = kAsyncBit | kGeneratorBit,
  };

  // Wraps a 64-bit integer to represent the bitmap for unboxed parameters and
  // return value. Two bits are used for each of them - the first one indicates
  // whether this value is unboxed or not, and the second one says whether it is
  // an integer or a double. It includes the two bits for the receiver, even
  // though currently we do not have information from TFA that allows the
  // receiver to be unboxed.
  class alignas(8) UnboxedParameterBitmap {
   public:
    static constexpr intptr_t kBitsPerParameter = 2;
    static constexpr intptr_t kParameterBitmask = (1 << kBitsPerParameter) - 1;
    static constexpr intptr_t kCapacity =
        (kBitsPerByte * sizeof(uint64_t)) / kBitsPerParameter;

    UnboxedParameterBitmap() : bitmap_(0) {}
    explicit UnboxedParameterBitmap(uint64_t bitmap) : bitmap_(bitmap) {}
    UnboxedParameterBitmap(const UnboxedParameterBitmap&) = default;
    UnboxedParameterBitmap& operator=(const UnboxedParameterBitmap&) = default;

    DART_FORCE_INLINE bool IsUnboxed(intptr_t position) const {
      if (position >= kCapacity) {
        return false;
      }
      ASSERT(Utils::TestBit(bitmap_, kBitsPerParameter * position) ||
             !Utils::TestBit(bitmap_, kBitsPerParameter * position + 1));
      return Utils::TestBit(bitmap_, kBitsPerParameter * position);
    }
    DART_FORCE_INLINE bool IsUnboxedInteger(intptr_t position) const {
      if (position >= kCapacity) {
        return false;
      }
      return Utils::TestBit(bitmap_, kBitsPerParameter * position) &&
             !Utils::TestBit(bitmap_, kBitsPerParameter * position + 1);
    }
    DART_FORCE_INLINE bool IsUnboxedDouble(intptr_t position) const {
      if (position >= kCapacity) {
        return false;
      }
      return Utils::TestBit(bitmap_, kBitsPerParameter * position) &&
             Utils::TestBit(bitmap_, kBitsPerParameter * position + 1);
    }
    DART_FORCE_INLINE void SetUnboxedInteger(intptr_t position) {
      ASSERT(position < kCapacity);
      bitmap_ |= Utils::Bit<decltype(bitmap_)>(kBitsPerParameter * position);
      ASSERT(!Utils::TestBit(bitmap_, kBitsPerParameter * position + 1));
    }
    DART_FORCE_INLINE void SetUnboxedDouble(intptr_t position) {
      ASSERT(position < kCapacity);
      bitmap_ |= Utils::Bit<decltype(bitmap_)>(kBitsPerParameter * position);
      bitmap_ |=
          Utils::Bit<decltype(bitmap_)>(kBitsPerParameter * position + 1);
    }
    DART_FORCE_INLINE uint64_t Value() const { return bitmap_; }
    DART_FORCE_INLINE bool IsEmpty() const { return bitmap_ == 0; }
    DART_FORCE_INLINE void Reset() { bitmap_ = 0; }
    DART_FORCE_INLINE bool HasUnboxedParameters() const {
      return (bitmap_ >> kBitsPerParameter) != 0;
    }
    DART_FORCE_INLINE bool HasUnboxedReturnValue() const {
      return (bitmap_ & kParameterBitmask) != 0;
    }

   private:
    uint64_t bitmap_;
  };

 private:
  friend class Class;
  friend class UnitDeserializationRoots;

  RAW_HEAP_OBJECT_IMPLEMENTATION(Function);

  uword entry_point_;            // Accessed from generated code.
  uword unchecked_entry_point_;  // Accessed from generated code.

  VISIT_FROM(CompressedObjectPtr, name)
  COMPRESSED_POINTER_FIELD(StringPtr, name)
  // Class or patch class or mixin class where this function is defined.
  COMPRESSED_POINTER_FIELD(ObjectPtr, owner)
  COMPRESSED_POINTER_FIELD(ArrayPtr, parameter_names)
  COMPRESSED_POINTER_FIELD(FunctionTypePtr, signature)
  // Additional data specific to the function kind. See Function::set_data()
  // for details.
  COMPRESSED_POINTER_FIELD(ObjectPtr, data)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) {
    switch (kind) {
      case Snapshot::kFullAOT:
      case Snapshot::kFull:
      case Snapshot::kFullCore:
      case Snapshot::kFullJIT:
        return reinterpret_cast<CompressedObjectPtr*>(&data_);
      case Snapshot::kMessage:
      case Snapshot::kNone:
      case Snapshot::kInvalid:
        break;
    }
    UNREACHABLE();
    return NULL;
  }
  // ICData of unoptimized code.
  COMPRESSED_POINTER_FIELD(ArrayPtr, ic_data_array);
  // Currently active code. Accessed from generated code.
  COMPRESSED_POINTER_FIELD(CodePtr, code);
  // Unoptimized code, keep it after optimization.
  NOT_IN_PRECOMPILED(COMPRESSED_POINTER_FIELD(CodePtr, unoptimized_code));
#if defined(DART_PRECOMPILED_RUNTIME)
  VISIT_TO(CompressedObjectPtr, code);
#else
  VISIT_TO(CompressedObjectPtr, unoptimized_code);
#endif

  NOT_IN_PRECOMPILED(UnboxedParameterBitmap unboxed_parameters_info_);
  NOT_IN_PRECOMPILED(TokenPosition token_pos_);
  NOT_IN_PRECOMPILED(TokenPosition end_token_pos_);
  Tags<uint32_t> kind_tag_;  // See Function::KindTagBits.
  uint32_t packed_fields_;

  // TODO(regis): Split packed_fields_ in 2 uint32_t if max values are too low.

  static constexpr intptr_t kMaxOptimizableBits = 1;
  static constexpr intptr_t kMaxTypeParametersBits = 7;
  static constexpr intptr_t kMaxHasNamedOptionalParametersBits = 1;
  static constexpr intptr_t kMaxFixedParametersBits = 10;
  static constexpr intptr_t kMaxOptionalParametersBits = 10;

  typedef BitField<uint32_t, bool, 0, kMaxOptimizableBits> PackedOptimizable;
  typedef BitField<uint32_t,
                   uint8_t,
                   PackedOptimizable::kNextBit,
                   kMaxTypeParametersBits>
      PackedNumTypeParameters;
  typedef BitField<uint32_t,
                   bool,
                   PackedNumTypeParameters::kNextBit,
                   kMaxHasNamedOptionalParametersBits>
      PackedHasNamedOptionalParameters;
  typedef BitField<uint32_t,
                   uint16_t,
                   PackedHasNamedOptionalParameters::kNextBit,
                   kMaxFixedParametersBits>
      PackedNumFixedParameters;
  typedef BitField<uint32_t,
                   uint16_t,
                   PackedNumFixedParameters::kNextBit,
                   kMaxOptionalParametersBits>
      PackedNumOptionalParameters;
  static_assert(PackedNumOptionalParameters::kNextBit <=
                    kBitsPerByte * sizeof(decltype(packed_fields_)),
                "UntaggedFunction::packed_fields_ bitfields don't fit.");

#define JIT_FUNCTION_COUNTERS(F)                                               \
  F(intptr_t, int32_t, usage_counter)                                          \
  F(intptr_t, uint16_t, optimized_instruction_count)                           \
  F(intptr_t, uint16_t, optimized_call_site_count)                             \
  F(int8_t, int8_t, deoptimization_counter)                                    \
  F(intptr_t, int8_t, state_bits)                                              \
  F(int, int8_t, inlining_depth)

#if !defined(DART_PRECOMPILED_RUNTIME)
  uint32_t kernel_offset_;

#define DECLARE(return_type, type, name) type name##_;
  JIT_FUNCTION_COUNTERS(DECLARE)
#undef DECLARE

#endif  // !defined(DART_PRECOMPILED_RUNTIME)

  friend class UntaggedFunctionType;  // To use same constants for packing.
};

class UntaggedClosureData : public UntaggedObject {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(ClosureData);

  VISIT_FROM(CompressedObjectPtr, context_scope)
  COMPRESSED_POINTER_FIELD(ContextScopePtr, context_scope)
  // Enclosing function of this local function.
#if defined(DART_PRECOMPILER)
  // Can be wrapped by a WSR in the precompiler.
  COMPRESSED_POINTER_FIELD(ObjectPtr, parent_function)
#else
  COMPRESSED_POINTER_FIELD(FunctionPtr, parent_function)
#endif
  // Closure object for static implicit closures.
  COMPRESSED_POINTER_FIELD(InstancePtr, closure)
  // Instantiate-to-bounds TAV for use when no TAV is provided.
  COMPRESSED_POINTER_FIELD(TypeArgumentsPtr, default_type_arguments)
  VISIT_TO(CompressedObjectPtr, default_type_arguments)

  enum class DefaultTypeArgumentsKind : uint8_t {
    // Only here to make sure it's explicitly set appropriately.
    kInvalid = 0,
    // Must instantiate the default type arguments before use.
    kNeedsInstantiation,
    // The default type arguments are already instantiated.
    kIsInstantiated,
    // Use the instantiator type arguments that would be used to instantiate
    // the default type arguments, as instantiating produces the same result.
    kSharesInstantiatorTypeArguments,
    // Use the function type arguments that would be used to instantiate
    // the default type arguments, as instantiating produces the same result.
    kSharesFunctionTypeArguments,
  };

  // kernel_to_il.cc assumes we can load the untagged value and box it in a Smi.
  static_assert(sizeof(DefaultTypeArgumentsKind) * kBitsPerByte <=
                    compiler::target::kSmiBits,
                "Default type arguments kind must fit in a Smi");

  DefaultTypeArgumentsKind default_type_arguments_kind_;

  friend class Function;
};

class UntaggedFfiTrampolineData : public UntaggedObject {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(FfiTrampolineData);

  VISIT_FROM(CompressedObjectPtr, signature_type)
  COMPRESSED_POINTER_FIELD(TypePtr, signature_type)
  COMPRESSED_POINTER_FIELD(FunctionTypePtr, c_signature)

  // Target Dart method for callbacks, otherwise null.
  COMPRESSED_POINTER_FIELD(FunctionPtr, callback_target)

  // For callbacks, value to return if Dart target throws an exception.
  COMPRESSED_POINTER_FIELD(InstancePtr, callback_exceptional_return)

  VISIT_TO(CompressedObjectPtr, callback_exceptional_return)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  // Callback id for callbacks.
  //
  // The callbacks ids are used so that native callbacks can lookup their own
  // code objects, since native code doesn't pass code objects into function
  // calls. The callback id is also used to for verifying that callbacks are
  // called on the correct isolate. See DLRT_VerifyCallbackIsolate for details.
  //
  // Will be 0 for non-callbacks. Check 'callback_target_' to determine if this
  // is a callback or not.
  uint32_t callback_id_;
};

class UntaggedField : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Field);

  VISIT_FROM(CompressedObjectPtr, name)
  COMPRESSED_POINTER_FIELD(StringPtr, name)
  // Class or patch class or mixin class where this field is defined or original
  // field.
  COMPRESSED_POINTER_FIELD(ObjectPtr, owner)
  COMPRESSED_POINTER_FIELD(AbstractTypePtr, type)
  // Static initializer function.
  COMPRESSED_POINTER_FIELD(FunctionPtr, initializer_function)
  // - for instance fields: offset in words to the value in the class instance.
  // - for static fields: index into field_table.
  COMPRESSED_POINTER_FIELD(SmiPtr, host_offset_or_field_id)
  COMPRESSED_POINTER_FIELD(SmiPtr, guarded_list_length)
  COMPRESSED_POINTER_FIELD(ArrayPtr, dependent_code)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) {
    switch (kind) {
      case Snapshot::kFull:
      case Snapshot::kFullCore:
      case Snapshot::kFullJIT:
      case Snapshot::kFullAOT:
        return reinterpret_cast<CompressedObjectPtr*>(&initializer_function_);
      case Snapshot::kMessage:
      case Snapshot::kNone:
      case Snapshot::kInvalid:
        break;
    }
    UNREACHABLE();
    return NULL;
  }
#if defined(DART_PRECOMPILED_RUNTIME)
  VISIT_TO(CompressedObjectPtr, dependent_code);
#else
  // For type test in implicit setter.
  COMPRESSED_POINTER_FIELD(SubtypeTestCachePtr, type_test_cache);
  VISIT_TO(CompressedObjectPtr, type_test_cache);
#endif
  TokenPosition token_pos_;
  TokenPosition end_token_pos_;
  ClassIdTagType guarded_cid_;
  ClassIdTagType is_nullable_;  // kNullCid if field can contain null value and
                                // kInvalidCid otherwise.

#if !defined(DART_PRECOMPILED_RUNTIME)
  uint32_t kernel_offset_;
#endif  // !defined(DART_PRECOMPILED_RUNTIME)

  // Offset to the guarded length field inside an instance of class matching
  // guarded_cid_. Stored corrected by -kHeapObjectTag to simplify code
  // generated on platforms with weak addressing modes (ARM).
  int8_t guarded_list_length_in_object_offset_;

  // Runtime tracking state of exactness of type annotation of this field.
  // See StaticTypeExactnessState for the meaning and possible values in this
  // field.
  int8_t static_type_exactness_state_;

  uint16_t kind_bits_;  // static, final, const, has initializer....

#if !defined(DART_PRECOMPILED_RUNTIME)
  // for instance fields, the offset in words in the target architecture
  int32_t target_offset_;
#endif  // !defined(DART_PRECOMPILED_RUNTIME)

  friend class CidRewriteVisitor;
  friend class GuardFieldClassInstr;     // For sizeof(guarded_cid_/...)
  friend class LoadFieldInstr;           // For sizeof(guarded_cid_/...)
  friend class StoreInstanceFieldInstr;  // For sizeof(guarded_cid_/...)
};

class alignas(8) UntaggedScript : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Script);

  VISIT_FROM(CompressedObjectPtr, url)
  COMPRESSED_POINTER_FIELD(StringPtr, url)
  COMPRESSED_POINTER_FIELD(StringPtr, resolved_url)
  COMPRESSED_POINTER_FIELD(ArrayPtr, compile_time_constants)
  COMPRESSED_POINTER_FIELD(TypedDataPtr, line_starts)
#if !defined(PRODUCT) && !defined(DART_PRECOMPILED_RUNTIME)
  COMPRESSED_POINTER_FIELD(ExternalTypedDataPtr, constant_coverage)
#endif  // !defined(PRODUCT) && !defined(DART_PRECOMPILED_RUNTIME)
  COMPRESSED_POINTER_FIELD(ArrayPtr, debug_positions)
  COMPRESSED_POINTER_FIELD(KernelProgramInfoPtr, kernel_program_info)
  COMPRESSED_POINTER_FIELD(StringPtr, source)
  VISIT_TO(CompressedObjectPtr, source)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) {
    switch (kind) {
      case Snapshot::kFullAOT:
#if defined(PRODUCT)
        return reinterpret_cast<CompressedObjectPtr*>(&url_);
#else
        return reinterpret_cast<CompressedObjectPtr*>(&resolved_url_);
#endif
      case Snapshot::kFull:
      case Snapshot::kFullCore:
      case Snapshot::kFullJIT:
        return reinterpret_cast<CompressedObjectPtr*>(&kernel_program_info_);
      case Snapshot::kMessage:
      case Snapshot::kNone:
      case Snapshot::kInvalid:
        break;
    }
    UNREACHABLE();
    return NULL;
  }

  int32_t line_offset_;
  int32_t col_offset_;
#if !defined(DART_PRECOMPILED_RUNTIME)
  int32_t flags_and_max_position_;

 public:
  using LazyLookupSourceAndLineStartsBit =
      BitField<decltype(flags_and_max_position_), bool, 0, 1>;
  using HasCachedMaxPositionBit =
      BitField<decltype(flags_and_max_position_),
               bool,
               LazyLookupSourceAndLineStartsBit::kNextBit,
               1>;
  using CachedMaxPositionBitField = BitField<decltype(flags_and_max_position_),
                                             intptr_t,
                                             HasCachedMaxPositionBit::kNextBit>;

 private:
#endif

#if !defined(PRODUCT) && !defined(DART_PRECOMPILED_RUNTIME)
  int64_t load_timestamp_;
  int32_t kernel_script_index_;
#else
  int32_t kernel_script_index_;
  int64_t load_timestamp_;
#endif
};

class UntaggedLibrary : public UntaggedObject {
  enum LibraryState {
    kAllocated,       // Initial state.
    kLoadRequested,   // Compiler or script requested load of library.
    kLoadInProgress,  // Library is in the process of being loaded.
    kLoaded,          // Library is loaded.
  };

  enum LibraryFlags {
    kDartSchemeBit = 0,
    kDebuggableBit,        // True if debugger can stop in library.
    kInFullSnapshotBit,    // True if library is in a full snapshot.
    kNnbdBit,              // True if library is non nullable by default.
    kNnbdCompiledModePos,  // Encodes nnbd compiled mode of constants in lib.
    kNnbdCompiledModeSize = 2,
    kNumFlagBits = kNnbdCompiledModePos + kNnbdCompiledModeSize,
  };
  COMPILE_ASSERT(kNumFlagBits <= (sizeof(uint8_t) * kBitsPerByte));
  class DartSchemeBit : public BitField<uint8_t, bool, kDartSchemeBit, 1> {};
  class DebuggableBit : public BitField<uint8_t, bool, kDebuggableBit, 1> {};
  class InFullSnapshotBit
      : public BitField<uint8_t, bool, kInFullSnapshotBit, 1> {};
  class NnbdBit : public BitField<uint8_t, bool, kNnbdBit, 1> {};
  class NnbdCompiledModeBits : public BitField<uint8_t,
                                               uint8_t,
                                               kNnbdCompiledModePos,
                                               kNnbdCompiledModeSize> {};

  RAW_HEAP_OBJECT_IMPLEMENTATION(Library);

  VISIT_FROM(CompressedObjectPtr, name)
  COMPRESSED_POINTER_FIELD(StringPtr, name)
  COMPRESSED_POINTER_FIELD(StringPtr, url)
  COMPRESSED_POINTER_FIELD(StringPtr, private_key)
  // Top-level names in this library.
  COMPRESSED_POINTER_FIELD(ArrayPtr, dictionary)
  // Metadata on classes, methods etc.
  COMPRESSED_POINTER_FIELD(ArrayPtr, metadata)
  // Class containing top-level elements.
  COMPRESSED_POINTER_FIELD(ClassPtr, toplevel_class)
  COMPRESSED_POINTER_FIELD(GrowableObjectArrayPtr, used_scripts)
  COMPRESSED_POINTER_FIELD(LoadingUnitPtr, loading_unit)
  // List of Namespaces imported without prefix.
  COMPRESSED_POINTER_FIELD(ArrayPtr, imports)
  // List of re-exported Namespaces.
  COMPRESSED_POINTER_FIELD(ArrayPtr, exports)
  COMPRESSED_POINTER_FIELD(ArrayPtr, dependencies)
  COMPRESSED_POINTER_FIELD(ExternalTypedDataPtr, kernel_data)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) {
    switch (kind) {
      case Snapshot::kFullAOT:
        return reinterpret_cast<CompressedObjectPtr*>(&exports_);
      case Snapshot::kFull:
      case Snapshot::kFullCore:
      case Snapshot::kFullJIT:
        return reinterpret_cast<CompressedObjectPtr*>(&kernel_data_);
      case Snapshot::kMessage:
      case Snapshot::kNone:
      case Snapshot::kInvalid:
        break;
    }
    UNREACHABLE();
    return NULL;
  }
  // Cache of resolved names in library scope.
  COMPRESSED_POINTER_FIELD(ArrayPtr, resolved_names);
  // Cache of exported names by library.
  COMPRESSED_POINTER_FIELD(ArrayPtr, exported_names);
  // Array of scripts loaded in this library.
  COMPRESSED_POINTER_FIELD(ArrayPtr, loaded_scripts);
  VISIT_TO(CompressedObjectPtr, loaded_scripts);

  Dart_NativeEntryResolver native_entry_resolver_;  // Resolves natives.
  Dart_NativeEntrySymbol native_entry_symbol_resolver_;
  classid_t index_;       // Library id number.
  uint16_t num_imports_;  // Number of entries in imports_.
  int8_t load_state_;     // Of type LibraryState.
  uint8_t flags_;         // BitField for LibraryFlags.

#if !defined(DART_PRECOMPILED_RUNTIME)
  uint32_t kernel_offset_;
#endif  // !defined(DART_PRECOMPILED_RUNTIME)

  friend class Class;
  friend class Isolate;
};

class UntaggedNamespace : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Namespace);

  VISIT_FROM(CompressedObjectPtr, target)
  // library with name dictionary.
  COMPRESSED_POINTER_FIELD(LibraryPtr, target)
  // list of names that are exported.
  COMPRESSED_POINTER_FIELD(ArrayPtr, show_names)
  // list of names that are hidden.
  COMPRESSED_POINTER_FIELD(ArrayPtr, hide_names)
  COMPRESSED_POINTER_FIELD(LibraryPtr, owner)
  VISIT_TO(CompressedObjectPtr, owner)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) {
    switch (kind) {
      case Snapshot::kFullAOT:
        return reinterpret_cast<CompressedObjectPtr*>(&target_);
      case Snapshot::kFull:
      case Snapshot::kFullCore:
      case Snapshot::kFullJIT:
        return reinterpret_cast<CompressedObjectPtr*>(&owner_);
      case Snapshot::kMessage:
      case Snapshot::kNone:
      case Snapshot::kInvalid:
        break;
    }
    UNREACHABLE();
    return NULL;
  }
};

class UntaggedKernelProgramInfo : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(KernelProgramInfo);

  VISIT_FROM(CompressedObjectPtr, string_offsets)
  COMPRESSED_POINTER_FIELD(TypedDataPtr, string_offsets)
  COMPRESSED_POINTER_FIELD(ExternalTypedDataPtr, string_data)
  COMPRESSED_POINTER_FIELD(TypedDataPtr, canonical_names)
  COMPRESSED_POINTER_FIELD(ExternalTypedDataPtr, metadata_payloads)
  COMPRESSED_POINTER_FIELD(ExternalTypedDataPtr, metadata_mappings)
  COMPRESSED_POINTER_FIELD(ArrayPtr, scripts)
  COMPRESSED_POINTER_FIELD(ArrayPtr, constants)
  COMPRESSED_POINTER_FIELD(GrowableObjectArrayPtr, potential_natives)
  COMPRESSED_POINTER_FIELD(GrowableObjectArrayPtr, potential_pragma_functions)
  COMPRESSED_POINTER_FIELD(ExternalTypedDataPtr, constants_table)
  COMPRESSED_POINTER_FIELD(ArrayPtr, libraries_cache)
  COMPRESSED_POINTER_FIELD(ArrayPtr, classes_cache)
  COMPRESSED_POINTER_FIELD(ObjectPtr, retained_kernel_blob)
  VISIT_TO(CompressedObjectPtr, retained_kernel_blob)

  uint32_t kernel_binary_version_;

  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) {
    return reinterpret_cast<CompressedObjectPtr*>(&constants_table_);
  }
};

class UntaggedWeakSerializationReference : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(WeakSerializationReference);

  VISIT_FROM(CompressedObjectPtr, target)
  COMPRESSED_POINTER_FIELD(ObjectPtr, target)
  COMPRESSED_POINTER_FIELD(ObjectPtr, replacement)
  VISIT_TO(CompressedObjectPtr, replacement)
};

class UntaggedCode : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Code);

  // When in the precompiled runtime, there is no disabling of Code objects
  // and thus no active_instructions_ field. Thus, the entry point caches are
  // only set once during deserialization. If not using bare instructions,
  // the caches should match the entry points for instructions_.
  //
  // Otherwise, they should contain entry points for active_instructions_.

  uword entry_point_;  // Accessed from generated code.

  // In AOT this entry-point supports switchable calls. It checks the type of
  // the receiver on entry to the function and calls a stub to patch up the
  // caller if they mismatch.
  uword monomorphic_entry_point_;  // Accessed from generated code (AOT only).

  // Entry-point used from call-sites with some additional static information.
  // The exact behavior of this entry-point depends on the kind of function:
  //
  // kRegularFunction/kSetter/kGetter:
  //
  //   Call-site is assumed to know that the (type) arguments are invariantly
  //   type-correct against the actual runtime-type of the receiver. For
  //   instance, this entry-point is used for invocations against "this" and
  //   invocations from IC stubs that test the class type arguments.
  //
  // kClosureFunction:
  //
  //   Call-site is assumed to pass the correct number of positional and type
  //   arguments (except in the case of partial instantiation, when the type
  //   arguments are omitted). All (type) arguments are assumed to match the
  //   corresponding (type) parameter types (bounds).
  //
  // kImplicitClosureFunction:
  //
  //   Similar to kClosureFunction, except that the types (bounds) of the (type)
  //   arguments are expected to match the *runtime signature* of the closure,
  //   which (unlike with kClosureFunction) may have more general (type)
  //   parameter types (bounds) than the declared type of the forwarded method.
  //
  // In many cases a distinct static entry-point will not be created for a
  // function if it would not be able to skip a lot of work (e.g., no argument
  // type checks are necessary or this Code belongs to a stub). In this case
  // 'unchecked_entry_point_' will refer to the same position as 'entry_point_'.
  //
  uword unchecked_entry_point_;              // Accessed from generated code.
  uword monomorphic_unchecked_entry_point_;  // Accessed from generated code.

  VISIT_FROM(ObjectPtr, object_pool)
  POINTER_FIELD(ObjectPoolPtr, object_pool)  // Accessed from generated code.
  POINTER_FIELD(InstructionsPtr,
                instructions)  // Accessed from generated code.
  // If owner_ is Function::null() the owner is a regular stub.
  // If owner_ is a Class the owner is the allocation stub for that class.
  // Else, owner_ is a regular Dart Function.
  POINTER_FIELD(ObjectPtr, owner)  // Function, Null, or a Class.
  POINTER_FIELD(ExceptionHandlersPtr, exception_handlers)
  POINTER_FIELD(PcDescriptorsPtr, pc_descriptors)
  // If FLAG_precompiled_mode, then this field contains
  //   TypedDataPtr catch_entry_moves_maps
  // Otherwise, it is
  //   SmiPtr num_variables
  POINTER_FIELD(ObjectPtr, catch_entry)
  POINTER_FIELD(CompressedStackMapsPtr, compressed_stackmaps)
  POINTER_FIELD(ArrayPtr, inlined_id_to_function)
  POINTER_FIELD(CodeSourceMapPtr, code_source_map)
  NOT_IN_PRECOMPILED(POINTER_FIELD(InstructionsPtr, active_instructions))
  NOT_IN_PRECOMPILED(POINTER_FIELD(ArrayPtr, deopt_info_array))
  // (code-offset, function, code) triples.
  NOT_IN_PRECOMPILED(POINTER_FIELD(ArrayPtr, static_calls_target_table))
  // If return_address_metadata_ is a Smi, it is the offset to the prologue.
  // Else, return_address_metadata_ is null.
  NOT_IN_PRODUCT(POINTER_FIELD(ObjectPtr, return_address_metadata))
  NOT_IN_PRODUCT(POINTER_FIELD(LocalVarDescriptorsPtr, var_descriptors))
  NOT_IN_PRODUCT(POINTER_FIELD(ArrayPtr, comments))

#if !defined(PRODUCT)
  VISIT_TO(ObjectPtr, comments);
#elif defined(DART_PRECOMPILED_RUNTIME)
  VISIT_TO(ObjectPtr, code_source_map);
#else
  VISIT_TO(ObjectPtr, static_calls_target_table);
#endif

  // Compilation timestamp.
  NOT_IN_PRODUCT(alignas(8) int64_t compile_timestamp_);

  // state_bits_ is a bitfield with three fields:
  // The optimized bit, the alive bit, and a count of the number of pointer
  // offsets.
  // Alive: If true, the embedded object pointers will be visited during GC.
  int32_t state_bits_;
  // Caches the unchecked entry point offset for instructions_, in case we need
  // to reset the active_instructions_ to instructions_.
  NOT_IN_PRECOMPILED(uint32_t unchecked_offset_);
  // Stores the instructions length when not using RawInstructions objects.
  ONLY_IN_PRECOMPILED(uint32_t instructions_length_);

  // Variable length data follows here.
  int32_t* data() { OPEN_ARRAY_START(int32_t, int32_t); }
  const int32_t* data() const { OPEN_ARRAY_START(int32_t, int32_t); }

  static bool ContainsPC(const ObjectPtr raw_obj, uword pc);

  friend class Function;
  template <bool>
  friend class MarkingVisitorBase;
  friend class StackFrame;
  friend class Profiler;
  friend class FunctionDeserializationCluster;
  friend class UnitSerializationRoots;
  friend class UnitDeserializationRoots;
  friend class CallSiteResetter;
};

class UntaggedObjectPool : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(ObjectPool);

  intptr_t length_;

  struct Entry {
    union {
      ObjectPtr raw_obj_;
      uword raw_value_;
    };
  };
  Entry* data() { OPEN_ARRAY_START(Entry, Entry); }
  Entry const* data() const { OPEN_ARRAY_START(Entry, Entry); }

  // The entry bits are located after the last entry. They are encoded versions
  // of `ObjectPool::TypeBits() | ObjectPool::PatchabililtyBit()`.
  uint8_t* entry_bits() { return reinterpret_cast<uint8_t*>(&data()[length_]); }
  uint8_t const* entry_bits() const {
    return reinterpret_cast<uint8_t const*>(&data()[length_]);
  }

  friend class Object;
  friend class CodeSerializationCluster;
  friend class UnitSerializationRoots;
  friend class UnitDeserializationRoots;
};

class UntaggedInstructions : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Instructions);
  VISIT_NOTHING();

  // Instructions size in bytes and flags.
  // Currently, only flag indicates 1 or 2 entry points.
  uint32_t size_and_flags_;

  // Variable length data follows here.
  uint8_t* data() { OPEN_ARRAY_START(uint8_t, uint8_t); }

  // Private helper function used while visiting stack frames. The
  // code which iterates over dart frames is also called during GC and
  // is not allowed to create handles.
  static bool ContainsPC(const InstructionsPtr raw_instr, uword pc);

  friend class UntaggedCode;
  friend class UntaggedFunction;
  friend class Code;
  friend class StackFrame;
  template <bool>
  friend class MarkingVisitorBase;
  friend class Function;
  friend class ImageReader;
  friend class ImageWriter;
  friend class AssemblyImageWriter;
  friend class BlobImageWriter;
};

// Used to carry extra information to the VM without changing the embedder
// interface, to provide memory accounting for the bare instruction payloads
// we serialize, since they are no longer part of RawInstructions objects,
// and to avoid special casing bare instructions payload Images in the GC.
class UntaggedInstructionsSection : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(InstructionsSection);
  VISIT_NOTHING();

  // Instructions section payload length in bytes.
  uword payload_length_;
  // The offset of the corresponding BSS section from this text section.
  word bss_offset_;
  // The relocated address of this text section in the shared object. Properly
  // filled for ELF snapshots, always 0 in assembly snapshots. (For the latter,
  // we instead get the value during BSS initialization and store it there.)
  uword instructions_relocated_address_;
  // The offset of the GNU build ID note section from this text section.
  word build_id_offset_;

  // Variable length data follows here.
  uint8_t* data() { OPEN_ARRAY_START(uint8_t, uint8_t); }

  friend class Image;
};

class UntaggedPcDescriptors : public UntaggedObject {
 public:
// The macro argument V is passed two arguments, the raw name of the enum value
// and the initialization expression used within the enum definition.  The uses
// of enum values inside the initialization expression are hardcoded currently,
// so the second argument is useless outside the enum definition and should be
// dropped by other users of this macro.
#define FOR_EACH_RAW_PC_DESCRIPTOR(V)                                          \
  /* Deoptimization continuation point. */                                     \
  V(Deopt, 1)                                                                  \
  /* IC call. */                                                               \
  V(IcCall, kDeopt << 1)                                                       \
  /* Call to a known target via stub. */                                       \
  V(UnoptStaticCall, kIcCall << 1)                                             \
  /* Runtime call. */                                                          \
  V(RuntimeCall, kUnoptStaticCall << 1)                                        \
  /* OSR entry point in unopt. code. */                                        \
  V(OsrEntry, kRuntimeCall << 1)                                               \
  /* Call rewind target address. */                                            \
  V(Rewind, kOsrEntry << 1)                                                    \
  /* Target-word-size relocation. */                                           \
  V(BSSRelocation, kRewind << 1)                                               \
  V(Other, kBSSRelocation << 1)                                                \
  V(AnyKind, -1)

  enum Kind {
#define ENUM_DEF(name, init) k##name = init,
    FOR_EACH_RAW_PC_DESCRIPTOR(ENUM_DEF)
#undef ENUM_DEF
        kLastKind = kOther,
  };

  static const char* KindToCString(Kind k);
  static bool ParseKind(const char* cstr, Kind* out);

  // Used to represent the absense of a yield index in PcDescriptors.
  static constexpr intptr_t kInvalidYieldIndex = -1;

  class KindAndMetadata {
   public:
    // Most of the time try_index will be small and merged field will fit into
    // one byte.
    static uint32_t Encode(intptr_t kind,
                           intptr_t try_index,
                           intptr_t yield_index) {
      return KindShiftBits::encode(Utils::ShiftForPowerOfTwo(kind)) |
             TryIndexBits::encode(try_index + 1) |
             YieldIndexBits::encode(yield_index + 1);
    }

    static intptr_t DecodeKind(uint32_t kind_and_metadata) {
      return 1 << KindShiftBits::decode(kind_and_metadata);
    }

    static intptr_t DecodeTryIndex(uint32_t kind_and_metadata) {
      return TryIndexBits::decode(kind_and_metadata) - 1;
    }

    static intptr_t DecodeYieldIndex(uint32_t kind_and_metadata) {
      return YieldIndexBits::decode(kind_and_metadata) - 1;
    }

   private:
    static const intptr_t kKindShiftSize = 3;
    static const intptr_t kTryIndexSize = 10;
    static const intptr_t kYieldIndexSize = 32 - kKindShiftSize - kTryIndexSize;

    class KindShiftBits
        : public BitField<uint32_t, intptr_t, 0, kKindShiftSize> {};
    class TryIndexBits : public BitField<uint32_t,
                                         intptr_t,
                                         KindShiftBits::kNextBit,
                                         kTryIndexSize> {};
    class YieldIndexBits : public BitField<uint32_t,
                                           intptr_t,
                                           TryIndexBits::kNextBit,
                                           kYieldIndexSize> {};
  };

 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(PcDescriptors);
  VISIT_NOTHING();

  // Number of descriptors.  This only needs to be an int32_t, but we make it a
  // uword so that the variable length data is 64 bit aligned on 64 bit
  // platforms.
  uword length_;

  // Variable length data follows here.
  uint8_t* data() { OPEN_ARRAY_START(uint8_t, intptr_t); }
  const uint8_t* data() const { OPEN_ARRAY_START(uint8_t, intptr_t); }

  friend class Object;
  friend class ImageWriter;
};

// CodeSourceMap encodes a mapping from code PC ranges to source token
// positions and the stack of inlined functions.
class UntaggedCodeSourceMap : public UntaggedObject {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(CodeSourceMap);
  VISIT_NOTHING();

  // Length in bytes.  This only needs to be an int32_t, but we make it a uword
  // so that the variable length data is 64 bit aligned on 64 bit platforms.
  uword length_;

  // Variable length data follows here.
  uint8_t* data() { OPEN_ARRAY_START(uint8_t, intptr_t); }
  const uint8_t* data() const { OPEN_ARRAY_START(uint8_t, intptr_t); }

  friend class Object;
  friend class ImageWriter;
};

// RawCompressedStackMaps is a compressed representation of the stack maps
// for certain PC offsets into a set of instructions, where a stack map is a bit
// map that marks each live object index starting from the base of the frame.
class UntaggedCompressedStackMaps : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(CompressedStackMaps);
  VISIT_NOTHING();

  // The most significant bits are the length of the encoded payload, in bytes.
  // The low bits determine the expected payload contents, as described below.
  uint32_t flags_and_size_;

  // Variable length data follows here. The contents of the payload depend on
  // the type of CompressedStackMaps (CSM) being represented. There are three
  // major types of CSM:
  //
  // 1) GlobalTableBit = false, UsesTableBit = false: CSMs that include all
  //    information about the stack maps. The payload for these contain tightly
  //    packed entries with the following information:
  //
  //   * A header containing the following three pieces of information:
  //     * An unsigned integer representing the PC offset as a delta from the
  //       PC offset of the previous entry (from 0 for the first entry).
  //     * An unsigned integer representing the number of bits used for
  //       spill slot entries.
  //     * An unsigned integer representing the number of bits used for other
  //       entries.
  //   * The body containing the bits for the stack map. The length of the body
  //     in bits is the sum of the spill slot and non-spill slot bit counts.
  //
  // 2) GlobalTableBit = false, UsesTableBit = true: CSMs where the majority of
  //    the stack map information has been offloaded and canonicalized into a
  //    global table. The payload contains tightly packed entries with the
  //    following information:
  //
  //   * A header containing just an unsigned integer representing the PC offset
  //     delta as described above.
  //   * The body is just an unsigned integer containing the offset into the
  //     payload for the global table.
  //
  // 3) GlobalTableBit = true, UsesTableBit = false: A CSM implementing the
  //    global table. Here, the payload contains tightly packed entries with
  //    the following information:
  //
  //   * A header containing the following two pieces of information:
  //     * An unsigned integer representing the number of bits used for
  //       spill slot entries.
  //     * An unsigned integer representing the number of bits used for other
  //       entries.
  //   * The body containing the bits for the stack map. The length of the body
  //     in bits is the sum of the spill slot and non-spill slot bit counts.
  //
  // In all types of CSM, each unsigned integer is LEB128 encoded, as generally
  // they tend to fit in a single byte or two. Thus, entry headers are not a
  // fixed length, and currently there is no random access of entries.  In
  // addition, PC offsets are currently encoded as deltas, which also inhibits
  // random access without accessing previous entries. That means to find an
  // entry for a given PC offset, a linear search must be done where the payload
  // is decoded up to the entry whose PC offset is >= the given PC.

  uint8_t* data() { OPEN_ARRAY_START(uint8_t, uint8_t); }
  const uint8_t* data() const { OPEN_ARRAY_START(uint8_t, uint8_t); }

  class GlobalTableBit : public BitField<uint32_t, bool, 0, 1> {};
  class UsesTableBit
      : public BitField<uint32_t, bool, GlobalTableBit::kNextBit, 1> {};
  class SizeField : public BitField<uint32_t,
                                    uint32_t,
                                    UsesTableBit::kNextBit,
                                    sizeof(flags_and_size_) * kBitsPerByte -
                                        UsesTableBit::kNextBit> {};

  friend class Object;
  friend class ImageWriter;
  friend class StackMapEntry;
};

class UntaggedLocalVarDescriptors : public UntaggedObject {
 public:
  enum VarInfoKind {
    kStackVar = 1,
    kContextVar,
    kContextLevel,
    kSavedCurrentContext,
  };

  enum {
    kKindPos = 0,
    kKindSize = 8,
    kIndexPos = kKindPos + kKindSize,
    // Since there are 24 bits for the stack slot index, Functions can have
    // only ~16.7 million stack slots.
    kPayloadSize = sizeof(int32_t) * kBitsPerByte,
    kIndexSize = kPayloadSize - kIndexPos,
    kIndexBias = 1 << (kIndexSize - 1),
    kMaxIndex = (1 << (kIndexSize - 1)) - 1,
  };

  class IndexBits : public BitField<int32_t, int32_t, kIndexPos, kIndexSize> {};
  class KindBits : public BitField<int32_t, int8_t, kKindPos, kKindSize> {};

  struct VarInfo {
    int32_t index_kind = 0;  // Bitfield for slot index on stack or in context,
                             // and Entry kind of type VarInfoKind.
    TokenPosition declaration_pos =
        TokenPosition::kNoSource;  // Token position of declaration.
    TokenPosition begin_pos =
        TokenPosition::kNoSource;  // Token position of scope start.
    TokenPosition end_pos =
        TokenPosition::kNoSource;   // Token position of scope end.
    int16_t scope_id;               // Scope to which the variable belongs.

    VarInfoKind kind() const {
      return static_cast<VarInfoKind>(KindBits::decode(index_kind));
    }
    void set_kind(VarInfoKind kind) {
      index_kind = KindBits::update(kind, index_kind);
    }
    int32_t index() const { return IndexBits::decode(index_kind) - kIndexBias; }
    void set_index(int32_t index) {
      index_kind = IndexBits::update(index + kIndexBias, index_kind);
    }
  };

 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(LocalVarDescriptors);
  // Number of descriptors. This only needs to be an int32_t, but we make it a
  // uword so that the variable length data is 64 bit aligned on 64 bit
  // platforms.
  uword num_entries_;

  CompressedObjectPtr* from() {
    return reinterpret_cast<CompressedObjectPtr*>(&names()[0]);
  }
  CompressedStringPtr* names() {
    // Array of [num_entries_] variable names.
    OPEN_ARRAY_START(CompressedStringPtr, CompressedStringPtr);
  }
  CompressedStringPtr* nameAddrAt(intptr_t i) { return &(names()[i]); }
  StringPtr name(intptr_t i) {
    return LoadCompressedPointer<StringPtr>(nameAddrAt(i));
  }
  void set_name(intptr_t i, StringPtr value) {
    StoreCompressedPointer(nameAddrAt(i), value);
  }
  VISIT_TO_LENGTH(CompressedObjectPtr, nameAddrAt(length - 1));

  // Variable info with [num_entries_] entries.
  VarInfo* data() {
    return reinterpret_cast<VarInfo*>(nameAddrAt(num_entries_));
  }

  friend class Object;
};

class UntaggedExceptionHandlers : public UntaggedObject {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(ExceptionHandlers);

  // Number of exception handler entries.
  int32_t num_entries_;

  // Array with [num_entries_] entries. Each entry is an array of all handled
  // exception types.
  VISIT_FROM(CompressedObjectPtr, handled_types_data)
  COMPRESSED_POINTER_FIELD(ArrayPtr, handled_types_data)
  VISIT_TO_LENGTH(CompressedObjectPtr, &handled_types_data_)

  // Exception handler info of length [num_entries_].
  const ExceptionHandlerInfo* data() const {
    OPEN_ARRAY_START(ExceptionHandlerInfo, intptr_t);
  }
  ExceptionHandlerInfo* data() {
    OPEN_ARRAY_START(ExceptionHandlerInfo, intptr_t);
  }

  friend class Object;
};

class UntaggedContext : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Context);

  int32_t num_variables_;

  VISIT_FROM(ObjectPtr, parent)
  POINTER_FIELD(ContextPtr, parent)
  // Variable length data follows here.
  VARIABLE_POINTER_FIELDS(ObjectPtr, element, data)
  VISIT_TO_LENGTH(ObjectPtr, &data()[length - 1]);

  friend class Object;
  friend class SnapshotReader;
};

class UntaggedContextScope : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(ContextScope);

  // TODO(iposva): Switch to conventional enum offset based structure to avoid
  // alignment mishaps.
  struct VariableDesc {
    CompressedSmiPtr declaration_token_pos;
    CompressedSmiPtr token_pos;
    CompressedStringPtr name;
    CompressedSmiPtr flags;
    static constexpr intptr_t kIsFinal = 0x1;
    static constexpr intptr_t kIsConst = 0x2;
    static constexpr intptr_t kIsLate = 0x4;
    CompressedSmiPtr late_init_offset;
    union {
      CompressedAbstractTypePtr type;
      CompressedInstancePtr value;  // iff is_const is true
    };
    CompressedSmiPtr context_index;
    CompressedSmiPtr context_level;
  };

  int32_t num_variables_;
  bool is_implicit_;  // true, if this context scope is for an implicit closure.

  CompressedObjectPtr* from() {
    VariableDesc* begin = const_cast<VariableDesc*>(VariableDescAddr(0));
    return reinterpret_cast<CompressedObjectPtr*>(begin);
  }
  // Variable length data follows here.
  CompressedObjectPtr const* data() const {
    OPEN_ARRAY_START(CompressedObjectPtr, CompressedObjectPtr);
  }
  const VariableDesc* VariableDescAddr(intptr_t index) const {
    ASSERT((index >= 0) && (index < num_variables_ + 1));
    // data() points to the first component of the first descriptor.
    return &(reinterpret_cast<const VariableDesc*>(data())[index]);
  }

#define DEFINE_ACCESSOR(type, name)                                            \
  type name##_at(intptr_t index) {                                             \
    return LoadCompressedPointer<type>(&VariableDescAddr(index)->name);        \
  }                                                                            \
  void set_##name##_at(intptr_t index, type value) {                           \
    StoreCompressedPointer(&VariableDescAddr(index)->name, value);             \
  }
  DEFINE_ACCESSOR(SmiPtr, declaration_token_pos)
  DEFINE_ACCESSOR(SmiPtr, token_pos)
  DEFINE_ACCESSOR(StringPtr, name)
  DEFINE_ACCESSOR(SmiPtr, flags)
  DEFINE_ACCESSOR(SmiPtr, late_init_offset)
  DEFINE_ACCESSOR(AbstractTypePtr, type)
  DEFINE_ACCESSOR(InstancePtr, value)
  DEFINE_ACCESSOR(SmiPtr, context_index)
  DEFINE_ACCESSOR(SmiPtr, context_level)
#undef DEFINE_ACCESSOR

  CompressedObjectPtr* to(intptr_t num_vars) {
    uword end = reinterpret_cast<uword>(VariableDescAddr(num_vars));
    // 'end' is the address just beyond the last descriptor, so step back.
    return reinterpret_cast<CompressedObjectPtr*>(end -
                                                  sizeof(CompressedObjectPtr));
  }
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind, intptr_t num_vars) {
    return to(num_vars);
  }

  friend class Object;
  friend class UntaggedClosureData;
  friend class SnapshotReader;
};

class UntaggedSingleTargetCache : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(SingleTargetCache);
  VISIT_FROM(ObjectPtr, target)
  POINTER_FIELD(CodePtr, target)
  VISIT_TO(ObjectPtr, target)
  uword entry_point_;
  ClassIdTagType lower_limit_;
  ClassIdTagType upper_limit_;
};

class UntaggedMonomorphicSmiableCall : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(MonomorphicSmiableCall);
  VISIT_FROM(ObjectPtr, target)
  POINTER_FIELD(CodePtr,
                target);  // Entrypoint PC in bare mode, Code in non-bare mode.
  VISIT_TO(ObjectPtr, target)
  uword expected_cid_;
  uword entrypoint_;
  ObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }
};

// Abstract base class for RawICData/RawMegamorphicCache
class UntaggedCallSiteData : public UntaggedObject {
 protected:
  POINTER_FIELD(StringPtr, target_name);  // Name of target function.
  // arg_descriptor in RawICData and in RawMegamorphicCache should be
  // in the same position so that NoSuchMethod can access it.
  POINTER_FIELD(ArrayPtr, args_descriptor);  // Arguments descriptor.
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(CallSiteData)
};

class UntaggedUnlinkedCall : public UntaggedCallSiteData {
  RAW_HEAP_OBJECT_IMPLEMENTATION(UnlinkedCall);
  VISIT_FROM(ObjectPtr, target_name)
  VISIT_TO(ObjectPtr, args_descriptor)
  ObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  bool can_patch_to_monomorphic_;
};

class UntaggedICData : public UntaggedCallSiteData {
  RAW_HEAP_OBJECT_IMPLEMENTATION(ICData);
  VISIT_FROM(ObjectPtr, target_name)
  POINTER_FIELD(ArrayPtr, entries)  // Contains class-ids, target and count.
  // Static type of the receiver, if instance call and available.
  NOT_IN_PRECOMPILED(POINTER_FIELD(AbstractTypePtr, receivers_static_type))
  POINTER_FIELD(ObjectPtr,
                owner)  // Parent/calling function or original IC of cloned IC.
  VISIT_TO(ObjectPtr, owner)
  ObjectPtr* to_snapshot(Snapshot::Kind kind) {
    switch (kind) {
      case Snapshot::kFullAOT:
        return reinterpret_cast<ObjectPtr*>(&entries_);
      case Snapshot::kFull:
      case Snapshot::kFullCore:
      case Snapshot::kFullJIT:
        return to();
      case Snapshot::kMessage:
      case Snapshot::kNone:
      case Snapshot::kInvalid:
        break;
    }
    UNREACHABLE();
    return NULL;
  }
  NOT_IN_PRECOMPILED(int32_t deopt_id_);
  uint32_t state_bits_;  // Number of arguments tested in IC, deopt reasons.
};

class UntaggedMegamorphicCache : public UntaggedCallSiteData {
  RAW_HEAP_OBJECT_IMPLEMENTATION(MegamorphicCache);

  VISIT_FROM(ObjectPtr, target_name)
  POINTER_FIELD(ArrayPtr, buckets)
  SMI_FIELD(SmiPtr, mask)
  VISIT_TO(ObjectPtr, mask)
  ObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  int32_t filled_entry_count_;
};

class UntaggedSubtypeTestCache : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(SubtypeTestCache);

  VISIT_FROM(ObjectPtr, cache)
  POINTER_FIELD(ArrayPtr, cache)
  VISIT_TO(ObjectPtr, cache)
};

class UntaggedLoadingUnit : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(LoadingUnit);

  VISIT_FROM(CompressedObjectPtr, parent)
  COMPRESSED_POINTER_FIELD(LoadingUnitPtr, parent)
  COMPRESSED_POINTER_FIELD(ArrayPtr, base_objects)
  VISIT_TO(CompressedObjectPtr, base_objects)
  int32_t id_;
  bool load_outstanding_;
  bool loaded_;
};

class UntaggedError : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Error);
};

class UntaggedApiError : public UntaggedError {
  RAW_HEAP_OBJECT_IMPLEMENTATION(ApiError);

  VISIT_FROM(CompressedObjectPtr, message)
  COMPRESSED_POINTER_FIELD(StringPtr, message)
  VISIT_TO(CompressedObjectPtr, message)
};

class UntaggedLanguageError : public UntaggedError {
  RAW_HEAP_OBJECT_IMPLEMENTATION(LanguageError);

  VISIT_FROM(CompressedObjectPtr, previous_error)
  COMPRESSED_POINTER_FIELD(ErrorPtr, previous_error)  // May be null.
  COMPRESSED_POINTER_FIELD(ScriptPtr, script)
  COMPRESSED_POINTER_FIELD(StringPtr, message)
  // Incl. previous error's formatted message.
  COMPRESSED_POINTER_FIELD(StringPtr, formatted_message)
  VISIT_TO(CompressedObjectPtr, formatted_message)
  TokenPosition token_pos_;  // Source position in script_.
  bool report_after_token_;  // Report message at or after the token.
  int8_t kind_;              // Of type Report::Kind.

  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }
};

class UntaggedUnhandledException : public UntaggedError {
  RAW_HEAP_OBJECT_IMPLEMENTATION(UnhandledException);

  VISIT_FROM(CompressedObjectPtr, exception)
  COMPRESSED_POINTER_FIELD(InstancePtr, exception)
  COMPRESSED_POINTER_FIELD(InstancePtr, stacktrace)
  VISIT_TO(CompressedObjectPtr, stacktrace)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }
};

class UntaggedUnwindError : public UntaggedError {
  RAW_HEAP_OBJECT_IMPLEMENTATION(UnwindError);

  VISIT_FROM(CompressedObjectPtr, message)
  COMPRESSED_POINTER_FIELD(StringPtr, message)
  VISIT_TO(CompressedObjectPtr, message)
  bool is_user_initiated_;
};

class UntaggedInstance : public UntaggedObject {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Instance);
};

class UntaggedLibraryPrefix : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(LibraryPrefix);

  VISIT_FROM(CompressedObjectPtr, name)
  // Library prefix name.
  COMPRESSED_POINTER_FIELD(StringPtr, name)
  // Libraries imported with this prefix.
  COMPRESSED_POINTER_FIELD(ArrayPtr, imports)
  // Library which declares this prefix.
  COMPRESSED_POINTER_FIELD(LibraryPtr, importer)
  VISIT_TO(CompressedObjectPtr, importer)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) {
    switch (kind) {
      case Snapshot::kFullAOT:
        return reinterpret_cast<CompressedObjectPtr*>(&imports_);
      case Snapshot::kFull:
      case Snapshot::kFullCore:
      case Snapshot::kFullJIT:
        return reinterpret_cast<CompressedObjectPtr*>(&importer_);
      case Snapshot::kMessage:
      case Snapshot::kNone:
      case Snapshot::kInvalid:
        break;
    }
    UNREACHABLE();
    return NULL;
  }
  uint16_t num_imports_;  // Number of library entries in libraries_.
  bool is_deferred_load_;
};

class UntaggedTypeArguments : public UntaggedInstance {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(TypeArguments);

  VISIT_FROM(ObjectPtr, instantiations)
  // The instantiations_ array remains empty for instantiated type arguments.
  POINTER_FIELD(ArrayPtr,
                instantiations)  // Of 3-tuple: 2 instantiators, result.
  SMI_FIELD(SmiPtr, length)
  SMI_FIELD(SmiPtr, hash)
  SMI_FIELD(SmiPtr, nullability)
  // Variable length data follows here.
  VARIABLE_POINTER_FIELDS(AbstractTypePtr, element, types)
  ObjectPtr* to(intptr_t length) {
    return reinterpret_cast<ObjectPtr*>(&types()[length - 1]);
  }

  friend class Object;
  friend class SnapshotReader;
};

class UntaggedAbstractType : public UntaggedInstance {
 public:
  enum TypeState {
    kAllocated,                // Initial state.
    kBeingFinalized,           // In the process of being finalized.
    kFinalizedInstantiated,    // Instantiated type ready for use.
    kFinalizedUninstantiated,  // Uninstantiated type ready for use.
    // Adjust kTypeStateBitSize if more are added.
  };

 protected:
  static constexpr intptr_t kTypeStateBitSize = 2;

  uword type_test_stub_entry_point_;  // Accessed from generated code.
  COMPRESSED_POINTER_FIELD(
      CodePtr,
      type_test_stub)  // Must be the last field, since subclasses use it
                       // in their VISIT_FROM.

 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(AbstractType);

  friend class ObjectStore;
  friend class StubCode;
};

class UntaggedType : public UntaggedAbstractType {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(Type);

  VISIT_FROM(CompressedObjectPtr, type_test_stub)
  COMPRESSED_POINTER_FIELD(SmiPtr, type_class_id)
  COMPRESSED_POINTER_FIELD(TypeArgumentsPtr, arguments)
  COMPRESSED_POINTER_FIELD(SmiPtr, hash)
  VISIT_TO(CompressedObjectPtr, hash)
  uint8_t type_state_;
  uint8_t nullability_;

  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  friend class CidRewriteVisitor;
  friend class UntaggedTypeArguments;
};

class UntaggedFunctionType : public UntaggedAbstractType {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(FunctionType);

  VISIT_FROM(CompressedObjectPtr, type_test_stub)
  // Array of TypeParameter.
  COMPRESSED_POINTER_FIELD(TypeArgumentsPtr, type_parameters)
  COMPRESSED_POINTER_FIELD(AbstractTypePtr, result_type)
  COMPRESSED_POINTER_FIELD(ArrayPtr, parameter_types)
  COMPRESSED_POINTER_FIELD(ArrayPtr, parameter_names);
  COMPRESSED_POINTER_FIELD(SmiPtr, hash)
  VISIT_TO(CompressedObjectPtr, hash)
  uint32_t packed_fields_;  // Number of parent type args and own parameters.
  uint8_t type_state_;
  uint8_t nullability_;

  static constexpr intptr_t kMaxParentTypeArgumentsBits = 8;
  static constexpr intptr_t kMaxImplicitParametersBits = 1;
  static constexpr intptr_t kMaxHasNamedOptionalParametersBits =
      UntaggedFunction::kMaxHasNamedOptionalParametersBits;
  static constexpr intptr_t kMaxFixedParametersBits =
      UntaggedFunction::kMaxFixedParametersBits;
  static constexpr intptr_t kMaxOptionalParametersBits =
      UntaggedFunction::kMaxOptionalParametersBits;

  // The bit fields are public for use in kernel_to_il.cc.
 public:
  typedef BitField<uint32_t, uint8_t, 0, kMaxParentTypeArgumentsBits>
      PackedNumParentTypeArguments;
  typedef BitField<uint32_t,
                   uint8_t,
                   PackedNumParentTypeArguments::kNextBit,
                   kMaxImplicitParametersBits>
      PackedNumImplicitParameters;
  typedef BitField<uint32_t,
                   bool,
                   PackedNumImplicitParameters::kNextBit,
                   kMaxHasNamedOptionalParametersBits>
      PackedHasNamedOptionalParameters;
  typedef BitField<uint32_t,
                   uint16_t,
                   PackedHasNamedOptionalParameters::kNextBit,
                   kMaxFixedParametersBits>
      PackedNumFixedParameters;
  typedef BitField<uint32_t,
                   uint16_t,
                   PackedNumFixedParameters::kNextBit,
                   kMaxOptionalParametersBits>
      PackedNumOptionalParameters;
  static_assert(PackedNumOptionalParameters::kNextBit <=
                    kBitsPerByte * sizeof(decltype(packed_fields_)),
                "UntaggedFunctionType::packed_fields_ bitfields don't fit.");
  static_assert(PackedNumOptionalParameters::kNextBit <=
                    compiler::target::kSmiBits,
                "In-place mask for number of optional parameters cannot fit in "
                "a Smi on the target architecture");

 private:
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  friend class Function;
};

class UntaggedTypeRef : public UntaggedAbstractType {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(TypeRef);

  VISIT_FROM(CompressedObjectPtr, type_test_stub)
  COMPRESSED_POINTER_FIELD(AbstractTypePtr, type)  // The referenced type.
  VISIT_TO(CompressedObjectPtr, type)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }
};

class UntaggedTypeParameter : public UntaggedAbstractType {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(TypeParameter);

  VISIT_FROM(CompressedObjectPtr, type_test_stub)
  COMPRESSED_POINTER_FIELD(StringPtr, name)
  COMPRESSED_POINTER_FIELD(SmiPtr, hash)
  // ObjectType if no explicit bound specified.
  COMPRESSED_POINTER_FIELD(AbstractTypePtr, bound)
  // The instantiation to bounds of this parameter as calculated by the CFE.
  //
  // TODO(dartbug.com/43901): Once a separate TypeParameters class has been
  // added, move these there and remove them from TypeParameter objects.
  COMPRESSED_POINTER_FIELD(AbstractTypePtr, default_argument)
  VISIT_TO(CompressedObjectPtr, default_argument)
  ClassIdTagType parameterized_class_id_;  // Or kFunctionCid for function tp.
  // TODO(regis): Can we use uint8_t twice below? Or keep uint16_t?
  // Warning: BuildTypeParameterTypeTestStub assumes uint16_t.
  uint16_t base_;  // Number of enclosing function type parameters.
  uint16_t index_;
  uint8_t flags_;
  uint8_t nullability_;

 public:
  using BeingFinalizedBit = BitField<decltype(flags_), bool, 0, 1>;
  using FinalizedBit =
      BitField<decltype(flags_), bool, BeingFinalizedBit::kNextBit, 1>;
  using GenericCovariantImplBit =
      BitField<decltype(flags_), bool, FinalizedBit::kNextBit, 1>;
  static constexpr intptr_t kFlagsBitSize = GenericCovariantImplBit::kNextBit;

 private:
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  friend class CidRewriteVisitor;
};

class UntaggedClosure : public UntaggedInstance {
 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(Closure);

  // No instance fields should be declared before the following fields whose
  // offsets must be identical in Dart and C++.

  // The following fields are also declared in the Dart source of class
  // _Closure.
  VISIT_FROM(ObjectPtr, instantiator_type_arguments)
  POINTER_FIELD(TypeArgumentsPtr, instantiator_type_arguments)
  POINTER_FIELD(TypeArgumentsPtr, function_type_arguments)
  POINTER_FIELD(TypeArgumentsPtr, delayed_type_arguments)
  POINTER_FIELD(FunctionPtr, function)
  POINTER_FIELD(ContextPtr, context)
  POINTER_FIELD(SmiPtr, hash)

  VISIT_TO(ObjectPtr, hash)

  ObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  // Note that instantiator_type_arguments_, function_type_arguments_ and
  // delayed_type_arguments_ are used to instantiate the signature of function_
  // when this closure is involved in a type test. In other words, these fields
  // define the function type of this closure instance.
  //
  // function_type_arguments_ and delayed_type_arguments_ may also be used when
  // invoking the closure. Whereas the source frontend will save a copy of the
  // function's type arguments in the closure's context and only use the
  // function_type_arguments_ field for type tests, the kernel frontend will use
  // the function_type_arguments_ vector here directly.
  //
  // If this closure is generic, it can be invoked with function type arguments
  // that will be processed in the prolog of the closure function_. For example,
  // if the generic closure function_ has a generic parent function, the
  // passed-in function type arguments get concatenated to the function type
  // arguments of the parent that are found in the context_.
  //
  // delayed_type_arguments_ is used to support the partial instantiation
  // feature. When this field is set to any value other than
  // Object::empty_type_arguments(), the types in this vector will be passed as
  // type arguments to the closure when invoked. In this case there may not be
  // any type arguments passed directly (or NSM will be invoked instead).
};

class UntaggedNumber : public UntaggedInstance {
  RAW_OBJECT_IMPLEMENTATION(Number);
};

class UntaggedInteger : public UntaggedNumber {
  RAW_OBJECT_IMPLEMENTATION(Integer);
};

class UntaggedSmi : public UntaggedInteger {
  RAW_OBJECT_IMPLEMENTATION(Smi);
};

class UntaggedMint : public UntaggedInteger {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Mint);
  VISIT_NOTHING();

  ALIGN8 int64_t value_;

  friend class Api;
  friend class Class;
  friend class Integer;
  friend class SnapshotReader;
};
COMPILE_ASSERT(sizeof(UntaggedMint) == 16);

class UntaggedDouble : public UntaggedNumber {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Double);
  VISIT_NOTHING();

  ALIGN8 double value_;

  friend class Api;
  friend class SnapshotReader;
  friend class Class;
};
COMPILE_ASSERT(sizeof(UntaggedDouble) == 16);

class UntaggedString : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(String);

 protected:
  VISIT_FROM(CompressedObjectPtr, length)
  COMPRESSED_SMI_FIELD(SmiPtr, length)
#if !defined(HASH_IN_OBJECT_HEADER)
  COMPRESSED_SMI_FIELD(SmiPtr, hash)
  VISIT_TO(CompressedObjectPtr, hash)
#else
  VISIT_TO(CompressedObjectPtr, length)
#endif

 private:
  friend class Library;
  friend class OneByteStringSerializationCluster;
  friend class TwoByteStringSerializationCluster;
  friend class OneByteStringDeserializationCluster;
  friend class TwoByteStringDeserializationCluster;
  friend class RODataSerializationCluster;
  friend class ImageWriter;
};

class UntaggedOneByteString : public UntaggedString {
  RAW_HEAP_OBJECT_IMPLEMENTATION(OneByteString);
  VISIT_NOTHING();

  // Variable length data follows here.
  uint8_t* data() { OPEN_ARRAY_START(uint8_t, uint8_t); }
  const uint8_t* data() const { OPEN_ARRAY_START(uint8_t, uint8_t); }

  friend class ApiMessageReader;
  friend class RODataSerializationCluster;
  friend class SnapshotReader;
  friend class String;
};

class UntaggedTwoByteString : public UntaggedString {
  RAW_HEAP_OBJECT_IMPLEMENTATION(TwoByteString);
  VISIT_NOTHING();

  // Variable length data follows here.
  uint16_t* data() { OPEN_ARRAY_START(uint16_t, uint16_t); }
  const uint16_t* data() const { OPEN_ARRAY_START(uint16_t, uint16_t); }

  friend class RODataSerializationCluster;
  friend class SnapshotReader;
  friend class String;
};

// Abstract base class for RawTypedData/RawExternalTypedData/RawTypedDataView/
// Pointer.
//
// TypedData extends this with a length field, while Pointer extends this with
// TypeArguments field.
class UntaggedPointerBase : public UntaggedInstance {
 protected:
  // The contents of [data_] depends on what concrete subclass is used:
  //
  //  - RawTypedData: Start of the payload.
  //  - RawExternalTypedData: Start of the C-heap payload.
  //  - RawTypedDataView: The [data_] field of the backing store for the view
  //    plus the [offset_in_bytes_] the view has.
  //  - RawPointer: Pointer into C memory (no length specified).
  //
  // During allocation or snapshot reading the [data_] can be temporarily
  // nullptr (which is the case for views which just got created but haven't
  // gotten the backing store set).
  uint8_t* data_;

 private:
  RAW_HEAP_OBJECT_IMPLEMENTATION(PointerBase);
};

// Abstract base class for RawTypedData/RawExternalTypedData/RawTypedDataView.
class UntaggedTypedDataBase : public UntaggedPointerBase {
 protected:
  // The length of the view in element sizes (obtainable via
  // [TypedDataBase::ElementSizeInBytes]).
  COMPRESSED_SMI_FIELD(SmiPtr, length);

 private:
  friend class UntaggedTypedDataView;
  RAW_HEAP_OBJECT_IMPLEMENTATION(TypedDataBase);
};

class UntaggedTypedData : public UntaggedTypedDataBase {
  RAW_HEAP_OBJECT_IMPLEMENTATION(TypedData);

 public:
  static intptr_t payload_offset() {
    return OFFSET_OF_RETURNED_VALUE(UntaggedTypedData, internal_data);
  }

  // Recompute [data_] pointer to internal data.
  void RecomputeDataField() { data_ = internal_data(); }

 protected:
  VISIT_FROM(CompressedObjectPtr, length)
  VISIT_TO_LENGTH(CompressedObjectPtr, &length_)

  // Variable length data follows here.

  uint8_t* internal_data() { OPEN_ARRAY_START(uint8_t, uint8_t); }
  const uint8_t* internal_data() const { OPEN_ARRAY_START(uint8_t, uint8_t); }

  uint8_t* data() {
    ASSERT(data_ == internal_data());
    return data_;
  }
  const uint8_t* data() const {
    ASSERT(data_ == internal_data());
    return data_;
  }

  friend class Api;
  friend class Instance;
  friend class NativeEntryData;
  friend class Object;
  friend class ObjectPool;
  friend class ObjectPoolDeserializationCluster;
  friend class ObjectPoolSerializationCluster;
  friend class UntaggedObjectPool;
  friend class SnapshotReader;
};

// All _*ArrayView/_ByteDataView classes share the same layout.
class UntaggedTypedDataView : public UntaggedTypedDataBase {
  RAW_HEAP_OBJECT_IMPLEMENTATION(TypedDataView);

 public:
  // Recompute [data_] based on internal/external [typed_data_].
  void RecomputeDataField() {
    const intptr_t offset_in_bytes = RawSmiValue(this->offset_in_bytes());
    uint8_t* payload = typed_data()->untag()->data_;
    data_ = payload + offset_in_bytes;
  }

  // Recompute [data_] based on internal [typed_data_] - needs to be called by
  // GC whenever the backing store moved.
  //
  // NOTICE: This method assumes [this] is the forwarded object and the
  // [typed_data_] pointer points to the new backing store. The backing store's
  // fields don't need to be valid - only it's address.
  void RecomputeDataFieldForInternalTypedData() {
    data_ = DataFieldForInternalTypedData();
  }

  uint8_t* DataFieldForInternalTypedData() const {
    const intptr_t offset_in_bytes = RawSmiValue(this->offset_in_bytes());
    uint8_t* payload =
        reinterpret_cast<uint8_t*>(UntaggedObject::ToAddr(typed_data()) +
                                   UntaggedTypedData::payload_offset());
    return payload + offset_in_bytes;
  }

  void ValidateInnerPointer() {
    if (typed_data()->untag()->GetClassId() == kNullCid) {
      // The view object must have gotten just initialized.
      if (data_ != nullptr || RawSmiValue(offset_in_bytes()) != 0 ||
          RawSmiValue(length()) != 0) {
        FATAL("RawTypedDataView has invalid inner pointer.");
      }
    } else {
      const intptr_t offset_in_bytes = RawSmiValue(this->offset_in_bytes());
      uint8_t* payload = typed_data()->untag()->data_;
      if ((payload + offset_in_bytes) != data_) {
        FATAL("RawTypedDataView has invalid inner pointer.");
      }
    }
  }

 protected:
  VISIT_FROM(CompressedObjectPtr, length)
  COMPRESSED_POINTER_FIELD(TypedDataBasePtr, typed_data)
  COMPRESSED_SMI_FIELD(SmiPtr, offset_in_bytes)
  VISIT_TO(CompressedObjectPtr, offset_in_bytes)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  friend class Api;
  friend class Object;
  friend class ObjectPoolDeserializationCluster;
  friend class ObjectPoolSerializationCluster;
  friend class UntaggedObjectPool;
  friend class GCCompactor;
  template <bool>
  friend class ScavengerVisitorBase;
  friend class SnapshotReader;
};

class UntaggedExternalOneByteString : public UntaggedString {
  RAW_HEAP_OBJECT_IMPLEMENTATION(ExternalOneByteString);

  const uint8_t* external_data_;
  void* peer_;
  friend class Api;
  friend class String;
};

class UntaggedExternalTwoByteString : public UntaggedString {
  RAW_HEAP_OBJECT_IMPLEMENTATION(ExternalTwoByteString);

  const uint16_t* external_data_;
  void* peer_;
  friend class Api;
  friend class String;
};

class UntaggedBool : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Bool);
  VISIT_NOTHING();

  bool value_;

  friend class Object;
};

class UntaggedArray : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Array);

  VISIT_FROM(ObjectPtr, type_arguments)
  ARRAY_POINTER_FIELD(TypeArgumentsPtr, type_arguments)
  SMI_FIELD(SmiPtr, length)
  // Variable length data follows here.
  VARIABLE_POINTER_FIELDS(ObjectPtr, element, data)
  VISIT_TO_LENGTH(ObjectPtr, &data()[length - 1])

  friend class LinkedHashMapSerializationCluster;
  friend class LinkedHashMapDeserializationCluster;
  friend class CodeSerializationCluster;
  friend class CodeDeserializationCluster;
  friend class Deserializer;
  friend class UntaggedCode;
  friend class UntaggedImmutableArray;
  friend class SnapshotReader;
  friend class GrowableObjectArray;
  friend class LinkedHashMap;
  friend class UntaggedLinkedHashMap;
  friend class Object;
  friend class ICData;            // For high performance access.
  friend class SubtypeTestCache;  // For high performance access.
  friend class ReversePc;
  template <typename Table, bool kAllCanonicalObjectsAreIncludedIntoSet>
  friend class CanonicalSetDeserializationCluster;
  friend class OldPage;
};

class UntaggedImmutableArray : public UntaggedArray {
  RAW_HEAP_OBJECT_IMPLEMENTATION(ImmutableArray);

  friend class SnapshotReader;
};

class UntaggedGrowableObjectArray : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(GrowableObjectArray);

  VISIT_FROM(ObjectPtr, type_arguments)
  POINTER_FIELD(TypeArgumentsPtr, type_arguments)
  SMI_FIELD(SmiPtr, length)
  POINTER_FIELD(ArrayPtr, data)
  VISIT_TO(ObjectPtr, data)
  ObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  friend class SnapshotReader;
  friend class ReversePc;
};

class UntaggedLinkedHashMap : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(LinkedHashMap);

  VISIT_FROM(ObjectPtr, type_arguments)
  POINTER_FIELD(TypeArgumentsPtr, type_arguments)
  POINTER_FIELD(TypedDataPtr, index)
  POINTER_FIELD(SmiPtr, hash_mask)
  POINTER_FIELD(ArrayPtr, data)
  POINTER_FIELD(SmiPtr, used_data)
  POINTER_FIELD(SmiPtr, deleted_keys)
  VISIT_TO(ObjectPtr, deleted_keys)

  friend class SnapshotReader;
};

class UntaggedFloat32x4 : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Float32x4);
  VISIT_NOTHING();

  ALIGN8 float value_[4];

  friend class SnapshotReader;
  friend class Class;

 public:
  float x() const { return value_[0]; }
  float y() const { return value_[1]; }
  float z() const { return value_[2]; }
  float w() const { return value_[3]; }
};
COMPILE_ASSERT(sizeof(UntaggedFloat32x4) == 24);

class UntaggedInt32x4 : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Int32x4);
  VISIT_NOTHING();

  ALIGN8 int32_t value_[4];

  friend class SnapshotReader;

 public:
  int32_t x() const { return value_[0]; }
  int32_t y() const { return value_[1]; }
  int32_t z() const { return value_[2]; }
  int32_t w() const { return value_[3]; }
};
COMPILE_ASSERT(sizeof(UntaggedInt32x4) == 24);

class UntaggedFloat64x2 : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Float64x2);
  VISIT_NOTHING();

  ALIGN8 double value_[2];

  friend class SnapshotReader;
  friend class Class;

 public:
  double x() const { return value_[0]; }
  double y() const { return value_[1]; }
};
COMPILE_ASSERT(sizeof(UntaggedFloat64x2) == 24);

// Define an aliases for intptr_t.
#if defined(ARCH_IS_32_BIT)
#define kIntPtrCid kTypedDataInt32ArrayCid
#define GetIntPtr GetInt32
#define SetIntPtr SetInt32
#define kUintPtrCid kTypedDataUint32ArrayCid
#define GetUintPtr GetUint32
#define SetUintPtr SetUint32
#elif defined(ARCH_IS_64_BIT)
#define kIntPtrCid kTypedDataInt64ArrayCid
#define GetIntPtr GetInt64
#define SetIntPtr SetInt64
#define kUintPtrCid kTypedDataUint64ArrayCid
#define GetUintPtr GetUint64
#define SetUintPtr SetUint64
#else
#error Architecture is not 32-bit or 64-bit.
#endif  // ARCH_IS_32_BIT

class UntaggedExternalTypedData : public UntaggedTypedDataBase {
  RAW_HEAP_OBJECT_IMPLEMENTATION(ExternalTypedData);

 protected:
  VISIT_FROM(CompressedObjectPtr, length)
  VISIT_TO(CompressedObjectPtr, length)
};

class UntaggedPointer : public UntaggedPointerBase {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Pointer);

  VISIT_FROM(CompressedObjectPtr, type_arguments)
  COMPRESSED_POINTER_FIELD(TypeArgumentsPtr, type_arguments)
  VISIT_TO(CompressedObjectPtr, type_arguments)

  friend class Pointer;
};

class UntaggedDynamicLibrary : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(DynamicLibrary);
  VISIT_NOTHING();
  void* handle_;

  friend class DynamicLibrary;
};

// VM implementations of the basic types in the isolate.
class alignas(8) UntaggedCapability : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(Capability);
  VISIT_NOTHING();
  uint64_t id_;
};

class alignas(8) UntaggedSendPort : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(SendPort);
  VISIT_NOTHING();
  Dart_Port id_;
  Dart_Port origin_id_;

  friend class ReceivePort;
};

class UntaggedReceivePort : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(ReceivePort);

  VISIT_FROM(CompressedObjectPtr, send_port)
  COMPRESSED_POINTER_FIELD(SendPortPtr, send_port)
  COMPRESSED_POINTER_FIELD(InstancePtr, handler)
#if !defined(PRODUCT)
  COMPRESSED_POINTER_FIELD(StringPtr, debug_name)
  COMPRESSED_POINTER_FIELD(StackTracePtr, allocation_location)
  VISIT_TO(CompressedObjectPtr, allocation_location)
#else
  VISIT_TO(CompressedObjectPtr, handler)
#endif  // !defined(PRODUCT)
};

class UntaggedTransferableTypedData : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(TransferableTypedData);
  VISIT_NOTHING();
};

// VM type for capturing stacktraces when exceptions are thrown,
// Currently we don't have any interface that this object is supposed
// to implement so we just support the 'toString' method which
// converts the stack trace into a string.
class UntaggedStackTrace : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(StackTrace);

  VISIT_FROM(CompressedObjectPtr, async_link)
  // Link to parent async stack trace.
  COMPRESSED_POINTER_FIELD(StackTracePtr, async_link);
  // Code object for each frame in the stack trace.
  COMPRESSED_POINTER_FIELD(ArrayPtr, code_array);
  // Offset of PC for each frame.
  COMPRESSED_POINTER_FIELD(TypedDataPtr, pc_offset_array);

  VISIT_TO(CompressedObjectPtr, pc_offset_array)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  // False for pre-allocated stack trace (used in OOM and Stack overflow).
  bool expand_inlined_;
  // Whether the link between the stack and the async-link represents a
  // synchronous start to an asynchronous function. In this case, we omit the
  // <asynchronous suspension> marker when concatenating the stacks.
  bool skip_sync_start_in_parent_stack;
};

// VM type for capturing JS regular expressions.
class UntaggedRegExp : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(RegExp);

  VISIT_FROM(CompressedObjectPtr, capture_name_map)
  COMPRESSED_POINTER_FIELD(ArrayPtr, capture_name_map)
  // Pattern to be used for matching.
  COMPRESSED_POINTER_FIELD(StringPtr, pattern)
  COMPRESSED_POINTER_FIELD(ObjectPtr, one_byte)  // FunctionPtr or TypedDataPtr
  COMPRESSED_POINTER_FIELD(ObjectPtr, two_byte)
  COMPRESSED_POINTER_FIELD(ObjectPtr, external_one_byte)
  COMPRESSED_POINTER_FIELD(ObjectPtr, external_two_byte)
  COMPRESSED_POINTER_FIELD(ObjectPtr, one_byte_sticky)
  COMPRESSED_POINTER_FIELD(ObjectPtr, two_byte_sticky)
  COMPRESSED_POINTER_FIELD(ObjectPtr, external_one_byte_sticky)
  COMPRESSED_POINTER_FIELD(ObjectPtr, external_two_byte_sticky)
  VISIT_TO(CompressedObjectPtr, external_two_byte_sticky)
  CompressedObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  intptr_t num_bracket_expressions_;

  // The same pattern may use different amount of registers if compiled
  // for a one-byte target than a two-byte target. For example, we do not
  // need to allocate registers to check whether the current position is within
  // a surrogate pair when matching a Unicode pattern against a one-byte string.
  intptr_t num_one_byte_registers_;
  intptr_t num_two_byte_registers_;

  // A bitfield with two fields:
  // type: Uninitialized, simple or complex.
  // flags: Represents global/local, case insensitive, multiline, unicode,
  //        dotAll.
  int8_t type_flags_;
};

class UntaggedWeakProperty : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(WeakProperty);

  VISIT_FROM(ObjectPtr, key)
  POINTER_FIELD(ObjectPtr, key)
  POINTER_FIELD(ObjectPtr, value)
  VISIT_TO(ObjectPtr, value)
  ObjectPtr* to_snapshot(Snapshot::Kind kind) { return to(); }

  // Linked list is chaining all pending weak properties. Not visited by
  // pointer visitors.
  WeakPropertyPtr next_;

  friend class GCMarker;
  template <bool>
  friend class MarkingVisitorBase;
  friend class Scavenger;
  template <bool>
  friend class ScavengerVisitorBase;
};

// MirrorReferences are used by mirrors to hold reflectees that are VM
// internal objects, such as libraries, classes, functions or types.
class UntaggedMirrorReference : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(MirrorReference);

  VISIT_FROM(CompressedObjectPtr, referent)
  COMPRESSED_POINTER_FIELD(ObjectPtr, referent)
  VISIT_TO(CompressedObjectPtr, referent)
};

// UserTag are used by the profiler to track Dart script state.
class UntaggedUserTag : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(UserTag);

  VISIT_FROM(CompressedObjectPtr, label)
  COMPRESSED_POINTER_FIELD(StringPtr, label)
  VISIT_TO(CompressedObjectPtr, label)

  // Isolate unique tag.
  uword tag_;

  friend class SnapshotReader;
  friend class Object;

 public:
  uword tag() const { return tag_; }
};

class UntaggedFutureOr : public UntaggedInstance {
  RAW_HEAP_OBJECT_IMPLEMENTATION(FutureOr);

  VISIT_FROM(ObjectPtr, type_arguments)
  POINTER_FIELD(TypeArgumentsPtr, type_arguments)
  VISIT_TO(ObjectPtr, type_arguments)

  friend class SnapshotReader;
};

}  // namespace dart

#endif  // RUNTIME_VM_RAW_OBJECT_H_
