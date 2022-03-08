#pragma once

#include "base.hpp"
UTL(SYSTEM_HEADER)

namespace utl {
	
	
	
	/*
	 * general purpose type-erased function object
	 * requires target to be copy constructible
	 */
	template <typename>
	class function; // undefined
	
	
	/*
	 * move only type-erased function object
	 * target ist not required to be copy constructible
	 */
	template <typename>
	class unique_function; // undefined
	
}

#include "debug.hpp"
#include "type_traits.hpp"
#include "bit.hpp"

#define UTL_INTERNAL_LOCAL_FUNCTION_SIZE_IN_WORDS 3

#if UTL_ANNOTATE_UTL_FUNCTION_ALLOCATION 
#define UTL_INTERNAL_LOGF_ALLOC(S, A) utl_log("utl::function allocates: (size = {}, align = {})", S, A)
#define UTL_INTERNAL_LOGF_DEALLOC(P, S, A) utl_log("utl::function deallocates: (p = {}, size = {}, align = {})", P, S, A)
#else // UTL_ANNOTATE_UTL_FUNCTION_ALLOCATION
#define UTL_INTERNAL_LOGF_ALLOC(...)
#define UTL_INTERNAL_LOGF_DEALLOC(...)
#endif // UTL_ANNOTATE_UTL_FUNCTION_ALLOCATION


namespace utl::_private::functionDetails {
	template <typename>
	union FunctionStorage;
	template <typename>
	struct FunctionInvokePtrImpl;
	template <typename Sig>
	using FunctionInvokePtr = typename FunctionInvokePtrImpl<Sig>::type;
}

namespace utl {
	
	template <typename F, typename Sig = typename __strip_signature<decltype(&F::operator())>::type>
	function(F) -> function<Sig>;
	
	template <typename R, typename... Args>
	function(R(*)(Args...)) -> function<R(Args...)>;
	
	template <typename F, typename Sig = typename __strip_signature<decltype(&F::operator())>::type>
	unique_function(F) -> unique_function<Sig>;
	
	template <typename R, typename... Args>
	unique_function(R(*)(Args...)) -> unique_function<R(Args...)>;
	
	
	struct nullfunction_t {
		nullfunction_t() = default;
		constexpr nullfunction_t(std::nullptr_t) {}
		template <typename Sig>
		operator function<Sig>() const noexcept { return {}; }
		template <typename Sig>
		operator unique_function<Sig>() const noexcept { return {}; }
		explicit operator bool() const noexcept { return false; }
		
		std::nullptr_t _{};
	} inline constexpr nullfunction;
	
	// MARK: - function
	template <typename R, typename... Args>
	class function<R(Args...)>: public unique_function<R(Args...)> {
		using _base = unique_function<R(Args...)>;
		
	public:
		using typename _base::result_type;
		
	public:
		// MARK: constructors
		using _base::_base;
		function() noexcept = default;
		function(function const& other) = default;
		function(function&& other) noexcept = default;
		template <typename F,
				  UTL_ENABLE_IF(std::is_invocable_r<R, F, Args...>::value),
				  UTL_ENABLE_IF(!std::is_same_v<utl::remove_cvref_t<F>, function>),
				  UTL_ENABLE_IF(std::is_copy_constructible<F>::value)>
		function(F&& f): _base(UTL_FORWARD(f)) {}
		
		// MARK: operator=
		using _base::operator=;
		function& operator=(function const& other) = default;
		function& operator=(function&& other) noexcept = default;
		
		template <typename F,
				  UTL_ENABLE_IF(std::is_invocable_r<R, F, Args...>::value),
				  UTL_ENABLE_IF(std::is_copy_constructible<F>::value)>
		function& operator=(std::reference_wrapper<F> f) noexcept {
			function(f).swap(*this);
		}
		
		// MARK: swap
		void swap(function& other) noexcept {
			this->storage.swap(other.storage);
		}
	};
	
	// MARK: - unique_function
	template <typename R, typename... Args>
	class unique_function<R(Args...)> {
		friend class function<R(Args...)>;
		
	public:
		using result_type = R;
		static constexpr std::size_t sbo_size = UTL_INTERNAL_LOCAL_FUNCTION_SIZE_IN_WORDS * sizeof(void*);
		
	public:
		// MARK: constructors
		unique_function() noexcept = default;
		unique_function(nullfunction_t) noexcept: unique_function() {}
		unique_function(std::nullptr_t) noexcept: unique_function(nullfunction) {}
	private: // prohibit copies, but still allow them for derived friend class 'function<>'
		unique_function(unique_function const& other) noexcept = default;
	public:
		unique_function(unique_function&& other) noexcept = default;
		
		template <typename F,
				  UTL_ENABLE_IF(std::is_invocable_r<R, F, Args...>::value),
				  UTL_ENABLE_IF(!std::is_same_v<utl::remove_cvref_t<F>, unique_function>)>
		unique_function(F&& f): storage(UTL_FORWARD(f)) {}
		
		unique_function(R(*f)(Args...)): storage(f) {}
	
		// MARK: operator=
	private: // prohibit copies, but still allow them for derived friend class 'function<>'
		unique_function& operator=(unique_function const& other) noexcept = default;
	public:
		unique_function& operator=(unique_function&& other) noexcept = default;
		unique_function& operator=(function<R(Args...)> const& other) noexcept {
			storage = other.storage;
			return *this;
		}
		unique_function& operator=(function<R(Args...)>&& other) noexcept {
			storage = std::move(other.storage);
			return *this;
		}
		unique_function& operator=(nullfunction_t) noexcept {
			storage = nullfunction;
			return *this;
		}
		unique_function& operator=(std::nullptr_t) noexcept {
			return *this = nullfunction;
		}
		template <typename F,
				  UTL_ENABLE_IF(std::is_invocable_r<R, F, Args...>::value),
				  UTL_ENABLE_IF(!std::is_same_v<utl::remove_cvref_t<F>, unique_function>)>
		unique_function& operator=(F&& f) {
			storage = UTL_FORWARD(f);
			return *this;
		}
		unique_function& operator=(R(*f)(Args...)) {
			storage = f;
			return *this;
		}
		
		template <typename F,
				  UTL_ENABLE_IF(std::is_invocable_r<R, F, Args...>::value)>
		unique_function& operator=(std::reference_wrapper<F> f) noexcept {
			unique_function(f).swap(*this);
		}
		
		// MARK: swap
		void swap(unique_function& other) noexcept {
			storage.swap(other.storage);
		}
		
		// MARK: operator bool
		explicit operator bool() const noexcept {
			return !storage.isNullFunction();
		}
		
		// MARK: operator()
		R operator()(Args... args) const {
			return storage.invoke(std::forward<Args>(args)...);
		}
		
		// MARK: private
	private:
		using Signature = R(Args...);
		using InvokePtr = _private::functionDetails::FunctionInvokePtr<Signature>;
		
	private:
		_private::functionDetails::FunctionStorage<Signature> storage;
	};
	
	// MARK: - unique_function comparison
	template <typename R, typename... Args>
	bool operator==(unique_function<R(Args...)> const& f, nullfunction_t) noexcept {
		return !f;
	}
	template <typename R, typename... Args>
	bool operator==(nullfunction_t, unique_function<R(Args...)> const& f) noexcept {
		return f == nullfunction;
	}
	
	template <typename R, typename... Args>
	bool operator!=(unique_function<R(Args...)> const& f, nullfunction_t) noexcept {
		return !(f == nullfunction);
	}
	template <typename R, typename... Args>
	bool operator!=(nullfunction_t, unique_function<R(Args...)> const& f) noexcept {
		return !(nullfunction == f);
	}
	
}

namespace utl::_private::functionDetails {
	
	inline constexpr auto ptrSize = sizeof(void*);
	static_assert(ptrSize == sizeof(nullptr));
	
	inline constexpr auto ptrAlign = alignof(void*);
	static_assert(ptrAlign == alignof(std::nullptr_t));
	
	inline constexpr std::size_t LocalFunctionStoragePtrCount = 4;
	
	inline constexpr std::size_t MaxLocalFunctionSize = ptrSize * UTL_INTERNAL_LOCAL_FUNCTION_SIZE_IN_WORDS;
	inline constexpr std::size_t MaxLocalFunctionAlign = alignof(std::max_align_t);
	
//	template <typename F>
//	static constexpr bool localFunctionMaxOffset = (alignof(F) > ptrAlign) ? (alignof(F) - ptrAlign) : 0;
	
	template <typename F, bool isPtr = std::is_function<std::remove_reference_t<F>>::value>
	static constexpr bool fitsInLocalFunction = sizeof(F) <= MaxLocalFunctionSize && alignof(F) <= MaxLocalFunctionAlign;
	
	template <typename F>
	static constexpr bool fitsInLocalFunction<F, true> = true;
	
	// MARK: - VTable
	struct FunctionVTable {
		virtual void copyConstruct(void* to, void const* from) const = 0;
		virtual void moveConstruct(void* to, void* from) const noexcept = 0;
		virtual void destroy(void*) const noexcept = 0;
		virtual void deallocate(void*, void* deallocator) const noexcept = 0;
		virtual std::size_t size() const noexcept = 0;
		virtual std::size_t align() const noexcept = 0;
	private:
		virtual void* invokePtr() const noexcept = 0;
	public:
		
		template <typename R, typename... Args, typename ... U>
		R invoke(void const* _storage, U&&... u) const {
			auto invokePtr = reinterpret_cast<R(*)(void*, Args...)>(this->invokePtr());
			void* storage = const_cast<void*>(_storage);
			return invokePtr(storage, std::forward<U>(u)...);
		}
	};
	
	static_assert(sizeof(FunctionVTable) == sizeof(void*));
	static_assert(alignof(FunctionVTable) == alignof(void*));
	
	struct FunctionVTableStorage {
//		static auto constexpr _size  =  sizeof(FunctionVTable);
//		static auto constexpr _align = alignof(FunctionVTable);
		FunctionVTableStorage() = default;
		FunctionVTableStorage(FunctionVTableStorage const& rhs) {
			std::memcpy(&this->data, &rhs.data, sizeof(void*));
		}
		
		void* data;
	};
	
	template <typename F, typename Sig, typename Deallocator = void>
	struct FunctionVTableImpl;
	
	template <typename F, typename Deallocator, typename R, typename... Args>
	struct FunctionVTableImpl<F, R(Args...), Deallocator>: FunctionVTable {
		FunctionVTableImpl() = default;
		
		void copyConstruct(void* to, void const* from) const final {
			if constexpr (std::is_copy_constructible_v<F>) {
				new (to) F(*static_cast<F const*>(from));
			}
			else {
				utl_debugbreak("this code must not be reachable."
							   "F is is not copy constructible.");
			}
			
		}
		
		void moveConstruct(void* to, void* from) const noexcept final {
			new (to) F(std::move(*static_cast<F*>(from)));
		}
		
		void destroy(void* obj) const noexcept final {
			static_cast<F*>(obj)->F::~F();
		}
		
		void deallocate(void* p, void* deallocatorContext) const noexcept final {
			if constexpr (!std::is_same_v<Deallocator, void>) {
				Deallocator{}(p, deallocatorContext);
			}
			else {
				utl_debugbreak("this must not be called with a void Deallocator");
			}
		}
		
		std::size_t size() const noexcept final {
			return sizeof(F);
		}
		std::size_t align() const noexcept final {
			return alignof(F);
		}
		
		
		static R _staticInvokePtr(void* _objPtr, Args... args) {
			F* objPtr = static_cast<F*>(_objPtr);
			return objPtr->operator()(static_cast<Args>(args) ...);
		}
		
		void* invokePtr() const noexcept final {
			return reinterpret_cast<void*>(_staticInvokePtr);
		}
	};
	
	// MARK: - LocalFunctionObjectStorage
	template <typename>
	struct LocalFunctionObjectStorage;
	
	template <typename R, typename... Args>
	struct alignas(MaxLocalFunctionAlign) LocalFunctionObjectStorage<R(Args...)> {
	public:
		using Signature = R(Args...);
		
		// constructor from function pointer
		template <typename F, UTL_ENABLE_IF(std::is_function<std::remove_reference_t<F>>::value)>
		LocalFunctionObjectStorage(F&& f): LocalFunctionObjectStorage([f](auto&&... args) {
			return f(UTL_FORWARD(args)...);
		}) {}
		template <typename F, UTL_ENABLE_IF(std::is_pointer<std::remove_reference_t<F>>::value)>
		LocalFunctionObjectStorage(F&& f): LocalFunctionObjectStorage([f](auto&&... args) {
			return f(UTL_FORWARD(args)...);
		}) {}
			
		// constructor from callable
		template <typename F, UTL_ENABLE_IF(!std::is_pointer<std::remove_reference_t<F>>::value &&
											!std::is_function<std::remove_reference_t<F>>::value)>
		LocalFunctionObjectStorage(F&& f) {
			using _F = remove_cvref_t<F>;
			using VTable = FunctionVTableImpl<_F, R(Args...)>;
			static_assert(fitsInLocalFunction<_F>);
			static_assert(sizeof(VTable) == sizeof(vTableStorage));
			::new (storagePtr()) _F(UTL_FORWARD(f));
			::new (&vTableStorage) VTable();
		}
		
		// copy constructor
		LocalFunctionObjectStorage(LocalFunctionObjectStorage const& other):
			vTableStorage(other.vTableStorage) {
				vTable()->copyConstruct(storagePtr(), other.storagePtr());
		}
		
		// move constructor
		LocalFunctionObjectStorage(LocalFunctionObjectStorage&& other):
			vTableStorage(other.vTableStorage) {
				vTable()->moveConstruct(storagePtr(), other.storagePtr());
		}
		
		// destructor
		~LocalFunctionObjectStorage() {
			vTable()->destroy(storagePtr());
		}
		
		void destroy() {
			this->~LocalFunctionObjectStorage();
		}

		// invoke
		template <typename ... U>
		R invoke(U&&... u) const {
			return vTable()->template invoke<R, Args...>(storagePtr(), UTL_FORWARD(u)...);
		}
		
	private:
		FunctionVTable const* vTable() const noexcept { return reinterpret_cast<FunctionVTable const*>(&vTableStorage); }
		
		void* storagePtr() noexcept {
			return as_mutable(as_const(this)->storagePtr());
		}
		
		void const* storagePtr() const noexcept {
			return _functionStorage;
		}
		
	private: // data
		char /* aligned by enclosing class */ _functionStorage[MaxLocalFunctionSize];
		FunctionVTableStorage vTableStorage;
	};
	
//	static_assert(sizeof(LocalFunctionObjectStorage<void(), true>) == MaxLocalFunctionSize + ptrSize * 2);
	
	
	// MARK: - HeapFunctionObjectStorage
	template <typename>
	struct HeapFunctionObjectStorage;
	
	template <typename R, typename ... Args>
	struct HeapFunctionObjectStorage<R(Args...)> {
	public:
		using Signature = R(Args...);
		
		
		// constructor from function pointer
		template <typename F, UTL_ENABLE_IF(std::is_function<std::remove_reference_t<F>>::value)>
		HeapFunctionObjectStorage(F&& f): HeapFunctionObjectStorage([f](auto&&... args) {
			return f(UTL_FORWARD(args)...);
		}) {}
		template <typename F, UTL_ENABLE_IF(std::is_pointer<std::remove_reference_t<F>>::value)>
		HeapFunctionObjectStorage(F&& f): HeapFunctionObjectStorage([f](auto&&... args) {
			return f(UTL_FORWARD(args)...);
		}) {}
			
		// constructor from callable
		template <typename F, UTL_ENABLE_IF(!std::is_pointer<std::remove_reference_t<F>>::value &&
											!std::is_function<std::remove_reference_t<F>>::value)>
		HeapFunctionObjectStorage(F&& f)
		{
				using _F = std::remove_cvref_t<F>;
				
				auto constexpr _dealloc = [](void* p, void* obj) {
					static_cast<HeapFunctionObjectStorage*>(obj)->deallocate(p, sizeof(_F), alignof(_F));
				};
				using Deallocator = decltype(_dealloc);
				using VTable = FunctionVTableImpl<_F, R(Args...), Deallocator>;
				static_assert(!fitsInLocalFunction<_F>);
				static_assert(sizeof(VTable) == sizeof(vTableStorage));
				new (&vTableStorage) VTable();
				this->functionStorage = allocate(sizeof(_F), alignof(_F));
				new (storagePtr()) _F(UTL_FORWARD(f));
		}
		
		
		// copy constructor
		HeapFunctionObjectStorage(HeapFunctionObjectStorage const& other):
			vTableStorage(other.vTableStorage)
		{
				this->functionStorage = allocate(vTable()->size(), vTable()->align());
				vTable()->copyConstruct(storagePtr(), other.storagePtr());
		}
		
		// move constructor
		HeapFunctionObjectStorage(HeapFunctionObjectStorage&& other):
			vTableStorage(other.vTableStorage)
		{
				this->functionStorage = other.functionStorage;
				other.functionStorage = nullptr;
		}
		
		// destructor
		~HeapFunctionObjectStorage() {
			if (storagePtr()) {
				vTable()->destroy(storagePtr());
				vTable()->deallocate(storagePtr(), this);
			}
		}
		
		void destroy() {
			this->~HeapFunctionObjectStorage();
		}
		
		// invoke
		template <typename ... U>
		R invoke(U&&... u) const {
			return vTable()->template invoke<R, Args...>(storagePtr(), UTL_FORWARD(u)...);
		}
		
	private:
		FunctionVTable const* vTable() const noexcept { return reinterpret_cast<FunctionVTable const*>(&vTableStorage); }
		
		template <typename F>
		static R staticInvokePtr(void const* obj, Args... args) {
			return static_cast<F*>(as_mutable(obj))->operator()(std::forward<Args>(args)...);
		};
		
		template <typename = void>
		void* storagePtr() noexcept {
			return this->functionStorage;
		}
		
		template <typename = void>
		void const* storagePtr() const noexcept {
			return this->functionStorage;
		}
		
		[[nodiscard]] void* allocate(std::size_t size, std::size_t align) {
			UTL_INTERNAL_LOGF_ALLOC(size, align);
			return ::operator new(size, static_cast<std::align_val_t>(align));
		}
		void deallocate(void* p, std::size_t size, std::size_t align) noexcept {
			UTL_INTERNAL_LOGF_DEALLOC(p, size, align);
			::operator delete(p, size, static_cast<std::align_val_t>(align));
		}
		
	private: // data
		static_assert(sizeof(FunctionVTableStorage) == sizeof(void*));
		FunctionVTableStorage vTableStorage;
		void* functionStorage;
		char _placeholder[(UTL_INTERNAL_LOCAL_FUNCTION_SIZE_IN_WORDS - 2) * sizeof(void*)];
		std::nullptr_t _heapIndicator{}; // this is always null if this is of type HeapFunctionObjectStorage
	};
	
	// MARK: - NullFunctionStorage
	struct NullFunctionStorage {
		void destroy() {
			this->~NullFunctionStorage();
		}
		
	private: // data
		std::nullptr_t _placeholder[UTL_INTERNAL_LOCAL_FUNCTION_SIZE_IN_WORDS + 1] = {};
	};
	
	
	// MARK: - FunctionStorage
	template <typename R, typename ... Args>
	union FunctionStorage<R(Args...)> {
		using Signature = R(Args...);
		
	private:
		template <std::size_t Offset, std::size_t Size>
		bool thisIsNull() const noexcept {
			std::aligned_storage_t<MaxLocalFunctionSize + sizeof(void*), MaxLocalFunctionAlign> null{};
			return std::memcmp(utl::offset(this, Offset), &null, Size) == 0;
		}
		
	public:
		/* these need to be called in this order like so:
		 * if (isLocalFunctionObject()) {
		 *     // ...
		 * }
		 * else if (isHeapFunctionObject()) {
		 *     // ...
		 * }
		 * else /*  is nullFunction * / {
		 *     // ...
		 * }
		 */
		bool isLocalFunctionObject() const noexcept {
			return !thisIsNull<MaxLocalFunctionSize, sizeof(void*)>();
		}
		bool isHeapFunctionObject() const noexcept {
			return !thisIsNull<0, sizeof(void*)>();
		}
		
		// this can be called on its own
		bool isNullFunction() const noexcept {
			static constexpr std::size_t localSize = (UTL_INTERNAL_LOCAL_FUNCTION_SIZE_IN_WORDS + 1) * sizeof(void*);
			static constexpr std::nullptr_t null[localSize] = {};
			return std::memcmp(this, null, localSize) == 0;
		}
		
		FunctionStorage(): nullFunction{} {}
		
		template <typename F, UTL_ENABLE_IF(fitsInLocalFunction<F>)>
		FunctionStorage(F&& f): localFunction(std::forward<F>(f)) {}
		
		template <typename F, UTL_ENABLE_IF(!fitsInLocalFunction<F>)>
		FunctionStorage(F&& f): heapFunction(std::forward<F>(f)) {}
		
		FunctionStorage(FunctionStorage const& other) {
			if (other.isLocalFunctionObject()) {
				new (&this->localFunction) LocalFunctionObjectStorage<Signature>(other.localFunction);
			}
			else if (other.isHeapFunctionObject()) {
				new (&this->heapFunction) HeapFunctionObjectStorage<Signature>(other.heapFunction);
			}
			else {
				new (&this->nullFunction) NullFunctionStorage();
			}
		}
		
		FunctionStorage(FunctionStorage&& other) {
			if (other.isLocalFunctionObject()) {
				new (&this->localFunction) LocalFunctionObjectStorage<Signature>(std::move(other.localFunction));
				other.localFunction.destroy();
				new (&other.nullFunction) NullFunctionStorage();
			}
			else if (other.isHeapFunctionObject()) {
				new (&this->heapFunction) HeapFunctionObjectStorage<Signature>(std::move(other.heapFunction));
				other.heapFunction.destroy();
				new (&other.nullFunction) NullFunctionStorage();
			}
			else {
				new (&this->nullFunction) NullFunctionStorage();
			}
		}
		
		~FunctionStorage() {
			if (this->isLocalFunctionObject()) {
				this->localFunction.destroy();
			}
			else if (this->isHeapFunctionObject()) {
				this->heapFunction.destroy();
			}
			else {
				this->nullFunction.destroy();
			}
		}
		
		FunctionStorage& operator=(FunctionStorage const& other) {
			this->destroy();
			new (this) FunctionStorage(other);
			return *this;
		}
		
		FunctionStorage& operator=(FunctionStorage&& other) noexcept {
			this->destroy();
			new (this) FunctionStorage(std::move(other));
			return *this;
		}
		
		FunctionStorage& operator=(nullfunction_t) noexcept {
			destroy();
			new (&nullFunction) NullFunctionStorage();
			return *this;
		}

		template <typename F>
		FunctionStorage& operator=(F&& f) noexcept {
			destroy();
			new (this) FunctionStorage(UTL_FORWARD(f));
			return *this;
		}
		
		template <typename... U>
		R invoke(U&&... u) const {
			utl_expect(!this->isNullFunction(), "trying to invoke a nullfunction");
			if (this->isLocalFunctionObject()) {
				return this->localFunction.invoke(std::forward<U>(u)...);
			}
			else {
				utl_assert(!this->isNullFunction(), "this is a nullfunction");
				return this->heapFunction.invoke(std::forward<U>(u)...);
			}
		}
		
		void destroy() noexcept {
			this->~FunctionStorage();
		}
		
		void swap(FunctionStorage& other) noexcept {
#if 0 /* following code is untested */
			if (this->isLocalFunctionObject()) {
				if (other.isLocalFunctionObject()) {
					std::swap(this->localFunction, other.localFunction);
					return;
				}
				else {
					/* do nothing */
				}
			}
			else if (this->isHeapFunctionObject()) {
				if (other.isLocalFunctionObject()) {
					/* do nothing */
				}
				else if (other.isHeapFunctionObject()) {
					std::swap(this->heapFunction, other.heapFunction);
					return;
				}
				else {
					/* do nothing */
				}
			}
			else {
				if (other.isLocalFunctionObject()) {
					/* do nothing */
				}
				else if (other.isHeapFunctionObject()) {
					/* do nothing */
				}
				else {
					return;
				}
			}
#endif
			std::swap(*this, other);
		}
		
		LocalFunctionObjectStorage<R(Args...)> localFunction;
		HeapFunctionObjectStorage<R(Args...)> heapFunction;
		NullFunctionStorage nullFunction;
	};

	static_assert(alignof(FunctionStorage<void()>) == MaxLocalFunctionAlign);
	
	template <typename R, typename ... Args>
	struct FunctionInvokePtrImpl<R(Args...)> {
		using type = R(*)(void const*, Args...);
	};
}


static_assert(sizeof(utl::function<void()>) == (UTL_INTERNAL_LOCAL_FUNCTION_SIZE_IN_WORDS + 1) * sizeof(void*));
