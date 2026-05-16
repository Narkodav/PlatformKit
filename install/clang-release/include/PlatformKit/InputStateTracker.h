#pragma once
#include <array>
#include <type_traits>
#include <limits>

namespace PlatformKit
{
	template <typename InputEnum, size_t s_numInputs,
		typename InputStateEnum, size_t s_numInputStates,
		typename StorageType = uint8_t>
		requires std::is_enum_v<InputEnum>&&
	std::is_enum_v<InputStateEnum>&&
	std::is_scalar_v<StorageType>&&
	std::is_unsigned_v<StorageType>
	class InputStateTracker
	{
	private:
		//static_assert(s_numInputs > 0, "Number of inputs must be greater than 0");
		//static_assert(s_numInputStates > 0, "Number of input states must be greater than 0");
		//static_assert(sizeof(StorageType) / 8 < std::numeric_limits<size_t>::max(),
		//	"StorageType size in bits cannot exceed maximum value of size_t");
		//static_assert(s_numInputs <= std::numeric_limits<size_t>::max() / 8, "s_numInputs too large");
		//static_assert(s_numInputStates <= std::numeric_limits<size_t>::max(), "s_numInputStates too large");

		static inline const size_t s_storageSize = sizeof(StorageType) * 8;

		using StateArray = std::array<StorageType, (s_numInputs - 1) / s_storageSize + 1>;
		std::array<StateArray, s_numInputStates> m_stateArrays = { 0 };

		static inline constexpr std::array<size_t, s_numInputs> inputByteIndices = []() {
			std::array<size_t, s_numInputs> InputIndices;
			for (size_t i = 0; i < s_numInputs; ++i)
				InputIndices[i] = i / s_storageSize;
			return InputIndices;
			}();

		static inline constexpr std::array<size_t, s_numInputs> inputBitOffsets = []() {
			std::array<size_t, s_numInputs> InputOffsets;
			for (size_t i = 0; i < s_numInputs; ++i)
				InputOffsets[i] = i % s_storageSize;
			return InputOffsets;
			}();

	public:

		template<InputStateEnum S_E>
		void setInputState(InputEnum Input, bool state)
		{
			auto mask = static_cast<StorageType>(1) << inputBitOffsets[static_cast<size_t>(Input)];
			auto stateMask = static_cast<StorageType>(state) << inputBitOffsets[static_cast<size_t>(Input)];
			m_stateArrays[static_cast<size_t>(S_E)]
				[inputByteIndices[static_cast<size_t>(Input)]] =
				stateMask | (~mask & m_stateArrays[static_cast<size_t>(S_E)]
					[inputByteIndices[static_cast<size_t>(Input)]]);
		}

		template<InputStateEnum S_E>
		void toggleInputState(InputEnum Input)
		{
			m_stateArrays[static_cast<size_t>(S_E)]
				[inputByteIndices[static_cast<size_t>(Input)]] ^=
				static_cast<StorageType>(1) << inputBitOffsets[static_cast<size_t>(Input)];
		}

		template<InputEnum E, InputStateEnum S_E>
		bool getInputState() const
		{
			return m_stateArrays[static_cast<size_t>(S_E)]
				[inputByteIndices[static_cast<size_t>(E)]] >> inputBitOffsets[static_cast<size_t>(E)] & 1;
		}

		template<InputStateEnum S_E>
		bool getInputState(InputEnum Input) const
		{
			return m_stateArrays[static_cast<size_t>(S_E)]
				[inputByteIndices[static_cast<size_t>(Input)]] >> inputBitOffsets[static_cast<size_t>(Input)] & 1;
		}
	};
}