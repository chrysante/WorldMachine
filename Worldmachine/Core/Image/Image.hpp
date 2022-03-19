#pragma once

#include "Core/Debug.hpp"
#include "Core/DataType.hpp"

#include <mtl/mtl.hpp>
#include <utl/vector.hpp>
#include <utl/functional.hpp>
#include <span>

namespace worldmachine {
	
	std::size_t imageHash(std::span<float const> data, std::size_t summands);
	
	/// MARK: Image
	class Image {
	public:
		Image(DataType dataType = DataType::float1): m_dataType(dataType) {}
		Image(DataType, mtl::uint2 size);
		
		mtl::usize2 size() const { return m_size; }
		
		void resize(mtl::uint2 newSize) {
			m_size = newSize;
			m_data.resize(_flatImageSize());
		}
		
		void clear() {
			m_size = { 0, 0 };
			m_data.clear();
		}
		
		std::size_t dataTypeSize() const { return worldmachine::dataTypeSize(m_dataType); }

		DataType dataType() const { return m_dataType; }
		void setDataType(DataType t) { m_dataType = t; }
		
		bool empty() const { return m_data.empty(); }
		
		float* data() { return m_data.data(); }
		float const* data() const { return m_data.data(); }
		
		auto begin() { return m_data.begin(); }
		auto begin() const { return m_data.begin(); }
		auto end() { return m_data.end(); }
		auto end() const { return m_data.end(); }
		
	private:
		std::size_t _flatImageSize() const { return m_size.fold(utl::multiplies) * utl::to_underlying(m_dataType); }
		
	private:
		DataType m_dataType = DataType::float1;
		mtl::usize2 m_size{};
		utl::vector<float> m_data;
	};
	
	/// MARK: ImageView
	template <typename> class ImageView;
	
	template <typename VT>
//	requires utl::any_of<typename mtl::get_underlying_type<VT>::type, float,  float const>
	class ImageView {
		using T = typename mtl::get_underlying_type<VT>::type;
		static constexpr bool _isConst = std::is_const_v<VT>;
		static constexpr std::size_t N = mtl::get_vector_size<VT>::value;
		static constexpr auto O = mtl::get_vector_options<VT>::value;
		
		template <typename> friend class ImageView;
		
	public:
		using ValueType = VT;
		
	public:
		ImageView() = default;
		ImageView(Image& img):
			_data(reinterpret_cast<ValueType*>(img.data())), _size(img.size())
		{
			WM_Assert(N != 3 || O.packed(),
					  "If N == 3 we mustn't use an aligned type because the"
					  "image has no padding values for the 4th component.");
			WM_Assert(dataTypeSize(img.dataType()) == sizeof(ValueType));
		}
		
		ImageView(Image const& img) requires(_isConst):
			ImageView(const_cast<Image&>(img)) // we can cast away constness here because if we are const then it will be cast back in the other constructor
		{}
		
		ImageView(T* data, mtl::usize2 size): _data(data), _size(size) {}
		
		ValueType* data() const { return _data; }
		mtl::usize2 size() const { return _size; }
		
		explicit operator bool() const { return _data; }
		
		operator ImageView<ValueType const>() const { return { _data, _size }; }
		
		// const returning non const references because this is a view
		ValueType& operator()(std::size_t i, std::size_t j) const {
			WM_BoundsCheck(i, 0, _size.x);
			WM_BoundsCheck(j, 0, _size.y);
			return *(data() + (_size.x * j + i));
		}
		
		ValueType& operator[](mtl::usize2 index) const {
			
			return (*this)(index.x, index.y);
		}
		
		ValueType& operator[](std::size_t i) const {
			WM_BoundsCheck(i, 0, _size.x * _size.y);
			return *(data() + i);
		}
		
		ValueType* begin() const { return _data; }
		ValueType* end() const { return _data + _size.x * _size.y; }
		
	private:
		ValueType* _data = nullptr;
		mtl::usize2 _size = 0;
	};

}
