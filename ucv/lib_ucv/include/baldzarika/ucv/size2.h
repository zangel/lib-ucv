#ifndef BALDZARIKA_UCV_SIZE2_H
#define BALDZARIKA_UCV_SIZE2_H

namespace baldzarika { namespace ucv {

	template <typename T>
	class size2
		: public gil::point2<T>
	{
	public:
		typedef gil::point2<T> base_t;
		size2() {}
		size2(T w, T h): base_t(w,h) {}
		size2(size2 const &s): base_t(s) {}
		~size2() {}

		T		width() const { return base_t::x; }
		T		height() const { return base_t::y; }
		T		area() const { return base_t::x*base_t::y; }
		bool	empty() const { return !(area()>0); }
	};

	typedef size2<float>			size2f;
	typedef size2<boost::uint32_t>	size2ui;

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SIZE2_H
