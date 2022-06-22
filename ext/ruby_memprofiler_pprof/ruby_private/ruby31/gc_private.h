#ifndef GC_PRIVATE_H
#define GC_PRIVATE_H

struct RMoved {
    VALUE flags;
    VALUE dummy;
    VALUE destination;
};

struct RRational {
    struct RBasic basic;
    VALUE num;
    VALUE den;
};

struct RComplex {
    struct RBasic basic;
    VALUE real;
    VALUE imag;
};

struct RSymbol {
    struct RBasic basic;
    st_index_t hashval;
    VALUE fstr;
    ID id;
};

typedef struct RVALUE {
    union {
	struct {
	    VALUE flags;		/* always 0 for freed obj */
	    struct RVALUE *next;
	} free;
        struct RMoved  moved;
	struct RBasic  basic;
	struct RObject object;
	struct RClass  klass;
	struct RFloat  flonum;
	struct RString string;
	struct RArray  array;
	struct RRegexp regexp;
	struct RHash   hash;
	struct RData   data;
	struct RTypedData   typeddata;
	struct RStruct rstruct;
	struct RBignum bignum;
	struct RFile   file;
	struct RMatch  match;
	struct RRational rational;
	struct RComplex complex;
    struct RSymbol symbol;
	union {
	    rb_cref_t cref;
	    struct vm_svar svar;
	    struct vm_throw_data throw_data;
	    struct vm_ifunc ifunc;
	    struct MEMO memo;
	    struct rb_method_entry_struct ment;
	    const rb_iseq_t iseq;
	    rb_env_t env;
	    struct rb_imemo_tmpbuf_struct alloc;
	    rb_ast_t ast;
	} imemo;
	struct {
	    struct RBasic basic;
	    VALUE v1;
	    VALUE v2;
	    VALUE v3;
	} values;
    } as;
#if GC_DEBUG
    const char *file;
    int line;
#endif
} RVALUE;

#define RANY(o) ((RVALUE*)(o))
#define STACK_CHUNK_SIZE 500

typedef struct stack_chunk {
    VALUE data[STACK_CHUNK_SIZE];
    struct stack_chunk *next;
} stack_chunk_t;

typedef struct mark_stack {
    stack_chunk_t *chunk;
    stack_chunk_t *cache;
    int index;
    int limit;
    size_t cache_size;
    size_t unused_cache_size;
} mark_stack_t;

typedef struct rb_heap_struct {
    struct heap_page *free_pages;
    struct list_head pages;
    struct heap_page *sweeping_page; /* iterator for .pages */
    struct heap_page *compact_cursor;
    RVALUE * compact_cursor_index;
#if GC_ENABLE_INCREMENTAL_MARK
    struct heap_page *pooled_pages;
#endif
    size_t total_pages;      /* total page count in a heap */
    size_t total_slots;      /* total slot count (about total_pages * HEAP_PAGE_OBJ_LIMIT) */
} rb_heap_t;

typedef struct rb_size_pool_struct {
    short slot_size;

    size_t allocatable_pages;

#if USE_RVARGC
    /* Sweeping statistics */
    size_t freed_slots;
    size_t empty_slots;

    /* Global statistics */
    size_t force_major_gc_count;
#endif

    rb_heap_t eden_heap;
    rb_heap_t tomb_heap;
} rb_size_pool_t;

struct heap_page_header {
    struct heap_page *page;
};

struct heap_page_body {
    struct heap_page_header header;
    /* char gap[];      */
    /* RVALUE values[]; */
};

typedef uintptr_t bits_t;
enum {
    BITS_SIZE = sizeof(bits_t),
    BITS_BITLENGTH = ( BITS_SIZE * CHAR_BIT )
};
#define HEAP_PAGE_ALIGN_LOG 14
#define CEILDIV(i, mod) (((i) + (mod) - 1)/(mod))
enum {
    HEAP_PAGE_ALIGN = (1UL << HEAP_PAGE_ALIGN_LOG),
    HEAP_PAGE_ALIGN_MASK = (~(~0UL << HEAP_PAGE_ALIGN_LOG)),
    HEAP_PAGE_SIZE = HEAP_PAGE_ALIGN,
    HEAP_PAGE_OBJ_LIMIT = (unsigned int)((HEAP_PAGE_SIZE - sizeof(struct heap_page_header))/sizeof(struct RVALUE)),
    HEAP_PAGE_BITMAP_LIMIT = CEILDIV(CEILDIV(HEAP_PAGE_SIZE, sizeof(struct RVALUE)), BITS_BITLENGTH),
    HEAP_PAGE_BITMAP_SIZE = (BITS_SIZE * HEAP_PAGE_BITMAP_LIMIT),
};
#define NUM_IN_PAGE(p)   (((bits_t)(p) & HEAP_PAGE_ALIGN_MASK)/sizeof(RVALUE))

struct heap_page {
    short slot_size;
    short total_slots;
    short free_slots;
    short pinned_slots;
    short final_slots;
    struct {
	unsigned int before_sweep : 1;
	unsigned int has_remembered_objects : 1;
	unsigned int has_uncollectible_shady_objects : 1;
	unsigned int in_tomb : 1;
    } flags;

    rb_size_pool_t *size_pool;

    struct heap_page *free_next;
    RVALUE *start;
    RVALUE *freelist;
    struct list_node page_node;

    bits_t wb_unprotected_bits[HEAP_PAGE_BITMAP_LIMIT];
    /* the following three bitmaps are cleared at the beginning of full GC */
    bits_t mark_bits[HEAP_PAGE_BITMAP_LIMIT];
    bits_t uncollectible_bits[HEAP_PAGE_BITMAP_LIMIT];
    bits_t marking_bits[HEAP_PAGE_BITMAP_LIMIT];

    /* If set, the object is not movable */
    bits_t pinned_bits[HEAP_PAGE_BITMAP_LIMIT];
};


typedef struct gc_profile_record {
    unsigned int flags;

    double gc_time;
    double gc_invoke_time;

    size_t heap_total_objects;
    size_t heap_use_size;
    size_t heap_total_size;
    size_t moved_objects;

#if GC_PROFILE_MORE_DETAIL
    double gc_mark_time;
    double gc_sweep_time;

    size_t heap_use_pages;
    size_t heap_live_objects;
    size_t heap_free_objects;

    size_t allocate_increase;
    size_t allocate_limit;

    double prepare_time;
    size_t removing_objects;
    size_t empty_objects;
#if GC_PROFILE_DETAIL_MEMORY
    long maxrss;
    long minflt;
    long majflt;
#endif
#endif
#if MALLOC_ALLOCATED_SIZE
    size_t allocated_size;
#endif

#if RGENGC_PROFILE > 0
    size_t old_objects;
    size_t remembered_normal_objects;
    size_t remembered_shady_objects;
#endif
} gc_profile_record;


typedef struct rb_objspace {
    struct {
	size_t limit;
	size_t increase;
#if MALLOC_ALLOCATED_SIZE
	size_t allocated_size;
	size_t allocations;
#endif
    } malloc_params;

    struct {
	unsigned int mode : 2;
	unsigned int immediate_sweep : 1;
	unsigned int dont_gc : 1;
	unsigned int dont_incremental : 1;
	unsigned int during_gc : 1;
        unsigned int during_compacting : 1;
	unsigned int gc_stressful: 1;
	unsigned int has_hook: 1;
	unsigned int during_minor_gc : 1;
#if GC_ENABLE_INCREMENTAL_MARK
	unsigned int during_incremental_marking : 1;
#endif
        unsigned int measure_gc : 1;
    } flags;

    rb_event_flag_t hook_events;
    size_t total_allocated_objects;
    VALUE next_object_id;

    rb_size_pool_t size_pools[SIZE_POOL_COUNT];

    struct {
	rb_atomic_t finalizing;
    } atomic_flags;

    mark_stack_t mark_stack;
    size_t marked_slots;

    struct {
	struct heap_page **sorted;
	size_t allocated_pages;
	size_t allocatable_pages;
	size_t sorted_length;
	RVALUE *range[2];
	size_t freeable_pages;

	/* final */
	size_t final_slots;
	VALUE deferred_final;
    } heap_pages;

    st_table *finalizer_table;

    struct {
	int run;
	unsigned int latest_gc_info;
	gc_profile_record *records;
	gc_profile_record *current_record;
	size_t next_index;
	size_t size;

#if GC_PROFILE_MORE_DETAIL
	double prepare_time;
#endif
	double invoke_time;

	size_t minor_gc_count;
	size_t major_gc_count;
	size_t compact_count;
	size_t read_barrier_faults;
#if RGENGC_PROFILE > 0
	size_t total_generated_normal_object_count;
	size_t total_generated_shady_object_count;
	size_t total_shade_operation_count;
	size_t total_promoted_count;
	size_t total_remembered_normal_object_count;
	size_t total_remembered_shady_object_count;

#if RGENGC_PROFILE >= 2
	size_t generated_normal_object_count_types[RUBY_T_MASK];
	size_t generated_shady_object_count_types[RUBY_T_MASK];
	size_t shade_operation_count_types[RUBY_T_MASK];
	size_t promoted_types[RUBY_T_MASK];
	size_t remembered_normal_object_count_types[RUBY_T_MASK];
	size_t remembered_shady_object_count_types[RUBY_T_MASK];
#endif
#endif /* RGENGC_PROFILE */

	/* temporary profiling space */
	double gc_sweep_start_time;
	size_t total_allocated_objects_at_gc_start;
	size_t heap_used_at_gc_start;

	/* basic statistics */
	size_t count;
	size_t total_freed_objects;
	size_t total_allocated_pages;
	size_t total_freed_pages;
        uint64_t total_time_ns;
        struct timespec start_time;
    } profile;
    struct gc_list *global_list;

    VALUE gc_stress_mode;

    struct {
	VALUE parent_object;
	int need_major_gc;
	size_t last_major_gc;
	size_t uncollectible_wb_unprotected_objects;
	size_t uncollectible_wb_unprotected_objects_limit;
	size_t old_objects;
	size_t old_objects_limit;

#if RGENGC_ESTIMATE_OLDMALLOC
	size_t oldmalloc_increase;
	size_t oldmalloc_increase_limit;
#endif

#if RGENGC_CHECK_MODE >= 2
	struct st_table *allrefs_table;
	size_t error_count;
#endif
    } rgengc;

    struct {
        size_t considered_count_table[T_MASK];
        size_t moved_count_table[T_MASK];
        size_t total_moved;
    } rcompactor;

#if GC_ENABLE_INCREMENTAL_MARK
    struct {
	size_t pooled_slots;
	size_t step_slots;
    } rincgc;
#endif

    st_table *id_to_obj_tbl;
    st_table *obj_to_id_tbl;

#if GC_DEBUG_STRESS_TO_CLASS
    VALUE stress_to_class;
#endif
} rb_objspace_t;

#endif
