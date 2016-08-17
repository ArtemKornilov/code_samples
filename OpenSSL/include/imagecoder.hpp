#ifndef CODER_HPP
#define CODER_HPP

#include <algorithm>
#include <cctype>
#include <cassert>
#include <cstring>

namespace bn
{

	template<class Iter1, class Iter2>
	void encode_b16(Iter1 start, Iter1 end, Iter2 out);

	template<class Iter1, class Iter2>
	void decode_b16(Iter1 start, Iter1 end, Iter2 out);

	namespace impl
	{

		namespace {

			char extract_partial_bits(char value, unsigned int start_bit, unsigned int bits_count)
			{
				assert(start_bit + bits_count < 8);
				char t1 = value >> (8 - bits_count - start_bit);
				char t2 = t1 & ~(-1 << bits_count);
				return t2;
			}

			char extract_overlapping_bits(char previous, char next, unsigned int start_bit, unsigned int bits_count)
			{
				assert(start_bit + bits_count < 16);
				int bits_count_in_previous = 8 - start_bit;
				int bits_count_in_next = bits_count - bits_count_in_previous;
				char t1 = previous << bits_count_in_next;
				char t2 = next >> (8 - bits_count_in_next) & ~(-1 << bits_count_in_next);
				return (t1 | t2) & ~(-1 << bits_count);
			}

		}

		struct b16_conversion_traits
		{
			static size_t group_length()
			{
				return 4;
			}

			static char encode(unsigned int index)
			{
				const char* const dictionary = "0123456789ABCDEF";
				assert(index < strlen(dictionary));
				return dictionary[index];
			}

			static char decode(char c)
			{
				if (c >= '0' && c <= '9') {
					return c - '0';
				}
				else if (c >= 'A' && c <= 'F') {
					return c - 'A' + 10;
				}
				return -1;
			}
		};

		

		template<class ConversionTraits, class Iter1, class Iter2>
		void decode(Iter1 start, Iter1 end, Iter2 out)
		{
			Iter1 iter = start;
			int output_current_bit = 0;
			char buffer = 0;

			while (iter != end) {
				if (std::isspace(*iter)) {
					++iter;
					continue;
				}
				char value = ConversionTraits::decode(*iter);
				if (value == -1) {
					++iter;
					continue;
				}
				unsigned int bits_in_current_byte = std::min<int>(output_current_bit + ConversionTraits::group_length(), 8) - output_current_bit;
				if (bits_in_current_byte == ConversionTraits::group_length()) {

					buffer |= value << (8 - output_current_bit - ConversionTraits::group_length());
					output_current_bit += ConversionTraits::group_length();

					if (output_current_bit == 8) {
						*out++ = buffer;
						buffer = 0;
						output_current_bit = 0;
					}
				}
				else {

					int bits_in_next_byte = ConversionTraits::group_length() - bits_in_current_byte;

					buffer |= value >> bits_in_next_byte;
					*out++ = buffer;
					buffer = 0;

					buffer |= value << (8 - bits_in_next_byte);
					output_current_bit = bits_in_next_byte;
				}
				++iter;
			}
		}

		template<class ConversionTraits, class Iter1, class Iter2>
		void encode(Iter1 start, Iter1 end, Iter2 out)
		{
			Iter1 iter = start;
			int start_bit = 0;
			bool has_backlog = false;
			char backlog = 0;

			while (has_backlog || iter != end) {
				if (!has_backlog) {
					if (start_bit + ConversionTraits::group_length() < 8) {

						char v = extract_partial_bits(*iter, start_bit, ConversionTraits::group_length());
						*out++ = ConversionTraits::encode(v);

						start_bit += ConversionTraits::group_length();
					}
					else {

						backlog = *iter++;
						has_backlog = true;
					}
				}
				else {

					char v;
					if (iter == end)
						v = extract_overlapping_bits(backlog, 0, start_bit, ConversionTraits::group_length());
					else
						v = extract_overlapping_bits(backlog, *iter, start_bit, ConversionTraits::group_length());
					*out++ = ConversionTraits::encode(v);
					has_backlog = false;
					start_bit = (start_bit + ConversionTraits::group_length()) % 8;
				}
			}
		}

	} 

	using namespace bn::impl;

	template<class Iter1, class Iter2>
	void encode_b16(Iter1 start, Iter1 end, Iter2 out)
	{
		encode<b16_conversion_traits>(start, end, out);
	}


	template<class Iter1, class Iter2>
	void decode_b16(Iter1 start, Iter1 end, Iter2 out)
	{
		decode<b16_conversion_traits>(start, end, out);
	}


} 

#endif 

