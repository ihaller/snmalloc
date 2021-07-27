#pragma once

#include "../backend/backend.h"
#include "../mem/corealloc.h"
#include "../mem/pool.h"
#include "../mem/slaballocator.h"
#include "commonconfig.h"

namespace snmalloc
{
  /**
   * A single fixed address range allocator configuration
   */
  template<
    SNMALLOC_CONCEPT(ConceptPAL) PAL,
    typename BackendTemplate = BackendAllocator<PAL, true>>
  class FixedGlobals : public CommonConfig
  {
  public:
    using Backend = BackendTemplate;

  private:
    inline static typename Backend::GlobalState backend_state;

    inline static ChunkAllocatorState slab_allocator_state;

    inline static PoolState<CoreAllocator<FixedGlobals>> alloc_pool;

  public:
    static typename Backend::GlobalState& get_backend_state()
    {
      return backend_state;
    }

    ChunkAllocatorState& get_slab_allocator_state()
    {
      return slab_allocator_state;
    }

    PoolState<CoreAllocator<FixedGlobals>>& pool()
    {
      return alloc_pool;
    }

    static constexpr bool IsQueueInline = true;

    // Performs initialisation for this configuration
    // of allocators.  Will be called at most once
    // before any other datastructures are accessed.
    void ensure_init() noexcept
    {
#ifdef SNMALLOC_TRACING
      std::cout << "Run init_impl" << std::endl;
#endif
    }

    static bool is_initialised()
    {
      return true;
    }

    // This needs to be a forward reference as the
    // thread local state will need to know about this.
    // This may allocate, so must be called once a thread
    // local allocator exists.
    static void register_clean_up()
    {
      snmalloc::register_clean_up();
    }

    static void init(void* base, size_t length)
    {
      // Initialise key for remote deallocation lists
      key_global = FreeListKey(get_entropy64<PAL>());

      get_backend_state().init(base, length);
    }

    constexpr static FixedGlobals get_handle()
    {
      return {};
    }
  };
}
