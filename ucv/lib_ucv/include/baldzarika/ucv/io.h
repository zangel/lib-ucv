#ifndef BALDZARIKA_UCV_IO_H
#define BALDZARIKA_UCV_IO_H

namespace baldzarika { namespace ucv {

	namespace detail {
		static const std::vector<std::string> io_extensions_png = { {"png"} };
		static const std::vector<std::string> io_extensions_jpg = { {"jpg"}, {"jpeg"} };
	}

	template <typename Image>
	inline void read_and_convert_image(std::string const &path, Image &image)
	{
		auto dot_pos = path.find_last_of('.');
		if (dot_pos == std::string::npos || dot_pos + 1 >= path.size())
		{
			return;
		}

		std::string extension = path.substr(dot_pos + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), [](char c) -> char { return std::tolower(c); });

#		if defined(UCV_HAS_PNG)
		if (std::find(detail::io_extensions_png.begin(), detail::io_extensions_png.end(), extension) != detail::io_extensions_png.end())
		{
			gil::read_and_convert_image(path, image, gil::png_tag());
			return;
		}
#		endif

#		if defined(UCV_HAS_JPG)
		if (std::find(detail::io_extensions_jpg.begin(), detail::io_extensions_jpg.end(), extension) != detail::io_extensions_jpg.end())
		{
			gil::read_and_convert_image(path, image, gil::jpeg_tag());
			return;
		}
#		endif
	}

	template <typename View>
	inline void write_view(std::string const &path, View view)
	{
		auto dot_pos = path.find_last_of('.');
		if (dot_pos == std::string::npos || dot_pos + 1 >= path.size())
		{
			return;
		}

		std::string extension = path.substr(dot_pos + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), [](char c) -> char { return std::tolower(c); });

#		if defined(UCV_HAS_PNG)
		if (std::find(detail::io_extensions_png.begin(), detail::io_extensions_png.end(), extension) != detail::io_extensions_png.end())
		{
			gil::write_view(path, view, gil::png_tag());
			return;
		}
#		endif

#		if defined(UCV_HAS_JPG)
		if (std::find(detail::io_extensions_jpg.begin(), detail::io_extensions_jpg.end(), extension) != detail::io_extensions_jpg.end())
		{
			gil::write_view(path, view, gil::jpeg_tag());
			return;
		}
#		endif
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_IO_H
